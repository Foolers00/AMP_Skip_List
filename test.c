

#ifndef TEST
#define TEST
#include "test.h"
#endif




//////////////////////////////// 
// Test: sequential skip list //
////////////////////////////////
void test_seq_1(){
    int numbers[] = {1, 2, 3, 4, 5};
        
        
    Skip_list_seq slist_seq;
    
    init_skip_list_seq(&slist_seq, 10);


    // add
    for(int i = 0; i < 5; i++){
        add_skip_list_seq(&slist_seq, numbers[i], numbers[i]);
    }

    print_skip_list_seq(&slist_seq);

    // remove
    remove_skip_list_seq(&slist_seq, 3);
    print_skip_list_seq(&slist_seq);

    // contains
    printf("4 is contained: %s\n", contains_skip_list_seq(&slist_seq, 4) ? "true" : "false");
    printf("3 is contained: %s\n", contains_skip_list_seq(&slist_seq, 3) ? "true" : "false");
    add_skip_list_seq(&slist_seq, numbers[2], numbers[2]);
    print_skip_list_seq(&slist_seq);

    //////////////////////////////// 
    // FREE ////////////////////////
    ////////////////////////////////

    free_skip_list_seq(&slist_seq);
}



//////////////////////////////// 
// Test: lock skip list ////////
////////////////////////////////

void test_lock_1(){
    
    int num_of_threads = 5; 
    int numbers[] = {1, 2, 3, 4, 5};

    Skip_list_seq slist_seq;
    Skip_list_l slist_l;
    

    // init skip list
    init_skip_list_seq(&slist_seq, 10);
    init_skip_list_l(&slist_l, 10, num_of_threads);


    // add
    for(int i = 0; i < 5; i++){
        add_skip_list_seq(&slist_seq, numbers[i], numbers[i]);
    }


    #pragma omp parallel num_threads(num_of_threads)
    {
        #pragma omp for
        for(int i = 0; i < 5; i++){
            add_skip_list_l(&slist_l, numbers[i], numbers[i]);
        }
    }

    print_skip_list_l(&slist_l);

    // remove
    remove_skip_list_seq(&slist_seq, 3);
    remove_skip_list_l(&slist_l, 3);
    print_skip_list_l(&slist_l);

    // contains
    printf("4 is contained in skip list seq: %s\n", contains_skip_list_seq(&slist_seq, 4) ? "true" : "false");
    printf("4 is contained in skip list lockfree: %s\n", contains_skip_list_l(&slist_l, 4) ? "true" : "false");

    printf("3 is contained in skip list seq: %s\n", contains_skip_list_seq(&slist_seq, 3) ? "true" : "false");
    printf("3 is contained in skip list lockfree: %s\n", contains_skip_list_l(&slist_l, 3) ? "true" : "false");
    

    // reset
    add_skip_list_seq(&slist_seq, numbers[2], numbers[2]);
    add_skip_list_l(&slist_l, numbers[2], numbers[2]);

    // compare
    compare_results_l(&slist_seq, &slist_l);

    //////////////////////////////// 
    // FREE ////////////////////////
    ////////////////////////////////
    free_skip_list_seq(&slist_seq);
    free_skip_list_l(&slist_l);
}



void test_lock_2(){
    for(int size = 13; size <= 1000; size++ /* size *= 10 */ ){
        
        Skip_list_seq seq;
        Skip_list_l l;
        
        // init lists
        init_skip_list_l(&l, size, size);
        init_skip_list_seq(&seq, size);
        
        // init numbers array
        int* numbers = (int*)malloc(sizeof(int)*size);
        random_array(numbers, size);

        // add
        for(int i = 0; i < size; i++){
            add_skip_list_seq(&seq, numbers[i], numbers[i]);
        }

        #pragma omp parallel num_threads(size)
        {
            #pragma omp for
            for(int i = 0; i < size; i++){
                add_skip_list_l(&l, numbers[i], numbers[i]);
            }
        }
        printf("Extensive test (%i Threads): Add: ", size);
        compare_results_l(&seq, &l);

        // free
        free_skip_list_seq(&seq);
        free_skip_list_l(&l);
    }
}



