#include "libdiogo.h"

char	*d_strcpy(char *dest, const char *src)
{
	char	*original_dest;

	original_dest = dest;
	while (*src)
		*dest++ = *src++;
	*dest = '\0';
	return (original_dest);
}
