#ifndef TEST
#define TEST
#include "test.h"
#endif



struct bench_result {
    float time_seq;
    float time_l;
    float time_lfree;
    // struct counters reduced_counters;
};

struct bench_result small_bench(int t, int times, int max_level);


int main(){


    //////////////////////////////// 
    // Test: sequential skip list //
    ////////////////////////////////
    
    //test_seq_1();

    //////////////////////////////// 
    // Test: lock skip list ////////
    ////////////////////////////////
    
    //test_lock_1();

    // test_lock_2();


    ////////////////////////////////
    // Test: lock free skip list ///
    ////////////////////////////////

    //test_lockfree_1();

    //test_lockfree_2();


    ////////////////////////////////
    /////// Test: Benchmarks ///////
    ////////////////////////////////

    small_bench(16, 10000, 3);

    return 0;
}


////////////////////////////////
///// Benchmark: Add nodes /////
////////////////////////////////

void bench_l_add(Skip_list_l *slist, int numbers[], int num_len, int num_threads) {
    
    #pragma omp parallel num_threads(num_threads)
    {
        #pragma omp for
        // add
        for(int i = 0; i < num_len; i++) {
            add_skip_list_l(slist, numbers[i], numbers[i]);
        }
    }
}

void bench_lfree_add(Skip_list_lfree *slist, int numbers[], int num_len, int num_threads) {

    #pragma omp parallel num_threads(num_threads)
    {
    // add
        #pragma omp for
        for(int i = 0; i < num_len; i++) {
            add_skip_list_lfree(slist, numbers[i], numbers[i]);
        }
    }
}

struct bench_result small_bench(int t, int times, int max_level) {
    struct bench_result result;
    double tic, toc;

    Skip_list_seq slist_seq;
    Skip_list_l slist_l;
    Skip_list_lfree slist_lfree;

    // init lists
    tic = omp_get_wtime();
    init_skip_list_seq(&slist_seq, max_level);
    init_skip_list_l(&slist_l, max_level, t);
    init_skip_list_lfree(&slist_lfree, max_level, t);
    toc = omp_get_wtime();
    // printf("Initialising lists took: %fs\n", toc - tic);

    // init numbers array
    int* numbers = (int*)malloc(sizeof(int)*times);
    tic = omp_get_wtime();
    random_array(numbers, times);
    toc = omp_get_wtime();
    // printf("Generating random numbers took: %fs\n", toc - tic);


    //////////////////////////////// 
    // Sequential Skip List ////////
    ////////////////////////////////

    // add
    tic = omp_get_wtime();
    for (int i = 0; i < times; i++) {
        add_skip_list_seq(&slist_seq, numbers[i], numbers[i]);
    }
    toc = omp_get_wtime();


    // remove 
    tic = omp_get_wtime();
    for (int i = 0; i < times; i++) {
        if(numbers[i]%2 == 0){
            remove_skip_list_seq(&slist_seq, numbers[i]);
        }
    }
    toc = omp_get_wtime();


    // contains
    tic = omp_get_wtime();
    for (int i = 0; i < times; i++) {
        contains_skip_list_seq(&slist_seq, numbers[i]);
    }
    toc = omp_get_wtime();

    free_skip_list_seq(&slist_seq);
    result.time_seq = (toc - tic);
    printf("Adding %d nodes to sequential skip list (%d levels) took: %fs\n", times, max_level, toc - tic);


    //////////////////////////////// 
    // Lock Skip List //////////////
    ////////////////////////////////

    // add
    tic = omp_get_wtime();
    #pragma omp parallel num_threads(t)
    {
        #pragma omp for
        for(int i = 0; i < times; i++) {
            add_skip_list_l(&slist_l, numbers[i], numbers[i]);
        }
    }
    toc = omp_get_wtime();


    // remove
    tic = omp_get_wtime();
    #pragma omp parallel num_threads(t)
    {
        #pragma omp for
        for(int i = 0; i < times; i++) {
            if(numbers[i] % 2 == 0){
                remove_skip_list_l(&slist_l, numbers[i]);
            }
        }
    }
    toc = omp_get_wtime();


    // contains
    tic = omp_get_wtime();
    #pragma omp parallel num_threads(t)
    {
        #pragma omp for
        for(int i = 0; i < times; i++) {
            contains_skip_list_l(&slist_l, numbers[i]);
        }
    }
    toc = omp_get_wtime();

    free_skip_list_l(&slist_l);
    result.time_l = (toc - tic);
    printf("Adding %d nodes to lock based skip list (%d levels) with %d threads took: %fs\n", times, max_level, t, toc - tic);


    //////////////////////////////// 
    // Lock Free Skip List /////////
    ////////////////////////////////
    
    // add
    tic = omp_get_wtime();
    #pragma omp parallel num_threads(t)
    {
        #pragma omp for
        for(int i = 0; i < times; i++) {
            add_skip_list_lfree(&slist_lfree, numbers[i], numbers[i]);
        }
    }
    toc = omp_get_wtime();


    // remove
    tic = omp_get_wtime();
    #pragma omp parallel num_threads(t)
    {
        #pragma omp for
        for(int i = 0; i < times; i++) {
            if(numbers[i] % 2 == 0){
                remove_skip_list_lfree(&slist_lfree, numbers[i]);
            }
        }
    }
    toc = omp_get_wtime();


    // contains
    tic = omp_get_wtime();
    #pragma omp parallel num_threads(t)
    {
        #pragma omp for
        for(int i = 0; i < times; i++) {
            contains_skip_list_lfree(&slist_lfree, numbers[i]);
        }
    }
    toc = omp_get_wtime();

    free_skip_list_lfree(&slist_lfree);
    result.time_lfree = (toc - tic);
    printf("Adding %d nodes to lock free skip list (%d levels) with %d threads took: %fs\n", times, max_level, t, toc - tic);

    return result;
}


