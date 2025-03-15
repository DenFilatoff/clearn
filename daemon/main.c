#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <glib.h>
#include <glib/gprintf.h>
#include <syslog.h>
#include <fcntl.h>
#include "dmadd.c"


char file_name[NAMESIZE];
char socket_name[NAMESIZE];
char lock_file[NAMESIZE];
char ini_file[NAMESIZE];
int sock;





static void sig_hup() {
    syslog (LOG_INFO, "read config");
    read_conf ();
}

static void sig_quit() {
    syslog (LOG_INFO, "delete socket");
    close (sock);
    unlink (socket_name);
}

int main (int argc, char **argv)
{
    if (argc < 2) {
        print_error("Use: app <path To Config-file>.\n");
    }

    int msgsock;
    struct sockaddr_un server;
    char buf[BUFFSIZE];
    struct stat statbuf;
    struct sigaction sa;

    strcpy(ini_file, argv[1]);
    read_conf();


    if (!(argc == 3 && strcmp (argv[2], "-test") == 0)) {
        daemonize("daemon_stat");
        if (already_running ()) {
            syslog(LOG_ERR, "daemon is running");
            return EXIT_FAILURE;}


        sa.sa_handler = sig_hup;
        sigfillset (&sa.sa_mask);
        sigdelset (&sa.sa_mask, SIGHUP);
        sa.sa_flags = 0;
        if (sigaction (SIGHUP, &sa, NULL) < 0) {
            syslog (LOG_ERR,
                    "unable to call SIGHUP: %s",
                    strerror (errno));
            return EXIT_FAILURE;
        }


        sa.sa_handler = sig_quit;
        sigfillset (&sa.sa_mask);
        sigdelset (&sa.sa_mask, SIGQUIT);
        sa.sa_flags = 0;
        if (sigaction (SIGQUIT, &sa, NULL) < 0) {
            syslog (LOG_ERR,
                    "unable get  SIGQUIT: %s",
                    strerror (errno));
            return EXIT_FAILURE;
        }
    }


    sock = socket (AF_UNIX, SOCK_STREAM, 0);

    if (sock < 0)
        print_error ("%s\n", "opening stream socket");

    server.sun_family = AF_UNIX;
    strcpy (server.sun_path, socket_name);

    if (bind (sock, (struct sockaddr *) &server, sizeof (struct sockaddr_un))) {
        print_error ("%s\n", "binding stream socket");
    }

    printf("Socket has name %s\n", server.sun_path);
    listen(sock, 5);
    while (1) {
        msgsock = accept (sock, 0, 0);

        if (stat (file_name, &statbuf) < 0)
            snprintf (buf, BUFFSIZE,
                      "unable to call stat:%s file %s\n",
                      strerror (errno), file_name);
        else
            snprintf (buf, BUFFSIZE,
                      "file size %s :%ld\n",
                      file_name, statbuf.st_size);

        send (msgsock, buf, strlen (buf), 0);
        close (msgsock);
    }

    //  return EXIT_SUCCESS;
}
