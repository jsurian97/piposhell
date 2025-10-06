/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_cd.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anpayot <anpayot@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/14 21:15:00 by anpayot           #+#    #+#             */
/*   Updated: 2025/10/06 18:13:47 by jsurian42        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

// Main cd builtin: no arg -> HOME, '-' -> OLDPWD, else specified path
int	ft_cd(t_scmd *scmd)
{
	char	old_pwd[1024];

	if (!scmd || !scmd->argv)
		return (1);
	if (!getcwd(old_pwd, sizeof(old_pwd)))
	{
		ft_putstr_fd("minishell: cd: error retrieving", 2);
		ft_putstr_fd(" current directory: getcwd: ", 2);
		ft_putendl_fd(strerror(errno), 2);
		return (1);
	}
	if (!scmd->argv[1])
		return (cd_to_home(scmd, old_pwd));
	else
		return (cd_to_path(scmd, scmd->argv[1], old_pwd));
}
