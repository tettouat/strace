/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tettouat <tettouat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/02/24 14:57:08 by tettouat          #+#    #+#             */
/*   Updated: 2017/02/24 14:59:18 by tettouat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <stdio.h>

#include "strace.h"

static void show_usage()
{
    fprintf(stderr, "usage: %s PROG [ARGS]\n", PROGRAM_NAME);
}

int main(int argc, char **argv)
{
    int exit_code;

    if (argc < 2) {
        show_usage();
        return (EXIT_FAILURE);
    }
    else {
        exit_code = strace(argv + 1);
        return (exit_code);
    }
}
