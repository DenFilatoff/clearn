#ifndef DMADD_H
#define DMADD_H

void read_conf(void);
void print_error(const char *format, ...);
int lockfile(int fd);
int already_running(void);
bool daemonize(const char *cmd);

#define BUFFSIZE 1024
#define NAMESIZE 200
#define LOCKMODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)

extern char file_name[NAMESIZE];
extern char socket_name[NAMESIZE];
extern char lock_file[NAMESIZE];
extern char ini_file[NAMESIZE];
extern int sock;


#endif // DMADD_H
