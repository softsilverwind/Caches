#include <iostream>
#include <cstdio>
#include <cstdlib>

using namespace std;

typedef enum
{
	Write_Allocate, Write_No_Allocate
}
WritePolicy;

typedef enum
{
	LRU, FIFO
}
KickPolicy;

class Cache
{
	private:
		struct Block
		{
			unsigned tag;
			unsigned time;
		};

		Block **data;
		unsigned size;
		unsigned assoc;
		unsigned blocksize;
		WritePolicy writep;
		KickPolicy kickp;

		unsigned blockbits;
		unsigned indexbits;
		unsigned tagbits;

		unsigned blockmask;
		unsigned indexmask;
		unsigned tagmask;

		unsigned hits;
		unsigned misses;
		unsigned total;

		static const char* const Green;
		static const char* const Red;
		static const char* const White;

		bool hit(unsigned pos);
		void allocate(unsigned pos);

		void updateKick(unsigned set, unsigned block);

	public:
		Cache(unsigned size, unsigned associativity,
				unsigned blocksize, WritePolicy writepolicy,
				KickPolicy kickpolicy = LRU);
		~Cache();

		void read(unsigned pos);
		void write(unsigned pos);

		void printStats();
};

const char* const Cache::Green = "\033[1;32m";
const char* const Cache::Red = "\033[1;31m";
const char* const Cache::White = "\033[0;0m";

inline int clog2(unsigned num)
{
	if (num > 1)
		return 1 + clog2(num >> 1);
	else
		return 0;
}

inline bool Cache::hit(unsigned pos)
{
	bool flag = false;
	
	unsigned indexpos = (pos & indexmask) >> blockbits;

	for (int i = 0; i < assoc; ++i)
	{
		if ((pos & tagmask) == data[indexpos][i].tag)
		{
			flag = true;

			if (kickp == LRU)
				updateKick(indexpos, i);

			break;
		}
	}

	++total;
	flag ? ++hits : ++misses;

	return flag;
}

inline void Cache::updateKick(unsigned set, unsigned block)
{
	data[set][block].time = assoc;

	for (int j = 0; j < assoc; ++j)
		(data[set][j].time > 0) ? --data[set][j].time : 0;
}

inline void Cache::allocate(unsigned pos)
{
	int set = 0;

	unsigned indexpos = (pos & indexmask) >> blockbits;

	for (int i = 0; i < assoc; ++i)
	{
		if (data[indexpos][i].time == 0)
		{
			data[indexpos][i].tag = pos & tagmask;

			updateKick(indexpos, i);

			break;
		}
	}
}

inline Cache::Cache(unsigned size, unsigned associativity,
		unsigned blocksize, WritePolicy writepolicy,
		KickPolicy kickpolicy)
	: size(size), assoc(associativity), blocksize(blocksize),
			writep(writepolicy), kickp(kickpolicy), blockmask(blocksize - 1),
			indexmask(size / blocksize / associativity - 1), tagmask(blockmask),
			hits(0), misses(0), total(0)
{
	blockbits = clog2(blocksize);
	blockmask = (1 << blockbits) - 1;

	indexbits = clog2(size / blocksize / assoc);
	indexmask = (1 << indexbits) - 1 << blockbits;

	tagbits = 32 - blockbits - indexbits;
	tagmask = (1 << tagbits) - 1 << indexbits << blockbits;

	data = new Block *[size / blocksize / assoc];

	for(int i = 0; i < size / blocksize / assoc; ++i)
	{
		data[i] = new Block[assoc];
		for(int j = 0; j < assoc; ++j)
		{
			data[i][j].tag = 0xFFFFFFFF;
			data[i][j].time = j;
		}
	}
}

inline Cache::~Cache()
{
	for(int i = 0; i < size / blocksize / assoc; ++i)
		delete [] data[i];

	delete [] data;
}

inline void Cache::read(unsigned pos)
{
	cout << (hit(pos) ? Green : (allocate(pos), Red));
	printf("R 0x%04x, ", pos);
	cout << White;
}

inline void Cache::write(unsigned pos)
{
	cout << (hit(pos) ? Green : (writep == Write_Allocate) ? (allocate(pos), Red) : Red);
	printf("W 0x%04x", pos);
	cout << White << endl;
}

void Cache::printStats()
{
	printf("%d/%d = %f\n", hits, total, (double) hits / total);
	printf("%d/%d = %f\n", misses, total, (double) misses / total);
}

/* ------------------------------------------------------------------------- */

class CachedNum
{
	private:
		static Cache *cache;
		static unsigned length;
		unsigned mempos;


		void read();
		void write();

	public:
		CachedNum(unsigned mempos);

		static void setLength(unsigned len);
		static unsigned getLength();

		static void setCache(Cache *cache);

