/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anpayot <anpayot@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/01 15:46:33 by jsurian42         #+#    #+#             */
/*   Updated: 2025/10/03 10:37:24 by jsurian42        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

t_shell	*g_shell = NULL;

void	shell_cleanup(t_shell *shell)
{
	static int	history_cleared = 0;

	if (!shell)
		return ;
	if (shell->head)
	{
		ft_lstclear(&shell->head, del_lst_scmd);
		shell->head = NULL;
	}
	if (shell->envp)
	{
		ft_split_free(shell->envp);
		shell->envp = NULL;
	}
	if (!history_cleared)
	{
		rl_clear_history();
		history_cleared = 1;
	}
	if (g_shell == shell)
		g_shell = NULL;
}

int	shell_init(t_shell *shell, char **envp)
{
	int		shell_lvl;
	char	*shlvl_str;
	char	**temp;

	shell->head = NULL;
	shell->envp = ft_strdupdup((const char **)envp);
	if (shell->envp == NULL)
		return (1);
	shell->last_exit_status = 0;
	shell->is_interactive = isatty(STDIN_FILENO);
	shell->stdout_isatty = isatty(STDOUT_FILENO);
	shell_lvl = ft_atoi(ft_get_env_value(shell->envp, "SHLVL"));
	shell_lvl++;
	shlvl_str = ft_itoa(shell_lvl);
	temp = update_env_var(shell->envp, "SHLVL", shlvl_str);
	free(shlvl_str);
	if (!temp)
	{
		ft_split_free(shell->envp);
		return (1);
	}
	shell->envp = temp;
	return (0);
}

int	main(int argc, char **argv, char **envp)
{
	t_shell	shell;

	if (argc != 1)
		return (0);
	(void)argv;
	if (shell_init(&shell, envp))
		return (1);
	g_shell = &shell;
	signals_set_interactive();
	while (1)
	{
		if (parsing(&shell))
			continue ;
		if (executing(&shell))
			continue ;
	}
	shell_cleanup(&shell);
	return (shell.last_exit_status);
}
