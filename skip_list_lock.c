/////////////////////////////////////////////////////////////////////////////////////
/* SKIP LIST LOCK */
/////////////////////////////////////////////////////////////////////////////////////

#ifndef SKIP_LIST_LOCK
#define SKIP_LIST_LOCK
#include "skip_list_lock.h"
#endif

bool init_skip_list_l(Skip_list_l* slist, int max_level, int num_of_threads){

    Node_l* header = NULL;
    Node_l* tail = NULL;

    init_random_l(slist, num_of_threads);

    if(!init_node_l(&tail, INT_MAX, 0,
    max_level)){ return false; }

    if(!init_node_l(&header, INT_MIN, 0,
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


bool init_node_l(Node_l** node, int key, int value, unsigned int level){

    *node = (Node_l*)malloc(sizeof(Node_l));

    if(!*node){
        fprintf(stderr, "Malloc failed");
        return false;
    }

    (*node)->level = level;
    (*node)->key = key;
    (*node)->value = value;

    (*node)->nexts = (Node_l**)malloc((level+1)*sizeof(Node_l*));

    if(!(*node)->nexts){
        fprintf(stderr, "Malloc failed");
        return false;
    }

    omp_init_nest_lock(&(*node)->lock);
    (*node)->marked = false;
    (*node)->fullylinked = false;

    return true;

}


bool init_random_l(Skip_list_l* slist, int num_of_threads){

    time_t t;
    int id;

    slist->random_seeds =
    (unsigned int*)malloc(sizeof(unsigned int)*num_of_threads); 
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


unsigned int random_level_generator_l(Skip_list_l* slist){

    unsigned int level = 0;
    unsigned int* seed = &slist->random_seeds[omp_get_thread_num()];
    unsigned int max_level = slist->max_level;

    double random_number = (double)rand_r(seed)/(double)RAND_MAX;

    while(random_number < FRACTION && level < max_level){
        level++;
    }

    return level;

}


bool add_skip_list_l(Skip_list_l* slist, int key, int value){

    Node_l* preds[slist->max_level+1];
    Node_l* succs[slist->max_level+1];
    Node_l* pred;
    Node_l* succ;
    Node_l* new_node;
    Window_l w;
    int highlock = -1;
    bool valid = false;
    int level = random_level_generator_l(slist);

    //init_preds_succs_l(&preds, &succs, slist->max_level);

    while(true){

        int f = find_skip_list_l(slist, key, preds, succs);
        if(f>=0){
            Node_l* found = succs[f];
            if(!found->marked){
                while(!found->fullylinked){}
                return false;
            }
            continue;
        }

        valid = true;
        for(int l = 0; valid&&(l<=level); l++){
            pred = preds[l];
            succ = succs[l];
            omp_set_nest_lock(&pred->lock);
            highlock = l;
            w = (Window_l) {.pred = pred, .curr = succ };
            valid = validate_skip_list_l(w, l);
        }

        if(!valid){
            unlock_preds_l(preds, highlock);
            continue;
        }

        init_node_l(&new_node, key, value, level);

        for(int l = 0; l<=level; l++){
            new_node->nexts[l] = succs[l];
            preds[l]->nexts[l] = new_node;
        }
        new_node->fullylinked = true;

        unlock_preds_l(preds, level);

        break;

    }

    return true;

}


bool remove_skip_list_l(Skip_list_l* slist, int key) {
    
    Node_l *pred;
    Node_l* preds[slist->max_level+1];
    Node_l* succs[slist->max_level+1];
    Node_l *victim;
    int victim_level = -1;
    int highlock = -1;
    bool marked = false;
    bool valid;

    while (true) {
        int f = find_skip_list_l(slist, key, preds, succs);
        if (f >= 0) victim = succs[f];
        if (marked || (f >= 0 && victim->fullylinked && victim->level==f && !victim->marked)) {
            if (!marked) {  // only mark victim once
                victim_level = victim->level;
                omp_set_nest_lock(&victim->lock);
                if (victim->marked) {
                    omp_unset_nest_lock(&victim->lock);
                    return false;
                }
                victim->marked = true;
                marked = true;
            }

            // now marked
            // try to link out
            highlock = -1;
            valid = true;
            for (int l = 0; valid && (l<=victim_level); l++) {
                pred = preds[l];
                omp_set_nest_lock(&pred->lock);
                highlock = l;
                valid = !pred->marked && pred->nexts[l] == victim;
            }

            if (!valid) {   // validation failed
                unlock_preds_l(preds, highlock);
                continue;
            }
            
            // link out from top to bottom
            for (int l = victim_level; l >= 0; l--) {
                preds[l]->nexts[l] = victim->nexts[l];
            }

            omp_unset_nest_lock(&victim->lock);
            unlock_preds_l(preds, highlock);
            return true;

        } else {
            return false;    
        }
    }
}


bool contains_skip_list_l(Skip_list_l* slist, int key){

    Node_l* preds[slist->max_level+1];
    Node_l* succs[slist->max_level+1];

    int foundlevel = find_skip_list_l(slist, key, preds, succs);

    return(foundlevel>=0 && succs[foundlevel]->fullylinked &&
    !succs[foundlevel]->marked);

}


void unlock_preds_l(Node_l* preds[], unsigned int level){

    for(int l = 0; l<=level; l++){
        omp_unset_nest_lock(&preds[l]->lock);
    }

}


void free_node_l(Node_l* node){

    if(node->nexts){
        free(node->nexts);
    }
    free(node);

}


void free_skip_list_l(Skip_list_l* slist){

    Node_l* node = slist->tail;
    Node_l* node_next = slist->tail->nexts[0];

    while(node && node_next){
        free_node_l(node);
        node = node_next;
        node_next = node_next->nexts[0];
    }

    free_node_l(node);
    free(slist->random_seeds);

}


void free_window_l(Window_l* w){
    free(w);
}


void print_skip_list_l(Skip_list_l* slist){

    Node_l* node = NULL;

    node = slist->header->nexts[0];

    printf("Skip_list_lock: ");

    while(node->nexts[0]){
        printf("(%d, %d) ", node->key, node->value);
        node = node->nexts[0];
    }
    printf("\n");

}





int find_skip_list_l(Skip_list_l *slist, int key, Node_l * preds[], Node_l *succs[]) {
    int foundlevel = -1; //not found at any level >= 0
    Node_l *pred = slist->header;
    for (int l = slist->max_level; l >= 0; l--) {
        volatile Node_l *curr = pred->nexts[l];
        while (key > curr->key) {
            pred = curr;
            curr = curr->nexts[l];
        }
        if (foundlevel == -1 && key == curr->key) {
            foundlevel = l; // found at level l
        }
        preds[l] = pred;
        succs[l] = curr;
    }
    return foundlevel;
}


bool validate_skip_list_l(Window_l w, int l) {
    return (!w.pred->marked) && !(w.curr->marked) && (w.pred->nexts[l] == w.curr);
}