#ifndef TORRENT_SERVER_H
#define TORRENT_SERVER_H

#include "tcp.h"

void ts_listener_callback(int, fd_set*);
void ts_client_callback(int, fd_set*);
void start_ts(int);

#endif
