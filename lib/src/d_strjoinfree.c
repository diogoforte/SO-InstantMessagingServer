#include "libdiogo.h"

char	*d_strjoinfree(char *s1, char *s2)
{
	char	*res;

	res = malloc(d_strlen(s1) + d_strlen(s2) + 1);
	if (!res)
		return (NULL);
	d_strlcpy(res, s1, d_strlen(s1) + 1);
	d_strlcat(res, s2, d_strlen(s1) + d_strlen(s2) + 1);
	free(s1);
	return (res);
}
