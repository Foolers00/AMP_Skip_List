/////////////////////////////////////////////////////////////////////////////////////
/* SKIP LIST LOCK */
/////////////////////////////////////////////////////////////////////////////////////

#ifndef SKIP_LIST_LOCK
#define SKIP_LIST_LOCK
#include "skip_list_lock.h"
#endif

bool init_skip_list_l(Skip_list_l* slist, int max_level){

    Node_l* header = NULL;
    Node_l* tail = NULL;

    init_random_l(slist);

    if(!init_node_l(&header, INT_MAX, 0,
    max_level)){ return false; }

    if(!init_node_l(&tail, INT_MIN, 0,
    max_level)){ return false; }

    #pragma omp parallel default(shared)
    {
        for(int i = 0; i <= max_level; i++){
            header->nexts[i] = NULL;
            header->prevs[i] = tail;
            tail->nexts[i] = header;
            tail->prevs[i] = NULL;
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
    (*node)->key = INT_MAX;
    (*node)->value = 0;

    (*node)->prevs = (Node_l**)malloc((level+1)*sizeof(Node_l*));
    (*node)->nexts = (Node_l**)malloc((level+1)*sizeof(Node_l*));

    if(!(*node)->prevs || !(*node)->nexts){
        fprintf(stderr, "Malloc failed");
        return false;
    }

    omp_init_nest_lock(&(*node)->lock);
    (*node)->marked = false;
    (*node)->fullylinked = false;

    return true;

}


bool init_random_l(Skip_list_l* slist){

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


unsigned int random_level_generator_l(Skip_list_l* slist){

    unsigned int level = 0;
    unsigned int* seed = &slist->random_seeds[omp_get_thread_num()];
    unsigned int max_level = slist->max_level;

    double random_number = (double)rand_r(seed)/(double)RAND_MAX;

    while(random_number < FRACTION && level <= max_level){
        level++;
    }

    return level;

}


bool init_prevs_nexts_l(Node_l*** prevs, Node_l*** nexts, unsigned int level){
    *prevs = (Node_l**)malloc(sizeof(Node_l*)*(level+1));
    *nexts = (Node_l**)malloc(sizeof(Node_l*)*(level+1));

    if(!*prevs || !*nexts){
        fprintf(stderr, "Malloc failed");
        return false;
    }

    return true;
}


bool add_skip_list_l(Skip_list_l* slist, int key, int value){

    Node_l** prevs;
    Node_l** nexts;
    Node_l* prev;
    Node_l* next;
    Node_l* new_node;
    Window_l w;
    int highlock = -1;
    bool valid = false;
    int level = random_level_generator_l(slist);

    init_prevs_nexts_l(&prevs, &nexts, slist->max_level);

    while(true){

        int f = find_skip_list_l(slist, key, prevs, nexts);
        if(f>=0){
            Node_l* found = nexts[f];
            if(!found->marked){
                while(!found->fullylinked){}
                return false;
            }
            continue;
        }

        for(int l = 0; valid&&(l<=level); l++){
            prev = prevs[l];
            next = nexts[l];
            omp_set_nest_lock(&prev->lock);
            highlock = l;
            w = (Window_l) {.pred = prev, .curr = next };
            valid = validate_skip_list_l(w, l);
        }

        if(!valid){
            unlock_prevs_l(prevs, highlock);
            continue;
        }

        init_node_l(&new_node, key, value, level);

        for(int l = 0; l<=level; l++){
            new_node->nexts[l] = nexts[l];
            prevs[l]->nexts[l] = new_node;
        }
        new_node->fullylinked = true;

        unlock_prevs_l(prevs, level);

        break;

    }

    return true;

}

bool remove_skip_list_l(Skip_list_l* slist, int key) {
    Node_l *prev;
    Node_l **prevs;
    Node_l **nexts;
    Node_l *victim;
    int k = -1;
    int highlock = -1;
    bool marked = false;
    bool valid;

    init_prevs_nexts_l(&prevs, &nexts, slist->max_level);

    while (true) {
        int f = find_skip_list_l(slist, key, prevs, nexts);
        if (f >= 0) victim = nexts[f];
        if (marked || (f >= 0 && victim->fullylinked && victim->level==f && !victim->marked)) {
            if (!marked) {  // only mark victim once
                k = victim->level;
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
            valid = true;
            for (int l = 0; valid && (l<=k); l++) {
                prev = prevs[l];
                omp_set_nest_lock(&prev->lock);
                highlock = l;
                valid = !prev->marked && prev->nexts[l] == victim;
            }

            if (!valid) {   // validation failed
                unlock_prevs_l(prevs, highlock);
                continue;
            }
            
            // link out from top to bottom
            for (int l = k; l >= 0; l--) {
                prevs[l]->nexts[l] = victim->nexts[l];
            }

            omp_unset_nest_lock(&victim->lock);
            return true;

        } else {
            return false;    
        }
    }
}


bool contains_skip_list_l(Skip_list_l* slist, int key){

    Node_l** prevs;
    Node_l** nexts;

    init_prevs_nexts_l(&prevs, &nexts, slist->max_level);

    int foundlevel = find_skip_list_l(slist, key, prevs, nexts);

    return(foundlevel>=0 && nexts[foundlevel]->fullylinked &&
    !nexts[foundlevel]->marked);

}


void unlock_prevs_l(Node_l** prevs, unsigned int level){

    for(int l = 0; l<=level; l++){
        omp_unset_nest_lock(&prevs[l]->lock);
    }

}


void free_node_l(Node_l* node){

    if(node->nexts){
        free(node->nexts);
    }
    if(node->prevs){
        free(node->prevs);
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
}


void free_window_l(Window_l* w){
    free(w);
}


void print_skip_list_l(Skip_list_l* slist){

    Node_l* node = NULL;

    node = slist->tail->nexts[0];

    fprintf(stdout, "Skip_list: ");

    while(node->nexts[0]){
        fprintf(stdout, "(%d, %d) ", node->key, node->value);
        node = node->nexts[0];
    }
    fprintf(stdout, "\n");

}


void compare_results_l(Skip_list_seq* slist_seq, Skip_list_l* slist_l){

    Node_seq* node_seq;
    Node_l* node_l;

    node_seq = slist_seq->tail->nexts[0];
    node_l = slist_l->tail->nexts[0];

    while(node_seq->nexts[0] && node_l->nexts[0]){
        if(node_seq->key != node_l->key){
            fprintf(stdout, "Comparison Failed: %d not same as %d\n",
            node_seq->key, node_l->key);
            return;
        }
    }

    if(node_seq->nexts[0] || node_l->nexts[0]){
        fprintf(stdout, "Comparison Failed: Lists are not the same length\n");
        return;
    }

    fprintf(stdout, "Comparison Succeded\n");

}

int find_skip_list_l(Skip_list_l *slist, int key, Node_l **prevs, Node_l **nexts) {
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
            prevs[l] = pred;
            nexts[l] = curr;
        }
    }
    return foundlevel;
}

bool validate_skip_list_l(Window_l w, int l) {
    return (!w.pred->marked) && !(w.curr->marked) && (w.pred->nexts[l] == w.curr);
}