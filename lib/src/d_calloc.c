#include "libdiogo.h"

void	*d_calloc(size_t nitems, size_t size)
{
	void	*d;

	d = malloc(nitems * size);
	if (!d)
		return (NULL);
	d_bzero(d, nitems * size);
	return (d);
}
