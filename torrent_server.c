#include "torrent_server.h"
/*TODO: seprating db handler from torrent server */
/*TODO: adding proper functions to header files */


void dump_db(TorrentDB* db) {
	int i,j;
  printf("==========================================\n");
  for(i = 0; i < db->num_of_connected; i++) {
    printf("user with fd %d \n", db->connected_clients[i]->fd);
    for(j = 0; j < db->connected_clients[i]->num_of_files; j++)
      printf("name: %s\n", db->connected_clients[i]->owned_files[j]->name);
    printf("-----------------------\n");
  }
  printf("===========================================\n");
}
void init_db(TorrentDB* db) {
  memset(db->connected_clients, 0, sizeof(db->connected_clients));
  memset(db->file_entries, 0, sizeof(db->file_entries));
  db->num_of_connected = 0;
  db->num_of_entries = 0;
}

void init_connected_client(ConnectedClient* client, char* ip, int port, int fd) {
  client->fd = fd;
  memset(client->ip, '\0', sizeof(client->ip));
  strcpy(client->ip, ip);
  client->port = port;
  memset(client->owned_files,0,sizeof(client->owned_files));
  client->num_of_files = 0;
}

void init_file_entry(FileEntry* entry, char* name, ConnectedClient* client) {
  memset(entry->name, '\0', sizeof(entry->name));
  strcpy(entry->name, name);
  memset(entry->owners, 0, sizeof(entry->owners));
  entry->last_who_gave = -1;
  entry->num_of_owners = 1;
  entry->owners[0] = client;
  client->owned_files[client->num_of_files++] = entry;
}


ConnectedClient* find_connected_client(TorrentDB* db, int fd) {
	int i;
  for(i = 0; i < MAX_CONNECTED; i++) {
    if(db->connected_clients[i] == 0)
      return 0;
    if(db->connected_clients[i]->fd == fd)
      return db->connected_clients[i];
  }
  return 0;
}

FileEntry* find_file_entry(TorrentDB* db, char* name) {
	int i;
  for(i = 0; i < MAX_FILES; i++) {
    if(db->file_entries[i] == 0)
      return 0;
    if(strcmp(db->file_entries[i]->name,name) == 0)
      return db->file_entries[i];
  }
  return 0;
}

void add_client(TorrentDB* db, char* ip, int port, int fd) {
  ConnectedClient* client = malloc(sizeof(ConnectedClient));
  init_connected_client(client,ip,port,fd);
  db->connected_clients[db->num_of_connected] = client;
  db->num_of_connected++;
}

void add_entry(TorrentDB* db, char* name, int fd) {
  ConnectedClient* client = find_connected_client(db, fd);
  if(client == 0) {
    printf("[ts] client not found \n");
    return;
  }
  FileEntry* entry = find_file_entry(db, name);
  if(entry == 0) {
    entry = malloc(sizeof(FileEntry));
    init_file_entry(entry, name, client);
  } else {
    entry->owners[entry->num_of_owners++] = client;
    client->owned_files[client->num_of_files++] = entry;
  }
  db->file_entries[db->num_of_entries++] = entry;
  return;
}

void remove_client_from_entry(FileEntry* entry, ConnectedClient* client) {
  int i,j;
	for(i = 0; i < entry->num_of_owners; i++) {
    if(entry->owners[i] == client) {
			int j;
      for(j = i; j < entry->num_of_owners - 1; j++)
	entry->owners[j] = entry->owners[j+1];
      entry->owners[entry->num_of_owners - 1] = 0;
      entry->num_of_owners--;
      if(entry->last_who_gave >= i)
	entry->last_who_gave--;
      return;
    }
  }
}

/* no client is left for this file */
void remove_entry_from_db(TorrentDB* db, FileEntry* entry) {
	int i,j;
  for(i = 0; i < db->num_of_entries; i++)
    if(db->file_entries[i] == entry) {
      for(j = i; j < db->num_of_entries - 1; j++)
	db->file_entries[j] = db->file_entries[j+1];
      db->file_entries[db->num_of_entries-1] = 0;
      db->num_of_entries--;
      free(entry);
      return;
    }
}

