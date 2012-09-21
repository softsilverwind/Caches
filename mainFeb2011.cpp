#include "cachedarray.h"

int main()
{
	Cache cache(1024, 1, 32, Write_Allocate);
	CachedNum::setLength(8);
	CachedNum::setCache(&cache);

	CachedArray2 A(11, 12);
	CachedArray2 B(11, 12);

	for(int i = 0; i < 5; i++)
		for(int j = 0; j < 12; j++)
			A(i,j) = A(i,j) * B(i,j % 4);

	cache.printStats();

	return 0;
}
