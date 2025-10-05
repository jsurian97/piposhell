/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_child_set_path.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anpayot <anpayot@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/17 17:03:00 by jsurian42         #+#    #+#             */
/*   Updated: 2025/10/05 20:59:35 by jsurian42        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

char	*ft_get_valid_path(t_scmd *self, char **paths_tab)
{
	int		i;
	char	*temp_path;
	char	*test_path;

	i = 0;
	while (paths_tab[i])
	{
		temp_path = ft_strjoin(paths_tab[i], "/");
		if (temp_path == NULL)
			return (NULL);
		test_path = ft_strjoin(temp_path, self->argv[0]);
		free(temp_path);
		if (test_path == NULL)
			return (NULL);
		if (access(test_path, X_OK) == 0)
			return (test_path);
		free(test_path);
		i++;
	}
	return (NULL);
}

void	exec_path_error(t_scmd *self, t_exec_data *data)
{
	ft_putstr_fd("minishell: ", 2);
	write(2, self->argv[0], ft_strlen(self->argv[0]));
	ft_putstr_fd(": command not found\n", 2);
	exec_cleanup_fd(data);
	ft_lstclear(&data->lst_simple_cmd, del_lst_scmd);
	ft_split_free(data->envp);
	rl_clear_history();
	exit(127);
}

//retour d'erreur si /aout et ne trouve pas lacces??
int	exec_child_set_path(t_scmd *self, t_exec_data *data)
{
	char	*path_value;
	char	**paths_tab;

	if (is_builtin(self->argv[0]))
		return (0);
	if (ft_strchr(self->argv[0], '/'))
	{
		self->command_path = ft_strdup(self->argv[0]);
		if (self->command_path == NULL)
			return (1);
		return (0);
	}
	path_value = ft_get_env_value(data->envp, "PATH");
	if (path_value != NULL)
	{
		paths_tab = ft_split(path_value, ':');
		if (paths_tab == NULL)
			return (1);
		self->command_path = ft_get_valid_path(self, paths_tab);
		ft_split_free(paths_tab);
	}
	if (self->command_path == NULL)
		exec_path_error(self, data);
	return (0);
}
