#include "libdiogo.h"

static int	wdc(char const *s, char c)
{
	int	i;

	if (!s)
		return (0);
	i = 0;
	while (*s)
	{
		if (*s == c)
			s++;
		else
		{
			i++;
			while (*s && *s != c)
				s++;
		}
	}
	return (i);
}

static size_t	word_size(const char *str, int n, char c)
{
	size_t	i;

	i = 0;
	while (str[++n] && str[n] != c)
		i ++;
	return (i);
}

char	**d_split(const char *str, char c)
{
	char	**ret;
	int		size;
	int		j;
	int		i;

	i = 0;
	j = -1;
	size = wdc(str, c);
	ret = (char **)malloc(sizeof(char *) * (size + 1));
	if (!ret)
		return (NULL);
	ret[size] = NULL;
	while (++j < size)
	{
		while (str[i] && str[i] == c)
			i ++;
		if (str[i] && str[i] != c)
			ret[j] = d_substr(str, i, word_size(str, i - 1, c));
		while (str[i] && str[i] != c)
			i ++;
	}
	return (ret);
}
