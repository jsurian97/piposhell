/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_child.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anpayot <anpayot@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/15 18:01:38 by jsurian42         #+#    #+#             */
/*   Updated: 2025/10/05 23:06:29 by jsurian42        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"
#include <sys/stat.h>

static int	exec_is_directory(char *path)
{
	struct stat	st;

	if (!path)
		return (0);
	if (stat(path, &st) == -1)
		return (0);
	return (S_ISDIR(st.st_mode));
}

int	exec_child_fd_pipe(t_fd *fd)
{
	if (fd->fd_prev != -1)
	{
		if (dup2(fd->fd_prev, STDIN_FILENO) == -1)
			return (1);
		close(fd->fd_prev);
		if (fd->fd_prev == fd->fd_pipe[0])
			fd->fd_pipe[0] = -1;
		fd->fd_prev = -1;
	}
	if (fd->fd_pipe[1] != -1)
	{
		if (dup2(fd->fd_pipe[1], STDOUT_FILENO) == -1)
			return (1);
		close(fd->fd_pipe[1]);
		fd->fd_pipe[1] = -1;
	}
	if (fd->fd_pipe[0] != -1)
	{
		close(fd->fd_pipe[0]);
		fd->fd_pipe[0] = -1;
	}
	return (0);
}

void	exec_child_builtin_exit(t_scmd *self, t_exec_data *data)
{
	int	ret;

	ret = execute_builtin(self);
	if (ret == BUILTIN_EXIT_SIGNAL)
		ret = self->exit_status & 0xFF;
	exec_cleanup_fd(data);
	ft_lstclear(&data->lst_simple_cmd, del_lst_scmd);
	ft_split_free(data->envp);
	rl_clear_history();
	exit(ret);
}

void	exec_child_scmd(t_scmd *self, t_exec_data *data)
{
	if (is_builtin(self->argv[0]))
		exec_child_builtin_exit(self, data);
	if (exec_is_directory(self->command_path))
	{
		ft_putstr_fd("minishell: ", 2);
		write(2, self->argv[0], ft_strlen(self->argv[0]));
		ft_putendl_fd(": is a directory", 2);
		exit(126);
	}
	execve(self->command_path, self->argv, self->env);
	ft_putstr_fd("minishell: ", 2);
	write(2, self->argv[0], ft_strlen(self->argv[0]));
	ft_putstr_fd(": ", 2);
	perror("");
	if (errno == EACCES || errno == EISDIR)
		exit(126);
	else
		exit(127);
}

int	exec_child(t_scmd *self, t_exec_data *data)
{
	if (self->pid != 0)
		return (0);
	signals_set_exec_child();
	if (exec_child_fd_pipe(&data->fd))
		exit(1);
	if (exec_expansion(self))
		exit(1);
	if (exec_redirections(self))
		exit(1);
	self->is_interactive = data->is_interactive;
	if (!self->argv || !self->argv[0])
		exit(0);
	if (exec_child_set_path(self, data))
		exit(1);
	exec_child_scmd(self, data);
	return (0);
}
