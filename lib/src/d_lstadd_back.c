#include "../inc/libdiogo.h"
void	d_lstadd_back(t_list **lst, t_list *new)
{
	t_list	*temp;

	if (!new)
		return ;
	if (!*lst)
	{
		*lst = new;
		return ;
	}
	temp = d_lstlast(*lst);
	temp->next = new;
}
