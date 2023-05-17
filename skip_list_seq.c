/////////////////////////////////////////////////////////////////////////////////////
/* SKIP LIST SEQUENTIAL */
/////////////////////////////////////////////////////////////////////////////////////

#ifndef SKIP_LIST_SEQ
#define SKIP_LIST_SEQ
#include "skip_list_seq.h"
#endif



bool init_skip_list_seq(Skip_list_seq* slist, unsigned int max_level){
    Node_seq* header = NULL;
    Node_seq* tail = NULL;

    init_random_seq();

    if(!init_node_seq(&header, INT_MAX, 0, 
    max_level)){ return false; }

    if(!init_node_seq(&tail, INT_MIN, 0, 
    max_level)){ return false; }

    for(int i = 0; i <= max_level; i++){
        header->nexts[i] = NULL;
        header->prevs[i] = tail;
        tail->nexts[i] = header;
        tail->prevs[i] = NULL;
    }

    slist->header = header;
    slist->tail = tail;
    slist->max_level = max_level;

    return true;
}


bool init_node_seq(Node_seq** node, int key, int value, unsigned int level){
    
    *node = (Node_seq*)malloc(sizeof(Node_seq));

    if(!*node){
        fprintf(stderr, "Malloc failed");
        return false;
    }

    (*node)->level = level;
    (*node)->key = INT_MAX;
    (*node)->value = 0;

    (*node)->prevs = (Node_seq**)malloc((level+1)*sizeof(Node_seq*));
    (*node)->nexts = (Node_seq**)malloc((level+1)*sizeof(Node_seq*));

    if(!(*node)->prevs || !(*node)->nexts){
        fprintf(stderr, "Malloc failed");
        return false;
    }
    
    return true;

}


bool add_skip_list_seq(Skip_list_seq* slist, int key, int value){
    Node_seq* new_node = NULL;
    Node_seq* proto[slist->max_level+1];
    memset(proto, -1, slist->max_level+1);

    new_node = find_skip_list_seq(slist, key, proto);

    if(new_node){
        new_node->value = value;
        return true;
    }


    new_node = (Node_seq*)malloc(sizeof(Node_seq));
    if(!new_node){
        fprintf(stderr, "Malloc failed");
        return false;
    }
    new_node->level = random_level_generator_seq(slist->max_level);
    new_node->key = key;
    new_node->value = value;

    new_node->nexts = (Node_seq**)malloc((new_node->level+1)*sizeof(Node_seq*));
    new_node->prevs = (Node_seq**)malloc((new_node->level+1)*sizeof(Node_seq*));
    if(!new_node->nexts || !new_node->prevs){
        fprintf(stderr, "Malloc failed");
        return false;
    }

    for(int i = new_node->level; i >= 0; i--){
        new_node->prevs[i] = proto[i];
        new_node->nexts[i] = proto[i]->nexts[i];
        proto[i]->nexts[i]->prevs[i] = new_node;
        proto[i]->nexts[i]= new_node;
    }
    
    return true;

}


bool remove_skip_list_seq(Skip_list_seq* slist, int key){
    Node_seq* node = find_skip_list_seq(slist, key, NULL);

    if(node){
        for(int i = node->level; i >= 0; i--){
            node->prevs[i]->nexts[i] = node->nexts[i];
            node->nexts[i]->prevs[i] = node->prevs[i];
        }
        free_node_seq(node);
        return true;
    }
    return false;
}


bool contains_skip_list_seq(Skip_list_seq* slist, int key){
    
    if(find_skip_list_seq(slist, key, NULL)){
        return true;
    }
    return false;
}


Node_seq* find_skip_list_seq(Skip_list_seq* slist, int key, Node_seq** proto){
    Node_seq* node = NULL;
    Window_seq* w = (Window_seq*)malloc(sizeof(Window_seq));
    if(!w){
        fprintf(stderr, "Malloc failed");
        return NULL;
    }
    w->pred = slist->tail;
    w->curr = slist->header;
    
    for(int i = slist->max_level; i >= 0; i--){
        node = find_list_seq(slist, i, key, w);
        if(node->key == key){
            free_window_seq(w);
            return node;
        }
        if(proto){
            proto[i] = node;
        }
    }
    free_window_seq(w);

    return NULL;
}

Node_seq* find_list_seq(Skip_list_seq* slist, unsigned int level, int key, Window_seq* w){

    while(w->pred != w->curr && w->pred->nexts[level] != w->curr){
        if(w->pred->nexts[level]->key == key){
            return w->pred->nexts[level];
        }
        else if(w->pred->nexts[level]->key < key){
            w->pred = w->pred->nexts[level];
        }

        if(w->curr->prevs[level]->key == key){
            return w->curr->prevs[level];
        }
        else if(w->curr->prevs[level]->key > key){
            w->curr = w->curr->prevs[level];
        }
    }

    return w->pred;
}


void init_random_seq(){
    time_t t;
    srand((unsigned) time(&t));
} 

int random_level_generator_seq(unsigned int max_level){
    unsigned int level = 0;
    double random_number = (double)rand()/(double)RAND_MAX;
    while(random_number < FRACTION && level <= max_level){
        level++;
    }
    return level;
}


void free_node_seq(Node_seq* node){
    if(node->nexts){
        free(node->nexts);
    }
    if(node->prevs){
        free(node->prevs);
    }
}

void free_skip_list_seq(Skip_list_seq* slist){
    if(slist->tail){
        free_node_seq(slist->tail);
    }
    if(slist->header){
        free_node_seq(slist->header);
    }
}


void free_window_seq(Window_seq* w){
    free(w);
}

void print_skip_list_seq(Skip_list_seq* slist){
    Node_seq* node = NULL;

    node = slist->tail->nexts[0];

    fprintf(stdout, "Skip_list: ");

    while(node->nexts[0]){
        fprintf(stdout, "(%d, %d) ", node->key, node->value);
        node = node->nexts[0];
    }
    fprintf(stdout, "\n");
}
