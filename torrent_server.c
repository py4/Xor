#include "torrent_server.h"

int read_client_port(int fd, fd_set* active_fd_set) {
  char buffer[MAXMSG];
  if(sample_read_callback(fd, active_fd_set, buffer) == 0)
    return atoi(buffer);
  else
    return -1;
}

//TODO: droping client with bad port
void ts_listener_callback(int fd, fd_set* active_fd_set) {
  int new_socket = sample_req_callback(fd, active_fd_set);
  if(new_socket > 0) {
    printf("[ts] waiting for client port to be declared\n");
    int port = read_client_port(new_socket, active_fd_set);
    printf("[ts] declared port: %d\n", port);
  }
}



void ts_client_callback(int fd, fd_set* active_fd_set) {

  char buffer[MAXMSG];
  sample_read_callback(fd, active_fd_set, buffer);
  printf("[ts] client told me: %s\n", buffer);
  
  /*int new_socket = sample_read_callback(fd, active_fd_set);
  if(new_socket > 0) {
    printf("[ts] waiting for client port to be declared\n");
    int port = read_client_port(new_socket, active_fd_set);
    printf("[ts] declared port: %d\n", port);
    }*/
}

void ts_event_callback(int fd, fd_set* active_fd_set, struct SockCont cont) {
  printf("[ts] triggered fd: %d\n", fd);
  if(fd == cont.listener_fd)
    ts_listener_callback(fd, active_fd_set);
  else {
    printf("[ts] a client has sent sth!\n");
    ts_client_callback(fd, active_fd_set);
  }
}

void start_ts(int port) {
  int listener_fd = create_socket(port);
  listen_on(listener_fd,10);

  struct SockCont init;
  init.listener_fd = listener_fd;
  init.server_fd = -1;
  init.stdin_fd = -1;

  monitor(init, ts_event_callback);
}
