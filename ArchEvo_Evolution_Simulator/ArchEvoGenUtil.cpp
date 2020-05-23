#include "ArchEvoGenUtil.h"

int ArchEvoGenUtil::true_mod(int n, int m)
{
	return (m + (n % m)) % m;
}
