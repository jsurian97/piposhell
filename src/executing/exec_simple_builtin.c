/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_simple_builtin.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anpayot <anpayot@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 14:50:03 by jsurian42         #+#    #+#             */
/*   Updated: 2025/10/06 18:36:12 by jsurian42        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

static int	exec_restore_builtin(t_scmd *self, t_exec_data *data,
		int exit_status, int ret)
{
	data->exit_status = exit_status;
	dup2(data->fd.fd_saved_in, STDIN_FILENO);
	dup2(data->fd.fd_saved_out, STDOUT_FILENO);
	exec_cleanup_fd(self, data);
	return (ret);
}

int	exec_simple_builtin(t_scmd *self, t_exec_data *data)
{
	self->exit_status = data->exit_status;
	self->is_interactive = data->is_interactive;
	if (exec_expansion(self))
		return (exec_restore_builtin(self, data, 1, 1));
	if (!self->argv || !self->argv[0])
		return (exec_restore_builtin(self, data, 0, 0));
	if (exec_redirections(self))
		return (exec_restore_builtin(self, data, 1, 1));
	data->exit_status = execute_builtin(self);
	if (data->exit_status == BUILTIN_EXIT_SIGNAL)
	{
		data->exit_status = self->exit_status & 0xFF;
		exec_cleanup_all(self, data);
		exit(data->exit_status);
	}
	data->envp = self->env;
	return (exec_restore_builtin(self, data, data->exit_status, 0));
}
