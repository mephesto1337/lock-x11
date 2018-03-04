#ifndef CONFIG_H
#define CONFIG_H

#define LOCK_FILENAME "/tmp/lock-x11-thomas"

static char *const lockcmd[] = { "/usr/bin/slock" };
static char *const sleepcmd[] = { "/usr/sbin/systemctl", "suspend" };

#endif // CONFIG_H
