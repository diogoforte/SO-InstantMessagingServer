#include "libdiogo.h"

void	d_putendl_fd(char *s, int fd)
{
	if (s)
	{
		write(fd, s, d_strlen(s));
		write(fd, "\n", 1);
	}
}
