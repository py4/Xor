#include "tcp.h"
#include <sys/ioctl.h>
int create_connector_socket(char* host, int port) {
  int sockfd = socket(PF_INET, SOCK_STREAM, 0);
  if(sockfd < 0) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = inet_addr(host);
  addr.sin_port = htons(port);

  if(connect(sockfd,(struct sockaddr *)&addr, sizeof(addr)) < 0) {
    perror("connect");
    exit(EXIT_FAILURE);
  }
  return sockfd;
}

int create_socket(int port) {
  int new_socket;
  struct sockaddr_in addr;

  new_socket = socket(PF_INET, SOCK_STREAM, 0);
  if(new_socket < 0) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = htonl(INADDR_ANY);

  if(bind(new_socket, (struct sockaddr*) &addr, sizeof(addr)) < 0) {
    perror("bind");
    exit(EXIT_FAILURE);
  }
  return new_socket;
}

int read_from_socket(int fd, char* buffer) {
  int nbytes;

  nbytes = read(fd,buffer,MAXMSG);
  if(nbytes < 0) {
    perror("read");
    exit(EXIT_FAILURE);
  } else if(nbytes == 0) {
    printf("[tcp] read 0 bytes \n");
    return -1;
  } else {
    buffer[nbytes] = '\0';
    return 0;
  }
}

void listen_on(int fd, int queue_size) {
  if(listen(fd, queue_size) < 0) {
    perror("listen");
    exit(EXIT_FAILURE);
  }
}

int sample_req_callback(int fd, fd_set* active_fd_set, char* ip) {
  struct sockaddr_in client_sockaddr;
  int size = sizeof(client_sockaddr);
  int new_socket = accept(fd, (struct sockaddr*)&client_sockaddr, &size);
  if(new_socket < 0) {
    perror("accept");
    return -1;
  }
  strcpy(ip, inet_ntoa(client_sockaddr.sin_addr));
  int port = client_sockaddr.sin_port;

  fprintf(stderr, "[ts] new connection: %s : %d \n", ip, port);
  FD_SET(new_socket, active_fd_set);
  return new_socket;
}

int sample_read_callback(int fd, fd_set* active_fd_set, char* buffer) {
  if(read_from_socket(fd, buffer) < 0) {
    close(fd);
    FD_CLR(fd, active_fd_set);
    return -1;
  }
  sanitize_buffer(buffer);
  printf("[debug] buffer: %s\n", buffer);
  return 0;
}

void read_from_stdin(char* buffer, int size) {
  memset(buffer, '\0', size);
  read(STDIN_FILENO, buffer, size);
  buffer[strlen(buffer)-1] = '\0';
}

void write_msg(int fd, char* msg) {
  printf("### fucking strlen: %d\n", strlen(msg));
  int nbytes = write(fd, msg, strlen(msg)*sizeof(char));
  if(nbytes < 0)
    perror("write");
  else
    printf("### wrote %d bytes\n", nbytes);
}

  


void monitor(struct SockCont container, callback events_callback) {

  fd_set active_fd_set, read_fd_set;
  int stdin_fd = container.stdin_fd;
  int server_fd = container.server_fd;
  int listener_fd = container.listener_fd;
	int i;

  FD_ZERO(&active_fd_set);
  if(stdin_fd != -1)
    FD_SET(stdin_fd, &active_fd_set);
  if(server_fd != -1)
    FD_SET(server_fd, &active_fd_set);
  if(listener_fd != -1)
    FD_SET(listener_fd, &active_fd_set);
  
  while(1) {
    read_fd_set = active_fd_set;
    printf("Waiting for something...\n");
    if(select(FD_SETSIZE, &read_fd_set, NULL,NULL,NULL) < 0) {
      perror("select");
      exit(EXIT_FAILURE);
    }
    for(i = 0; i < FD_SETSIZE; i++) {
      if(FD_ISSET(i, &read_fd_set))
	events_callback(i, &active_fd_set, container);
    }
  }  
}
