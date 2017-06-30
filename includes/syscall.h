/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   syscall.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tettouat <tettouat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/02/24 14:57:08 by tettouat          #+#    #+#             */
/*   Updated: 2017/03/23 17:31:07 by tettouat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SYSCALL_H
# define SYSCALL_H

#define MAX_ARGS 6
#define SYSCALLS_INFO_COUNT 314

extern const char *last_syscall;

enum e_type
{
    int_,
    uint_,
    long_,
    ulong_,
    pointer_,
    string_,
    array_,
};
typedef enum e_type type;

struct s_syscall_info
{
    const char *name;
    type return_type;
    unsigned arg_count;
    type args_type[MAX_ARGS];
};
typedef struct s_syscall_info syscall_info;
typedef void * syscall_arg;

const syscall_info SYSCALLS[SYSCALLS_INFO_COUNT];

const syscall_info *get_syscall_info(long syscall_id);

#endif