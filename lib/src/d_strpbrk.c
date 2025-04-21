#include "../inc/libdiogo.h"
char	*d_strpbrk(char *str, char *set)
{
	char	*p;
	char	*s;

	p = str;
	while (*p)
	{
		s = set;
		while (*s)
		{
			if (*p == *s)
				return (p);
			s++;
		}
		p++;
	}
	return (NULL);
}
