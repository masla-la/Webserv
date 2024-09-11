/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masla-la <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/29 10:55:36 by masla-la          #+#    #+#             */
/*   Updated: 2021/12/01 12:00:11 by masla-la         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line_bonus.h"

char	*ft_line(char *g)
{
	int		i;
	char	*c;

	i = 0;
	if (!g[i])
		return (NULL);
	while (g[i] && g[i] != '\n')
		i++;
	c = (char *)malloc(sizeof(char) * (i + 2));
	if (!c)
		return (NULL);
	i = 0;
	while (g[i] && g[i] != '\n')
	{
		c[i] = g[i];
		i++;
	}
	if (g[i] == '\n')
	{
		c[i] = g[i];
		i++;
	}
	c[i] = '\0';
	return (c);
}

char	*ft_read(int fd, char *g)
{
	char	*buff;
	int		i;

	buff = malloc((BUFFER_SIZE + 1) * sizeof(char));
	if (!buff)
		return (NULL);
	i = 1;
	while (!ft_strchr(g, '\n') && i != 0)
	{
		i = read(fd, buff, BUFFER_SIZE);
		if (i == -1)
		{
			free(buff);
			return (NULL);
		}
		buff[i] = '\0';
		g = ft_strjoin(g, buff);
	}
	free(buff);
	return (g);
}

char	*get_next_line(int fd)
{
	char		*line;
	static char	*g[4096];

	if (fd < 0 || BUFFER_SIZE <= 0)
		return (0);
	g[fd] = ft_read(fd, g[fd]);
	if (!g[fd])
		return (NULL);
	line = ft_line(g[fd]);
	g[fd] = ft_save(g[fd]);
	return (line);
}
