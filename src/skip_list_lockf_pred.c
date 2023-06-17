/////////////////////////////////////////////////////////////////////////////////////
/* SKIP LIST LOCK FREE PRED */
/////////////////////////////////////////////////////////////////////////////////////

#ifndef SKIP_LIST_LOCKF_PRED
#define SKIP_LIST_LOCKF_PRED
#include "skip_list_lockf_pred.h"
#endif

bool init_skip_list_lfree_pred(Skip_list_lfree_pred* slist, int max_level, int num_of_threads){

    Node_lfree_pred* header = NULL;
    Node_lfree_pred* tail = NULL;

    init_random_lfree_pred(slist, num_of_threads);

    if(!init_node_lfree_pred(&header, INT_MIN, 0,
                    max_level, header)){ return false; }

    if(!init_node_lfree_pred(&tail, INT_MAX, 0,
                    max_level, header)){ return false; }

    

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

bool init_node_lfree_pred(Node_lfree_pred** node, int key, int value, int level, Node_lfree_pred* pred){

    *node = (Node_lfree_pred*)malloc(sizeof(Node_lfree_pred));

    if(!*node){
        fprintf(stderr, "Malloc failed");
        return false;
    }

    (*node)->level = level;
    (*node)->key = key;
    (*node)->value = value;

    (*node)->nexts = (Node_lfree_pred **)malloc((level + 1) * sizeof(Node_lfree_pred *));
    ////////////
    // old_2
    ////////////
    // new_2
    (*node)->preds = (Node_lfree_pred **)malloc((level + 1) * sizeof(Node_lfree_pred *));
    ////////////

    if(!(*node)->nexts || !(*node)->preds){
        fprintf(stderr, "Malloc failed");
        return false;
    }

    ////////////
    // old_2
    ////////////
    // new_2
    for(int i = 0; i < level+1; i++){
        (*node)->preds[i] = pred;
    }
    ////////////

    return true;

}

bool init_random_lfree_pred(Skip_list_lfree_pred* slist, int num_of_threads){

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

unsigned int random_level_generator_lfree_pred(Skip_list_lfree_pred* slist){

    int level = 0;
    unsigned int* seed = &slist->random_seeds[omp_get_thread_num()];
    int max_level = slist->max_level;

    double random_number = (double)rand_r(seed)/(double)RAND_MAX;

    while(random_number < FRACTION && level < max_level){
        level++;
    }

    return level;

}


bool add_skip_list_lfree_pred(Skip_list_lfree_pred* slist, int key, int value){
    
    Node_lfree_pred* preds[slist->max_level+1];
    Node_lfree_pred* succs[slist->max_level+1];
    Node_lfree_pred* new_node;
    Node_lfree_pred* pred;
    Node_lfree_pred* succ;
    int level = random_level_generator_lfree_pred(slist);
    ////////////
    // old_2
    ////////////
    // new_2
    Node_lfree_pred* pred_temp;
    Node_lfree_pred** preds_temp;
    pred_temp = NULL;
    preds_temp = NULL;
    ////////////

    init_node_lfree_pred(&new_node, key, value, level, slist->header);

    
    while(true){

        restart_1:


        ////////////
        // old_2
        // if(find_skip_list_lfree_pred(slist, key, preds, succs))
        ////////////
        // new_2
        if(find_skip_list_lfree_pred(slist, key, preds, succs, preds_temp)){ 
        ////////////
            free_node_lfree_pred(new_node); 
            return false; 
        }
        else{                

            for(int l = 0; l<=level; l++){
                new_node->nexts[l] = succs[l];
                ////////////
                // old_2
                ////////////
                // new_2
                new_node->preds[l] = preds[l];
                ////////////
            }
            pred = preds[0];
            succ = getpointer_pred(succs[0]);


            while(!CAS_pred(&pred->nexts[0], &succ, new_node)){
                INC(slist->fail);
                if(ismarked_pred(pred->nexts[0])){
                    ////////////
                    // old_2
                    ////////////
                    // new_2
                    preds_temp = new_node->preds;
                    ////////////
                    goto restart_1;
                }
                succ = getpointer_pred(pred->nexts[0]);
                while(succ->key < new_node->key){
                    pred = succ;
                    succ = succ->nexts[0];
                }
                new_node->nexts[0] = succ;
            }
            INC(slist->adds);


            ////////////
            // old_2
            ////////////
            // new_2
            pred_temp = getpointer_pred(new_node->nexts[0]->preds[0]);
            CAS_pred(&new_node->nexts[0]->preds[0], &pred_temp, new_node);
            // while(!CAS_pred(&new_node->nexts[0]->preds[0], &pred_temp, new_node)){
            //     if(new_node->nexts[0]->preds[0] != pred_temp){
            //         break;
            //     }
            //     pred_temp = getpointer_pred(new_node->nexts[0]->preds[0]);
            // }
            ////////////

            for(int l = 1; l<=level; l++){
                
                restart_2:

                pred = preds[l];
                succ = getpointer_pred(succs[l]);

                while(!CAS_pred(&pred->nexts[l], &succ, new_node)){
                    INC(slist->fail);
                    if(ismarked_pred(pred->nexts[0])){
                        ////////////
                        // old_2
                        // find_skip_list_lfree_pred(slist, key, preds, succs);
                        ////////////
                        // new_2
                        preds_temp = new_node->preds;
                        find_skip_list_lfree_pred(slist, key, preds, succs, preds_temp);
                        ////////////
                        goto restart_2;
                    }
                    succ = getpointer_pred(pred->nexts[l]);
                    while(succ->key < new_node->key){
                        pred = succ;
                        succ = succ->nexts[l];
                    }
                    new_node->nexts[l] = succ;
                }
                INC(slist->adds);

                ////////////
                // old_2
                ////////////
                // new_2
                pred_temp = getpointer_pred(new_node->nexts[l]->preds[l]);
                CAS_pred(&new_node->nexts[l]->preds[l], &pred_temp, new_node);
                // while(!CAS_pred(&new_node->nexts[l]->preds[l], &pred_temp, new_node)){
                //     if(new_node->nexts[l]->preds[l] != pred_temp){
                //         break;
                //     }
                //     pred_temp = getpointer_pred(new_node->nexts[l]->preds[l]);
                // }
                ////////////
            }
            return true;
        }
    }
}

bool remove_skip_list_lfree_pred(Skip_list_lfree_pred* slist, int key) {
    Node_lfree_pred *preds[slist->max_level+1];
    Node_lfree_pred *succs[slist->max_level+1];
    Node_lfree_pred *succ;
    Node_lfree_pred *markedsucc;
    bool marked;
    bool done;
    ////////////
    // old_2
    ////////////
    // new_2
    Node_lfree_pred* pred_temp;
    ////////////

    while (true) {
        ////////////
        // old_2
        //if (!find_skip_list_lfree_pred(slist, key, preds, succs))
        ////////////
        // new_2
        if (!find_skip_list_lfree_pred(slist, key, preds, succs, NULL)) {
        ////////////
            return false;
        } else {
            // shortcut lists from level down to b+1
            Node_lfree_pred *rem_node = succs[0];
            for (int l = rem_node->level; l >= 0+1; l--) {
                marked = ismarked_pred(rem_node->nexts[l]);
                succ = getpointer_pred(rem_node->nexts[l]);
                while (!marked) {
                    markedsucc = setmark_pred(succ);
                    #ifdef COUNTERS
                    if (!CAS_pred(&rem_node->nexts[l], &succ, markedsucc)) INC(slist->fail);
                    #else
                    CAS_pred(&rem_node->nexts[l], &succ, markedsucc);
                    #endif
                    marked = ismarked_pred(rem_node->nexts[l]);
                    succ = getpointer_pred(rem_node->nexts[l]);
                }
                ////////////
                // old_2
                ////////////
                // new_2
                pred_temp = getpointer_pred(succ->preds[l]);
                CAS_pred(&succ->preds[l], &pred_temp, preds[l]);
                // while(!CAS_pred(&succ->preds[l], &pred_temp, preds[l])){
                //     if(succ->preds[l] != pred_temp){
                //         break;
                //     }
                //     pred_temp = getpointer_pred(succ->preds[l]);
                // }
                ////////////
            }

            // level 0 list
            marked = ismarked_pred(rem_node->nexts[0]);
            succ = getpointer_pred(rem_node->nexts[0]);
            while (true) {
                markedsucc = setmark_pred(succ);
                done = CAS_pred(&rem_node->nexts[0], &succ, markedsucc);
                marked = ismarked_pred(succs[0]->nexts[0]);
                succ = getpointer_pred(succs[0]->nexts[0]);
                if (done) {
                    ////////////
                    // old_2
                    // find_skip_list_lfree_pred(slist, key, preds, succs); // cleanup
                    ////////////
                    // new_2
                    pred_temp = getpointer_pred(succ->preds[0]);
                    CAS_pred(&succ->preds[0], &pred_temp, preds[0]);
                    // while(!CAS_pred(&succ->preds[0], &pred_temp, preds[0])){
                    //     pred_temp = getpointer_pred(succ->preds[0]);
                    // }
                    find_skip_list_lfree_pred(slist, key, preds, succs, NULL); // cleanup
                    ////////////
                    
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


bool contains_skip_list_lfree_pred(Skip_list_lfree_pred* slist, int key){
    bool marked = false;
    Node_lfree_pred* pred = slist->header;
    Node_lfree_pred* curr = NULL;
    Node_lfree_pred* succ = NULL;

    for(int l = slist->max_level; l>=0; l--){
        curr = getpointer_pred(pred->nexts[l]);
        while(true){
            succ = getpointer_pred(curr->nexts[l]);
            marked = ismarked_pred(curr->nexts[l]);
            while(marked){
                curr = getpointer_pred(curr->nexts[l]);
                succ = getpointer_pred(curr->nexts[l]);
                marked = ismarked_pred(curr->nexts[l]);
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
int find_skip_list_lfree_pred(Skip_list_lfree_pred* slist, int key, Node_lfree_pred* preds[], Node_lfree_pred* succs[], Node_lfree_pred* preds_temp[]){
    bool marked = false;
    Node_lfree_pred* pred = NULL;
    Node_lfree_pred* curr = NULL;
    Node_lfree_pred* succ = NULL;
    ////////////
    // old_2
    ////////////
    // new_2
    Node_lfree_pred* pred_temp;
    ////////////

    

    while(true){
        pred = slist->header;
        for(int l = slist->max_level; l >= 0; l--){

            ////////////
            // old_2
            ////////////
            // new_2
            if(preds_temp){
                pred_temp = preds_temp[l];
                while(ismarked_pred(getpointer_pred(pred_temp)->nexts[l])){
                    pred_temp = getpointer_pred(pred_temp);
                    pred_temp = pred_temp->preds[l];
                }
                pred = pred_temp;
            }
            else{
                pred = slist->header;
            }
            ////////////

            curr = getpointer_pred(pred->nexts[l]);
            while(true){
                succ = getpointer_pred(curr->nexts[l]);
                marked = ismarked_pred(curr->nexts[l]);
                while(marked){
                    
                    if(!CAS_pred(&pred->nexts[l], &curr, succ)){
                        INC(slist->fail);
                        if(ismarked_pred(pred->nexts[l])){
                            goto _continue;
                        }
                    }

                    ////////////
                    // old_2
                    ////////////
                    // new_2
                    else{
                        if(pred->nexts[l]->preds[l] != pred){
                            pred_temp = getpointer_pred(pred->nexts[l]->preds[l]);
                            CAS_pred(&pred->nexts[l]->preds[l], &pred_temp, pred);
                            // while(!CAS_pred(&pred->nexts[l]->preds[l], &pred_temp, pred)){
                            //     if(pred->nexts[l]->preds[l] != pred_temp){
                            //         break;
                            //     }
                            //     pred_temp = getpointer_pred(pred->nexts[l]->preds[l]);
                            // }
                        }
                    }
                    ////////////
                    curr = getpointer_pred(pred->nexts[l]);
                    succ = getpointer_pred(curr->nexts[l]);
                    marked = ismarked_pred(curr->nexts[l]);
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


void free_node_lfree_pred(Node_lfree_pred* node){
    if(node->nexts){
        free(node->nexts);
    }
    ////////////
    // old_2
    ////////////
    // new_2
    if(node->preds){
        free(node->preds);
    }
    ////////////
    free(node);
}


void free_skip_list_lfree_pred(Skip_list_lfree_pred* slist){

    Node_lfree_pred* node = slist->header;
    Node_lfree_pred* node_next = slist->header->nexts[0];

    while(node && node_next){
        free_node_lfree_pred(node);
        node = node_next;
        node_next = node_next->nexts[0];
    }

    free_node_lfree_pred(node);
    free(slist->random_seeds);

}


void print_skip_list_lfree_pred(Skip_list_lfree_pred* slist){

    Node_lfree_pred* node = NULL;

    node = getpointer_pred(slist->header->nexts[0]);

    printf("Skip_list_lock_free: ");

    while(node->nexts[0]){
        if(ismarked_pred(node->nexts[0])){
            node = getpointer_pred(node->nexts[0]);
            continue;
        }
        printf("(%d, %d) ", node->key, node->value);
        node = getpointer_pred(node->nexts[0]);
        
    }

    printf("\n");

}

void print_marked_skip_list_lfree_pred(Skip_list_lfree_pred* slist) {
    Node_lfree_pred* node = NULL;
    node = slist->header;

    printf("Marked nodes: ");

    while (getpointer_pred(node->nexts[0]) != NULL) {
        node = getpointer_pred(node->nexts[0]);
        if (node->nexts[0] != NULL && ismarked_pred(node->nexts[0])) {
            printf("(%d, %d) ", node->key, node->value);
        }
    }

    printf("\n");
}