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
  for(int i = 0; i < strlen(buffer) / 2; i++) {
    char tmp = buffer[i];
    buffer[i] = buffer[strlen(buffer) - 1 - i];
    buffer[strlen(buffer) - 1 - i] = tmp;
  }
}

void extract_path(char* buffer, char* path) {
  for(int i = strlen(buffer) - 1; i >= 0 && buffer[i] != ' '; i--)
    path[strlen(buffer) - 1 - i] = buffer[i];
  path[strlen(buffer)] = '\0';
  reverse(path);
}

void extract_filename(char* buffer, char* file_name) {
  int i;
  for(i = strlen(buffer) - 1; i >= 0 && buffer[i] != '/' && buffer[i] != ' '; i--)
    file_name[strlen(buffer) - 1 - i] = buffer[i];
  file_name[strlen(buffer) - 1 - i] = '\0';
  reverse(file_name);
}

void int_to_char(int num, char* buf) {
  //memset(buf, '\0', sizeof(buf));
  sprintf(buf,"%d", num);
}

void init_string(char* arr, int size) {
  memset(arr, '\0', size*sizeof(char));
}
