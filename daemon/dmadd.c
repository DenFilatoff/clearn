#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <glib-2.0/glib.h>
#include <glib-2.0/glib/gprintf.h>
#include <syslog.h>
#include <fcntl.h>
#include "stdbool.h"
#include <sys/resource.h>
#include <sys/types.h>
#include <unistd.h>
#define BUFFSIZE 1024
#define NAMESIZE 200
#define LOCKMODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)
char file_name[NAMESIZE];
char socket_name[NAMESIZE];
 char lock_file[NAMESIZE];
char ini_file[NAMESIZE];
 int sock;

void print_error(const char *format, ...)
{
    va_list argptr;
    va_start (argptr, format);
    vprintf (format, argptr);
    va_end (argptr);
    exit (EXIT_FAILURE);
}

void createFile(char *FName){

    FILE* fd = fopen(FName, "w");
    int dim1 = 10 + rand() / (RAND_MAX / 103);
    int dim2 = 20 + rand() / (RAND_MAX / 103);
    int list[dim1][dim2];
     for (int i=0; i<dim1; i++)
        {
            for (int j=0; j<dim2; j++)
            {
               list[i][j] = rand() % 10;
                fprintf (fd, "%d ", list[i][j]);
            }
            putchar('\n');
        }
    fclose(fd);
}

void read_conf(void)
{
    g_autoptr (GError) error = NULL;
    g_autoptr (GKeyFile) key_file = g_key_file_new ();

    if (!g_key_file_load_from_file(key_file, ini_file, G_KEY_FILE_NONE, &error)) {
        if (!g_error_matches(error, G_FILE_ERROR, G_FILE_ERROR_NOENT)) {
            g_warning ("Error loading key file: %s", error->message);
        }
        return;
    }

    g_autofree gchar *g_file_name =
            g_key_file_get_string(key_file, "First Group", "FileName", &error);
    if (g_file_name == NULL && !g_error_matches(error, G_KEY_FILE_ERROR,
                 G_KEY_FILE_ERROR_KEY_NOT_FOUND)) {
        g_warning ("Error finding key in key file: %s", error->message);
        return;
    } else if (g_file_name == NULL) {
        // Fall back to a default value.
        g_file_name = g_strdup ("default-value");
    }

    strcpy (file_name, (char *) g_file_name);

    g_autofree gchar *g_socket_name =
            g_key_file_get_string (key_file, "Settings", "SocketFile", &error);
    if (g_socket_name == NULL && !g_error_matches (error, G_KEY_FILE_ERROR,
                 G_KEY_FILE_ERROR_KEY_NOT_FOUND)) {
        g_warning ("Error finding key in key file: %s", error->message);
        return;
    } else if (g_socket_name == NULL) {
        // Fall back to a default value.
        g_socket_name = g_strdup ("default-value");
    }

    strcpy (socket_name, (char *) g_socket_name);

    g_autofree gchar *g_lock_file =
            g_key_file_get_string (key_file, "Settings", "LockFile", &error);
    if (g_lock_file == NULL && !g_error_matches (error, G_KEY_FILE_ERROR,
                 G_KEY_FILE_ERROR_KEY_NOT_FOUND)) {
        g_warning ("Error finding key in key file: %s", error->message);
        return;
    } else if (g_lock_file == NULL) {
        // Fall back to a default value.
        g_lock_file = g_strdup ("default-value");
    }

    strcpy (lock_file, (char *) g_lock_file);

    createFile(file_name);
}

int lockfile(int fd) {
    struct flock fl;
    fl.l_type = F_WRLCK;
    fl.l_start = 0;
    fl.l_whence = SEEK_SET;
    fl.l_len = 0;
    return (fcntl (fd, F_SETLK, &fl));
}

int already_running(void) {
    //check daemon

    int fd;
    char buf[16];
    fd = open (lock_file, O_RDWR | O_CREAT, LOCKMODE);
    if (fd < 0)
    {
        syslog (LOG_ERR, "unable to read %s: %s",
            lock_file, strerror (errno));
        exit (EXIT_FAILURE);
    }
    if (lockfile (fd) < 0)
    {
        if (errno == EACCES || errno == EAGAIN)
    {
        close (fd);
        return 1;
    }
        syslog (LOG_ERR,
            "Unable to set lock on %s: %s",
            lock_file, strerror (errno));
        exit (EXIT_FAILURE);
    }
    ftruncate (fd, 0);
    sprintf (buf, "%ld", (long) getpid ());
    write (fd, buf, strlen (buf) + 1);
    return 0;
}



bool daemonize(const char *cmd)
{
    umask(0);

    pid_t pid;
    if ((pid = fork()) < 0) {
        perror("unable to call fork");
        return false;
    }
    if (pid != 0) {
        _exit(0);  // use _exit() to avoid triggering atexit() processing
    }


    if (setsid() == (pid_t)-1) {
        perror("unable to call setsid");
        return false;
    }


    openlog(cmd, LOG_CONS, LOG_DAEMON);
    struct rlimit rl;
    if (getrlimit(RLIMIT_NOFILE, &rl) < 0) {
        perror("unable to get descriptor");
        return false;
    }
    if (rl.rlim_max == RLIM_INFINITY) {
        rl.rlim_max = 1024;
    }
    for (size_t i = 0; i < rl.rlim_max; i++) {
        close(i);
    }


    int fd0 = open("/dev/null", O_RDWR);
    int fd1 = dup(0);
    int fd2 = dup(0);
    if (fd0 != 0 || fd1 != 1 || fd2 != 2) {
        syslog(LOG_CRIT, "file descriptor is failed %d %d %d", fd0, fd1, fd2);
        return false;
    }

    struct sigaction sa;
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGHUP, &sa, NULL) < 0) {
        syslog(LOG_CRIT, "unable ignore SIGHUP");
        return false;
    }


    if ((pid = fork()) < 0) {
        syslog(LOG_CRIT, "unable to call fork");
        return false;
    } else if (pid != 0) {
        _exit(0);
    }
    return true;
}



