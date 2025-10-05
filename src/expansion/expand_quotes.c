/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_quotes.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsurian42 <jsurian@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/22 11:20:35 by jsurian42         #+#    #+#             */
/*   Updated: 2025/09/25 22:54:22 by jsurian42        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "main.h"

int	expand_quotes_check(t_token	*token)
{
	int		i;
	char	quote;

	i = 0;
	quote = 0;
	while (token->str[i])
	{
		if (token->str[i] == '\'' || token->str[i] == '"')
		{
			if (quote == 0)
				quote = token->str[i];
			else if (token->str[i] == quote)
				quote = 0;
		}
		i++;
	}
	if (quote != 0)
	{
		ft_putstr_fd("minishell: syntax error: unclosed quote\n", 2);
		return (1);
	}
	token->type = WORD;
	return (0);
}

//ne pas remove les quote a linterieur de quote
static int	expand_toggle_quote(char c, char *quote)
{
	if (*quote == 0 && (c == '\'' || c == '"'))
	{
		*quote = c;
		return (1);
	}
	if (*quote == c)
	{
		*quote = 0;
		return (1);
	}
	return (0);
}

static char	*expand_alloc_clean(char *str)
{
	char	*clean;

	clean = ft_calloc(ft_strlen(str) + 1, sizeof(char));
	if (!clean)
	{
		free(str);
		return (NULL);
	}
	return (clean);
}

static void	expand_process_char(t_quote_clean *ctx)
{
	if (expand_toggle_quote(ctx->str[ctx->i], &ctx->quote))
	{
		ctx->i++;
		return ;
	}
	if (ctx->str[ctx->i] == '\\' && ctx->quote == '"'
		&& (ctx->str[ctx->i + 1] == '"' || ctx->str[ctx->i + 1] == '\\'))
	{
		ctx->clean[ctx->j++] = ctx->str[ctx->i + 1];
		ctx->i += 2;
		return ;
	}
	ctx->clean[ctx->j++] = ctx->str[ctx->i++];
}

char	*expand_remove_quote(char *str)
{
	t_quote_clean	ctx;

	if (!str)
		return (NULL);
	ctx.clean = expand_alloc_clean(str);
	if (!ctx.clean)
		return (NULL);
	ctx.str = str;
	ctx.i = 0;
	ctx.j = 0;
	ctx.quote = 0;
	while (ctx.str[ctx.i])
		expand_process_char(&ctx);
	ctx.clean[ctx.j] = '\0';
	free(str);
	return (ctx.clean);
}
