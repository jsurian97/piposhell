/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_dollar.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anpayot <anpayot@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/05 13:25:00 by anpayot           #+#    #+#             */
/*   Updated: 2025/10/05 13:25:00 by anpayot          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

static char	*expand_dollar_status(size_t *i, int last_status)
{
	*i += 1;
	return (ft_itoa(last_status));
}

static char	*expand_dollar_braced(char *str, size_t *i, char **envp)
{
	t_expand_dollar	d;

	d.start = *i + 1;
	*i += 1;
	while (ft_isalnum(str[*i]) || str[*i] == '_')
		*i += 1;
	if (str[*i] == '}' && *i > d.start)
	{
		d.name = ft_substr(str, d.start, *i - d.start);
		d.value = ft_get_env_value(envp, d.name);
		free(d.name);
		*i += 1;
		if (d.value != NULL)
			return (ft_strdup(d.value));
		return (ft_strdup(""));
	}
	*i = d.start - 1;
	return (ft_strdup("$"));
}

static char	*expand_dollar_name(char *str, size_t *i, char **envp)
{
	t_expand_dollar	d;

	d.start = *i;
	while (ft_isalnum(str[*i]) || str[*i] == '_')
		*i += 1;
	d.name = ft_substr(str, d.start, *i - d.start);
	d.value = ft_get_env_value(envp, d.name);
	free(d.name);
	if (d.value != NULL)
		return (ft_strdup(d.value));
	return (ft_strdup(""));
}

char	*expand_dollar(char *str, size_t *i, char **envp, int last_status)
{
	*i += 1;
	if (str[*i] == '?')
		return (expand_dollar_status(i, last_status));
	if (str[*i] == '{')
		return (expand_dollar_braced(str, i, envp));
	if (str[*i] == '"')
		return (ft_strdup(""));
	if (ft_isalnum(str[*i]) || str[*i] == '_')
		return (expand_dollar_name(str, i, envp));
	return (ft_strdup("$"));
}
