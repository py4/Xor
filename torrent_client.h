#ifndef TORRENT_CLIENT_H_
#define TORRENT_CLIENT_H_

#include "tcp.h"

void tc_stdin_callback(int, fd_set*, SockCont); // user has types
void tc_listener_callback(int, fd_set*, SockCont); // client has requested
void tc_server_callback(int, fd_set*, SockCont); // server has sent sth
void tc_client_callback(int, fd_set*, SockCont); // client has sent sth
void tc_event_callback(int, fd_set*, SockCont);
void start_tc(int, char*, int);
#endif
