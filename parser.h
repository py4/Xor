#ifndef PARSER_H_
#define PARSER_H_

void extract_command(char*, char*);
void extract_filename(char*, char*);
void extract_path(char*, char*);
void sanitize_buffer(char*);
void int_to_char(int, char*);
void init_string(char*, int);
void parse_lookup_output(char*, int*, char*, int *);
#endif
