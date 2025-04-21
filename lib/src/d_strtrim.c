#include "../inc/libdiogo.h"
char	*d_strtrim(char const *s1, char const *set)
{
	size_t	i;

	if (*set == 0)
		return (d_strdup(s1));
	while (*s1 && d_strchr(set, *s1))
		s1++;
	i = d_strlen((char *) s1);
	while (i > 0 && d_strchr(set, s1[i]))
		i--;
	return (d_substr(s1, 0, i + 1));
}