/* in case of disconnecting */
void remove_client(TorrentDB* db, int fd) {
	int i,j;
  ConnectedClient* client = find_connected_client(db,fd);
  if(client == 0)
    return;

  /* removing from entries */
  for(i = 0; i < client->num_of_files; i++) {
    remove_client_from_entry(client->owned_files[i], client);
    if(client->owned_files[i]->num_of_owners == 0)
      remove_entry_from_db(db, client->owned_files[i]);
  }

  /* removing form db */
  for(i = 0; i < db->num_of_connected; i++)
    if(db->connected_clients[i] == client) {
      for(j = i; j < db->num_of_connected - 1; j++)
	db->connected_clients[j] = db->connected_clients[j+1];
      db->connected_clients[db->num_of_connected-1] = 0;

      db->num_of_connected--;
      free(client);
      return;
    }
}

ConnectedClient* get_current_seeder(FileEntry* entry) {
  entry->last_who_gave++;
  if(entry->last_who_gave == entry->num_of_owners)
    entry->last_who_gave = 0;
  return entry->owners[entry->last_who_gave];
}

ConnectedClient* get_a_seeder(TorrentDB* db, char* name) {
  FileEntry* entry = find_file_entry(db,name);
  if(entry == 0)
    return 0;
  return get_current_seeder(entry);
}



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
    printf("[ts] waiting for client port to be declared\n");
    int port = read_client_port(new_socket, active_fd_set);
    printf("[ts] received port: %d\n", port);
    add_client(db, ip, port, new_socket);
    printf("[ts] added client: %s - listen port: %d - fd: %d \n", ip, port, new_socket);
    printf("[ts] now connected: %d\n", db->num_of_connected);
  } else
    printf("[ts][error] in accepting request\n");
  return;
}

void ts_register_file(int fd, char* buffer, TorrentDB* db) {
  char name[100];
  printf("[debug] buffer: %s\n");
  extract_filename(buffer, name);
  printf("[ts] registering: %s\n", name);
  add_entry(db, name, fd);
  printf("[ts] now entry count: %d\n", db->num_of_entries);
}

void ts_lookup_file(int fd, char* buffer, TorrentDB* db) {
  char name[100];
  extract_filename(buffer, name);
  printf("[ts] looking for: %s\n", name);
  ConnectedClient* client = get_a_seeder(db, name);
  if(client == 0) {
    printf("[ts] lookup failed\n");
    write_msg(fd, "0 0 0");
  } else {
    printf("[ts] found: %s - %d\n", client->ip, client->port);
    char msg[1000];
    memset(msg, '\0', sizeof(msg));
    char port[1000];
    int_to_char(client->port, port);
    strcat(msg, "1 ");
    strcat(msg, client->ip);
    strcat(msg, " ");
    strcat(msg, port);
    printf("[ts][debug] aim to write this msg: %s\n", msg);
    write_msg(fd, msg);
  }
}
void ts_client_callback(int fd, fd_set* active_fd_set, TorrentDB* db) {
  char buffer[MAXMSG];
  int read_status = sample_read_callback(fd, active_fd_set, buffer);
  if(read_status < 0) {
    printf("[ts] disconnecting client with fd %d\n", fd);
    remove_client(db,fd);
    close(fd);
    return;
  } else {
    printf("[ts] >>>>>>> read: %s\n", buffer);
  }
  
  char command[100];
  extract_command(buffer, command);
  printf("[ts] command:  %s\n", command);
  if(strcmp(command,"register") == 0)
    ts_register_file(fd, buffer, db);
  else if(strcmp(command, "lookup") == 0){
    ts_lookup_file(fd, buffer, db);
  } else {
    printf("[ts] command not found \n");
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
    printf("[ts] >>>>>>>>>> INIT <<<<<<<<<< \n");
    init_db(&db);
    db.init = 1;
  }
  
  printf("[ts] triggered fd: %d\n", fd);
  if(fd == cont.listener_fd) {
    ts_listener_callback(fd, active_fd_set, &db);
  } else if(fd == cont.stdin_fd) {
    ts_stdin_callback(fd, active_fd_set, &db);
  }
  else {
    printf("[ts] a client has sent sth!\n");
    ts_client_callback(fd, active_fd_set, &db);
  }
}

/* TODO: adding exit flag to monitor */
void start_ts(int port) {
  int listener_fd = create_socket(port);
  listen_on(listener_fd,10);

  struct SockCont init;
  init.listener_fd = listener_fd;
  init.server_fd = -1;
  init.stdin_fd = STDIN_FILENO;

  monitor(init, ts_event_callback);
}
