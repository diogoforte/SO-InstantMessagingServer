#include "../inc/libdiogo.h"
void	d_putchar_fd(char c, int fd)
{
	write(fd, &c, 1);
}
