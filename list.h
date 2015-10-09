#ifndef LIST_H_
#define LIST_H_
typedef struct Node {
  void* data;
  struct Node* next;
  struct Node* prev;
} Node;

typedef struct List {
  Node* head;
  Node* tail;
  int size;
} List;

void init_list(List*);
void insert_node(List* l, void* data);
void* remove_node(List* l, Node* node);
void dump_as_int(List* l);

#endif
