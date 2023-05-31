#ifndef TEST
#define TEST
#include "test.h"
#endif



struct bench_result {
    float time;
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

    test_lock_2();


    ////////////////////////////////
    // Test: lock free skip list ///
    ////////////////////////////////

    //test_lockfree_1();

    //test_lockfree_2();


    ////////////////////////////////
    /////// Test: Benchmarks ///////
    ////////////////////////////////

    // small_bench(16, 10000);

    return 0;
}


////////////////////////////////
///// Benchmark: Add nodes /////
////////////////////////////////

void bench_l_add(Skip_list_l *slist_l, int numbers[], int num_len) {
    int tid = omp_get_thread_num();
    printf("Thread %d started.\n", tid);
    // for (int i = 0; i < num_len; i++) {
    //     printf("numbers[%d]: %d\n", i, numbers[i]);
    // }

    // Barrier to force OMP to start all threads at the same time
    #pragma omp barrier

    // add
    for(int i = 0; i < num_len; i++) {
        add_skip_list_l(slist_l, numbers[i], numbers[i]);
    }
}

struct bench_result small_bench(int t, int times, int max_level) {
    struct bench_result result;
    double tic, toc;

    Skip_list_l slist_l;

    // init lists
    init_skip_list_l(&slist_l, max_level, t); 

    // init numbers array
    int* numbers = (int*)malloc(sizeof(int)*times);
    random_array(numbers, times);

    omp_set_num_threads(t);
    tic = omp_get_wtime();
    {
        #pragma omp parallel for
        for (int i=0; i<t; i++) {
            bench_l_add(&slist_l, numbers, times);
        }
    }
    toc = omp_get_wtime();

    // free
    free_skip_list_l(&slist_l);

    result.time = (toc - tic);
    printf("Adding %d nodes to lock based skip list (%d levels) with %d threads took: %fs\n", times, max_level, t, toc - tic);

    return result;
}


