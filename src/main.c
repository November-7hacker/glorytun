#include "common.h"

#include <stdio.h>

#include "../argz/argz.h"

volatile sig_atomic_t gt_alarm;
volatile sig_atomic_t gt_reload;
volatile sig_atomic_t gt_quit;

static void
gt_sa_handler(int sig)
{
    switch (sig) {
    case SIGALRM:
        gt_alarm = 1;
        return;
    case SIGHUP:
        gt_reload = 1; /* FALLTHRU */
    default:
        gt_quit = 1;
    }
}

static void
gt_set_signal(void)
{
    struct sigaction sa = {
        .sa_flags = 0,
    };

    sigemptyset(&sa.sa_mask);

    sa.sa_handler = gt_sa_handler;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGHUP, &sa, NULL);
    sigaction(SIGALRM, &sa, NULL);

    sa.sa_handler = SIG_IGN;
    sigaction(SIGPIPE, &sa, NULL);
    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);
}

int
main(int argc, char **argv)
{
    gt_set_signal();

    struct {
        char *name;
        char *help;
        int (*call)(int, char **);
    } cmd[] = {
        {"list", "list all tunnels", gt_list},
        {"show", "show tunnel information", gt_show},
        {"bench", "start a crypto bench", gt_bench},
        {"bind", "start a new tunnel", gt_bind},
        {"set", "change tunnel properties", gt_set},
        {"keygen", "generate a new secret key", gt_keygen},
        {"path", "manage paths", gt_path},
        {"version", "show version", gt_version},
        {NULL}};

    if (argv[1]) {
        for (int k = 0; cmd[k].name; k++) {
            if (!strcmp(cmd[k].name, argv[1]))
                return cmd[k].call(argc - 1, argv + 1);
        }
    }

    printf("available commands:\n\n");

    int len = 0;

    for (int k = 0; cmd[k].name; k++)
        len = MAX(len, (int)strlen(cmd[k].name));

    for (int k = 0; cmd[k].name; k++)
        printf("    %-*s    %s\n", len, cmd[k].name, cmd[k].help);

    printf("\n");

    return 1;
}
