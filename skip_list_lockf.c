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
    
}

bool remove_skip_list_lfree(Skip_list_lfree* slist, int key) {
    Node_lfree *preds[slist->max_level+1];
    Node_lfree *succs[slist->max_level+1];
    Node_lfree *succ;
    int b = 0;
    bool* marked[1];
    bool done;


    while (true) {
        if (find_skip_list_lfree(slist, key, preds, succs)) {
            return false;
        } else {
            // shortcut lists from level down to b+1
            Node_lfree *rem_node = succs[b];
            for (int l = rem_node->level; l >= b+1; l--) {
                marked[0] = false;
                succ = get_markable_reference(rem_node->nexts[l], marked);
                while (!marked[0]) {
                    // TODO: CAS
                    succ = get_markable_reference(rem_node->nexts[l], marked);
                }
            }

            // level 0 list
            marked[0] = false;
            succ = get_markable_reference(rem_node->nexts[b], marked);
            while (true) {
                // TODO: CAS
                succ = get_markable_reference(succs[b]->nexts[b], marked);
                if (done) {
                    find_skip_list_lfree(slist, key, preds, succs);
                    return true;
                } else if (marked[0]) {
                    return false;
                }
            }
        }
    }
}
