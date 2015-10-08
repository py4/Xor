#include "tcp.h"

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
  memset(buffer,0,sizeof(buffer));
  int nbytes;

  nbytes = read(fd,buffer,MAXMSG);
  if(nbytes < 0) {
    perror("read");
    exit(EXIT_FAILURE);
  } else if(nbytes == 0) {
    return -1;
  } else {
    //fprintf(stderr, "Server: got message: %s\n", buffer);
    return 0;
  }
}

void listen_on(int fd, int queue_size) {
  if(listen(fd, queue_size) < 0) {
    perror("listen");
    exit(EXIT_FAILURE);
  }
}

void sample_req_callback(int fd, fd_set* active_fd_set) {
  struct sockaddr_in client_sockaddr;
  int size = sizeof(client_sockaddr);
  int new_socket = accept(fd, (struct sockaddr*)&client_sockaddr, &size);
  if(new_socket < 0) {
    perror("accept");
    return;
  }
  fprintf(stderr, "Server: connect from host %s, port %hd. \n", inet_ntoa(client_sockaddr.sin_addr), ntohs(client_sockaddr.sin_port));
  FD_SET(new_socket, active_fd_set);
}

void sample_read_callback(int fd, fd_set* active_fd_set) {
  char buffer[MAXMSG];
  if(read_from_socket(fd, buffer) < 0) {
    printf("FUCK!\n");
    close(fd);
    FD_CLR(fd, active_fd_set);
  } else
    fprintf(stderr, "Server: got message: %s\n", buffer);

  memset(buffer,0,sizeof(buffer));
}

void sample_callback(int main_fd, fd_set* active_fd_set, fd_set* read_fd_set) {
  for(int i = 0; i < FD_SETSIZE; i++) {
    if(FD_ISSET(i, read_fd_set)) {
      if(i == main_fd)
	sample_req_callback(i, active_fd_set);
      else
	sample_read_callback(i, active_fd_set);
      
    }
  }
}

  
int start_server(int port, socket_callback callback) {
  fd_set active_fd_set, read_fd_set;
  int main_socket = create_socket(port);
  listen_on(main_socket,10);
  FD_ZERO(&active_fd_set);
  FD_SET(main_socket, &active_fd_set);

  while(1) {
    read_fd_set = active_fd_set;
    printf("Waiting for something...\n");
    if(select(FD_SETSIZE, &read_fd_set, NULL,NULL,NULL) < 0) {
      perror("select");
      exit(EXIT_FAILURE);
    }
    callback(main_socket, &active_fd_set, &read_fd_set);
  }
}

