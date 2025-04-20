#include "libdiogo.h"

void	*d_memcpy(void *dest, const void *src, size_t n)
{
	size_t			i;
	unsigned char	*a;
	unsigned char	*b;

	a = (unsigned char *)src;
	b = (unsigned char *)dest;
	i = 0;
	if (dest == src || n == 0)
		return (dest);
	while (i < n)
	{
		b[i] = a[i];
		i++;
	}
	return (dest);
}
