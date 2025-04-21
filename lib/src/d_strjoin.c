#include "../inc/libdiogo.h"
char	*d_strjoin(char const *s1, char const *s2)
{
	char	*res;

	res = malloc(d_strlen((char *)s1) + d_strlen((char *)s2) + 1);
	if (!res)
		return (NULL);
	d_strlcpy(res, s1, d_strlen((char *)s1) + 1);
	d_strlcat(res, s2, d_strlen((char *)s1) + d_strlen((char *)s2) + 1);
	return (res);
}
