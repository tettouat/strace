/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   peek.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tettouat <tettouat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/02/24 14:57:08 by tettouat          #+#    #+#             */
/*   Updated: 2017/02/24 15:04:07 by tettouat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PEEK_H
# define PEEK_H

#include <stdlib.h>

#include "syscall.h"

#define STRING_PEEK_MAX_SIZE 32

long peek_user(pid_t pid, long offset);
void *peek_value(pid_t pid, type value_type, long data);
void peek_args(pid_t pid, long syscall_id, syscall_arg *args);

#endif