#ifndef CLIENT_DB_
#define CLIENT_DB_

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define MAXMSG 512
#define MAX_ENTRIES 1000

typedef struct Entry {
  char path[MAXMSG];
  char name[MAXMSG];
} Entry;

typedef struct FileDB {
  Entry* entries[MAX_ENTRIES];
  int num_of_entries;
  int init;
} FileDB;


void init_tc_db(FileDB*);
void add_to_db(FileDB*,char*,char*);
void clear_db(FileDB*);
void get_entry_path(FileDB*,char*,char*);
void dump_tc_db(FileDB*);

#endif
