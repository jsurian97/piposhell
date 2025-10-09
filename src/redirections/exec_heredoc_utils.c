/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_heredoc_utils.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anpayot <anpayot@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 07:45:00 by anpayot           #+#    #+#             */
/*   Updated: 2025/10/09 12:19:14 by anpayot          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

int	heredoc_cleanup(int fd_pipe[2], char *line, int result)
{
	if (line)
		free(line);
	close(fd_pipe[0]);
	close(fd_pipe[1]);
	signals_set_interactive();
	return (result);
}

int	heredoc_expand_line(t_red *red, char **line, char **envp,
		int exit_status)
{
	char	*expanded;

	if (red->quoted_heredoc)
		return (0);
	expanded = expand_str_heredoc(*line, envp, exit_status);
	if (expanded == NULL)
	{
		*line = NULL;
		return (1);
	}
	*line = expanded;
	return (0);
}

static int	heredoc_abort_signal(t_heredoc_ctx *ctx, char *line)
{
	return (heredoc_cleanup(ctx->fd_pipe, line, -1));
}

void	exec_cleanup_heredocs(t_exec_data *data)
{
	t_list	*lst_current;
	size_t	i;

	lst_current = data->lst_simple_cmd;
	while (lst_current)
	{
		i = 0;
		while (i < lst_current->scmd->nbr_of_red)
		{
			if (lst_current->scmd->red[i].type == RED_HEREDOC
				&& lst_current->scmd->red[i].fd_heredoc != -1)
			{
				close(lst_current->scmd->red[i].fd_heredoc);
				lst_current->scmd->red[i].fd_heredoc = -1;
			}
			i++;
		}
		lst_current = lst_current->next;
	}
}

int	heredoc_collect_lines(t_red *red, t_heredoc_ctx *ctx)
{
	char	*line;

	while (1)
	{
		if (g_signal_received == SIGINT)
			return (heredoc_abort_signal(ctx, NULL));
		line = readline("> ");
		if (g_signal_received == SIGINT)
			return (heredoc_abort_signal(ctx, line));
		if (line == NULL)
			return (heredoc_cleanup(ctx->fd_pipe, NULL, -1));
		if (ft_strlen(line) == ctx->delim_len
			&& !ft_strncmp(line, red->word, ctx->delim_len))
			break ;
		if (heredoc_expand_line(red, &line, ctx->envp, ctx->exit_status))
			return (heredoc_cleanup(ctx->fd_pipe, NULL, 1));
		write(ctx->fd_pipe[1], line, ft_strlen(line));
		write(ctx->fd_pipe[1], "\n", 1);
		free(line);
	}
	free(line);
	return (0);
}
