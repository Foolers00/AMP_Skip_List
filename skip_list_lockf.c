/////////////////////////////////////////////////////////////////////////////////////
/* SKIP LIST LOCK FREE */
/////////////////////////////////////////////////////////////////////////////////////

#ifndef SKIP_LIST_LOCKF
#define SKIP_LIST_LOCKF
#include "skip_list_lockf.h"
#endif

bool init_skip_list_lfree(Skip_list_lfree* slist, int max_level){

    Node_lfree* header = NULL;
    Node_lfree* tail = NULL;

    init_random_lfree(slist);

    if(!init_node_lfree(&tail, INT_MAX, 0,
                    max_level)){ return false; }

    if(!init_node_lfree(&header, INT_MIN, 0,
                    max_level)){ return false; }

    #pragma omp parallel default(shared)
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

bool init_node_lfree(Node_lfree** node, int key, int value, unsigned int level){

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

bool init_random_lfree(Skip_list_lfree* slist){

    time_t t;
    int id;

    slist->random_seeds =
            (unsigned int*)malloc(sizeof(unsigned int)*omp_get_num_threads());
    if(!slist->random_seeds){
        fprintf(stderr, "Malloc failed");
        return false;
    }

    #pragma omp parallel private(t, id)
    {
        id = omp_get_thread_num();
        slist->random_seeds[id] = (unsigned) time(&t) + id;
    }

    return true;


}

unsigned int random_level_generator_lfree(Skip_list_lfree* slist){

    unsigned int level = 0;
    unsigned int* seed = &slist->random_seeds[omp_get_thread_num()];
    unsigned int max_level = slist->max_level;

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
    int b = 0;
    bool marked;
    bool done;


    while (true) {
        if (find_skip_list_lfree(slist, key, preds, succs)) {
            return false;
        } else {
            // shortcut lists from level down to b+1
            Node_lfree *rem_node = succs[b];
            for (unsigned int l = rem_node->level; l >= b+1; l--) {
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
            marked = ismarked(rem_node->nexts[b]);
            succ = getpointer(rem_node->nexts[b]);
            while (true) {
                markedsucc = setmark(succ);
                done = CAS(&rem_node->nexts[b], &succ, markedsucc);
                marked = ismarked(succs[b]->nexts[b]);
                succ = getpointer(succs[b]->nexts[b]);
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
        curr = pred->nexts[l];
        while(true){
            succ = curr->nexts[l];
            marked = ismarked(succ);
            while(marked){
                curr = curr->nexts[l];
                succ = curr->nexts[l];
                marked = ismarked(succ);
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
    int b = 0;
    bool marked = false;
    Node_lfree *pred, *curr, *succ;
    while(true){
        pred = slist->header;
        for(int l = slist->max_level; l >= 0; l--){
            curr = pred->nexts[l];
            while(true){
                succ = curr->nexts[l];
                marked = ismarked(succ);
                while(marked){
                    if(!CAS(&pred->nexts[l], &curr, succ)) goto _continue;
                    curr = pred->nexts[l];
                    succ = curr->nexts[l];
                    marked = ismarked(succ);
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