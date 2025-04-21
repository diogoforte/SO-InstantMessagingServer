#include "../inc/libdiogo.h"
void	d_putstr_fd(char *s, int fd)
{
	int		i;

	i = 0;
	if (s)
	{
		while (s[i])
			write(fd, &s[i++], 1);
	}
}
