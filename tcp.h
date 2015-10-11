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
#include "parser.h"

#define MAXMSG 512

typedef struct SockCont {
  int stdin_fd;
  int server_fd;
  int listener_fd;
} SockCont;

//typedef void (*socket_callback)(int, fd_set*, fd_set*);
typedef void(*callback)(int, fd_set*, struct SockCont);
	     
int create_socket(int);
int read_from_socket(int,char[]);
//int start_server(int,socket_callback);
void listen_on(int,int);

/* samples */
//void sample_callback(int, fd_set*, fd_set*);
int sample_req_callback(int, fd_set*, char*);
int sample_read_callback(int, fd_set*, char*);
void sample_stdin_res_handler(int);

//void monitor(int,callback,int,callback,int,callback,callback);
void monitor(struct SockCont, callback);

int create_connector_socket(char*,int);
void read_from_stdin();
void write_msg(int, char*);

#endif
