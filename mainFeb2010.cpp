#include "cachedarray.h"

int main()
{
	Cache cache(1024, 2, 32, Write_Allocate, LRU);
	CachedNum::setLength(8);
	CachedNum::setCache(&cache);

	CachedArray2 A(9, 8, 0x0300);
	CachedArray2 B(8, 8, 0x1500);
	CachedArray2 C(8, 8, 0x2100);

	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
			B(i,j) = A(i,j) + C(i,j) + A(i+1, j);

	cache.printStats();

	return 0;
}
