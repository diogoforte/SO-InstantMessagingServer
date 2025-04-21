#include "../inc/libdiogo.h"
char	*d_strmapi(char const *s, char (*f)(unsigned int, char))
{
	char			*str;
	unsigned int	n;
	unsigned int	i;

	if (!s)
		return (NULL);
	n = d_strlen(s);
	i = -1;
	str = malloc(n + 1);
	if (!str)
		return (NULL);
	while (n > ++i)
		str[i] = f(i, s[i]);
	str[i] = '\0';
	return (str);
}
