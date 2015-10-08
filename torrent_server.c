#include "torrent_server.h"

void ts_listener_callback(int fd, fd_set* active_fd_set) {
  sample_req_callback(fd, active_fd_set);
}

void ts_client_callback(int fd, fd_set* active_fd_set) {
  sample_read_callback(fd, active_fd_set);
}

void start_ts(int port) {
  int listener_fd = create_socket(port);
  listen_on(listener_fd,10);

  fd_set active_fd_set, read_fd_set;
  FD_ZERO(&active_fd_set);
  FD_SET(listener_fd, &active_fd_set);

  while(1) {
    read_fd_set = active_fd_set;
  }
  
}
