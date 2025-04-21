#include "../inc/libdiogo.h"
int	d_strncmp(const char *str1, const char *str2, size_t n)
{
	size_t	i;

	i = 0;
	while (n--)
	{
		if (str1[i] == str2[i] && str1[i] && str2[i])
			i++;
		else
			return (((unsigned char *) str1)[i] - ((unsigned char *) str2)[i]);
	}
	return (0);
}
