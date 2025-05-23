#include "../inc/libdiogo.h"
static int	counter(int n)
{
	int	size;

	size = 0;
	if (n == 0)
		return (1);
	if (n < 0)
		size += 1;
	while (n != 0)
	{
		n /= 10;
		size++;
	}
	return (size);
}

char	*d_itoa(int n)
{
	int			size;
	char		*str;
	long int	nb;

	nb = n;
	size = counter(nb);
	str = malloc(size + 1);
	if (!str)
		return (NULL);
	str[size] = '\0';
	while (size > 0)
	{
		if (nb < 0)
			nb *= -1;
		str[--size] = (nb % 10) + 48;
		nb /= 10;
	}
	if (str[0] == '0')
		str[0] = '-';
	if (n == 0)
		str[0] = '0';
	return (str);
}
