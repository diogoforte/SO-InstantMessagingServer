#include "libdiogo.h"

char	*d_strrchr(const char *str, int c)
{
	int	a;

	a = 0;
	while (*str)
	{
		str++;
		a++;
	}
	while (a >= 0)
	{
		if (*str == (unsigned char) c)
			return ((char *)str);
		a--;
		str--;
	}
	return (0);
}
