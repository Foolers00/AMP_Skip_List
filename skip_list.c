#ifndef SKIP_LIST
#define SKIP_LIST
#include "skip_list.h"
#endif



void init_skip_list(Skip_list* slist, int max_level){
    Node** h_next;
    Node** t_next;
    Node* header;
    Node* tail;

    header = (Node*)malloc(sizeof(Node));
    tail = (Node*)malloc(sizeof(Node));

    if(header == NULL || tail == NULL){
        fprintf(stderr, "Malloc failed");
    }

    header->level = max_level;
    header->value = INT_MAX;

    tail->level = max_level;
    tail->value = INT_MIN;

    h_next = (Node**)malloc(max_level*sizeof(Node*));
    t_next = (Node**)malloc(max_level*sizeof(Node*));

    if(h_next == NULL || t_next == NULL){
        fprintf(stderr, "Malloc failed");
    }

    for(int i = 0; i < max_level; i++){
        h_next[i] = NULL;
        t_next[i] = header;
    }

    header->next = h_next;
    tail->next = t_next;

    slist->header = header;
    slist->tail = tail;
    slist->curr_level = 0;
    slist->max_level = max_level;
}