#include "parser.h"
#include <stdio.h>
#include <string.h>

void sanitize_buffer(char* buffer) {
  if('\n' == buffer[strlen(buffer) - 1])
    buffer[strlen(buffer) - 1] = '\0';
}
void extract_command(char* buffer, char* command) {
  for(int i = 0; i < strlen(buffer) && buffer[i] != ' '; i++)
    command[i] = buffer[i];
}

void extract_filename(char* buffer, char* file_name) {
  memset(file_name, '\0', sizeof(file_name));
  for(int i = strlen(buffer) - 1; i > 0 && buffer[i] != '/' && buffer[i] != ' '; i--)
    file_name[strlen(buffer) - 1 - i] = buffer[i];

  printf("before:  %s\n", file_name);
  
  for(int i = 0; i < strlen(file_name) / 2; i++) {
    char tmp = file_name[i];
    file_name[i] = file_name[strlen(file_name) - 1 - i];
    file_name[strlen(file_name) - 1 - i] = tmp;
  }

  printf("after:  %s\n", file_name);
}
