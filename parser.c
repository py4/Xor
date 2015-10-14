#include "parser.h"
#include <unistd.h>

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
  path[strlen(buffer) - 1 - i] = '\0';
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
  sprintf(buf,"%d", num);
}

void init_string(char* arr, int size) {
  memset(arr, '\0', size*sizeof(char));
}

void parse_lookup_output(char* buffer, int* success, char* ip, int *port) {
  sscanf(buffer, "%d %s %d", success, ip, port);
}

/*void cstrcat(char* result, int num, ...) {
  va_list arguments;
  va_start(arguments, num);
  for(int i = 0; i < num; i++)
    strcat(result, va_arg(arguments,char*));
}*/

/*void cprintf(int num, ...) {
  char result[1000];
  memset(result,'\0',sizeof(result));
  va_list arguments;
  va_start(arguments, num);
  for(int i = 0; i < num; i++) {
    char* a = va_arg(arguments, char*);
    write(STDOUT_FILENO, a, strlen(a));
  }
  }*/


void csnprintf(const char* format, ...) {
  int count = 0;
  for(int i = 0; i < strlen(format); i++)
    if(format[i] == '%')
      count++;
  
  va_list args;
  va_start(args, format);
  char result[1000];
  memset(result, '\0', sizeof(result));
  vsnprintf(result, sizeof(result), format, args);
  write(STDOUT_FILENO, result, sizeof(result));
}

void cperror(const char* prefix) {
  csnprintf("%s: %s\n", prefix, strerror(errno));
}
