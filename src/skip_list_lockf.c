/////////////////////////////////////////////////////////////////////////////////////
/* SKIP LIST LOCK FREE */
/////////////////////////////////////////////////////////////////////////////////////

#ifndef SKIP_LIST_LOCKF
#define SKIP_LIST_LOCKF
#include "skip_list_lockf.h"
#endif

bool init_skip_list_lfree(Skip_list_lfree* slist, int max_level, int num_of_threads){

    Node_lfree* header = NULL;
    Node_lfree* tail = NULL;

    init_random_lfree(slist, num_of_threads);

    if(!init_node_lfree(&tail, INT_MAX, 0,
                    max_level)){ return false; }

    if(!init_node_lfree(&header, INT_MIN, 0,
                    max_level)){ return false; }

    #pragma omp parallel default(shared) num_threads(num_of_threads)
    {
        for(int i = 0; i <= max_level; i++){
            header->nexts[i] = tail;
            tail->nexts[i] = NULL;
        }
    }


    slist->header = header;
    slist->tail = tail;
    slist->max_level = max_level;

    return true;

}

bool init_node_lfree(Node_lfree** node, int key, int value, int level){

    *node = (Node_lfree*)malloc(sizeof(Node_lfree));

    if(!*node){
        fprintf(stderr, "Malloc failed");
        return false;
    }

    (*node)->level = level;
    (*node)->key = key;
    (*node)->value = value;

    (*node)->nexts = (Node_lfree **)malloc((level + 1) * sizeof(Node_lfree *));

    if(!(*node)->nexts){
        fprintf(stderr, "Malloc failed");
        return false;
    }

    return true;

}

bool init_random_lfree(Skip_list_lfree* slist, int num_of_threads){

    time_t t;
    int id;

    slist->random_seeds =
            (unsigned int*)malloc(sizeof(unsigned int)*num_of_threads);

    if(!slist->random_seeds){
        fprintf(stderr, "Malloc failed");
        return false;
    }

    #pragma omp parallel private(t, id) num_threads(num_of_threads)
    {
        id = omp_get_thread_num();
        slist->random_seeds[id] = (unsigned) time(&t) + id;
    }

    return true;


}

unsigned int random_level_generator_lfree(Skip_list_lfree* slist){

    int level = 0;
    unsigned int* seed = &slist->random_seeds[omp_get_thread_num()];
    int max_level = slist->max_level;

    double random_number = (double)rand_r(seed)/(double)RAND_MAX;

    while(random_number < FRACTION && level < max_level){
        level++;
    }

    return level;

}


bool add_skip_list_lfree(Skip_list_lfree* slist, int key, int value){
    
    Node_lfree* preds[slist->max_level+1];
    Node_lfree* succs[slist->max_level+1];
    Node_lfree* new_node;
    Node_lfree* pred;
    Node_lfree* succ;
    int level = random_level_generator_lfree(slist);

    init_node_lfree(&new_node, key, value, level);

    while(true){
        if(find_skip_list_lfree(slist, key, preds, succs)){ 
            free_node_lfree(new_node); 
            return false; 
        }
        else{            
            for(int l = 0; l<=level; l++){
                succ = succs[l];
                new_node->nexts[l] = succ;
            }
            pred = preds[0];
            succ = getpointer(succs[0]);
            if(!CAS(&pred->nexts[0], &succ, new_node)){
                continue;
            }

            for(int l = 1; l<=level; l++){
                while(true){
                    pred = preds[l];
                    succ = getpointer(succs[l]);

                    if(CAS(&pred->nexts[l], &succ, new_node)){
                        break;
                    }
                    find_skip_list_lfree(slist, key, preds, succs);
                }
            }
            return true;
        }
    }
}

