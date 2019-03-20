#ifndef CONFIG_H
#define CONFIG_H

#include <stdlib.h>

#define LOCK_FILENAME "/tmp/lock-x11.pid"
#define DEFAULT_INACTIVITY "5m"

static char *const lockcmd[] =
#ifdef USE_SLOCK
{ "/usr/bin/slock", "/usr/sbin/systemctl", "suspend", NULL };
#else
{ "/usr/bin/i3lock", "-n", "-i", "/home/thomas/.config/i3/bg.png", "-e", "-f", NULL };
#endif

#endif // CONFIG_H
