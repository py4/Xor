#ifndef TORRENT_CLIENT_H_
#define TORRENT_CLIENT_H_

#define MAX_ENTRIES 1000

#include "tcp.h"

typedef struct Entry {
  char path[MAXMSG];
  char name[MAXMSG];
} Entry;

typedef struct FileDB {
  Entry* entries[MAX_ENTRIES];
  int num_of_entries;
  int init;
} FileDB;


void tc_stdin_callback(int, fd_set*, SockCont, FileDB*); // user has types
void tc_listener_callback(int, fd_set*, SockCont); // client has requested
void tc_server_callback(int, fd_set*, SockCont); // server has sent sth
void tc_client_callback(int, fd_set*, SockCont); // client has sent sth
void tc_event_callback(int, fd_set*, SockCont);
void send_lport_to_ts(int,int);
void start_tc(char*, int);



void init_tc_db(FileDB*);
void add_to_db(FileDB*,char*,char*);
void clear_db(FileDB*);
void get_entry_path(FileDB*,char*,char*);
void dump_tc_db(FileDB*);



#endif
