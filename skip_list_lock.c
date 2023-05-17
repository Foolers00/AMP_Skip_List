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


bool init_prev_next_l(Node_l*** prev, Node_l*** next, unsigned int level){
    *prev = (Node_l**)malloc(sizeof(Node_l*)*(level+1));
    *next = (Node_l**)malloc(sizeof(Node_l*)*(level+1));

    if(!*prev || !*next){
        fprintf(stderr, "Malloc failed");
        return false;
    }

    return true;
}


bool add_skip_list_seq(Skip_list_l* slist, int key, int value){

    Node_l** prevs;
    Node_l** nexts;
    Node_l* prev;
    Node_l* next;
    Window_l w;
    int highlock = -1;
    bool valid = false;
    int level = random_level_generator_l(slist);

    init_prev_next_l(&prevs, &nexts, slist->max_level);

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
            valid = validate_skip_list_l(w);
        }

        if(!valid) continue;


    
    }


}


bool contains_skip_list_l(Skip_list_l* slist, int key){

    Node_l** prev;
    Node_l** next;

    init_prev_next_l(&prev, &next, slist->max_level);

    int foundlevel = find_skip_list_l(slist, key, prev, next);

    return(foundlevel>=0 && next[foundlevel]->fullylinked &&
    !next[foundlevel]->marked);

}
