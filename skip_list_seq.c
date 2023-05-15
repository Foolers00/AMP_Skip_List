/////////////////////////////////////////////////////////////////////////////////////
/* SKIP LIST SEQUENTIAL */
/////////////////////////////////////////////////////////////////////////////////////

#ifndef SKIP_LIST_SEQ
#define SKIP_LIST_SEQ
#include "skip_list_seq.h"
#endif



void init_skip_list_seq(Skip_list_seq* slist, int max_level){
    Node_seq* header = NULL;
    Node_seq* tail = NULL;
    Node_seq** h_next = NULL;
    Node_seq** t_next = NULL;
    Node_seq** h_prev = NULL;
    Node_seq** t_prev = NULL;

    init_random_seq();


    header = (Node_seq*)malloc(sizeof(Node_seq));
    tail = (Node_seq*)malloc(sizeof(Node_seq));

    if(!header || !tail){
        fprintf(stderr, "Malloc failed");
    }

    header->level = max_level;
    header->key = INT_MAX;
    header->value = 0;

    tail->level = max_level;
    tail->value = INT_MIN;
    header->value = 0;

    h_next = (Node_seq**)malloc((max_level+1)*sizeof(Node_seq*));
    t_next = (Node_seq**)malloc((max_level+1)*sizeof(Node_seq*));
    h_prev = (Node_seq**)malloc((max_level+1)*sizeof(Node_seq*));
    t_prev = (Node_seq**)malloc((max_level+1)*sizeof(Node_seq*));

    if(!h_next || !t_next || !h_prev || !t_prev){
        fprintf(stderr, "Malloc failed");
    }

    for(int i = 0; i <= max_level; i++){
        h_next[i] = NULL;
        h_prev[i] = tail;
        t_next[i] = header;
        t_prev[i] = NULL;
    }

    header->next = h_next;
    header->prev = h_prev;
    tail->next = t_next;
    tail->prev = t_prev;

    slist->header = header;
    slist->tail = tail;
    slist->max_level = max_level;
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

    new_node->next = (Node_seq**)malloc((new_node->level+1)*sizeof(Node_seq*));
    new_node->prev = (Node_seq**)malloc((new_node->level+1)*sizeof(Node_seq*));
    if(!new_node->next || !new_node->prev){
        fprintf(stderr, "Malloc failed");
        return false;
    }

    for(int i = new_node->level; i >= 0; i--){
        new_node->prev[i] = proto[i];
        new_node->next[i] = proto[i]->next[i];
        proto[i]->next[i]->prev[i] = new_node;
        proto[i]->next[i]= new_node;
    }
    
    return true;

}


bool remove_skip_list_seq(Skip_list_seq* slist, int key){
    Node_seq* node = find_skip_list_seq(slist, key, NULL);

    if(node){
        for(int i = node->level; i >= 0; i--){
            node->prev[i]->next[i] = node->next[i];
            node->next[i]->prev[i] = node->prev[i];
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

Node_seq* find_list_seq(Skip_list_seq* slist, int level, int key, Window_seq* w){

    while(w->pred != w->curr && w->pred->next[level] != w->curr){
        if(w->pred->next[level]->key == key){
            return w->pred->next[level];
        }
        else if(w->pred->next[level]->key < key){
            w->pred = w->pred->next[level];
        }

        if(w->curr->prev[level]->key == key){
            return w->curr->prev[level];
        }
        else if(w->curr->prev[level]->key > key){
            w->curr = w->curr->prev[level];
        }
    }

    return w->pred;
}


void init_random_seq(){
    time_t t;
    srand((unsigned) time(&t));
} 

int random_level_generator_seq(int max_level){
    int level = 0;
    while(rand() % 2 == 0 && level > max_level){
        level++;
    }
    return level;
}


void free_node_seq(Node_seq* node){
    if(node->next){
        free(node->next);
    }
    if(node->prev){
        free(node->prev);
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

    node = slist->tail->next[0];

    fprintf(stdout, "Skip_list: ");

    while(node->next[0]){
        fprintf(stdout, "(%d, %d) ", node->key, node->value);
        node = node->next[0];
    }
    fprintf(stdout, "\n");
}
