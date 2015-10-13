#include "torrent_client.h"
#include "parser.h"
#include <sys/stat.h>

void dump_tc_db(FileDB* db) {
	int i;
  printf("==================================\n");
  for(i = 0; i < db->num_of_entries; i++)
    printf("%s : %s\n", db->entries[i]->name, db->entries[i]->path);
  printf("==================================\n");
}

void tc_disconnect(int server_fd) {
  close(server_fd);
  exit(EXIT_SUCCESS);
}

void tc_lookup(int server_fd, fd_set* active_fd_set, char* buffer) {
  char filename[1000];
  extract_filename(buffer, filename);
    
  write_msg(server_fd, buffer);
  char response[MAXMSG];
  sample_read_callback(server_fd, active_fd_set, response);
  printf("[tc] response: %s\n", response);
  int success;
  char ip[MAXMSG];
  int port;
  parse_lookup_output(response, &success, ip, &port);
  printf("success: %d - ip: %s - port: %d\n", success, ip, port);
  if(success == 1) {
    int host_fd = create_connector_socket(ip,port);
    write_msg(host_fd, filename);

    /* begin downloading file */
    char path[100];
    memset(path, 0, sizeof(path));
    strcat(path, "downloads/");
    strcat(path, filename);
    int file_fd = open(path, O_CREAT | O_WRONLY,S_IRUSR | S_IWUSR);
    if(file_fd == -1)
      perror("writing to file");
    else {
      int byte_read = 1;
      char buffer[512];
      while(byte_read  > 0) {
	byte_read = read(host_fd, buffer, sizeof(buffer));
	if(byte_read == 0)
	  break;
	printf("bytes read from server: %d\n", byte_read);
	int byte_wrote = write(file_fd, buffer, byte_read);
	printf("bytes written: %d\n", byte_wrote);
	if(byte_wrote < 0)
	  perror("failed to write bytes");
      }
      close(file_fd);
      close(host_fd);
    }
      
    /* end */
    
  } else {
    printf("file not found!\n");
  }
}

void tc_stdin_callback(int fd, fd_set* active_fd_set, struct SockCont cont, FileDB* db) {

  char buffer[MAXMSG];
  read_from_stdin(buffer, MAXMSG*sizeof(char));
  
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
  } else if(strcmp(command, "disconnect") == 0) {
    tc_disconnect(cont.server_fd);
  } else if(strcmp(command, "lookup") == 0) {
    tc_lookup(cont.server_fd, active_fd_set, buffer);
  } else {
    printf("command not found!\n");
  }
}

void tc_listener_callback(int fd, fd_set* active_fd_set, struct SockCont cont, FileDB* db) {
  char ip[MAXMSG];
  int new_socket = sample_req_callback(fd, active_fd_set,ip);
  printf("[tc][debug] requested accepted. waiting for message\n");
  char buffer[MAXMSG];
  if(sample_read_callback(new_socket, active_fd_set, buffer) == 0) {
    char path[MAXMSG];
    memset(path,0,sizeof(path));
    get_entry_path(db,buffer, path);
    printf("found path: %s\n", path);

    /* begin sending file */
    int file_fd = open(path, O_RDONLY);
    if(file_fd == -1) {
      perror("opening file");
      return;
    }
    int byte_read = 1;
    char buffer[512];
    while(byte_read > 0) {
      byte_read = read(file_fd, buffer, sizeof(buffer));
      if(byte_read == 0)
	break;
      printf("[from file] bytes read %d\n", byte_read);
      int byte_sent = write(new_socket, buffer, byte_read);
      
        printf(" bytes written to requesters %d\n", byte_sent);

        if(byte_sent < 0)
            perror("Failed to send bytes");
    }
    /* end */
    close(file_fd);
    close(new_socket);
    FD_CLR(new_socket, active_fd_set);
    
  } else {
    printf("[tc][debug] CRAP!!!\n");
  }
}

void tc_server_callback(int fd, fd_set* active_fd_set, struct SockCont cont) {
  char buffer[MAXMSG];
  sample_read_callback(fd, active_fd_set, buffer);
  printf("[tc] server told me: %s\n", buffer);
}

void tc_client_callback(int fd, fd_set* active_fd_set, struct SockCont cont) {
  char buffer[MAXMSG];
  if(sample_read_callback(fd, active_fd_set, buffer) > 0)
    printf("[tc] client[%d] told me: %s\n", fd, buffer);
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
    tc_listener_callback(fd, active_fd_set,cont,&db);
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
    perror("listen");
    return;
  }

  printf("[tc] listening to: %d\n", my_port);
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
	int i;
  for(i = 0; i < db->num_of_entries; i++)
    free(db->entries[i]);
}

void get_entry_path(FileDB* db, char* name, char* path) {
	int i;
  for(i = 0; i < db->num_of_entries; i++)
    if(strcmp(db->entries[i]->name,name) == 0) {
      strcpy(path, db->entries[i]->path);
      return;
    }
}
