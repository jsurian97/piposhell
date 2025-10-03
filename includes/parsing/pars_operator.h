/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pars_operator.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anpayot <anpayot@student.42lausanne.ch>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/07 13:58:17 by jsurian42         #+#    #+#             */
/*   Updated: 2025/10/03 14:04:59 by jsurian42        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARS_OPERATOR_H
# define PARS_OPERATOR_H

# include "libft.h"

typedef enum e_type
{
	RED_APPEND,
	RED_HEREDOC,
	RED_OUTPUT,
	RED_INPUT,
	PIPE,
	ESPACE,
	WORD,
	INVALID
}	t_type;

typedef struct s_utils_op
{
	t_list	*list_head;
	int		state_node;
	size_t	i_start;
	size_t	i_continue;
	size_t	y;
}	t_utils_op;

t_list	*lst_str_split(char *str);

#endif
