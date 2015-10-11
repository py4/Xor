#ifndef TORRENT_SERVER_H
#define TORRENT_SERVER_H

#include "tcp.h"
#define MAX_OWNERS 500
#define MAX_CONNECTED 500
#define MAX_FILES 500
#define MAX_IP_SIZE 500
#define MAX_NAME_SIZE 500

struct FileEntry;
struct ConnectedClient;
typedef struct ConnectedClient {
  int fd;
  char ip[MAX_IP_SIZE];
  int port;
  struct FileEntry* owned_files[MAX_FILES];
  int num_of_files;
} ConnectedClient;

typedef struct TorrentDB {
  ConnectedClient* connected_clients[MAX_CONNECTED];
  struct FileEntry* file_entries[MAX_FILES];
  int num_of_connected;
  int num_of_entries;
  int init;
} TorrentDB;


typedef struct FileEntry {
  char name[MAX_NAME_SIZE];
  ConnectedClient* owners[MAX_OWNERS];
  int last_who_gave;
  int num_of_owners;
} FileEntry;


void ts_listener_callback(int, fd_set*, TorrentDB*);
void ts_client_callback(int, fd_set*, TorrentDB*);
void ts_event_callback(int, fd_set*, struct SockCont);
void start_ts(int);


void init_db(TorrentDB*);
void init_connected_client(ConnectedClient*, char*, int, int);
void init_file_entry(FileEntry*, char*, ConnectedClient*);
ConnectedClient* find_connected_client(TorrentDB*,int);
FileEntry* find_file_entry(TorrentDB*,char*);
void add_client(TorrentDB*,char*,int,int);
void add_entry(TorrentDB*,char*,int);
void remove_client_from_entry(FileEntry*, ConnectedClient*);
void remove_entry_from_db(TorrentDB*, FileEntry*);
void remove_client(TorrentDB*, int);
ConnectedClient* get_current_seeder(FileEntry*);
ConnectedClient* get_a_seeder(TorrentDB*, char*);
void ts_register_file(int,char*,TorrentDB*);
#endif
