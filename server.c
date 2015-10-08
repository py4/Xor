#include "tcp.h"

#define PORT 5555
#define MAXMSG 512

// torrent server starts a server
// torrent client starts a client and a server
// clients sends message to torrent server to register themselves
// clients send message to torrent server to find file
// torrent server send host information to owner of the request
// requester connets to file owner
// requester send file name to file owner
// file owner send data to requester

int main(int argc, char** argv) {
  start_server(5555, sample_callback);
  return 0;
}
