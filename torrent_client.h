#ifndef TORRENT_CLIENT_H_
#define TORRENT_CLIENT_H_

#include "tcp.h"
#include "client_db.h"


void tc_stdin_callback(int, fd_set*, SockCont, FileDB*);
void tc_listener_callback(int, fd_set*, SockCont, FileDB*);
void tc_server_callback(int, fd_set*, SockCont);
void tc_client_callback(int, fd_set*, SockCont);
void tc_event_callback(int, fd_set*, SockCont);
void start_tc(char*, int);






#endif
