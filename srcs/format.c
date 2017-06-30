/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   format.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tettouat <tettouat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/02/24 14:57:08 by tettouat          #+#    #+#             */
/*   Updated: 2017/02/24 14:59:08 by tettouat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "log.h"
#include "peek.h"
#include "tools.h"

void str_printf(char *str, int *written, const char *format, ...)
{
    va_list args;

    va_start(args, format);
    if (*written < MAX_LINE_SIZE) {
        *written += vsnprintf(
            str + *written,
            MAX_LINE_SIZE - *written,
            format,
            args
        );
    }
    va_end(args);
}

static void write_int(char *out, int *written, void *d)
{
        str_printf(out, written, "%d", (int)(long)d);
}

static void write_uint(char *out, int *written, void *u)
{
    str_printf(out, written, "%u", (unsigned)(long)u);
}

static void write_long(char *out, int *written, void *ld)
{
    str_printf(out, written, "%ld", (long)ld);
}

static void write_ulong(char *out, int *written, void *lu)
{
    str_printf(out, written, "%lu", (unsigned long)lu);
}

static void write_pointer(char *out, int *written, void *p)
{
    if(p == NULL)
        str_printf(out, written, "NULL");
    else
        str_printf(out, written, "%p", p);
}

static void write_string(char *out, int *written, void *p_str)
{
    int elide;
    char *str, *escaped;

    str = p_str;
    escaped = escape(str);
    elide = (
        str[STRING_PEEK_MAX_SIZE - 1] &&
        strlen(str) == STRING_PEEK_MAX_SIZE
    );

    str_printf(
        out,
        written,
        "\"%s\"%s",
        escaped,
        elide ? "..." : ""
    );

    free(escaped);
    free(str);
}

static void write_array(char *out, int *written, void *p_array)
{
    unsigned i;
    char **array;

    array = p_array;

    str_printf(out, written, "[");
    for (i = 0; array[i]; ++i);
    if (i <= 3) {
        if (*array) {
            write_string(out, written, *array);
            for (i = 1; array[i]; ++i) {
                str_printf(out, written, ", ");
                write_string(out, written, array[i]);
            }
        }
    }
    else
        str_printf(out, written, "/* %d vars */", i);
    str_printf(out, written, "]");
    free(array);
}

void write_arg(void *arg, type arg_type, char *str, int *written)
{
    typedef void (*arg_writer_t)(char *, int *, void *);
    static arg_writer_t writers[] = {
        &write_int,
        &write_uint,
        &write_long,
        &write_ulong,
        &write_pointer,
        &write_string,
        &write_array,
    };

    (*writers[arg_type])(str, written, arg);
}
