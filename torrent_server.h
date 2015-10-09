#ifndef TORRENT_SERVER_H
#define TORRENT_SERVER_H

#include "tcp.h"
#define MAX_OWNERS 500
#define MAX_CONNECTED 500
#define MAX_FILES 500

void ts_listener_callback(int, fd_set*);
void ts_client_callback(int, fd_set*);
void ts_event_callback(int, fd_set*, struct SockCont);
void start_ts(int);


typedef struct ConnectedClient {
  int fd;
  char* ip;
  int port;
  FileEntry* owned_files[MAX_FILES];
  int num_of_files;
} ConnectedClient;

typedef struct TorrentDB {
  ConnectedClient* connected_clients[MAX_CONNECTED];
  FileEntry* file_entries[MAX_FILES];
  int num_of_connected;
  int num_of_entries;  
} TorrentDB;


typedef struct FileEntry {
  char* name;
  ConnectedClient* owners[MAX_OWNERS];
  int last_owner;
  int num_of_owners;
} FileEntry;

void init_db(TorrentDB*);
void init_file_entry(FileEntry*);
void add_client(TorrentDB*,char*,int,int);
ConnectedClient* find_connected_client(TorrentDB*,int);
FileEntry* find_file_entry(TorrentDB*,char*);
void add_entry(TorrentDB*,char*,int);
  
#endif
