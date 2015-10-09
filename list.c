#include "list.h"

void init_list(List* l) {
  l->head = 0;
  l->tail = 0;
  l->size = 0;
}

void insert_node(List* l, void* data) {
  Node* node = malloc(sizeof(node));
  node->next = 0;
  node->prev = 0;
  node->data = data;
  if(l->tail == 0) { //list is empty
    l->head = node;
    l->tail = node;
  } else {
    l->tail->next = node;
    node->prev = l->tail;
    l->tail = node;
  }
  l->size++;
}

void* remove_node(List* l, Node* node) {
  void* data = node->data;
  l->size--;
  if(l->size == 0) {
    l->head = 0;
    l->tail = 0;
  } else {
    if(node->next == 0) { // it's tail
      l->tail = node->prev;
      l->tail->next = 0;
      
    } else if(node->prev == 0) { // it's head
      l->head = node->next;
      l->head->prev = 0;
    } else { // it's in middle
      node->prev->next = node->next;
      node->next->prev = node->prev;
    }
  }
  free(node);
  return data;
}

void dump_as_int(List* l) {
  for(Node* node = l->head; node != 0; node = node->next)
    printf("%d ", (int)node->data);
  printf("\n");
}
