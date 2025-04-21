#include "../inc/libdiogo.h"
t_list	*d_lstmap(t_list *lst, void *(*f)(void *), void (*del)(void *))
{
	t_list	*newnode;
	t_list	*lista;

	lista = NULL;
	while (lst)
	{
		newnode = d_lstnew(f(lst->content));
		if (!newnode)
			del(newnode);
		else
			d_lstadd_back(&lista, newnode);
		lst = lst->next;
	}
	return (lista);
}
