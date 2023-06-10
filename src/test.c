

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
    for(int size = 2; size <= 1000; size++ /* size *= 10 */ ){
        
        Skip_list_seq seq;
        Skip_list_l l;
        
        // init lists
        init_skip_list_l(&l, size, size);
        init_skip_list_seq(&seq, size);
        
        // init numbers array
        int* numbers = (int*)malloc(sizeof(int)*size);
        random_array(numbers, size);

        // contains array
        bool* contains_array = (bool*)malloc(sizeof(bool)*size);

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

        //remove
        for(int i = 0; i < size; i++){
            if(numbers[i]%2 == 0){
                remove_skip_list_seq(&seq, numbers[i]);
            }
        }

        #pragma omp parallel num_threads(size)
        {
            #pragma omp for
            for(int i = 0; i < size; i++){
                if(numbers[i]%2 == 0){
                    remove_skip_list_l(&l, numbers[i]);
                }
            }
        }

        //compare
        printf("Extensive test (%i Threads): Remove: ", size);
        if(!compare_results_l(&seq, &l)){
            printf("\nTest failed\n");
            exit(1);
        }

        printf("Extensive test (%i Threads): Contains: ", size);
        #pragma omp parallel num_threads(size)
        {
            #pragma omp for
            for(int i = 0; i < size; i++){
                contains_array[i] = contains_skip_list_l(&l, numbers[i]);
            }
        }

        for(int i = 0; i < size; i++){
            if(contains_array[i] != contains_skip_list_seq(&seq, numbers[i])){
                printf("Comparison Failed: %i %s in locked skiplist and %s in sequential skiplist.\n", numbers[i], !contains_array[i] ? "contained" : "not contained", contains_array[i] ? "contained": "not contained");
                exit(1);
            }
        }
        printf("Comparison Succeeded\n\n");
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
    int size = 1000;
    int max_level = 3;
    double tic, toc;
    double seq_exec_time;
    double par_exec_time;

    for(int t = 2; t <= 16; t*=2 /* size *= 10 */ ){
        
        Skip_list_seq slist_seq;
        Skip_list_lfree slist_lfree;
        
        // init lists
        init_skip_list_seq(&slist_seq, max_level);
        init_skip_list_lfree(&slist_lfree, max_level, t);
        
        // init numbers array
        int* numbers = (int*)malloc(sizeof(int)*size);
        random_array(numbers, size);

        // contains array
        bool* contains_array = (bool*)malloc(sizeof(bool)*size);

        // add
        #pragma omp single
        {
            tic = omp_get_wtime();
            for(int i = 0; i < size; i++){
                add_skip_list_seq(&slist_seq, numbers[i], numbers[i]);
            }
            toc = omp_get_wtime();
        }
        seq_exec_time = toc-tic;

        #pragma omp parallel num_threads(t)
        {   
            tic = omp_get_wtime();
            #pragma omp for
            for(int i = 0; i < size; i++){
                add_skip_list_lfree(&slist_lfree, numbers[i], numbers[i]);
            }
            toc = omp_get_wtime();
        }
        par_exec_time = toc-tic;

        printf("Extensive test (%i Threads): Add: ", t);
        
        // add compare 
        if(!compare_results_lfree(&slist_seq, &slist_lfree)){
            printf("\nTest failed\n");
            break;
        }

        printf("Time: Seq: %f, Par: %f\n", seq_exec_time, par_exec_time);

        // remove
        for(int i = 0; i < size; i++){
            if(numbers[i]%2 == 0){
                remove_skip_list_seq(&slist_seq, numbers[i]);
            }
        }

        #pragma omp parallel num_threads(t)
        {
            #pragma omp for
            for(int i = 0; i < size; i++){
                if(numbers[i]%2 == 0){
                    remove_skip_list_lfree(&slist_lfree, numbers[i]);
                }
            }
        }
        printf("Extensive test (%i Threads): Remove: ", t);

        // remove compare 
        if(!compare_results_lfree(&slist_seq, &slist_lfree)){
            printf("\nTest failed\n");
            break;
        }

        
        // contains

        printf("Extensive test (%i Threads): Contains: ", t);

        #pragma omp parallel num_threads(t)
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


/////////////////////////////////////////
// Test: lock free skip list improved ///
////////////////////////////////////////



void test_lockfree_improved_1(){
    int size = 10000;
    int max_level = 3;
    double tic, toc;
    double seq_exec_time;
    double par_exec_time;

    for(int t = 2; t <= 16; t*=2 /* size *= 10 */ ){
        
        Skip_list_seq slist_seq;
        Skip_list_lfree_improved slist_lfree_improved;
        
        // init lists
        init_skip_list_seq(&slist_seq, max_level);
        init_skip_list_lfree_improved(&slist_lfree_improved, max_level, t);
        
        // init numbers array
        int* numbers = (int*)malloc(sizeof(int)*size);
        random_array(numbers, size);

        // contains array
        bool* contains_array = (bool*)malloc(sizeof(bool)*size);

        // add
        #pragma omp single
        {   
            tic = omp_get_wtime();
            for(int i = 0; i < size; i++){
                add_skip_list_seq(&slist_seq, numbers[i], numbers[i]);
            }
            toc = omp_get_wtime();
        }
        seq_exec_time = toc-tic;

        #pragma omp parallel num_threads(t)
        {
            tic = omp_get_wtime();
            #pragma omp for
            for(int i = 0; i < size; i++){
                add_skip_list_lfree_improved(&slist_lfree_improved, numbers[i], numbers[i]);
            }
            toc = omp_get_wtime();
        }
        par_exec_time = toc-tic;

        printf("Extensive test (%i Threads): Add: ", t);
        
        // add compare 
        if(!compare_results_lfree_improved(&slist_seq, &slist_lfree_improved)){
            printf("\nTest failed\n");
            break;
        }

        printf("Time: Seq: %f, Par: %f\n", seq_exec_time, par_exec_time);

        // remove
        for(int i = 0; i < size; i++){
            if(numbers[i]%2 == 0){
                remove_skip_list_seq(&slist_seq, numbers[i]);
            }
        }

        #pragma omp parallel num_threads(t)
        {
            #pragma omp for
            for(int i = 0; i < size; i++){
                if(numbers[i]%2 == 0){
                    remove_skip_list_lfree_improved(&slist_lfree_improved, numbers[i]);
                }
            }
        }
        printf("Extensive test (%i Threads): Remove: ", t);

        // remove compare 
        if(!compare_results_lfree_improved(&slist_seq, &slist_lfree_improved)){
            printf("\nTest failed\n");
            break;
        }

        
        // contains

        printf("Extensive test (%i Threads): Contains: ", t);

        #pragma omp parallel num_threads(t)
        {
            #pragma omp for
            for(int i = 0; i < size; i++){
                contains_array[i] = contains_skip_list_lfree_improved(&slist_lfree_improved, numbers[i]);
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
        free_skip_list_lfree_improved(&slist_lfree_improved);
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



bool compare_results_lfree_improved(Skip_list_seq* slist_seq, Skip_list_lfree_improved* slist_lfree_improved){
    Node_seq* node_seq;
    Node_lfree_improved* node_lfree_improved;

    node_seq = slist_seq->header->nexts[0];
    node_lfree_improved = getpointer_improved(slist_lfree_improved->header->nexts[0]);

    while (node_seq->nexts[0] && node_lfree_improved->nexts[0]) {
   
        if(ismarked_improved(node_lfree_improved->nexts[0])){
            node_lfree_improved = getpointer_improved(node_lfree_improved->nexts[0]);
            continue;
        }
        if (node_seq->key != node_lfree_improved->key) {
            printf("Comparison Failed: %d and %d are not the same.\n",
                    node_seq->key, node_lfree_improved->key);
            return false;
        }
        node_seq = node_seq->nexts[0];
        node_lfree_improved = getpointer_improved(node_lfree_improved->nexts[0]);
          
    }   

    if (node_seq->nexts[0] || node_lfree_improved->nexts[0]) {
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