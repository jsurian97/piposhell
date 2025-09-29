/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_init.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anpayot <anpayot@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/15 11:15:13 by jsurian42         #+#    #+#             */
/*   Updated: 2025/09/29 22:21:25 by anpayot          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

void	exec_cleanup_fd(t_exec_data *data)
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
