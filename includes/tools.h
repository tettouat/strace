/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tools.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tettouat <tettouat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/02/24 14:57:08 by tettouat          #+#    #+#             */
/*   Updated: 2017/02/24 15:48:28 by tettouat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TOOLS_H
# define TOOLS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int null_bytes_in_word(long word);
int get_exit_code(int status);
char *escape(const char *str);
void fatal(const char *description);
char *resolve_program_path(const char *program);

#endif