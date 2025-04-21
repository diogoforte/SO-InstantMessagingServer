#include "../inc/libdiogo.h"
int	d_isprint(int a)
{
	if (a >= 32 && a <= 126)
		return (1);
	else
		return (0);
}