		friend bool operator+(CachedNum x, CachedNum y);
		friend bool operator+(bool x, CachedNum y);
		friend bool operator+(CachedNum x, bool y);

		friend bool operator-(CachedNum x, CachedNum y);
		friend bool operator-(bool x, CachedNum y);
		friend bool operator-(CachedNum x, bool y);

		friend bool operator*(CachedNum x, CachedNum y);
		friend bool operator*(bool x, CachedNum y);
		friend bool operator*(CachedNum x, bool y);

		friend bool operator/(CachedNum x, CachedNum y);
		friend bool operator/(bool x, CachedNum y);
		friend bool operator/(CachedNum x, bool y);

		void operator=(bool s);
		void operator=(CachedNum x);
};

Cache *CachedNum::cache = NULL;
unsigned CachedNum::length = 4;

inline bool operator+(CachedNum x, CachedNum y)
{
	x.read();
	y.read();
	return false;
}

inline bool operator+(bool x, CachedNum y)
{
	y.read();
	return false;
}

inline bool operator+(CachedNum x, bool y)
{
	x.read();
	return false;
}

inline bool operator-(CachedNum x, CachedNum y)
{
	x.read();
	y.read();
	return false;
}

inline bool operator-(bool x, CachedNum y)
{
	y.read();
	return false;
}

inline bool operator-(CachedNum x, bool y)
{
	x.read();
	return false;
}

inline bool operator*(CachedNum x, CachedNum y)
{
	x.read();
	y.read();
	return false;
}

inline bool operator*(bool x, CachedNum y)
{
	y.read();
	return false;
}

inline bool operator*(CachedNum x, bool y)
{
	x.read();
	return false;
}

inline bool operator/(CachedNum x, CachedNum y)
{
	x.read();
	y.read();
	return false;
}

inline bool operator/(bool x, CachedNum y)
{
	y.read();
	return false;
}

inline bool operator/(CachedNum x, bool y)
{
	x.read();
	return false;
}

inline void CachedNum::operator=(bool s)
{
	write();
}

inline void CachedNum::operator=(CachedNum x)
{
	x.read();
	write();
}

inline void CachedNum::read()
{
	cache->read(mempos);
}

inline void CachedNum::write()
{
	cache->write(mempos);
}

inline CachedNum::CachedNum(unsigned mempos)
	: mempos(mempos)
{
}

inline void CachedNum::setLength(unsigned length)
{
	CachedNum::length = length;
}

inline unsigned CachedNum::getLength()
{
	return length; 
}

inline void CachedNum::setCache(Cache *cache)
{
	CachedNum::cache = cache;
}

/* ------------------------------------------------------------------------- */

class CachedArray
{
	private:
		unsigned length;
		CachedNum** contents;
	public:
		CachedNum operator() (unsigned pos);
		CachedArray(unsigned length, unsigned membegin);
		~CachedArray();
};

inline CachedArray::CachedArray(unsigned length, unsigned membegin)
	: length(length)
{
	contents = new CachedNum* [length];

	for (unsigned i = 0; i < length; ++i)
		contents[i] = new CachedNum(membegin + CachedNum::getLength() * i);
}

inline CachedNum CachedArray::operator() (unsigned pos)
{
	if (pos >= length)
	{
		cerr << "Error, array access out of bounds!" << endl;
		exit(42);
	}

	return *contents[pos];
}

inline CachedArray::~CachedArray()
{
	for (unsigned i = 0; i < length; ++i)
		delete contents[i];

	delete [] contents;
}

/* ------------------------------------------------------------------------- */

class CachedArray2
{
	private:
		unsigned rowlength, columnlength;
		CachedNum** contents;

	public:
		CachedNum operator() (unsigned rowpos, unsigned columnpos);
		CachedArray2(unsigned rowlength, unsigned columnlength, unsigned membegin);
		~CachedArray2();
};

inline CachedArray2::CachedArray2(unsigned rowlength, unsigned columnlength, unsigned membegin)
	: rowlength(rowlength), columnlength(columnlength)
{
	contents = new CachedNum *[rowlength * columnlength];

	for (unsigned i = 0; i < rowlength * columnlength; ++i)
		contents[i] = new CachedNum(membegin + CachedNum::getLength() * i);
}

inline CachedNum CachedArray2::operator() (unsigned rowpos, unsigned columnpos)
{
	if (rowpos >= rowlength || columnpos >= columnlength)
	{   
		cerr << "Error, array access out of bounds!" << endl;
		exit(42);
	}	  

	return *contents[rowpos * columnlength + columnpos];
}

inline CachedArray2::~CachedArray2()
{
	for (unsigned i = 0; i < rowlength * columnlength; ++i)
		delete contents[i];

	delete [] contents;
}
