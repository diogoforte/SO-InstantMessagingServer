#include "../inc/libdiogo.h"
int	d_memcmp(const void *str1, const void *str2, size_t n)
{
	size_t	i;

	i = 0;
	while (n > i)
	{
		if (((unsigned char *) str1)[i] == ((unsigned char *) str2)[i])
			i++;
		else
			return (((unsigned char *)str1)[i] - ((unsigned char *)str2)[i]);
	}
	return (0);
}
