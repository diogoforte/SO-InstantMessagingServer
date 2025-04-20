#include "libdiogo.h"

void	*d_memchr(const void *str, int c, size_t n)
{
	size_t				i;
	const unsigned char	*strcast;
	unsigned char		ch;

	i = 0;
	strcast = (const unsigned char *) str;
	ch = (unsigned char) c;
	while (i < n)
	{
		if (*(unsigned char *)(strcast + i) == ch)
			return ((unsigned char *)(str + i));
		i++;
	}
	return (NULL);
}
