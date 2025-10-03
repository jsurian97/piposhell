/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strjoin.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsurian <jsurian@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/04 13:58:05 by jsurian           #+#    #+#             */
/*   Updated: 2025/10/03 10:39:01 by jsurian42        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strjoin_free(char *s1, char *s2)
{
	char	*ret;

	ret = ft_strjoin(s1, s2);
	free(s1);
	return (ret);
}

char	*ft_strjoin(char const *s1, char const *s2)
{
	int		i;
	int		y;
	char	*tab;

	i = 0;
	y = 0;
	tab = malloc(sizeof(char) * (ft_strlen(s1) + ft_strlen(s2) + 1));
	if (tab == NULL)
		return (0);
	while (s1[i] != '\0')
	{
		tab[i] = s1[i];
		i++;
	}
	while (s2[y] != '\0')
	{
		tab[i + y] = s2[y];
		y++;
	}
	tab[i + y] = '\0';
	return (tab);
}
