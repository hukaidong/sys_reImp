#ifndef MY_LINKED_MAP_NEW_H
#define MY_LINKED_MAP_NEW_H

#include "my_malloc.h"
#include "utils.h"

#define __define_lmap(node_t, head_t, key_t, val_t, name) \
\
typedef struct node_t {\
  key_t key;\
  val_t val;\
  struct node_t *next;\
} node_t;\
\
typedef struct head_t {\
  node_t *next;\
} head_t; \
\
void _f_##head_t##_init(head_t** init_head) {\
  head_t *head = (head_t *)_lib_malloc(sizeof(head_t));\
  head->next = NULL;\
  *init_head = head;\
}\
\
void _f_##head_t##_insert(head_t* head, key_t key, val_t val) {\
  node_t *new_node = (node_t*)_lib_malloc(sizeof(node_t));\
  new_node->key = key;\
  new_node->val = val;\
  new_node->next = head->next;\
  head->next = new_node;\
}\
\
int _f_##head_t##_pop(head_t *head, key_t key, val_t *val){\
  node_t **last = &head->next, \
         *current = head->next;\
  while (current != NULL) {\
    if (current->key == key) {\
      *val = current->val;\
      *last = current->next;\
      _lib_free(current);\
      return 1;\
    }\
    last = &current->next;\
    current = current->next;\
  }\
  return -1;\
}\
\
int _f_##head_t##_peek(head_t *head, key_t key, val_t *val){\
  node_t *current = head->next;\
  while (current != NULL) {\
    if (current->key == key) {\
      *val = current->val;\
      return 1;\
    }\
    current = current->next;\
  }\
  return -1;\
}\


#define define_lmap(name, key_t, val_t) \
  __define_lmap(__lmp_node_##name, __lmp_head_##name, key_t, val_t, name)
#define lm_head_t(name) __lmp_head_##name*
#define lm_node_t(name) __lmp_node_##name
#define lm_h_init(head) __lmp_init_##head_t(&head)
#define lm_insert(head, key, val)\
  __typeof__(head)_insert(head, key, val)
#define lm_pop(head, key, val)\
  __typeof__(head)_pop(head, key, val)
#define lm_peek(head, key, val)\
  __typeof__(head)_peek(head, key, val)



// Example:
// define_lmap(my_map, int, int);
// lm_head_t(my_map) map;
// lm_h_init(map);
// lmp_insert(map, 1, 1);
// int a;
// lmp_pop(map, 1, &a);  # return 1, a == 1
//
#endif /* ifndef MY_LINKED_MAP_NEW_H */
