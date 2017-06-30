/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tettouat <tettouat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/02/24 14:57:08 by tettouat          #+#    #+#             */
/*   Updated: 2017/02/24 15:48:46 by tettouat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#define _GNU_SOURCE /* Enable asprintf */
#include <unistd.h>

#include "tools.h"

static int should_skip_path_search(const char *program)
{
    int is_relative;
    int is_absolute;

    is_relative = (program[0] == '.');
    is_absolute = (program[0] == '/');

    return (is_relative || is_absolute);
}

static char *find_in_path(const char *program, char *path)
{
    char *prefix;
    char *concatenated, *normalized;

    prefix = strtok(path, ":");
    while (prefix) {
        if(asprintf(&concatenated, "%s/%s", prefix, program) == -1)
            printf("asprintf error");
        normalized = realpath(concatenated, NULL);
        if (!normalized)
            normalized = strdup(concatenated);
        free(concatenated);
        if (access(normalized, F_OK) == 0)
            return (normalized);
        else
            free(normalized);
        prefix = strtok(NULL, ":");
    }

    return (NULL);
}

char *resolve_program_path(const char *program)
{
    char *env_path;
    char *path;

    if (should_skip_path_search(program)) {
        if (access(program, F_OK) == 0)
            path = strdup(program);
        else
            path = NULL;
    }
    else {
        env_path = getenv("PATH");
        if (!env_path)
            path = strdup(program);
        else
            path = find_in_path(program, env_path);
    }

    return (path);
}
