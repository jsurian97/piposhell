/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals_heredoc.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anpayot <anpayot@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 07:41:00 by anpayot           #+#    #+#             */
/*   Updated: 2025/10/08 22:08:35 by anpayot          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <signal.h>
#include <sys/ioctl.h>
#include "main.h"

#ifdef rl_replace_line

static void	clear_heredoc_line(void)
{
	rl_replace_line("", 0);
}

#else

static void	clear_heredoc_line(void)
{
}

#endif

static void	sigint_heredoc(int sig)
{
	char	newline;

	g_signal_received = sig;
	rl_on_new_line();
	clear_heredoc_line();
	newline = '\n';
	if (isatty(STDIN_FILENO))
		ioctl(STDIN_FILENO, TIOCSTI, &newline);
}

void	signals_set_heredoc(void)
{
	struct sigaction	sa;

	sa.sa_handler = sigint_heredoc;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGINT, &sa, NULL);
	sa.sa_handler = SIG_IGN;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGQUIT, &sa, NULL);
}
