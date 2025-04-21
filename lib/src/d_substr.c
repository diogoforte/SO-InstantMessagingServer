#include "../inc/libdiogo.h"
char	*d_substr(char const *s, unsigned int start, size_t len)
{
	char		*substring;
	size_t		i;

	i = d_strlen(s);
	if (!(s))
		return (NULL);
	if (start >= i)
		len = 0;
	else if (len > (i - start))
		len = i - start;
	substring = malloc(len + 1);
	if (!(substring))
		return (NULL);
	substring[len] = '\0';
	i = 0;
	while (i < len)
	{
		substring[i] = s[start + i];
		i++;
	}
	return (substring);
}
