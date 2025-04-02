#include <alloca.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#define MAX_BUF_SIZE 4096
#define MAX_TEXT_SIZE 1000
#define MAX_FONT_SIZE 100



int main(int argc, char *argv[]) {

  if ((argc > 3 || (argc == 1)) || (argc == 2)) {
    printf("Please enter text and desired font \nand make input your text format: <text (upto 1000 chars) font>, exiting..\n");
    exit(EXIT_FAILURE);
  }

  struct addrinfo hints, *res, *p;
  int status;
  char ipstr[INET6_ADDRSTRLEN];

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;

  if ((status = getaddrinfo("telehack.com", "23", &hints, &res))) {
    printf("getaddrinfo error:%s\n", gai_strerror(status));
    exit(EXIT_FAILURE);
  }
  for (p = res; p != NULL; p = p->ai_next) {
    void *addr;

    if (p->ai_family == AF_INET) {
      struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
      addr = &(ipv4->sin_addr);
    } else {
      struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
      addr = &(ipv6->sin6_addr);
    }
    inet_ntop(p->ai_family, addr, ipstr, sizeof(ipstr));
  }

  int s = socket(res->ai_family, res->ai_socktype,
                 res->ai_protocol); // get a socket
  if (s < 0) {
    perror("Unble to get socket");
    exit(EXIT_FAILURE);
  }

  int c_done = connect(s, res->ai_addr, res->ai_addrlen);

  if (c_done < 0) {
    perror("Unable to connect to socket\n");
    exit(EXIT_FAILURE);
  }

  char s_msg[MAX_TEXT_SIZE] = {0};
  size_t s_msg_len = snprintf(0, 0, "figlet /%s %s\r\n", argv[2], argv[1]);
  snprintf(s_msg, s_msg_len + 1, "figlet /%s %s\r\n", argv[2], argv[1]);
  static char buffer[MAX_BUF_SIZE] = {0};
  int len = 0;
  int r = -1;
  do {
    r = (recv(s, &buffer[len], MAX_BUF_SIZE - len, 0));
    len += r;
  } while (((strcmp(buffer + (len - 3), "\r\n."))));
  int sret = -1;
  int left = s_msg_len + 1;
  do {
    sret = send(s, s_msg, left, 0);
    if (sret < 0) {
      printf("unable to send command to the server, error %d\n", errno);
      left = -1;
      len = -1;
      shutdown(s, SHUT_RDWR);
      freeaddrinfo(res);
      freeaddrinfo(p);
      exit(EXIT_FAILURE);
    }
    left -= sret;

  } while (left > 0);
  len = 0;
  r = -1;
  memset(buffer, 0, MAX_BUF_SIZE);
  while (((r = (recv(s, &buffer[len], MAX_BUF_SIZE - len, 0))) > 0)) {
    len += r;
    if (len >= MAX_BUF_SIZE) {
      printf("max buffer size exeeded!\n");
      buffer[MAX_BUF_SIZE - 1] = '\0';
      break;
    }
    if (!(strncmp(buffer + (len - 3), "\r\n.", 3)))
      break;
  }

  if (r < 0) {
    printf("error reading server response\n");
    shutdown(s, SHUT_RDWR);
    freeaddrinfo(res);
    freeaddrinfo(p);
    exit(EXIT_FAILURE);
  }
  buffer[len - 2] = '\0';
  puts(buffer + s_msg_len);
  return 0;
}
