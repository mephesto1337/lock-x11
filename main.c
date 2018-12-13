#include <X11/extensions/scrnsaver.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "config.h"
#include "desktop-utils/desktop-utils.h"
#include "desktop-utils/log.h"
#include "desktop-utils/macros.h"

bool parse_time(const char *str, unsigned long *msecs);
bool hash_fullscreen_window(Display *dpy);
bool lock_screen(void);
void sighandler(int signum);
char *const *environ = NULL;

int main(int argc, char *const argv[], char *const envp[]) {
    Display *dpy = NULL;
    XScreenSaverInfo *info = NULL;
    unsigned long max_inactivity = 0;
    struct sigaction sa;
    int ret = EXIT_SUCCESS;

    environ = envp;
    if (argc == 2) {
        CHK_FALSE(parse_time(argv[1], &max_inactivity));
    } else {
        CHK_FALSE(parse_time(DEFAULT_INACTIVITY, &max_inactivity));
    }
    debug("max_inactivity = %lu msecs", max_inactivity);

    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = sighandler;
    sigemptyset(&sa.sa_mask);
    sigaddset(&sa.sa_mask, SIGUSR1);
    sigaddset(&sa.sa_mask, SIGUSR2);

    CHK_FALSE(daemonize_uniq(LOCK_FILENAME));
    CHK_NULL(dpy = XOpenDisplay(NULL));
    CHK_NULL(info = XScreenSaverAllocInfo());
    CHK_NEG(sigaction(SIGUSR1, &sa, NULL));

    while (true) {
        CHK_FALSE(XScreenSaverQueryInfo(dpy, DefaultRootWindow(dpy), info));
        debug("info->idle = %lu", info->idle);
        if (info->idle >= max_inactivity) {
            CHK_FALSE(lock_screen());
        } else {
            debug("info->idle = %lu", info->idle);
            CHK_NEG(usleep(1000000UL));
        }
    }
    ret = EXIT_SUCCESS;

fail:
    SAFE_FREE(XCloseDisplay, dpy);
    SAFE_FREE(XFree, info);
    return ret;
}

bool parse_time(const char *str, unsigned long *msecs) {
    char unit = 'm';
    unsigned long _msecs;
    int n;

    n = sscanf(str, "%lu%c", &_msecs, &unit);
    switch (n) {
        case 0:
            return false;
        case 1:
            unit = 'm';
        default:
            break;
    }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wimplicit-fallthrough"
    switch (unit) {
        case 'H':
        case 'h':
            _msecs *= 60;
        case 'M':
        case 'm':
            _msecs *= 60;
        case 'S':
        case 's':
            _msecs *= 1000;
            break;
        default:
            return false;
    }
#pragma GCC diagnostic pop

    *msecs = _msecs;
    return true;
}

bool hash_fullscreen_window(Display *dpy) {
    Window focus;
    int revert;

    CHK_NEG(XGetInputFocus(dpy, &focus, &revert));
    return true;

fail:
    return false;
}

bool lock_screen(void) { return spawn(lockcmd, environ); }

void sighandler(int signum) {
    if (signum == SIGINT) {
        CHK_FALSE(lock_screen());
    }
fail:
    return;
}
