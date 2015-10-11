#include <stdio.h>
#include <string.h>
#include "torrent_server.h"
#include "torrent_client.h"

int main(int argc, char** argv) {
  if(strcmp(argv[1],"server") == 0) {
    printf("[TS] listening on port: %d\n",atoi(argv[2]));
    start_ts(atoi(argv[2]));
  }
  else if(strcmp(argv[1], "client") == 0) {
    start_tc(argv[2],atoi(argv[3]));
  }
  return 0;
}
