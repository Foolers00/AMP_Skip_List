/////////////////////////////////////////////////////////////////////////////////////
/* SKIP LIST LOCK FREE IMPROVED */
/////////////////////////////////////////////////////////////////////////////////////

#ifndef SKIP_LIST_LOCKF_IMPROVED
#define SKIP_LIST_LOCKF_IMPROVED
#include "skip_list_lockf_improved.h"
#endif

bool init_skip_list_lfree_improved(Skip_list_lfree_improved* slist, int max_level, int num_of_threads){

    Node_lfree_improved* header = NULL;
    Node_lfree_improved* tail = NULL;

    init_random_lfree_improved(slist, num_of_threads);

    if(!init_node_lfree_improved(&tail, INT_MAX, 0,
                    max_level)){ return false; }

    if(!init_node_lfree_improved(&header, INT_MIN, 0,
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

    #ifdef COUNTERS
    slist->adds = 0;
    slist->rems = 0;
    slist->cons = 0;
    slist->trav = 0;
    slist->fail = 0;
    slist->rtry = 0;
    #endif

    return true;
}

bool init_node_lfree_improved(Node_lfree_improved** node, int key, int value, int level){

    *node = (Node_lfree_improved*)malloc(sizeof(Node_lfree_improved));

    if(!*node){
        fprintf(stderr, "Malloc failed");
        return false;
    }

    (*node)->level = level;
    (*node)->key = key;
    (*node)->value = value;

    (*node)->nexts = (Node_lfree_improved **)malloc((level + 1) * sizeof(Node_lfree_improved *));

    if(!(*node)->nexts){
        fprintf(stderr, "Malloc failed");
        return false;
    }

    return true;

}

bool init_random_lfree_improved(Skip_list_lfree_improved* slist, int num_of_threads){

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

unsigned int random_level_generator_lfree_improved(Skip_list_lfree_improved* slist){

    int level = 0;
    unsigned int* seed = &slist->random_seeds[omp_get_thread_num()];
    int max_level = slist->max_level;

    double random_number = (double)rand_r(seed)/(double)RAND_MAX;

    while(random_number < FRACTION && level < max_level){
        level++;
    }

    return level;

}


bool add_skip_list_lfree_improved(Skip_list_lfree_improved* slist, int key, int value){
    
    Node_lfree_improved* preds[slist->max_level+1];
    Node_lfree_improved* succs[slist->max_level+1];
    Node_lfree_improved* new_node;
    Node_lfree_improved* pred;
    Node_lfree_improved* succ;
    int level = random_level_generator_lfree_improved(slist);

    init_node_lfree_improved(&new_node, key, value, level);

    
    while(true){
        ////////////
        // old
        ////////////
        // new
        restart_1:
        ///////////
        if(find_skip_list_lfree_improved(slist, key, preds, succs)){ 
            free_node_lfree_improved(new_node); 
            return false; 
        }
        else{            
            for(int l = 0; l<=level; l++){
                succ = succs[l];
                new_node->nexts[l] = succ;
            }
            pred = preds[0];
            succ = getpointer_improved(succs[0]);

            ////////////
            // old
            // if(!CAS_improved(&pred->nexts[0], &succ, new_node)){
            //     continue;
            // }

            ////////////
            // new
            while(!CAS_improved(&pred->nexts[0], &succ, new_node)){
                INC(slist->fail);
                if(ismarked_improved(pred->nexts[0])){
                    INC(slist->rtry);
                    goto restart_1;
                }
                succ = getpointer_improved(pred->nexts[0]);
                while(succ->key < new_node->key){
                    pred = succ;
                    succ = succ->nexts[0];
                }
                new_node->nexts[0] = succ;
            }
            ////////////
            INC(slist->adds);

            for(int l = 1; l<=level; l++){
                
                ////////////
                // old
                //while(true){
                ////////////
                // new
                restart_2:
                ///////////
                pred = preds[l];
                succ = getpointer_improved(succs[l]);

                ////////////
                // old
                // if(CAS_improved(&pred->nexts[l], &succ, new_node)){
                //     INC(slist->adds);
                //     break;
                // }
                // find_skip_list_lfree_improved(slist, key, preds, succs);
                // INC(slist->fail);

                ////////////
                // new
                while(!CAS_improved(&pred->nexts[l], &succ, new_node)){
                    INC(slist->fail);
                    if(ismarked_improved(pred->nexts[0])){
                        find_skip_list_lfree_improved(slist, key, preds, succs);
                        INC(slist->rtry);
                        goto restart_2;
                    }
                    succ = getpointer_improved(pred->nexts[l]);
                    while(succ->key < new_node->key){
                        pred = succ;
                        succ = succ->nexts[l];
                    }
                    new_node->nexts[l] = succ;
                }
                INC(slist->adds);
                ////////////

                ////////////
                //old
                //}
                ////////////
                //new
                ////////////
            }
            return true;
        }
    }
}

bool remove_skip_list_lfree_improved(Skip_list_lfree_improved* slist, int key) {
    Node_lfree_improved *preds[slist->max_level+1];
    Node_lfree_improved *succs[slist->max_level+1];
    Node_lfree_improved *succ;
    Node_lfree_improved *markedsucc;
    bool marked;
    bool done;

    while (true) {
        if (!find_skip_list_lfree_improved(slist, key, preds, succs)) {
            return false;
        } else {
            // shortcut lists from level down to b+1
            Node_lfree_improved *rem_node = succs[0];
            for (int l = rem_node->level; l >= 0+1; l--) {
                marked = ismarked_improved(rem_node->nexts[l]);
                succ = getpointer_improved(rem_node->nexts[l]);
                while (!marked) {
                    markedsucc = setmark_improved(succ);
                    if (!CAS_improved(&rem_node->nexts[l], &succ, markedsucc)) INC(slist->fail);
                    marked = ismarked_improved(rem_node->nexts[l]);
                    succ = getpointer_improved(rem_node->nexts[l]);
                }
                INC(slist->rems);
            }

            // level 0 list
            marked = ismarked_improved(rem_node->nexts[0]);
            succ = getpointer_improved(rem_node->nexts[0]);
            while (true) {
                markedsucc = setmark_improved(succ);
                done = CAS_improved(&rem_node->nexts[0], &succ, markedsucc);
                marked = ismarked_improved(succs[0]->nexts[0]);
                succ = getpointer_improved(succs[0]->nexts[0]);
                if (done) {
                    find_skip_list_lfree_improved(slist, key, preds, succs); // cleanup
                    INC(slist->rems);
                    return true;
                } else if (marked) {
                    return false;
                }
                INC(slist->fail);
            }
        }
    }
}


bool contains_skip_list_lfree_improved(Skip_list_lfree_improved* slist, int key){
    bool marked = false;
    Node_lfree_improved* pred = slist->header;
    Node_lfree_improved* curr = NULL;
    Node_lfree_improved* succ = NULL;

    for(int l = slist->max_level; l>=0; l--){
        curr = getpointer_improved(pred->nexts[l]);
        while(true){
            succ = getpointer_improved(curr->nexts[l]);
            marked = ismarked_improved(curr->nexts[l]);
            while(marked){
                curr = getpointer_improved(curr->nexts[l]);
                succ = getpointer_improved(curr->nexts[l]);
                marked = ismarked_improved(curr->nexts[l]);
            }
            if(curr->key < key){
                pred = curr;
                curr = succ;
                INC(slist->cons);
            }
            else{
                break;
            }
        }
    }

    return (curr->key == key); 
}

// Thomas
int find_skip_list_lfree_improved(Skip_list_lfree_improved* slist, int key, Node_lfree_improved* preds[], Node_lfree_improved* succs[]){
    bool marked = false;
    Node_lfree_improved* pred = NULL;
    Node_lfree_improved* curr = NULL;
    Node_lfree_improved* succ = NULL;

    while(true){
        pred = slist->header;
        for(int l = slist->max_level; l >= 0; l--){
            curr = getpointer_improved(pred->nexts[l]);
            while(true){
                succ = getpointer_improved(curr->nexts[l]);
                marked = ismarked_improved(curr->nexts[l]);
                while(marked){
                    ////////////
                    // old
                    // if(!CAS_improved(&pred->nexts[l], &curr, succ)){
                    //     INC(slist->fail);
                    //     goto _continue;
                    // }
                    
                    ////////////
                    // new
                    if(!CAS_improved(&pred->nexts[l], &curr, succ)){
                        INC(slist->fail);
                        if(ismarked_improved(pred->nexts[l])){
                            INC(slist->rtry);
                            goto _continue;
                        }
                    }
                    curr = getpointer_improved(pred->nexts[l]);
                    succ = getpointer_improved(curr->nexts[l]);
                    marked = ismarked_improved(curr->nexts[l]);
                    ///////////
                }
    
                if(curr->key < key){
                    pred = curr; curr = succ;
                    INC(slist->trav);
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


void free_node_lfree_improved(Node_lfree_improved* node){
    if(node->nexts){
        free(node->nexts);
    }
    free(node);
}


void free_skip_list_lfree_improved(Skip_list_lfree_improved* slist){

    Node_lfree_improved* node = slist->header;
    Node_lfree_improved* node_next = slist->header->nexts[0];

    while(node && node_next){
        free_node_lfree_improved(node);
        node = node_next;
        node_next = node_next->nexts[0];
    }

    free_node_lfree_improved(node);
    free(slist->random_seeds);

}


void print_skip_list_lfree_improved(Skip_list_lfree_improved* slist){

    Node_lfree_improved* node = NULL;

    node = getpointer_improved(slist->header->nexts[0]);

    printf("Skip_list_lock_free: ");

    while(node->nexts[0]){
        if(ismarked_improved(node->nexts[0])){
            node = getpointer_improved(node->nexts[0]);
            continue;
        }
        printf("(%d, %d) ", node->key, node->value);
        node = getpointer_improved(node->nexts[0]);
        
    }

    printf("\n");

}

void print_marked_skip_list_lfree_improved(Skip_list_lfree_improved* slist) {
    Node_lfree_improved* node = NULL;
    node = slist->header;

    printf("Marked nodes: ");

    while (getpointer_improved(node->nexts[0]) != NULL) {
        node = getpointer_improved(node->nexts[0]);
        if (node->nexts[0] != NULL && ismarked_improved(node->nexts[0])) {
            printf("(%d, %d) ", node->key, node->value);
        }
    }

    printf("\n");
}