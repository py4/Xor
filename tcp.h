#ifndef TCP_H_
#define TCP_H_

#include "tcp.h"
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

#define MAXMSG 512

typedef void (*socket_callback)(int, fd_set*, fd_set*);

int create_socket(int);
int read_from_socket(int,char[]);
int start_server(int,socket_callback);
void listen_on(int,int);

/* samples */
void sample_callback(int, fd_set*, fd_set*);
void sample_req_callback(int, fd_set*);
void sample_read_callback(int, fd_set*);


#endif
