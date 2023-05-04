

#ifndef LIMITS
#define LIMITS
#include <limits.h>
#endif

typedef struct Node{
    Node** next;
    int value;
    int level;
}Node;


typedef struct Skip_list{
    Node* header;
    Node* tail;
    int curr_level;
    int max_level;
}Skip_list;


void init_skip_list(int max_level);