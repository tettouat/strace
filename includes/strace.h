/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   strace.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tettouat <tettouat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/02/24 14:57:08 by tettouat          #+#    #+#             */
/*   Updated: 2017/03/23 14:42:10 by tettouat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STRACE_H
# define STRACE_H

#define PROGRAM_NAME "ft_strace"

#define SYSCALL_TRAP_MASK 0x80

enum e_trap
{
    syscall_,
    signal_,
    exit_,
};
typedef enum e_trap trap_t;

int strace(char **command);

#endif