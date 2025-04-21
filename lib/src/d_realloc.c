#include "../inc/libdiogo.h"
void	*d_realloc(void *ptr, size_t nitems, size_t size)
{
	void	*new_ptr;

	if (!ptr)
		return (d_calloc(nitems * size + 1, size));
	if (nitems == 0)
	{
		free(ptr);
		return (NULL);
	}
	new_ptr = d_calloc(nitems * size, size);
	if (!new_ptr)
		return (NULL);
	d_memmove(new_ptr, ptr, nitems * size);
	free(ptr);
	return (new_ptr);
}
