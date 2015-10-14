#include "server_db.h"

void dump_db(TorrentDB* db) {
	int i,j;
  csnprintf("==========================================\n");
  for(i = 0; i < db->num_of_connected; i++) {
    csnprintf("user with fd %d \n", db->connected_clients[i]->fd);
    for(j = 0; j < db->connected_clients[i]->num_of_files; j++)
      csnprintf("name: %s\n", db->connected_clients[i]->owned_files[j]->name);
    csnprintf("-----------------------\n");
  }
  csnprintf("===========================================\n");
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
    csnprintf("[ts] client not found \n");
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
