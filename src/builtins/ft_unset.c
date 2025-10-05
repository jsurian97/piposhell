/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_unset.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anpayot <anpayot@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/14 21:15:00 by anpayot           #+#    #+#             */
/*   Updated: 2025/10/05 10:19:11 by anpayot          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/main.h"
#include <stdio.h>
#include <unistd.h>

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
		if (ft_strlen(scmd->argv[i]) == 0)
		{
			i++;
			continue ;
		}
		if (is_valid_identifier(scmd->argv[i]))
			scmd->env = remove_env_var(scmd->env, scmd->argv[i]);
		i++;
	}
	return (ret);
}
