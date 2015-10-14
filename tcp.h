#ifndef TCP_H_
#define TCP_H_

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
#include <fcntl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/stat.h> 

#define MAXMSG 512
#define MAXFN 512
#define BUFSIZE 512
#define MAX_NAME_SIZE 512

typedef struct SockCont {
  int stdin_fd;
  int server_fd;
  int listener_fd;
} SockCont;

typedef void(*callback)(int, fd_set*, struct SockCont);
	     
int create_socket(int);
int read_from_socket(int,char[]);
void listen_on(int,int);

int sample_req_callback(int, fd_set*, char*);
int sample_read_callback(int, fd_set*, char*);
void sample_stdin_res_handler(int);

void monitor(struct SockCont, callback);

int create_connector_socket(char*,int);
void read_from_stdin();
void write_msg(int, char*);
void disconnect(int,int,int);
void download_data(int, char*);
void send_data(int, char*);
void write_int(int,int);
#endif
