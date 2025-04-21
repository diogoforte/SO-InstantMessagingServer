#include "../inc/libdiogo.h"
void	d_matrixadd_back(char ***matrix, char *str)
{
	int	len;

	len = d_matrix_len(*matrix);
	*matrix = d_realloc(*matrix, len + 2, sizeof(char *));
	(*matrix)[len] = d_strdup(str);
	(*matrix)[len + 1] = NULL;
}
