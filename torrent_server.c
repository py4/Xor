#include "torrent_server.h"
/*TODO: seprating db handler from torrent server */
/*TODO: adding proper functions to header files */

int read_client_port(int fd, fd_set* active_fd_set) {
  char buffer[MAXMSG];
  if(sample_read_callback(fd, active_fd_set, buffer) == 0)
    return atoi(buffer);
  else
    return -1;
}

/* TODO: droping client with bad port */
void ts_listener_callback(int fd, fd_set* active_fd_set, TorrentDB* db) {
  char ip[MAXMSG];
  int new_socket = sample_req_callback(fd, active_fd_set, ip);
  if(new_socket > 0) {
    int port = read_client_port(new_socket, active_fd_set);
    add_client(db, ip, port, new_socket);
    char tmsg[1000];
    memset(result,0,sizeof(tmsg))
    snprintf(result, 1000,
    csnprintf("New connection: %s : %d - fd: %d \n", ip, port, new_socket);
    csnprintf("Num of connections: %d\n", db->num_of_connected);
  } else
    csnprintf("Error in accepting requests\n");
  return;
}

void ts_register_file(int fd, char* buffer, TorrentDB* db) {
  char name[100];
  extract_filename(buffer, name);
  csnprintf("Registering: %s\n", name);
  add_entry(db, name, fd);
  csnprintf("Added %s\n", name);
  csnprintf("New entry count: %d\n", db->num_of_entries);
}

void ts_lookup_file(int fd, char* buffer, TorrentDB* db) {
  char name[MAXFN];
  extract_filename(buffer, name);
  csnprintf("Searching for %s\n", name);
  ConnectedClient* client = get_a_seeder(db, name);
  if(client == 0) {
    csnprintf("Lookup failed\n");
    write_msg(fd, "0 0 0");
  } else {
    csnprintf("Found: %s - %d\n", client->ip, client->port);
    char msg[MAXMSG];
    memset(msg, '\0', sizeof(msg));
    char port[MAXMSG];
    snprintf(msg,MAXMSG,"1 %s %d",client->ip, client->port);
    write_msg(fd, msg);
  }
}

void ts_client_callback(int fd, fd_set* active_fd_set, TorrentDB* db) {
  char buffer[MAXMSG];
  int read_status = sample_read_callback(fd, active_fd_set, buffer);
  if(read_status < 0) {
    csnprintf("Disconnecting client[%d]\n", fd);
    remove_client(db,fd);
    close(fd);
    return;
  }
  
  char command[BUFSIZE];
  extract_command(buffer, command);
  if(strcmp(command,"register") == 0)
    ts_register_file(fd, buffer, db);
  else if(strcmp(command, "lookup") == 0){
    ts_lookup_file(fd, buffer, db);
  } else {
    csnprintf("Command not found \n");
  }
}

void ts_stdin_callback(int fd, fd_set* active_fd_set, TorrentDB* db) {
  char buffer[MAXMSG];
  read_from_stdin(buffer, MAXMSG*sizeof(char));
  char command[MAXMSG];
  extract_command(buffer, command);
  if(strcmp(command, "debug") == 0) {
    dump_db(db);
  }
}

void ts_event_callback(int fd, fd_set* active_fd_set, struct SockCont cont) {

  static TorrentDB db = {{},{},0,0,0};
  if(db.init == 0) {
    csnprintf("[ts] >>>>>>>>>> INIT <<<<<<<<<< \n");
    init_db(&db);
    db.init = 1;
  }
  
  csnprintf("[ts] triggered fd: %d\n", fd);
  if(fd == cont.listener_fd) {
    ts_listener_callback(fd, active_fd_set, &db);
  } else if(fd == cont.stdin_fd) {
    ts_stdin_callback(fd, active_fd_set, &db);
  }
  else {
    ts_client_callback(fd, active_fd_set, &db);
  }
}

void start_ts(int port) {
  int listener_fd = create_socket(port);
  listen_on(listener_fd,10);

  struct SockCont init;
  init.listener_fd = listener_fd;
  init.server_fd = -1;
  init.stdin_fd = STDIN_FILENO;

  monitor(init, ts_event_callback);
}
