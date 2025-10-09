/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_space.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anpayot <anpayot@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/08 14:21:38 by jsurian42         #+#    #+#             */
/*   Updated: 2025/10/09 11:02:09 by anpayot          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

char	*ft_strjoin_free2(char *s1, char *s2)
{
	char	*ret;

	ret = ft_strjoin(s1, s2);
	free(s1);
	free(s2);
	return (ret);
}

char	*expand_space(const char *value, int double_quote)
{
	t_expand_space	v;

	if (!value)
		return (ft_strdup(""));
	if (double_quote)
		return (ft_strdup(value));
	v.split = ft_split(value, ' ');
	if (!v.split)
		return (NULL);
	v.result = ft_strdup("");
	v.i = 0;
	v.first = 1;
	while (v.split[v.i])
	{
		if (ft_strlen(v.split[v.i]) > 0)
		{
			if (!v.first)
				v.result = ft_strjoin_free2(v.result, ft_strdup(" "));
			v.temp = ft_strdup(v.split[v.i]);
			v.result = ft_strjoin_free2(v.result, v.temp);
			v.first = 0;
		}
		v.i++;
	}
	return (ft_split_free(v.split), v.result);
}
