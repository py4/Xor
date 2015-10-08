#include "torrent_client.h"
#include "parser.h"

void tc_stdin_callback(int fd, fd_set* active_fd_set, struct SockCont cont) {
  char buffer[1000];
  memset(buffer, '\0', sizeof(buffer));
  read(STDIN_FILENO, buffer, sizeof(buffer));
  sanitize_buffer(buffer);
  if(strlen(buffer) == 0)
    return;
  char command[1000];
  extract_command(buffer, command);
  if(strcmp(command, "register") == 0) {
    char filename[1000];
    extract_filename(buffer, filename);
    printf("registering: %s\n", filename);
    write_msg(cont.server_fd, filename);
  } else {
    printf("command not found!\n");
  }
}

void tc_listener_callback(int fd, fd_set* active_fd_set, struct SockCont cont) {
  sample_req_callback(fd, active_fd_set);
}

void tc_server_callback(int fd, fd_set* active_fd_set, struct SockCont cont) {
  sample_read_callback(fd, active_fd_set);
}

void tc_client_callback(int fd, fd_set* active_fd_set, struct SockCont cont) {
  sample_read_callback(fd, active_fd_set);
}

void tc_event_callback(int fd, fd_set* active_fd_set, struct SockCont cont) {

  printf("[tc] trigged fd: %d\n", fd);
  printf("[tc] stdin_fd: %d\n", cont.stdin_fd);
  if(fd == cont.stdin_fd)
    tc_stdin_callback(fd, active_fd_set,cont);
  else if(fd == cont.server_fd)
    tc_server_callback(fd, active_fd_set,cont);
  else if(fd == cont.listener_fd)
    tc_listener_callback(fd, active_fd_set,cont);
  else{
    printf("a client has sent sth!\n");
    tc_client_callback(fd, active_fd_set,cont);
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