////////////////////////////////
// Test: lock free skip list ///
////////////////////////////////

void test_lockfree_1(){

    int num_of_threads = 5;
    int numbers[] = {1, 2, 3, 4, 5};

    Skip_list_seq slist_seq;
    Skip_list_lfree slist_lfree;
    

    // init
    init_skip_list_seq(&slist_seq, 10);
    init_skip_list_lfree(&slist_lfree, 10, 5);


    // add
    for(int i = 0; i < 5; i++){
        add_skip_list_seq(&slist_seq, numbers[i], numbers[i]);
    }
    
    #pragma omp parallel num_threads(num_of_threads)
    {
        #pragma omp for
        for(int i = 0; i < 5; i++){
            add_skip_list_lfree(&slist_lfree, numbers[i], numbers[i]);
        }
    }

    // compare
    compare_results_lfree(&slist_seq, &slist_lfree);

    print_skip_list_lfree(&slist_lfree);

    // remove
    remove_skip_list_seq(&slist_seq, 3);
    remove_skip_list_lfree(&slist_lfree, 3);
    print_skip_list_lfree(&slist_lfree);

    // contains
    printf("4 is contained in skip list seq: %s\n", contains_skip_list_seq(&slist_seq, 4) ? "true" : "false");
    printf("4 is contained in skip list lockfree: %s\n", contains_skip_list_lfree(&slist_lfree, 4) ? "true" : "false");

    printf("3 is contained in skip list seq: %s\n", contains_skip_list_seq(&slist_seq, 3) ? "true" : "false");
    printf("3 is contained in skip list lockfree: %s\n", contains_skip_list_lfree(&slist_lfree, 3) ? "true" : "false");
    
    // reset
    add_skip_list_seq(&slist_seq, numbers[2], numbers[2]);
    add_skip_list_lfree(&slist_lfree, numbers[2], numbers[2]);

    // compare
    compare_results_lfree(&slist_seq, &slist_lfree);

    //////////////////////////////// 
    // FREE ////////////////////////
    ////////////////////////////////
    free_skip_list_seq(&slist_seq);
    free_skip_list_lfree(&slist_lfree);
}


void test_lockfree_2(){
    for(int size = 1; size <= 1000000; size*=10 /* size *= 10 */ ){
        
        Skip_list_seq slist_seq;
        Skip_list_lfree slist_lfree;
        
        // init lists
        init_skip_list_seq(&slist_seq, size);
        init_skip_list_lfree(&slist_lfree, size, size);
        
        // init numbers array
        int* numbers = (int*)malloc(sizeof(int)*size);
        random_array(numbers, size);

        // contains array
        bool* contains_array = (bool*)malloc(sizeof(bool)*size);

        // add
        for(int i = 0; i < size; i++){
            add_skip_list_seq(&slist_seq, numbers[i], numbers[i]);
        }

        #pragma omp parallel num_threads(size)
        {
            #pragma omp for
            for(int i = 0; i < size; i++){
                add_skip_list_lfree(&slist_lfree, numbers[i], numbers[i]);
            }
        }
        printf("Extensive test (%i Threads): Add: ", size);
        
        // add compare 
        if(!compare_results_lfree(&slist_seq, &slist_lfree)){
            printf("\nTest failed\n");
            break;
        }

        // remove
        for(int i = 0; i < size; i++){
            if(numbers[i]%2 == 0){
                remove_skip_list_seq(&slist_seq, numbers[i]);
            }
        }

        #pragma omp parallel num_threads(size)
        {
            #pragma omp for
            for(int i = 0; i < size; i++){
                if(numbers[i]%2 == 0){
                    remove_skip_list_lfree(&slist_lfree, numbers[i]);
                }
            }
        }
        printf("Extensive test (%i Threads): Remove: ", size);

        // remove compare 
        if(!compare_results_lfree(&slist_seq, &slist_lfree)){
            printf("\nTest failed\n");
            break;
        }

        
        // contains

        printf("Extensive test (%i Threads): Contains: ", size);

        #pragma omp parallel num_threads(size)
        {
            #pragma omp for
            for(int i = 0; i < size; i++){
                contains_array[i] = contains_skip_list_lfree(&slist_lfree, numbers[i]);
            }
        }

        for(int i = 0; i<size; i++){
            if(contains_array[i] != contains_skip_list_seq(&slist_seq, numbers[i])){
                printf("Comparison Failed: %s and %s are not the same.\n", !contains_array[i] ? "true" : "false", 
                contains_array[i] ? "true" : "false");
                goto _out;
            }
        }

        printf("Comparison Succeeded\n\n");


        // free
        free_skip_list_seq(&slist_seq);
        free_skip_list_lfree(&slist_lfree);
    }

    _out:;
}



