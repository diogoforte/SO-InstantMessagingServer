#ifndef LIBDIOGO_H
# define LIBDIOGO_H

# include <limits.h>
# include <stdarg.h>
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>

# define DECIMAL "0123456789"
# define HEXA "0123456789abcdef"
# define HEXA2 "0123456789ABCDEF"
# define BUFFER_SIZE 256

typedef struct s_list
{
	void			*content;
	struct s_list	*next;
}					t_list;

int					d_atoi(const char *str);
int					d_isalnum(int a);
int					d_isalpha(int a);
int					d_isascii(int a);
int					d_isdigit(int a);
int					d_isprint(int a);
size_t				d_strlen(char const *str);
void				*d_memset(void *str, int c, size_t n);
void				d_bzero(void *s, size_t n);
void				*d_memcpy(void *dest, const void *src, size_t n);
size_t				d_strlcpy(char *dest, const char *src, size_t size);
size_t				d_strlcat(char *dest, const char *src, size_t size);
int					d_toupper(int ch);
int					d_tolower(int ch);
char				*d_strchr(const char *str, int c);
char				*d_strrchr(const char *str, int c);
int					d_strncmp(const char *str1, const char *str2, size_t n);
char				*d_strnstr(const char *big, const char *little,
						size_t len);
void				*d_memchr(const void *str, int c, size_t n);
int					d_memcmp(const void *str1, const void *str2, size_t n);
void				*d_memmove(void *dest, const void *src, size_t n);
void				*d_calloc(size_t nitems, size_t size);
char				*d_strdup(const char *s);
char				*d_strjoin(char const *s1, char const *s2);
char				*d_strtrim(char const *s1, char const *set);
char				*d_itoa(int n);
char				*d_substr(char const *s, unsigned int start, size_t len);
void				d_putchar_fd(char c, int fd);
void				d_putstr_fd(char *s, int fd);
void				d_putendl_fd(char *s, int fd);
void				d_putnbr_fd(int n, int fd);
char				*d_strmapi(char const *s, char (*f)(unsigned int, char));
void				d_striteri(char *s, void (*f)(unsigned int, char *));
char				**d_split(char const *s, char c);
t_list				*d_lstnew(void *content);
void				d_lstadd_front(t_list **lst, t_list *new);
int					d_lstsize(t_list *lst);
t_list				*d_lstlast(t_list *lst);
void				d_lstadd_back(t_list **lst, t_list *new);
void				d_lstdelone(t_list *lst, void (*del)(void *));
void				d_lstclear(t_list **lst, void (*del)(void *));
void				d_lstiter(t_list *lst, void (*f)(void *));
t_list				*d_lstmap(t_list *lst, void *(*f)(void *),
						void (*del)(void *));
char				*d_strncpy(char *dest, const char *src, size_t n);
char				*d_strcat(char *dest, const char *src);
char				*d_strjoin_var(char *first, ...);
void				d_freematrix(char **list);
void				d_freetensor(char ***tensor);
char				*d_strpbrk(char *str, char *set);
int					d_isspace(int c);
char				*d_strcpy(char *dest, const char *src);
int					d_strcmp(const char *s1, const char *s2);
int					d_dprintf(int fd, const char *str, ...);
char				*d_strjoinfree(char *s1, char *s2);
char				*get_next_line(int fd);
int					d_matrix_len(char **str);
void				*d_realloc(void *ptr, size_t nitems, size_t size);
void				d_matrixadd_back(char ***matrix, char *str);
char				*d_strjoin_address(char **original, char *add);
int					d_charcount(char *str, char c);
void				d_matrixadd_front(char ***matrix, char *str);
char				*d_itoa_address(char *str, int n);
int					d_max(int a, int b);
int					d_min(int a, int b);

#endif