#include <stdio.h>
#include "list.h"
int main() {
  List* l;
  init_list(l);
  insert_node(l, (void*)1);
  insert_node(l, (void*)2);
  insert_node(l, (void*)3);
  insert_node(l, (void*)4);
  dump_as_int(l);
  return 0;
}
