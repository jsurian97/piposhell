/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anpayot <anpayot@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 16:30:58 by jsurian42         #+#    #+#             */
/*   Updated: 2025/10/03 14:08:27 by jsurian42        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REDIRECTIONS_H
# define REDIRECTIONS_H

typedef struct s_red			t_red;
typedef struct s_exec_data		t_exec_data;
typedef struct s_heredoc_ctx	t_heredoc_ctx;

struct s_heredoc_ctx
{
	int		*fd_pipe;
	char	**envp;
	size_t	delim_len;
	int		exit_status;
};

int		exec_redirections(t_scmd *self);
int		exec_heredoc(t_exec_data *data);
void	exec_cleanup_heredocs(t_exec_data *data);

//heredoc utils
int		heredoc_cleanup(int fd_pipe[2], char *line, int result);
int		heredoc_expand_line(t_red *red, char **line,
			char **envp, int exit_status);
void	heredoc_write_and_free(int write_fd, char *line);
void	exec_cleanup_heredocs(t_exec_data *data);
int		heredoc_collect_lines(t_red *red, t_heredoc_ctx *ctx);

#endif
