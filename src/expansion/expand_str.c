/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_str.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anpayot <anpayot@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/21 14:47:01 by jsurian42         #+#    #+#             */
/*   Updated: 2025/10/03 10:38:55 by jsurian42        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

static void	expand_str_handle_quote(char *str, t_expand_view *view)
{
	char	*fragment;

	if (view->quote == 0)
		view->quote = str[view->i];
	else if (view->quote == str[view->i])
		view->quote = 0;
	fragment = ft_substr(str, view->i, 1);
	view->newstr = ft_strjoin_free(view->newstr, fragment);
	free(fragment);
	view->i++;
}

static char	*expand_str_backslash(char *str, t_expand_view *view)
{
	char	next;

	if (view->quote == '\'')
		return (ft_substr(str, view->i++, 1));
	next = str[view->i + 1];
	if (next == '\0')
		return (ft_substr(str, view->i++, 1));
	if (view->quote == '"' && next != '$' && next != '"' && next != '\\')
		return (ft_substr(str, view->i++, 1));
	view->i += 2;
	return (ft_substr(str, view->i - 1, 1));
}

char	*expand_tilde(char *str, size_t *i, char **envp)
{
	char	*home_value;

	if (str[*i + 1] == '/' || str[*i + 1] == '\0' || str[*i + 1] == ' ')
	{
		*i += 1;
		home_value = ft_get_env_value(envp, "HOME");
		if (home_value != NULL)
			return (ft_strdup(home_value));
		return (ft_strdup("~"));
	}
	return (ft_substr(str, (*i)++, 1));
}

static char	*expand_str_fragment(char *str, t_expand_view *view,
		char **envp, int last_status)
{
	if (str[view->i] == '\'' || str[view->i] == '"')
	{
		expand_str_handle_quote(str, view);
		return (NULL);
	}
	if (str[view->i] == '\\')
		return (expand_str_backslash(str, view));
	if (str[view->i] == '$' && view->quote != '\'')
		return (expand_dollar(str, &view->i, envp, last_status));
	if (str[view->i] == '~' && view->quote != '\'')
		return (expand_tilde(str, &view->i, envp));
	return (ft_substr(str, view->i++, 1));
}

char	*expand_str_heredoc(char *str, char **envp, int last_status)
{
	t_expand_view	v;
	char			*fragment;

	v.i = 0;
	v.newstr = ft_strdup("");
	while (str[v.i])
	{
		if (str[v.i] == '$')
			fragment = expand_dollar(str, &v.i, envp, last_status);
		else
			fragment = ft_substr(str, v.i++, 1);
		v.newstr = ft_strjoin_free(v.newstr, fragment);
		free(fragment);
	}
	free(str);
	return (v.newstr);
}

char	*expand_str(char *str, char **envp, int last_status)
{
	t_expand_view	v;
	char			*fragment;

	v.i = 0;
	v.quote = 0;
	v.newstr = ft_strdup("");
	while (str[v.i])
	{
		fragment = expand_str_fragment(str, &v, envp, last_status);
		if (fragment == NULL)
			continue ;
		v.newstr = ft_strjoin_free(v.newstr, fragment);
		free(fragment);
	}
	free(str);
	return (v.newstr);
}
