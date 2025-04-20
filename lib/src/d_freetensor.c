#include "libdiogo.h"

void	d_freetensor(char ***tensor)
{
	int	i;
	int	j;

	i = -1;
	if (tensor)
	{
		while (tensor[++i] != NULL)
		{
			j = -1;
			while (tensor[i][++j] != NULL)
				free(tensor[i][j]);
			free(tensor[i]);
		}
		free(tensor);
	}
}
