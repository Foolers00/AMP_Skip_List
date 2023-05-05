#ifndef STDLIB
#define STDLIB
#include <stdlib.h>
#endif

#ifndef STDIO
#define STDIO
#include <stdio.h>
#endif

#ifndef LIMITS
#define LIMITS
#include <limits.h>
#endif

typedef struct Node{
    struct Node** next;
    int value;
    int level;
}Node;


typedef struct Skip_list{
    Node* header;
    Node* tail;
    int curr_level;
    int max_level;
}Skip_list;


void init_skip_list(Skip_list* slist, int max_level);