bool remove_skip_list_lfree(Skip_list_lfree* slist, int key) {
    Node_lfree *preds[slist->max_level+1];
    Node_lfree *succs[slist->max_level+1];
    Node_lfree *succ;
    Node_lfree *markedsucc;
    bool marked;
    bool done;

    while (true) {
        if (!find_skip_list_lfree(slist, key, preds, succs)) {
            return false;
        } else {
            // shortcut lists from level down to b+1
            Node_lfree *rem_node = succs[0];
            for (int l = rem_node->level; l >= 0+1; l--) {
                marked = ismarked(rem_node->nexts[l]);
                succ = getpointer(rem_node->nexts[l]);
                while (!marked) {
                    markedsucc = setmark(succ);
                    CAS(&rem_node->nexts[l], &succ, markedsucc);
                    marked = ismarked(rem_node->nexts[l]);
                    succ = getpointer(rem_node->nexts[l]);
                }
            }

            // level 0 list
            marked = ismarked(rem_node->nexts[0]);
            succ = getpointer(rem_node->nexts[0]);
            while (true) {
                markedsucc = setmark(succ);
                done = CAS(&rem_node->nexts[0], &succ, markedsucc);
                marked = ismarked(succs[0]->nexts[0]);
                succ = getpointer(succs[0]->nexts[0]);
                if (done) {
                    find_skip_list_lfree(slist, key, preds, succs);
                    return true;
                } else if (marked) {
                    return false;
                }
            }
        }
    }
}


bool contains_skip_list_lfree(Skip_list_lfree* slist, int key){
    bool marked = false;
    Node_lfree* pred = slist->header;
    Node_lfree* curr = NULL;
    Node_lfree* succ = NULL;

    for(int l = slist->max_level; l>=0; l--){
        curr = getpointer(pred->nexts[l]);
        while(true){
            succ = getpointer(curr->nexts[l]);
            marked = ismarked(curr->nexts[l]);
            while(marked){
                curr = getpointer(curr->nexts[l]);
                succ = getpointer(curr->nexts[l]);
                marked = ismarked(curr->nexts[l]);
            }
            if(curr->key < key){
                pred = curr;
                curr = succ;
            }
            else{
                break;
            }
        }
    }

    return (curr->key == key); 
}

// Thomas
int find_skip_list_lfree(Skip_list_lfree* slist, int key, Node_lfree* preds[], Node_lfree* succs[]){
    bool marked = false;
    Node_lfree* pred = NULL;
    Node_lfree* curr = NULL;
    Node_lfree* succ = NULL;

    while(true){
        pred = slist->header;
        for(int l = slist->max_level; l >= 0; l--){
            curr = getpointer(pred->nexts[l]);
            while(true){
                succ = getpointer(curr->nexts[l]);
                marked = ismarked(curr->nexts[l]);
                while(marked){
                    if(!CAS(&pred->nexts[l], &curr, succ)) goto _continue;
                    curr = getpointer(pred->nexts[l]);
                    succ = getpointer(curr->nexts[l]);
                    marked = ismarked(curr->nexts[l]);
                }
                if(curr->key < key){
                    pred = curr; curr = succ;
                }else{
                    break;
                }
            }
            preds[l] = pred; succs[l] = curr;
        }
        return (curr->key == key);
        _continue:;
    }

}


void free_node_lfree(Node_lfree* node){
    if(node->nexts){
        free(node->nexts);
    }
    free(node);
}


void free_skip_list_lfree(Skip_list_lfree* slist){

    Node_lfree* node = slist->header;
    Node_lfree* node_next = slist->header->nexts[0];

    while(node && node_next){
        free_node_lfree(node);
        node = node_next;
        node_next = node_next->nexts[0];
    }

    free_node_lfree(node);
    free(slist->random_seeds);

}


void print_skip_list_lfree(Skip_list_lfree* slist){

    Node_lfree* node = NULL;

    node = getpointer(slist->header->nexts[0]);

    printf("Skip_list_lock_free: ");

    while(node->nexts[0]){
        if(ismarked(node->nexts[0])){
            node = getpointer(node->nexts[0]);
            continue;
        }
        printf("(%d, %d) ", node->key, node->value);
        node = getpointer(node->nexts[0]);
        
    }

    printf("\n");

}

void print_marked_skip_list_lfree(Skip_list_lfree* slist) {
    Node_lfree* node = NULL;
    node = slist->header;

    printf("Marked nodes: ");

    while (getpointer(node->nexts[0]) != NULL) {
        node = getpointer(node->nexts[0]);
        if (node->nexts[0] != NULL && ismarked(node->nexts[0])) {
            printf("(%d, %d) ", node->key, node->value);
        }
    }

    printf("\n");
}