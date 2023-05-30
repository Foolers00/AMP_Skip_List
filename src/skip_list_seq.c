/////////////////////////////////////////////////////////////////////////////////////
/* SKIP LIST SEQUENTIAL */
/////////////////////////////////////////////////////////////////////////////////////

#ifndef SKIP_LIST_SEQ
#define SKIP_LIST_SEQ
#include "skip_list_seq.h"
#endif



bool init_skip_list_seq(Skip_list_seq* slist, int max_level){
    Node_seq* header = NULL;
    Node_seq* tail = NULL;

    init_random_seq();

    if(!init_node_seq(&tail, INT_MAX, 0, 
    max_level)){ return false; }

    if(!init_node_seq(&header, INT_MIN, 0, 
    max_level)){ return false; }

    for(int i = 0; i <= max_level; i++){
        header->nexts[i] = tail;
        tail->nexts[i] = NULL;
    }

    slist->header = header;
    slist->tail = tail;
    slist->max_level = max_level;

    return true;
}


bool init_node_seq(Node_seq** node, int key, int value, int level){
    
    *node = (Node_seq*)malloc(sizeof(Node_seq));

    if(!*node){
        fprintf(stderr, "Malloc failed");
        return false;
    }

    (*node)->level = level;
    (*node)->key = key;
    (*node)->value = value;

    (*node)->nexts = (Node_seq**)malloc((level+1)*sizeof(Node_seq*));

    if(!(*node)->nexts){
        fprintf(stderr, "Malloc failed");
        return false;
    }
    
    return true;

}


bool add_skip_list_seq(Skip_list_seq* slist, int key, int value){
    Node_seq* new_node = NULL;
    Window_seq proto[slist->max_level+1];

    new_node = find_skip_list_seq(slist, key, proto);

    if(new_node){
        new_node->value = value;
        return true;
    }

    init_node_seq(&new_node, key, value, random_level_generator_seq(slist->max_level));
    

    for(int i = new_node->level; i >= 0; i--){
        new_node->nexts[i] = proto[i].curr;
        proto[i].pred->nexts[i] = new_node;
    }
    
    return true;

}


bool remove_skip_list_seq(Skip_list_seq* slist, int key){

    Window_seq proto[slist->max_level+1];
    Node_seq* node = find_skip_list_seq(slist, key, proto);

    if(node){
        for(int i = node->level; i >= 0; i--){
            proto[i].pred->nexts[i] = node->nexts[i];
        }
        free_node_seq(node);
        return true;
    }
    return false;
}


bool contains_skip_list_seq(Skip_list_seq* slist, int key){
    
    Window_seq proto[slist->max_level+1];

    if(find_skip_list_seq(slist, key, proto)){
        return true;
    }
    return false;
}


Node_seq* find_skip_list_seq(Skip_list_seq* slist, int key, Window_seq proto[]){
    
    Node_seq* node = NULL;

    proto[slist->max_level].pred = slist->header;
    proto[slist->max_level].curr = slist->tail;
    
    for(int i = slist->max_level; i >= 0; i--){
        node = find_list_seq(i, key, &proto[i]);
        
        if(i != 0){
            proto[i-1].pred = proto[i].pred;
            proto[i-1].curr = proto[i].curr;
        }
    }

    return node;
}


Node_seq* find_list_seq(int level, int key, Window_seq* w){

    while(w->pred != w->curr && w->pred->nexts[level]->key < key){
        w->pred = w->pred->nexts[level];
    }
    w->curr = w->pred->nexts[level];

    if(w->curr->key == key){
        return w->curr;
    }
    
    return NULL;

}


void init_random_seq(){
    time_t t;
    srand((unsigned) time(&t));
} 


int random_level_generator_seq(int max_level){
    int level = 0;
    double random_number = (double)rand()/(double)RAND_MAX;
    while(random_number < FRACTION && level < max_level){
        level++;
    }
    return level;
}


void free_node_seq(Node_seq* node){
    if(node->nexts){
        free(node->nexts);
    }
    free(node);
}


void free_skip_list_seq(Skip_list_seq* slist){

    Node_seq* node = slist->tail;
    Node_seq* node_next = slist->tail->nexts[0];

    while(node && node_next){
        free_node_seq(node);
        node = node_next;
        node_next = node_next->nexts[0];
    }
    
    free_node_seq(node);
    
}


void free_window_seq(Window_seq* w){
    free(w);
}


void print_skip_list_seq(Skip_list_seq* slist){

    Node_seq* node = NULL;

    node = slist->header->nexts[0];

    printf("Skip_list_seq: ");

    while(node->nexts[0]){
        printf("(%d, %d) ", node->key, node->value);
        node = node->nexts[0];
    }
    printf("\n");

}
