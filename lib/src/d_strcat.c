#include "libdiogo.h"

char	*d_strcat(char *dest, const char *src)
{
	char	*temp;

	temp = dest;
	while (*dest != '\0')
		dest++;
	while (*src != '\0')
	{
		*dest = *src;
		dest++;
		src++;
	}
	*dest = '\0';
	return (temp);
}
