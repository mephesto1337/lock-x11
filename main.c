#include <X11/extensions/scrnsaver.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "config.h"
#include "desktop-utils/log.h"
#include "desktop-utils/desktop-utils.h"
#include "desktop-utils/macros.h"

#define DEFAULT_INACTIVITY      "10m"

enum laptop_state_e {
    OPEN,
    CLOSED,
};

bool parse_time(const char *str, unsigned long *msecs);

int main(int argc, char *const argv[], char *const envp[]) {
    Display *dpy = NULL;
    XScreenSaverInfo *info = NULL;
    unsigned long max_inactivity = 0;;
    int ret = EXIT_SUCCESS;

    if ( argc == 2 ) {
        CHK_FALSE(parse_time(argv[1], &max_inactivity));
    } else {
        CHK_FALSE(parse_time(DEFAULT_INACTIVITY, &max_inactivity));
    }
    debug("max_inactivity = %lu msecs", max_inactivity);

    CHK_FALSE(daemonize_uniq(LOCK_FILENAME));
    CHK_NULL(dpy = XOpenDisplay(NULL));
    CHK_NULL(info = XScreenSaverAllocInfo());

    while ( true ) {
        CHK_FALSE(XScreenSaverQueryInfo(dpy, DefaultRootWindow(dpy), info));
        debug("info->idle = %lu", info->idle);
        if ( info->idle >= max_inactivity ) {
            CHK_NEG(spawn(sleepcmd, envp));
            CHK_NEG(spawn(lockcmd, envp));
        } else {
            debug("info->idle = %lu, state = %s", info->idle, state == CLOSED ? "closed" : "open" );
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
    switch ( n ) {
        case 0 :
            return false;
        case 1 :
            unit = 'm';
        default :
            break;
    }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wimplicit-fallthrough"
    switch ( unit ) {
        case 'H' :
        case 'h' :
            _msecs *= 60;
        case 'M' :
        case 'm' :
            _msecs *= 60;
        case 'S' :
        case 's' :
            _msecs *= 1000;
            break;
        default :
            return false;
    }
#pragma GCC diagnostic pop

    *msecs = _msecs;
    return true;
}

#if 0
bool get_laptop_state(enum laptop_state_e *state) {
    FILE *f = NULL;
    char str_state[256];

    CHK_NULL(f = fopen("/proc/acpi/button/lid/LID0/state", "r"));
    CHK(fscanf(f, "state:%*[\t ]%" STRINGIFY(sizeof(str_state))  "s", str_state), != 1);
    CHK_NEG(fclose(f));

    if ( strcasecmp(str_state, STRINGIFY(OPEN)) == 0 ) {
        *state = OPEN;
        return true;
    } else if ( strcasecmp(str_state, STRINGIFY(CLOSED)) == 0 ) {
        *state = CLOSED;
        return true;
    } else {
        return false;
    }

    fail:
    SAFE_FREE(fclose, f);
    return false;
}
#endif
