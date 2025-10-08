/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anpayot <anpayot@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/01 15:46:33 by jsurian42         #+#    #+#             */
/*   Updated: 2025/10/08 13:35:20 by jsurian          ###   ####lausanne.ch   */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

int	handle_envp(t_shell *shell)
{
	char	*cwd;
	char	*pwd_entry;
	char	*shlvl_entry;

	cwd = getcwd(NULL, 0);
	if (cwd == NULL)
		return (1);
	pwd_entry = ft_strjoin("PWD=", cwd);
	free(cwd);
	if (pwd_entry == NULL)
		return (1);
	shlvl_entry = ft_strdup("SHLVL=1");
	if (shlvl_entry == NULL)
		return (free(pwd_entry), 1);
	shell->envp = malloc(sizeof(char *) * 3);
	if (shell->envp == NULL)
		return (free(pwd_entry), free(shlvl_entry), 1);
	shell->envp[0] = pwd_entry;
	shell->envp[1] = shlvl_entry;
	shell->envp[2] = NULL;
	return (0);
}

int	shell_init(t_shell *shell, char **envp)
{
	t_init_view	v;

	shell->head = NULL;
	if (envp && envp[0])
	{
		shell->envp = ft_strdupdup((const char **)envp);
		if (shell->envp == NULL)
			return (1);
	}
	else
	{
		if (handle_envp(shell))
			return (1);
	}
	shell->last_exit_status = 0;
	shell->is_interactive = isatty(STDIN_FILENO);
	shell->stdout_isatty = isatty(STDOUT_FILENO);
	v.shell_lvl = ft_atoi(ft_get_env_value(shell->envp, "SHLVL"));
	v.shell_lvl++;
	v.shlvl_str = ft_itoa(v.shell_lvl);
	v.temp = update_env_var(shell->envp, "SHLVL", v.shlvl_str);
	free(v.shlvl_str);
	if (!v.temp)
		return (ft_split_free(shell->envp), 1);
	return (shell->envp = v.temp, 0);
}

void	shell_finish(t_shell *shell)
{
	if (shell->head)
		ft_lstclear(&shell->head, del_lst_scmd);
	if (shell->envp)
		ft_split_free(shell->envp);
}

int	main(int argc, char **argv, char **envp)
{
	t_shell	shell;

	if (argc != 1)
		return (0);
	(void)argv;
	if (shell_init(&shell, envp))
		return (1);
	signals_set_interactive();
	while (1)
	{
		if (parsing(&shell))
			continue ;
		if (executing(&shell))
			continue ;
	}
	shell_finish(&shell);
	rl_clear_history();
	return (shell.last_exit_status);
}
