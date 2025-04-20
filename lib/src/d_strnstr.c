#include "libdiogo.h"

char	*d_strnstr(const char *big, const char *little, size_t len)
{
	size_t	i;
	size_t	size;

	if (!len && little[0])
		return (NULL);
	i = -1;
	size = d_strlen(little);
	if (d_strlen(big) >= size && len >= size)
	{
		while (++i <= len - size)
		{
			if (!d_strncmp(&big[i], little, size))
				return ((char *)&big[i]);
		}
	}
	return (NULL);
}
