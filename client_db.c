#include "client_db.h"

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

void dump_tc_db(FileDB* db) {
	int i;
  csnprintf("==================================\n");
  for(i = 0; i < db->num_of_entries; i++)
    csnprintf("%s : %s\n", db->entries[i]->name, db->entries[i]->path);
  csnprintf("==================================\n");
}

