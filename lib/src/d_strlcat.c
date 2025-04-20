#include "libdiogo.h"

size_t	d_strlcat(char *dest, const char *src, size_t size)
{
	size_t	i;
	size_t	j;

	i = d_strlen(dest);
	j = 0;
	if (size == 0 || size <= i)
		return (d_strlen((char *)src) + size);
	while (src[j] && j < (size - i - 1))
	{
		dest[i + j] = src[j];
		j++;
	}
	dest[i + j] = '\0';
	return (i + d_strlen((char *)src));
}
