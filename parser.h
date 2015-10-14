#ifndef PARSER_H_
#define PARSER_H_

#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>


void extract_command(char*, char*);
void extract_filename(char*, char*);
void extract_path(char*, char*);
void sanitize_buffer(char*);
void int_to_char(int, char*);
void init_string(char*, int);
void parse_lookup_output(char*, int*, char*, int *);

void csnprintf(const char* format,...);
void cperror(const char*);
#endif
