#include "cachedarray.h"

int main()
{
	Cache cache(256, 2, 32, Write_Allocate, LRU);
	CachedNum::setLength(8);
	CachedNum::setCache(&cache);

	CachedArray c(32, 0x1f80);
	CachedArray2 a(8, 32);
	CachedArray b(34);

	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 32; j++)
			c(j) = c(j) + a(i,j) + b(j + 2);

	// In the exams, it is a[i][j] * b[j+2], but it won't work that way,
	// since the way of the assignment is not the way compilers work :p

	cache.printStats();

	return 0;
}
