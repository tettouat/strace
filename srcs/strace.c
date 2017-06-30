/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   strace.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tettouat <tettouat@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/02/24 14:57:08 by tettouat          #+#    #+#             */
/*   Updated: 2017/03/23 19:05:12 by tettouat         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdio.h>

#include <sys/ptrace.h>
#include <sys/reg.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "strace.h"
#include "syscall.h"
#include "peek.h"
#include "log.h"
#include "tools.h"

extern char **environ;
const char *last_syscall;

static trap_t next_trap(pid_t pid, int *status)
{
    int sig_num;
    sigset_t ss;
    sigset_t ss_blocked;

    sigemptyset(&ss);
    sigemptyset(&ss_blocked);
    sigaddset(&ss_blocked, SIGINT);
    sigaddset(&ss_blocked, SIGHUP);
    sigaddset(&ss_blocked, SIGTERM);
    sigaddset(&ss_blocked, SIGQUIT);
    sigaddset(&ss_blocked, SIGPIPE);

    while(1) {
        (void)ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
        sigprocmask(SIG_SETMASK, &ss, NULL);
        if (waitpid(pid, status, 0) == -1)
            fatal("waitpid failed");
        sigprocmask(SIG_BLOCK, &ss_blocked, NULL);

        if (WIFSTOPPED(*status) || WIFCONTINUED(*status)) {
            sig_num = WSTOPSIG(*status);
            if (sig_num & SYSCALL_TRAP_MASK)
                return (syscall_);
            else
                return (signal_);
        }
        if (WIFEXITED(status) || WIFSIGNALED(status))
            return (exit_);
    }
}

static int signal_trap(pid_t pid, int *status)
{
    siginfo_t siginfo;
    sigset_t ss;
    sigset_t ss_blocked;

    sigemptyset(&ss);
    sigemptyset(&ss_blocked);
    sigaddset(&ss_blocked, SIGINT);
    sigaddset(&ss_blocked, SIGHUP);
    sigaddset(&ss_blocked, SIGTERM);
    sigaddset(&ss_blocked, SIGQUIT);
    sigaddset(&ss_blocked, SIGPIPE);

    if (ptrace(PTRACE_GETSIGINFO, pid, NULL, &siginfo))
        fatal("PTRACE_GETSIGINFO failed");

    if(siginfo.si_signo == SIGCHLD && !(siginfo.si_code == CLD_EXITED))
        next_trap(pid, status);

    if(siginfo.si_signo == SIGCHLD && siginfo.si_code == CLD_EXITED)
    {
        output_signal(&siginfo);
        next_trap(pid, status);
    }

    if (!(siginfo.si_signo == SIGTRAP && siginfo.si_pid == pid) && !(siginfo.si_signo == SIGCHLD))
        output_signal(&siginfo);

    if (siginfo.si_signo != SIGCONT) {
        if (ptrace(PTRACE_CONT, pid, NULL, siginfo.si_signo))
            fatal("PTRACE_CONT failed");
    }

    if (siginfo.si_signo == SIGSTOP) {
        sigprocmask(SIG_SETMASK, &ss, NULL);
        if (waitpid(pid, status, WUNTRACED) == -1)
            fatal("waitpid failed");
        sigprocmask(SIG_BLOCK, &ss_blocked, NULL);

        if (ptrace(PTRACE_LISTEN, pid, NULL, NULL))
            fatal("PTRACE_LISTEN failed");

        sigprocmask(SIG_SETMASK, &ss, NULL);
        if (waitpid(pid, status, WCONTINUED) == -1)
            fatal("waitpid failed");
        sigprocmask(SIG_BLOCK, &ss_blocked, NULL);
    }
    return (WIFEXITED(*status) || WIFSIGNALED(*status));
}

static int syscall_trap(pid_t pid, int *status)
{
    long syscall_id, ret_val;
    syscall_arg args[MAX_ARGS];
    trap_t trap;
    const syscall_info *info;
    void *value;

    syscall_id = peek_user(pid, ORIG_RAX);
    if(syscall_id != 219)
    {
        info = get_syscall_info(syscall_id);
        last_syscall = info->name;
    }
    peek_args(pid, syscall_id, args);
    output_invocation(syscall_id, args);


    trap = next_trap(pid, status);
    if (trap == exit_) {
        output_unknown_return_value();
        return (1);
    }

    else if (trap == syscall_) {
        ret_val = peek_user(pid, RAX);
        info = get_syscall_info(syscall_id);
        value = (info ? peek_value(pid, info->return_type, ret_val) : NULL);
        output_return_value(ret_val, syscall_id, value);
    }
    else if (trap == signal_)
        return (signal_trap(pid, status));

    return (0);
}

static int trace_process(pid_t pid)
{
    int last_status;
    int exit_code;
    trap_t trap;


    if (ptrace(PTRACE_SEIZE, pid, NULL, PTRACE_O_TRACESYSGOOD))
        fatal("PTRACE_SEIZE failed");

    while(1) {
        trap = next_trap(pid, &last_status);
        if (trap == syscall_) {
            if (syscall_trap(pid, &last_status))
                break ;
        }
        else if (trap == signal_) {
            if (signal_trap(pid, &last_status))
                break ;
        }
        else if (trap == exit_)
        {
            break ;
        }

    }

    exit_code = get_exit_code(last_status);
    output_exit(last_status, exit_code);
    if (WIFSIGNALED(last_status))
        kill(getpid(), WTERMSIG(last_status));
    return (exit_code);
}

int strace(char **command)
{
    char *program;
    pid_t pid;
    int exit_code;
    sigset_t ss;
    sigset_t ss_blocked;

    exit_code = 0;
    program = resolve_program_path(command[0]);
    if (!program)
        fatal("stat failed");

    pid = fork();
    if (pid == -1)
        fatal("fork failed");
    else if (pid == 0) {
        kill(getpid(), SIGSTOP);
        execve(program, command, environ);
        fatal("exec failed");
    }
    else {
        sigemptyset(&ss);
        sigemptyset(&ss_blocked);
        sigaddset(&ss_blocked, SIGINT);
        sigaddset(&ss_blocked, SIGHUP);
        sigaddset(&ss_blocked, SIGTERM);
        sigaddset(&ss_blocked, SIGQUIT);
        sigaddset(&ss_blocked, SIGPIPE);
        sigprocmask(SIG_SETMASK, &ss, NULL);
        if (waitpid(pid, NULL, WUNTRACED) == -1)
            fatal("waitpid failed");
        sigprocmask(SIG_BLOCK, &ss_blocked, NULL);
        exit_code = trace_process(pid);
    }

    return (exit_code);
}
