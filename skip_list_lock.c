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
    Node_l** h_next = NULL;
    Node_l** t_next = NULL;
    Node_l** h_prev = NULL;
    Node_l** t_prev = NULL;

    #pragma omp parallel default(shared)
    {   


        #pragma omp master 
        {
            init_random_l(slist);

            header = (Node_l*)malloc(sizeof(Node_l));
            tail = (Node_l*)malloc(sizeof(Node_l));

            if(!header || !tail){
                fprintf(stderr, "Malloc failed");
                return false;
            }
        

            header->level = max_level;
            header->key = INT_MAX;
            header->value = 0;

            tail->level = max_level;
            tail->value = INT_MIN;
            header->value = 0;

            h_next = (Node_l**)malloc((max_level+1)*sizeof(Node_l*));
            t_next = (Node_l**)malloc((max_level+1)*sizeof(Node_l*));
            h_prev = (Node_l**)malloc((max_level+1)*sizeof(Node_l*));
            t_prev = (Node_l**)malloc((max_level+1)*sizeof(Node_l*));

            if(!h_next || !t_next || !h_prev || !t_prev){
                fprintf(stderr, "Malloc failed");
                return false;
            }
        }
        #pragma omp barrier

        for(int i = 0; i <= max_level; i++){
            h_next[i] = NULL;
            h_prev[i] = tail;
            t_next[i] = header;
            t_prev[i] = NULL;
        }


        #pragma omp master
        {
            header->next = h_next;
            header->prev = h_prev;
            tail->next = t_next;
            tail->prev = t_prev;

            slist->header = header;
            slist->tail = tail;
            slist->max_level = max_level;
        }
    }
    
    return true;
}


bool init_random_l(Skip_list_l* slist){

    time_t t;
    int id = omp_get_thread_num();
    
    #pragma omp paralell private(t, id)
    {
        #pragma omp single
        {
            slist->random_seeds = 
            (unsigned int*)malloc(sizeof(unsigned int)*omp_get_num_threads());
            if(!slist->random_seeds){
                fprintf(stderr, "Malloc failed");
                return false;
            }
        }
    
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
