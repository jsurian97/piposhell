/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_unset.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anpayot <anpayot@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/14 21:15:00 by anpayot           #+#    #+#             */
/*   Updated: 2025/10/05 17:00:34 by anpayot          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/main.h"
#include <stdio.h>
#include <unistd.h>

void	ft_unset_error_msg(char *str)
{
	ft_putstr_fd("minishell: unset: `", 2);
	ft_putstr_fd(str, 2);
	ft_putstr_fd("': not a valid identifier\n", 2);
}

// unset builtin: remove each valid identifier from the environment
int	ft_unset(t_scmd *scmd)
{
	int	i;
	int	ret;

	if (!scmd || !scmd->argv)
		return (1);
	if (!scmd->argv[1])
		return (0);
	ret = 0;
	i = 1;
	while (scmd->argv[i])
	{
		if (ft_strlen(scmd->argv[i]) != 0)
		{
			if (!is_valid_identifier(scmd->argv[i]))
			{
				ft_unset_error_msg(scmd->argv[i]);
				ret = 1;
			}
			else
				scmd->env = remove_env_var(scmd->env, scmd->argv[i]);
		}
		i++;
	}
	return (ret);
}
