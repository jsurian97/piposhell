/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pars_pipe_split.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anpayot <anpayot@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/07 13:44:11 by jsurian42         #+#    #+#             */
/*   Updated: 2025/09/30 11:02:58 by anpayot          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

static int	is_pipe(t_list *node)
{
	if (!node || !node->token)
		return (0);
	return (!ft_strncmp(node->token->str, "|", 2));
}

static int	remove_trailing_pipe(t_list **sublist)
{
	t_list	*prev;
	t_list	*cur;

	if (!sublist || !*sublist)
		return (0);
	prev = NULL;
	cur = *sublist;
	while (cur->next)
	{
		prev = cur;
		cur = cur->next;
	}
	if (!cur->token || cur->token->type != PIPE)
		return (0);
	if (prev)
		prev->next = NULL;
	else
		*sublist = NULL;
	del_token(cur->token);
	free(cur);
	return (1);
}

static int	report_pipe_syntax(t_pars_data *data)
{
	ft_putstr_fd("minishell: syntax error near unexpected token `|'\n", 2);
	ft_lstclear(&data->lst_simple_cmd, del_lst_token);
	data->err_status = 2;
	return (1);
}

static int	pars_pipe_normalize(t_pars_data *data)
{
	t_list	*node;
	int		removed;

	node = data->lst_simple_cmd;
	while (node)
	{
		removed = remove_trailing_pipe(&node->lst);
		if (!node->lst || is_pipe(node->lst))
			return (report_pipe_syntax(data));
		if (removed && node->next == NULL)
			return (report_pipe_syntax(data));
		node = node->next;
	}
	return (0);
}

int	pars_pipe_split(t_pars_data *data)
{
	data->lst_simple_cmd = ft_lstsplit(data->lst_token, &is_pipe);
	if (data->lst_simple_cmd == NULL)
	{
		data->err_status = 2;
		return (1);
	}
	if (pars_pipe_normalize(data))
		return (1);
	return (0);
}
