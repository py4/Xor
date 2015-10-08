#include "torrent_client.h"

void tc_stdin_callback(int fd, fd_set* active_fd_set) {
  read_from_stdin(fd, active_fd_set);
}

void tc_listener_callback(int fd, fd_set* active_fd_set) {
  sample_req_callback(fd, active_fd_set);
}

void tc_server_callback(int fd, fd_set* active_fd_set) {
  sample_read_callback(fd, active_fd_set);
}

void tc_client_callback(int fd, fd_set* active_fd_set) {
  sample_read_callback(fd, active_fd_set);
}

void tc_event_callback(int fd, fd_set* active_fd_set, struct SockCont cont) {

  printf("[TORRENT CLIENT CALLBACK]\n");
  static struct SockCont my_cont;
  my_cont.stdin_fd = cont.stdin_fd;
  my_cont.server_fd = cont.server_fd;
  my_cont.listener_fd = cont.listener_fd;
  
  if(fd == my_cont.stdin_fd)
    tc_stdin_callback(fd, active_fd_set);
  if(fd == my_cont.server_fd)
    tc_server_callback(fd, active_fd_set);
  if(fd == my_cont.listener_fd)
    tc_listener_callback(fd, active_fd_set);
  else {
    printf("a client has sent sth!\n");
    tc_client_callback(fd, active_fd_set);
  }
}

void start_tc(int my_port, char* server_ip, int server_port) {
  int listener_fd = create_socket(my_port);
  listen_on(listener_fd,10);
  int server_fd = create_connector_socket(server_ip, server_port);

  struct SockCont init;
  init.stdin_fd = STDIN_FILENO;
  init.server_fd = server_fd;
  init.listener_fd = listener_fd;

  monitor(init, tc_event_callback);
}
