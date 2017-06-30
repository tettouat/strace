/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tools.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tettouat <tettouat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/02/24 14:57:08 by tettouat          #+#    #+#             */
/*   Updated: 2017/03/20 17:23:23 by tettouat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <limits.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/wait.h>

#include "strace.h"

int null_bytes_in_word(long word)
{
    static long low_magic = 0x0101010101010101L;
    static long high_magic = 0x8080808080808080L;

    return (((word - low_magic) & ~word & high_magic) != 0);
}

int get_exit_code(int status)
{
    if (WIFEXITED(status))
        return (WEXITSTATUS(status));
    else if (WIFSIGNALED(status))
        return (CHAR_MAX + 1 + WTERMSIG(status));
    return (EXIT_FAILURE);
}

char *escape(const char *str)
{
    static const char escape_chars[] = {
        '\0',  '\a',  '\b',  '\t',  '\n',  '\v',  '\f',  '\r',  '\\'
    };
    static const char* escape_strs[] = {
        "\\0", "\\a", "\\b", "\\t", "\\n", "\\v", "\\f", "\\r", "\\\\"
    };
    char *escaped;
    size_t size;
    int written;
    unsigned i, found;

    size = strlen(str) * 4;
    escaped = (char *)malloc(sizeof(char) * (size + 1));
    written = 0;
    for (; *str; ++str) {
        if (isprint(*str))
            written += sprintf(escaped + written, "%c", *str);
        else {
            found = 0;
            for (i = 0; i < sizeof(escape_chars) / sizeof(char); ++i) {
                if (*str == escape_chars[i]) {
                    written += sprintf(escaped + written, "%s", escape_strs[i]);
                    found = 1;
                    break ;
                }
            }
            if (!found)
                written += sprintf(escaped + written, "\\%o", (unsigned char)*str);
        }
    }
    escaped[written] = '\0';

    return (escaped);
}

void fatal(const char *description)
{
    fprintf(
        stderr,
        "%s: %s: %s\n",
        PROGRAM_NAME,
        description,
        strerror(errno)
    );
    exit(EXIT_FAILURE);
}