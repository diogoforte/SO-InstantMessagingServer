#include "libdiogo.h"

void	*d_memset(void *str, int c, size_t n)
{
	size_t			i;
	unsigned char	*a;

	a = (unsigned char *) str;
	i = 0;
	while (i < n)
	{
		a[i] = c;
		i++;
	}
	return (str);
}
