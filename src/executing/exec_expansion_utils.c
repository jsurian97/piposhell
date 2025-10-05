/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_expansion_utils.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anpayot <anpayot@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/05 10:27:26 by anpayot           #+#    #+#             */
/*   Updated: 2025/10/05 10:27:33 by anpayot          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

size_t	exec_trim_empty_prefix(t_scmd *self)
{
	size_t	count;

	count = 0;
	while (self->argv[count] && self->argv[count][0] == '\0')
	{
		free(self->argv[count]);
		self->argv[count] = NULL;
		count++;
	}
	return (count);
}

void	exec_shift_trimmed_argv(t_scmd *self, size_t skipped)
{
	size_t	y;

	y = 0;
	while (self->argv[skipped + y])
	{
		self->argv[y] = self->argv[skipped + y];
		self->argv[skipped + y] = NULL;
		y++;
	}
	while (y < self->nbr_of_argv)
	{
		self->argv[y] = NULL;
		y++;
	}
}
