#include "torrent_server.h"

void ts_listener_callback(int fd, fd_set* active_fd_set) {
  sample_req_callback(fd, active_fd_set);
}

void ts_client_callback(int fd, fd_set* active_fd_set) {
  sample_read_callback(fd, active_fd_set);
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
