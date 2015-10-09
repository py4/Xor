#include "torrent_server.h"

void init_db(TorrentDB* db) {
  memset(db->connected_clients, 0, sizeof(connected_clients));
  memset(db->file_entries, 0, sizeof(file_entries));
  db->num_of_connected = 0;
  db->num_of_entries = 0;
}

void init_connected_client(ConnectedClient* client, char* ip, int port, int fd) {
  client->fd = fd;
  strcpy(client->ip, ip);
  client->port = port;
  memset(client->owned_files,0,sizeof(client->owned_files));
  client->num_of_files = 0;
}

void init_file_entry(FileEntry* entry, char* name, ConnectedClient* client) {
  strcpy(entry->name, name);
  memset(entry->owners, 0, sizeof(entry->owners));
  entry->last_owner = 0;
  entry->num_of_owners = 1;
  entry->owners[0] = client;
  client->owned_files[num_of_files++] = entry;
}


ConnectedClient* find_connected_client(TorrentDB* db, int fd) {
  for(int i = 0; i < MAX_CONNECTED; i++) {
    if(db->connected_clients[i] == 0)
      return 0;
    if(db->connected_clients[i]->fd == fd)
      return db->connected_clients[i];
  }
  return 0;
}

FileEntry* find_file_entry(TorrentDB* db, char* name) {
  for(int i = 0; i < MAX_FILES; i++) {
    if(db->file_entries[i] == 0)
      return 0;
    if(strcmp(db->file_entries[i]->name,name) == 0)
      return db->file_entries[i];
  }
  return 0;
}

void add_client(TorrentDB* db, const char* ip, int port, int fd) {
  ConnectedClient* client = malloc(sizeof(client));
  init_connected_client(client,ip,port,fd)
  db->connected_clients[db->num_of_connected++] = client;
}

void add_entry(TorrentDB* db, char* name, int fd) {
  ConnectedClient* client = get_connected_client(db, fd);
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
    client->owned_files[num_of_files++] = entry;
  }
  db->file_entries[num_of_entries++] = entry;
  return;
}

void remove_client_from_entry(FileEntry* entry, ConnectedClient* client) {
  for(int i = 0; i < entry->num_of_owners; i++) {
    if(entry->owners[i] == client) {
      for(int j = i; j < entry->num_of_owners - 1; j++)
	entry->owners[j] = entry->owners[j+1];
      entry->owners[entry->num_of_owners - 1] = 0;
      entry->num_of_owners--;
      if(entry->last_owner >= i)
	entry->last_owner--;
    }
  }
}

// no client is left this file
void remove_entry_from_db(TorrentDB* db, FileEntry* entry) {
  for(int i = 0; i < db->num_of_entries; i++)
    if(db->file_entries[i] == entry) {
      for(int j = i; j < db->num_of_entries; j++)
	db->file_entries[j] = db->file_entries[j+1];
      db->file_entries[db->num_of_entries-1] = 0;
    }
  free(entry);
}

// in case of disconnecting
void remove_client(TorrentDB* db, int fd) {
  ConnectedClient* client = find_connected_client(db,fd);
  if(client == 0)
    return;
  for(int i = 0; i < client->num_of_files; i++) {
    remove_client_from_entry(client->owned_files[i], client);
    if(client->owned_files[i]->num_of_owners == 0)
      remove_entry_from_db(db, client->owned_files[i]);
  }
}



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
}

void ts_event_callback(int fd, fd_set* active_fd_set, struct SockCont cont) {
  printf("[ts] triggered fd: %d\n", fd);

  if(fd == cont.listener_fd) {

    
    ts_listener_callback(fd, active_fd_set);
  }
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
