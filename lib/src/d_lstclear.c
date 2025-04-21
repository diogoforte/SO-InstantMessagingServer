#include "../inc/libdiogo.h"
void	d_lstclear(t_list **lst, void (*del)(void*))
{
	t_list	*d;

	while (*lst)
	{
		d = (*lst)->next;
		d_lstdelone(*lst, del);
		*lst = d;
	}
	*lst = NULL;
}
