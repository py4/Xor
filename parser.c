#include "parser.h"
#include <stdio.h>
#include <string.h>

void sanitize_buffer(char* buffer) {
  if('\n' == buffer[strlen(buffer) - 1])
    buffer[strlen(buffer) - 1] = '\0';
}
void extract_command(char* buffer, char* command) {
  int i;
  for(i = 0; i < strlen(buffer) && buffer[i] != ' '; i++)
    command[i] = buffer[i];
  command[i] = '\0';
}

void reverse(char* buffer) {
	int i;
  for(i = 0; i < strlen(buffer) / 2; i++) {
    char tmp = buffer[i];
    buffer[i] = buffer[strlen(buffer) - 1 - i];
    buffer[strlen(buffer) - 1 - i] = tmp;
  }
}

void extract_path(char* buffer, char* path) {
	int i;
  for(i = strlen(buffer) - 1; i >= 0 && buffer[i] != ' '; i--)
    path[strlen(buffer) - 1 - i] = buffer[i];
  path[strlen(buffer)] = '\0';
  reverse(path);
}

void extract_filename(char* buffer, char* file_name) {
  int i;
  //printf("[debug] strlen: %d\n", strlen(buffer));
  for(i = strlen(buffer) - 1; i >= 0 && buffer[i] != '/' && buffer[i] != ' '; i--) {
    file_name[strlen(buffer) - 1 - i] = buffer[i];
    //printf("saving %c at %d\n", buffer[i], strlen(buffer) - 1);
  }
  file_name[strlen(buffer) - 1 - i] = '\0';
  //  printf("[debug] before: %s\n", file_name);
  reverse(file_name);
}

void int_to_char(int num, char* buf) {
  sprintf(buf,"%d", num);
}

void init_string(char* arr, int size) {
  memset(arr, '\0', size*sizeof(char));
}

void parse_lookup_output(char* buffer, int* success, char* ip, int *port) {
  sscanf(buffer, "%d %s %d", success, ip, port);
}
