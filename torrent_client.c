#include "torrent_client.h"
#include "parser.h"
#include <sys/stat.h>


void tc_lookup(int server_fd, fd_set* active_fd_set, char* buffer) {
  char filename[MAXMSG], ip[MAXMSG], response[MAXMSG];
  int success,port,host_fd;

  extract_filename(buffer, filename);
  write_msg(server_fd, buffer);
  sample_read_callback(server_fd, active_fd_set, response);
  parse_lookup_output(response, &success, ip, &port);
	
  if(success == 1) {
    printf("Found entry: %s : %d\n", ip, port);
    host_fd = create_connector_socket(ip,port);
    write_msg(host_fd, filename);

    char path[MAX_NAME_SIZE];
    memset(path, 0, sizeof(path));
    strcat(path, "downloads/");
    strcat(path, filename);
    download_data(host_fd, path);
  } else {
    printf("file not found!\n");
  }
}

void tc_stdin_callback(int fd, fd_set* active_fd_set, struct SockCont cont, FileDB* db) {

  char buffer[MAXMSG];
  read_from_stdin(buffer, MAXMSG*sizeof(char));
  if(strlen(buffer) == 0)
    return;

  char command[BUFSIZE];
  extract_command(buffer, command);
  
  if(strcmp(command, "register") == 0) {
    char filename[MAXFN];
    char path[MAXFN];
    extract_filename(buffer, filename);
    extract_path(buffer, path);
    add_to_db(db, path, filename);
    write_msg(cont.server_fd, buffer);
  } else if(strcmp(command, "debug") == 0) {
    dump_tc_db(db);
  } else if(strcmp(command, "disconnect") == 0) {
    disconnect(cont.server_fd, 1, EXIT_SUCCESS);
  } else if(strcmp(command, "lookup") == 0) {
    tc_lookup(cont.server_fd, active_fd_set, buffer);
  } else {
    printf("command not found!\n");
  }
}

void tc_listener_callback(int fd, fd_set* active_fd_set, struct SockCont cont, FileDB* db) {
  char ip[MAXMSG], buffer[MAXMSG];
  int new_socket = sample_req_callback(fd, active_fd_set,ip);
  if(sample_read_callback(new_socket, active_fd_set, buffer) == 0) {
    char path[MAXMSG];
    memset(path,0,sizeof(path));
    get_entry_path(db,buffer, path);
    csnprintf("Found path: %s\n", path);
    send_data(new_socket, path);
    FD_CLR(new_socket, active_fd_set);
  } else {
    csnprintf("[tc][debug] CRAP!!!\n");
  }
}

void tc_server_callback(int fd, fd_set* active_fd_set, struct SockCont cont) {
  char buffer[MAXMSG];
  sample_read_callback(fd, active_fd_set, buffer);
  csnprintf("- Server: %s\n", buffer);
}

void tc_client_callback(int fd, fd_set* active_fd_set, struct SockCont cont) {
  char buffer[MAXMSG];
  if(sample_read_callback(fd, active_fd_set, buffer) > 0)
    csnprintf("- Client[%d]: %s\n", fd, buffer);
}

void tc_event_callback(int fd, fd_set* active_fd_set, struct SockCont cont) {

  csnprintf("[tc] trigged fd: %d\n", fd);

  static FileDB db = {{},0,0};
  if(db.init == 0) {
    init_tc_db(&db);
    db.init = 1;
  }
  
  if(fd == cont.stdin_fd)
    tc_stdin_callback(fd, active_fd_set,cont,&db);
  else if(fd == cont.server_fd)
    tc_server_callback(fd, active_fd_set,cont);
  else if(fd == cont.listener_fd)
    tc_listener_callback(fd, active_fd_set,cont,&db);
  else
    tc_client_callback(fd, active_fd_set,cont);
}

void start_tc(char* server_ip, int server_port) {

  int my_port;
  int listener_fd = create_socket(0);
  listen_on(listener_fd,10);
  struct sockaddr_in sin;
  int addrlen = sizeof(sin);
  if(getsockname(listener_fd, (struct sockaddr *)&sin, &addrlen) ==0 && sin.sin_family == AF_INET &&
     addrlen == sizeof(sin)) {
    my_port = ntohs(sin.sin_port);
  } else{
    cperror("listen");
    return;
  }

  csnprintf("Listening on %d\n", my_port);
  int server_fd = create_connector_socket(server_ip, server_port);
  write_int(server_fd, my_port);

  struct SockCont init;
  init.stdin_fd = STDIN_FILENO;
  init.server_fd = server_fd;
  init.listener_fd = listener_fd;

  monitor(init, tc_event_callback);
}

