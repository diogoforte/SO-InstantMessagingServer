#include "../inc/libdiogo.h"
void	*d_memmove(void *dest, const void *src, size_t n)
{
	char	*s;
	char	*d;

	s = (char *) src;
	d = (char *) dest;
	if (s < d)
	{
		while (n--)
		{
			d[n] = s[n];
		}
	}
	else
	{
		d_memcpy(d, s, n);
	}
	return (dest);
}
