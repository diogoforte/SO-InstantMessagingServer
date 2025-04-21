#include "../inc/libdiogo.h"
void	d_putnbr_fd(int n, int fd)
{
	if (n == -2147483648)
		write(fd, "-2147483648", 11);
	else if (n < 0)
	{
		d_putchar_fd('-', fd);
		n = -n;
		d_putnbr_fd(n, fd);
	}
	else if (n > 9)
	{
		d_putnbr_fd(n / 10, fd);
		d_putnbr_fd(n % 10, fd);
	}
	else
		d_putchar_fd(n + 48, fd);
}
