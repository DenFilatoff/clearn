#ifndef HTTP_H
#define HTTP_H


typedef struct async_data
{
  int fd;
  size_t client_status;
  char workspace[2048];
  char error_request[4096];
} async_data;

#define MAX_EPOLL_EVENTS 128
static struct epoll_event events[MAX_EPOLL_EVENTS];
#define BACKLOG 128

char *status(int client_status);
int setNoBlocking(int sock);
void  readFile(struct async_data *ptr);
size_t digCount(size_t number);
void writeFile(struct async_data *ptr);

#endif // HTTP_H


