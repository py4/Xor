#ifndef TORRENT_CLIENT_H_
#define TORRENT_CLIENT_H_

#include "tcp.h"

void tc_stdin_callback(int, fd_set*); // user has types
void tc_listener_callback(int, fd_set*); // client has requested
void tc_server_callback(int, fd_set*); // server has sent sth
void tc_client_callback(int, fd_set*); // client has sent sth
void tc_event_callback(int, fd_set*, struct SockCont);
void start_tc(int, char*, int);
#endif
