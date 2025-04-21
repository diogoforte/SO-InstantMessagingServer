#include "../inc/libdiogo.h"
int	d_isalnum(int a)
{
	if (d_isalpha(a))
		return (1);
	if (d_isdigit(a))
		return (1);
	else
		return (0);
}
