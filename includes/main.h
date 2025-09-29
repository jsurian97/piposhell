/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anpayot <anpayot@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/03 14:45:13 by jsurian42         #+#    #+#             */
/*   Updated: 2025/09/29 14:57:38 by jsurian42        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MAIN_H
# define MAIN_H

# include <unistd.h>
# include <string.h>
# include <stdlib.h>
# include <stdio.h>
# include <errno.h>
# include <sys/wait.h>
# include <sys/stat.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <fcntl.h>

# include "libft.h"

# include "parsing.h"
# include "executing.h"
# include "expansion.h"
# include "builtins.h"
# include "redirections.h"
# include "signals.h"

typedef struct s_red
{
	int		type;
	char	*word;
	int		fd_heredoc;
	int		quoted_heredoc;
}	t_red;

typedef struct s_scmd
{
	char	*command_path;
	char	**env;
	char	**argv;
	size_t	nbr_of_argv;
	size_t	nbr_of_red;
	int		pid;
	int		exit_status;
	int		is_interactive;
	t_red	*red;
}	t_scmd;

typedef struct s_shell
{
	t_list	*head;
	char	**envp;
	int		last_exit_status;
	int		is_interactive;
	int		stdout_isatty;
}	t_shell;

int	parsing(t_shell *shell);
int	executing(t_shell *shell);

#endif
