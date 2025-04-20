#include "libdiogo.h"

char	*d_strdup(const char *s)
{
	size_t	i;
	char	*s2;

	s2 = malloc(d_strlen((char *) s) + 1);
	if (!s2)
		return (NULL);
	i = 0;
	while (((char *)s)[i])
	{
		s2[i] = ((char *)s)[i];
		i++;
	}
	s2[i] = '\0';
	return (s2);
}
