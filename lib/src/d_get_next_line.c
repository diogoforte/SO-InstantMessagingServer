#include "../inc/libdiogo.h"
size_t	gnl_strlen(const char *str)
{
	size_t	i;

	i = 0;
	if (!str)
		return (0);
	while (str[i] && str[i] != 10)
		i++;
	if (str[i] == 10)
		i++;
	return (i);
}

char	*gnl_strjoin(char *str1, char *str2)
{
	char	*ret;
	size_t	i;

	ret = malloc(gnl_strlen(str1) + gnl_strlen(str2) + 1);
	if (!ret)
		return (NULL);
	i = 0;
	while (str1 && str1[i])
	{
		ret[i] = str1[i];
		i++;
	}
	free(str1);
	while (*str2)
	{
		ret[i++] = *str2;
		if (*str2++ == 10)
			break ;
	}
	ret[i] = 0;
	return (ret);
}

int	nl(char *buf)
{
	int	i;
	int	g;
	int	j;

	i = 0;
	g = 0;
	j = 0;
	while (buf[i])
	{
		if (j == 1)
			buf[g++] = buf[i];
		if (buf[i] == 10)
			j = 1;
		buf[i++] = 0;
	}
	return (j);
}

char	*get_next_line(int fd)
{
	char		*line;
	static char	buf[FOPEN_MAX][BUFFER_SIZE + 1];
	int			i;

	i = 0;
	if (read(fd, 0, 0) < 0 || BUFFER_SIZE < 1 || fd >= FOPEN_MAX)
	{
		if (fd > 0 && fd < FOPEN_MAX)
			while (buf[fd][i])
				buf[fd][i++] = 0;
		return (NULL);
	}
	line = NULL;
	while (*(buf[fd]) || read(fd, buf[fd], BUFFER_SIZE) > 0)
	{
		line = gnl_strjoin(line, buf[fd]);
		if (nl(buf[fd]) == 1)
			break ;
	}
	return (line);
}
