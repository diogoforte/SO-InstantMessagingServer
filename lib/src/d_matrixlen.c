#include "libdiogo.h"

int	d_matrix_len(char **str)
{
	int	i;

	i = 0;
	while (str && str[i])
		i++;
	return (i);
}