////////////////////////////////
// Test: additional methods ////
////////////////////////////////

bool compare_results(Skip_list_seq* slist_seq, Skip_list_l* slist_l, Skip_list_lfree* slist_lfree) {
    Node_seq* node_seq;
    Node_l* node_l;
    Node_lfree* node_lfree;

    node_seq = slist_seq->header->nexts[0];
    node_l = slist_l->header->nexts[0];
    node_lfree = getpointer(slist_lfree->header->nexts[0]);

    while (node_seq->nexts[0] && node_l->nexts[0] && getpointer(node_lfree->nexts[0])) {
        if (node_seq->key != node_l->key || node_l->key != node_lfree->key || node_seq->key != node_lfree->key) {
            printf("Comparison Failed: %d, %d and %d are not all the same.\n",
                    node_seq->key, node_l->key, node_lfree->key);
            return false;
        }
        node_seq = node_seq->nexts[0];
        node_l = node_l->nexts[0];
        node_lfree = getpointer(node_lfree->nexts[0]);
    }

    if (node_seq->nexts[0] || node_l->nexts[0] || getpointer(node_lfree->nexts[0])) {
        printf("Comparison Failed: Lists are not the same length\n");
        return false;
    }

    printf("Comparison Succeeded\n");
    return true;
}


bool compare_results_l(Skip_list_seq* slist_seq, Skip_list_l* slist_l){

    Node_seq* node_seq;
    Node_l* node_l;

    node_seq = slist_seq->header->nexts[0];
    node_l = slist_l->header->nexts[0];

    while(node_seq->nexts[0] && node_l->nexts[0]){
        if(node_seq->key != node_l->key){
            printf("Comparison Failed: %d not same as %d\n",
            node_seq->key, node_l->key);
            return false;
        }
        node_seq = node_seq->nexts[0];
        node_l = node_l->nexts[0];
    }

    if(node_seq->nexts[0] || node_l->nexts[0]){
        printf("Comparison Failed: Lists are not the same length\n");
        return false;
    }

    printf("Comparison Succeded\n");
    return true;

}


bool compare_results_lfree(Skip_list_seq* slist_seq, Skip_list_lfree* slist_lfree){
    Node_seq* node_seq;
    Node_lfree* node_lfree;

    node_seq = slist_seq->header->nexts[0];
    node_lfree = getpointer(slist_lfree->header->nexts[0]);

    while (node_seq->nexts[0] && node_lfree->nexts[0]) {
   
        if(ismarked(node_lfree->nexts[0])){
            node_lfree = getpointer(node_lfree->nexts[0]);
            continue;
        }
        if (node_seq->key != node_lfree->key) {
            printf("Comparison Failed: %d and %d are not the same.\n",
                    node_seq->key, node_lfree->key);
            return false;
        }
        node_seq = node_seq->nexts[0];
        node_lfree = getpointer(node_lfree->nexts[0]);
          
    }   

    if (node_seq->nexts[0] || node_lfree->nexts[0]) {
        printf("Comparison Failed: Lists are not the same length\n");
        return false;
    }

    printf("Comparison Succeeded\n");
    return true;
}


void random_array(int* numbers, int size){
    srand(time(NULL));
    for (int i = 0; i < size; i++) {
        numbers[i] = rand();
    }
}