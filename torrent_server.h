#ifndef TORRENT_SERVER_H
#define TORRENT_SERVER_H

#include "tcp.h"
#include "server_db.h"

void ts_listener_callback(int, fd_set*, TorrentDB*);
void ts_client_callback(int, fd_set*, TorrentDB*);
void ts_event_callback(int, fd_set*, struct SockCont);
void start_ts(int);
void ts_register_file(int,char*,TorrentDB*);


#endif
