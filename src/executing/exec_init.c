/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_init.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anpayot <anpayot@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/15 11:15:13 by jsurian42         #+#    #+#             */
/*   Updated: 2025/10/06 18:00:02 by jsurian42        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

void	exec_cleanup_all(t_scmd *self, t_exec_data *data)
{
	exec_cleanup_fd(self, data);
	ft_lstclear(&data->lst_simple_cmd, del_lst_scmd);
	ft_split_free(data->envp);
	rl_clear_history();
}

void	exec_cleanup_fd_saved(t_exec_data *data)
{
	if (data->fd.fd_saved_in != -1)
	{
		close(data->fd.fd_saved_in);
		data->fd.fd_saved_in = -1;
	}
	if (data->fd.fd_saved_out != -1)
	{
		close(data->fd.fd_saved_out);
		data->fd.fd_saved_out = -1;
	}
}

void	exec_cleanup_heredoc(t_scmd *self)
{
	size_t	i;

	i = 0;
	if (self == NULL)
		return ;
	while (i < self->nbr_of_red)
	{
		if (self->red[i].fd_heredoc != -1)
		{
			close(self->red[i].fd_heredoc);
			self->red[i].fd_heredoc = -1;
		}
		i++;
	}
}

void	exec_cleanup_fd(t_scmd *self, t_exec_data *data)
{
	exec_cleanup_heredoc(self);
	exec_cleanup_fd_saved(data);
	if (data->fd.fd_prev != -1)
	{
		close(data->fd.fd_prev);
		data->fd.fd_prev = -1;
	}
	if (data->fd.fd_pipe[0] != -1)
	{
		close(data->fd.fd_pipe[0]);
		data->fd.fd_pipe[0] = -1;
	}
	if (data->fd.fd_pipe[1] != -1)
	{
		close(data->fd.fd_pipe[1]);
		data->fd.fd_pipe[1] = -1;
	}
}

int	exec_init(t_exec_data *data, t_list *head, char **envp, int last_exitstatus)
{
	data->lst_simple_cmd = head;
	data->envp = envp;
	data->exit_status = last_exitstatus;
	data->fd.fd_prev = -1;
	data->fd.fd_pipe[0] = -1;
	data->fd.fd_pipe[1] = -1;
	data->fd.fd_saved_in = dup(STDIN_FILENO);
	if (data->fd.fd_saved_in == -1)
		return (1);
	data->fd.fd_saved_out = dup(STDOUT_FILENO);
	if (data->fd.fd_saved_out == -1)
	{
		close(data->fd.fd_saved_in);
		return (1);
	}
	data->is_interactive = 0;
	return (0);
}
