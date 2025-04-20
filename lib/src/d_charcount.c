#include "libdiogo.h"

int	d_charcount(char *str, char c)
{
	int	count;

	if (!str)
		return (0);
	count = 0;
	while (*str)
		if (*str++ == c)
			count++;
	return (count);
}
