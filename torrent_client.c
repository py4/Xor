#include "torrent_client.h"
#include "parser.h"

void dump_tc_db(FileDB* db) {
  printf("==================================\n");
  for(int i = 0; i < db->num_of_entries; i++)
    printf("%s : %s\n", db->entries[i]->name, db->entries[i]->path);
  printf("==================================\n");
}
void tc_stdin_callback(int fd, fd_set* active_fd_set, struct SockCont cont, FileDB* db) {

  char buffer[MAXMSG];
  read_from_stdin(buffer, MAXMSG*sizeof(char));
  /*memset(buffer, '\0', sizeof(buffer));
  read(STDIN_FILENO, buffer, MAXMSG);
  buffer[strlen(buffer)-1] = '\0'; // to remove \n */
  
  printf("[tc][debug] here1\n");
  
  //sanitize_buffer(buffer);
  if(strlen(buffer) == 0)
    return;

  char command[1000];
  extract_command(buffer, command);
  
  if(strcmp(command, "register") == 0) {
    char filename[1000];
    char path[1000];
    extract_filename(buffer, filename);
    extract_path(buffer, path);
    add_to_db(db, path, filename);
    printf("registering: %s\n", filename);
    write_msg(cont.server_fd, buffer);
  } else if(strcmp(command, "debug") == 0) {
    dump_tc_db(db);
  }else {
    printf("command not found!\n");
  }
}

void tc_listener_callback(int fd, fd_set* active_fd_set, struct SockCont cont) {
  char ip[MAXMSG];
  sample_req_callback(fd, active_fd_set,ip);
}

void tc_server_callback(int fd, fd_set* active_fd_set, struct SockCont cont) {
  char buffer[MAXMSG];
  sample_read_callback(fd, active_fd_set, buffer);
  printf("[tc] server told me: %s\n", buffer);
}

void tc_client_callback(int fd, fd_set* active_fd_set, struct SockCont cont) {
  char buffer[MAXMSG];
  sample_read_callback(fd, active_fd_set, buffer);
  printf("[tc] another client told me: %s\n", buffer);
}

void tc_event_callback(int fd, fd_set* active_fd_set, struct SockCont cont) {

  printf("[tc] trigged fd: %d\n", fd);
  printf("[tc] stdin_fd: %d\n", cont.stdin_fd);

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
    tc_listener_callback(fd, active_fd_set,cont);
  else{
    printf("a client has sent sth!\n");
    tc_client_callback(fd, active_fd_set,cont);
  }
}

void send_lport_to_ts(int fd, int port) {
  char buffer[1000];
  int_to_char(port, buffer);
  write_msg(fd, buffer);
}

void start_tc(int my_port, char* server_ip, int server_port) {
  int listener_fd = create_socket(my_port);
  listen_on(listener_fd,10);
  int server_fd = create_connector_socket(server_ip, server_port);
  send_lport_to_ts(server_fd, my_port);
  
  printf("[tc] connected to the server\n");
  printf("[tc] sent my port. \n");
  
  struct SockCont init;
  init.stdin_fd = STDIN_FILENO;
  init.server_fd = server_fd;
  init.listener_fd = listener_fd;

  monitor(init, tc_event_callback);
}

void init_tc_db(FileDB* db) {
  memset(db->entries, 0, sizeof(Entry));
  db->num_of_entries = 0;
}

void add_to_db(FileDB* db, char* path, char* name) {
  Entry* entry = malloc(sizeof(Entry));
  strcpy(entry->path, path);
  strcpy(entry->name, name);
  db->entries[db->num_of_entries++] = entry;
}

void clear_db(FileDB* db) {
  for(int i = 0; i < db->num_of_entries; i++)
    free(db->entries[i]);
}

void get_entry_path(FileDB* db, char* name, char* path) {
  for(int i = 0; i < db->num_of_entries; i++)
    if(strcmp(db->entries[i]->name,name) == 0) {
      strcpy(path, db->entries[i]->path);
      return;
    }
}
