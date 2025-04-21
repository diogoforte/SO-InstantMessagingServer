#include "../inc/libdiogo.h"
char	*d_strjoin_address(char **original, char *add)
{
	int		i;
	int		j;
	char	*joined;

	if (!original || !(*original) || !add)
		return (NULL);
	joined = d_calloc(sizeof(char), (d_strlen((*original)) + d_strlen(add)
				+ 1));
	if (!joined)
		return (NULL);
	i = -1;
	while ((*original)[++i])
		joined[i] = (*original)[i];
	j = -1;
	while (add[++j])
		joined[i + j] = add[j];
	free((*original));
	*original = joined;
	return (joined);
}
