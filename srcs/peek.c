/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   peek.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tettouat <tettouat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/02/24 14:57:08 by tettouat          #+#    #+#             */
/*   Updated: 2017/02/24 15:22:16 by tettouat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <sys/ptrace.h>
#include <sys/reg.h>

#include "syscall.h"
#include "peek.h"
#include "tools.h"

long peek_user(pid_t pid, long offset)
{
    long data;

    data = ptrace(PTRACE_PEEKUSER, pid, sizeof(long) * offset, NULL);
    if (data == -1 && errno)
        fatal("PTRACE_PEEKUSER failed");

    return (data);
}

static long peek_data(pid_t pid, long offset)
{
    long data;

    data = ptrace(PTRACE_PEEKDATA, pid, offset, NULL);
    if (data == -1 && errno)
        fatal("PTRACE_PEEKDATA failed");

    return (data);
}

static char *peek_string(pid_t pid, long addr)
{
    long data;
    char *string;
    unsigned i;
    int next;

    string = (char *)malloc(sizeof(char) * (STRING_PEEK_MAX_SIZE + 1));
    bzero(string, STRING_PEEK_MAX_SIZE + 1);
    i = 0;
    do {
        data = peek_data(pid, addr + i);
        memcpy(string + i, &data, sizeof(long));
        i += sizeof(long);
        next = i < STRING_PEEK_MAX_SIZE;
        next &= !null_bytes_in_word(data);
    } while (next);

    return (string);
}

static char **peek_array(pid_t pid, long addr)
{
    long data;
    char **array;
    unsigned i;
    int next;

    array = (char **)malloc(sizeof(char *) * 256);
    i = 0;
    do {
        data = peek_data(pid, addr + i * sizeof(long));
        array[i] = data ? peek_string(pid, data) : NULL;
        ++i;
        next = (data != 0);
        next &= (i < 256);
    } while (next);

    return (array);
}

void *peek_value(pid_t pid, type value_type, long data)
{
    void *value;

    switch (value_type) {
        case int_:
        case uint_:
        case long_:
        case ulong_:
        case pointer_:
            value = (void *)data;
            break ;
        case string_:
            value = (void *)peek_string(pid, data);
            break ;
        case array_:
            value = (void *)peek_array(pid, data);
            break ;
        default:
            value = NULL;
            break ;
    }

    return (value);
}

void peek_args(pid_t pid, long syscall_id, syscall_arg *args)
{
    static long REGS[MAX_ARGS] = { RDI, RSI, RDX, RCX, R8, R9 };
    const syscall_info *info;
    unsigned i;
    long data;

    info = get_syscall_info(syscall_id);
    if (info) {
        for (i = 0; i < info->arg_count; ++i) {
            data = peek_user(pid, REGS[i]);
            args[i] = peek_value(pid, info->args_type[i], data);
        }
    }
}
