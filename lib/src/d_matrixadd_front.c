#include "../inc/libdiogo.h"
void	d_matrixadd_front(char ***matrix, char *str)
{
	int	rows;

	rows = 0;
	while ((*matrix)[rows])
		rows++;
	*matrix = d_realloc(*matrix, rows + 2, sizeof(char *));
	d_memmove((*matrix) + 1, (*matrix), (rows + 1) * sizeof(char *));
	(*matrix)[0] = d_strdup(str);
}
