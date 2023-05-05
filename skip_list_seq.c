#ifndef SKIP_LIST_SEQ
#define SKIP_LIST_SEQ
#include "skip_list_seq.h"
#endif



void init_skip_list(Skip_list* slist, int max_level){
    Node* header = NULL;
    Node* tail = NULL;
    Node** h_next = NULL;
    Node** t_next = NULL;
    Node** h_prev = NULL;
    Node** t_prev = NULL;


    header = (Node*)malloc(sizeof(Node));
    tail = (Node*)malloc(sizeof(Node));

    if(!header || !tail){
        fprintf(stderr, "Malloc failed");
    }

    header->level = max_level;
    header->value = INT_MAX;

    tail->level = max_level;
    tail->value = INT_MIN;

    h_next = (Node**)malloc(max_level*sizeof(Node*));
    t_next = (Node**)malloc(max_level*sizeof(Node*));
    h_prev = (Node**)malloc(max_level*sizeof(Node*));
    t_prev = (Node**)malloc(max_level*sizeof(Node*));

    if(!h_next || !t_next || !h_prev || !t_prev){
        fprintf(stderr, "Malloc failed");
    }

    for(int i = 0; i < max_level; i++){
        h_next[i] = NULL;
        h_prev[i] = tail;
        t_next[i] = header;
        t_next[i] = NULL;
    }

    slist->header = header;
    slist->tail = tail;
    slist->curr_level = 0;
    slist->max_level = max_level;
}


void add_skip_list(Skip_list* slist, int key, int value){
    Node* new_node = NULL;
    Node* proto[slist->max_level+1];

    new_node = find_skip_list(slist, key, proto);

    if(new_node){
        new_node->value = value;
        return;
    }


    new_node = (Node*)malloc(sizeof(Node));
    if(!new_node){
        fprintf(stderr, "Malloc failed");
    }
    new_node->level = random_level_generator(slist->max_level);
    new_node->value = value;

    new_node->next = (Node**)malloc(new_node->level*sizeof(Node*));
    new_node->prev = (Node**)malloc(new_node->level*sizeof(Node*));
    if(!new_node->next || !new_node->prev){
        fprintf(stderr, "Malloc failed");
    }

    for(int i = new_node->level; i >= 0; i--){
        new_node->prev[i] = proto[i];
        new_node->next[i] = proto[i]->next[i];
        proto[i]->next[i] = new_node;
    }


}


void remove_skip_list(Skip_list* slist, int key){
    Node* node = find_skip_list(slist, key, NULL);

    if(node){
        for(int i = node->level; i >= 0; i--){
            node->prev[i] = node->next[i];
        }
    }

    free_node(node);
}


Node* find_skip_list(Skip_list* slist, int key, Node** proto){
    Node* node = NULL;
    
    for(int i = slist->max_level; i >= 0; i--){
        node = find_list(slist, i, key);
        if(node->key == key){
            return node;
        }
        if(proto){
            proto[i] = node;
        }
    }

    return NULL;
}

Node* find_list(Skip_list* slist, int level, int key){
    Node* t_node = slist->tail;
    Node* h_node = slist->header;

    while(t_node->next[level] != h_node){
        if(t_node->next[level]->key == key){
            return t_node->next[level];
        }
        else if(t_node->next[level]->key < key){
            t_node = t_node->next[level];
        }

        if(h_node->prev[level]->key == key){
            return h_node->prev[level];
        }
        else if(h_node->prev[level]->key > key){
            h_node = h_node->prev[level];
        }
    }

    return t_node;
}


void init_random(){
    time_t t;
    srand((unsigned) time(&t));
} 

int random_level_generator(int max_level){
    int level = 0;
    while(rand() % 2 == 0 && level > max_level){
        level++;
    }
    return level;
}


void free_node(Node* node){
    if(node->next){
        free(node->next);
    }
    if(node->prev){
        free(node->prev);
    }
}

void free_skip_list(Skip_list* slist){
    if(slist->tail){
        free_node(slist->tail);
    }
    if(slist->header){
        free_node(slist->header);
    }
}

void print_skip_list(Skip_list* slist){
    Node* node = NULL;

    node = slist->tail->next[0];

    fprintf(stdout, "Skip_list: ");

    while(node){
        fprintf(stdout, "(%d, %d) ", node->key, node->value);
        node = node->next[0];
    }
    fprintf(stdout, "\n");
}