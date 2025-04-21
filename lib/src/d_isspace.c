#include "../inc/libdiogo.h"
int	d_isspace(int c)
{
	return (c == 32 || (c > 8 && c < 14));
}
