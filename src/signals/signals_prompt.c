/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals_prompt.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anpayot <anpayot@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/28 01:20:00 by anpayot           #+#    #+#             */
/*   Updated: 2025/09/29 21:09:20 by anpayot          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

static void	sigint_prompt(int sig)
{
	g_signal_received = sig;
	write(STDOUT_FILENO, "\n", 1);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
}

void	signals_set_interactive(void)
{
	signal(SIGINT, sigint_prompt);
	signal(SIGQUIT, SIG_IGN);
}
