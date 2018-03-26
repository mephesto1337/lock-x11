#ifndef CONFIG_H
#define CONFIG_H

#define LOCK_FILENAME "/tmp/lock-x11-thomas"

static char *const lockcmd[] = { "/usr/bin/slock", NULL };
static char *const sleepcmd[] = { "/usr/sbin/systemctl", "suspend", NULL };

#endif // CONFIG_H
