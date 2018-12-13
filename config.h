#ifndef CONFIG_H
#define CONFIG_H

#define LOCK_FILENAME "/tmp/lock-x11.pid"
#define DEFAULT_INACTIVITY "5m"

static char *const lockcmd[] = { "/usr/bin/slock", "/usr/sbin/systemctl", "suspend", NULL };

#endif // CONFIG_H
