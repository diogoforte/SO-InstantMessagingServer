#include "../inc/libdiogo.h"
void	d_bzero(void *s, size_t n)
{
	size_t			i;
	unsigned char	*a;

	a = (unsigned char *)s;
	i = 0;
	while (i < n)
	{
		a[i] = 0;
		i++;
	}
}
