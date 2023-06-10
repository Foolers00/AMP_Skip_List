#ifndef TEST
#define TEST
#include "test.h"
#endif

// #undef COUNTERS

#ifdef COUNTERS
#define INC(_c) ((_c)++)
#else
#ifdef INC
#undef INC
#endif
#define INC(_c)
#endif

typedef struct bench_result {
    double time_seq;
    double time_l;
    double time_lfree;
    double throughput_seq;
    double throughput_l;
    double throughput_lfree;
    // struct counters reduced_counters;
} bench_result;

bench_result small_bench(int t, int times, int max_level);


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

    /////////////////////////////////////////
    // Test: lock free skip list improved ///
    ////////////////////////////////////////

    //test_lockfree_improved_1();


    ////////////////////////////////
    /////// Test: Benchmarks ///////
    ////////////////////////////////

    //small_bench(16, 100000, 3);

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

bench_result benchmark_random(int n_ops, int percent_adds, int percent_rems, int max_level, int t) {    // TODO: add perfomance counters (implementation missing)
    bench_result result;
    int i;
    double exec_time;
    unsigned long long tops = 0;

    #ifdef COUNTERS
    unsigned long long adds, rems, cons, trav, fail, rtry;
    adds = 0;
    rems = 0;
    cons = 0;
    trav = 0;
    fail = 0;
    rtry = 0;
    #endif

    Skip_list_seq slist_seq;
    Skip_list_l slist_l;
    Skip_list_lfree slist_lfree;

    // init lists
    init_skip_list_seq(&slist_seq, max_level);
    init_skip_list_l(&slist_l, max_level, t);
    init_skip_list_lfree(&slist_lfree, max_level, t);

    // init numbers array
    int *numbers = (int*)malloc(sizeof(int)*n_ops);
    random_array(numbers, n_ops);
    for (i = 0; i < n_ops; i++) {
        numbers[i] %= 10*n_ops;         // limit keys to 10 times the number of operations
    }

    // precompute operations as percentage 0-100
    int *operations = (int*)malloc(sizeof(int)*n_ops);
    random_array(operations, n_ops);
    for (i = 0; i < n_ops; i++) {
        operations[i] %= 100;
    }

    printf("Running random throughput benchmark with %d threads...\n", t);
    printf("Type \tTime (ms) \tTotal ops \tThroughput (Kops/s)\n");

    double tic, toc;
    #pragma omp single
    {
        int ops = 0;

        tic = omp_get_wtime();
        for (i = 0; i < n_ops; i++) {
            if (operations[i] < percent_adds) {
                add_skip_list_seq(&slist_seq, numbers[i], numbers[i]);
                INC(ops);
            } else if (operations[i] < percent_adds+percent_rems) {
                remove_skip_list_seq(&slist_seq, numbers[i]);
                INC(ops);
            } else {
                contains_skip_list_seq(&slist_seq, numbers[i]);
                INC(ops);
            }
        }
        toc = omp_get_wtime();
        tops += ops;
    }
    exec_time = toc-tic;
    result.time_seq = exec_time;
    result.throughput_seq = ((double)tops/exec_time)/1000;
    printf("SEQ \t%7.2f \t%llu \t\t%7.2f\n", exec_time*1000, tops, ((double)tops/exec_time)/1000);
    tops = 0;

    #pragma omp parallel num_threads(t)
    {
        int ops = 0;

        #pragma omp barrier
        tic = omp_get_wtime();
        #pragma omp for
        for (i = 0; i < n_ops; i++) {
            if (operations[i] < percent_adds) {
                add_skip_list_l(&slist_l, numbers[i], numbers[i]);
                INC(ops);
            } else if (operations[i] < percent_adds+percent_rems) {
                remove_skip_list_l(&slist_l, numbers[i]);
                INC(ops);
            } else {
                contains_skip_list_l(&slist_l, numbers[i]);
                INC(ops);
            }
        }
        toc = omp_get_wtime();
        tops += ops;
    }
    exec_time = toc-tic;
    result.time_l = exec_time;
    result.throughput_l = ((double)tops/exec_time)/1000;
    printf("LOCK \t%7.2f \t%llu \t\t%7.2f\n", exec_time*1000, tops, ((double)tops/exec_time)/1000);
    tops = 0;

    #pragma omp parallel num_threads(t)
    {
        int ops = 0;

        #pragma omp barrier
        tic = omp_get_wtime();
        #pragma omp for
        for (i = 0; i < n_ops; i++) {
            if (operations[i] < percent_adds) {
                add_skip_list_lfree(&slist_lfree, numbers[i], numbers[i]);
                INC(ops);
            } else if (operations[i] < percent_adds+percent_rems) {
                remove_skip_list_lfree(&slist_lfree, numbers[i]);
                INC(ops);
            } else {
                contains_skip_list_lfree(&slist_lfree, numbers[i]);
                INC(ops);
            }
        }
        toc = omp_get_wtime();
        tops += ops;

        #ifdef COUNTERS
        adds += slist_lfree.adds;
        rems += slist_lfree.rems;
        cons += slist_lfree.cons;
        trav += slist_lfree.trav;
        fail += slist_lfree.fail;
        rtry += slist_lfree.rtry;
        #endif
    }
    exec_time = toc-tic;
    result.time_lfree = exec_time;
    result.throughput_lfree = ((double)tops/exec_time)/1000;
    printf("LFREE \t%7.2f \t%llu \t\t%7.2f\n", exec_time*1000, tops, ((double)tops/exec_time)/1000);

    free(numbers);
    return result;
}

bench_result benchmark_separate(int times, int max_level, int t) {
    bench_result result;
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
    for (int i = 0; i < times; i++) {
        numbers[i] %= 10*times;         // limit keys to 10 times the number of operations
    }
    toc = omp_get_wtime();
    // printf("Generating random numbers took: %fs\n", toc - tic);


    //////////////////////////////// 
    // Sequential Skip List ////////
    ////////////////////////////////

    // add
    tic = omp_get_wtime();
    for (int i = 0; i < times; i++) {
        add_skip_list_seq(&slist_seq, numbers[i], numbers[i]);
        printf("Added node: %d\n", i);
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
    printf("Adding %d nodes to lock free skip list (%d levels) with %d threads took: %fs\n", times, max_level, t, toc - tic);
    #ifdef COUNTERS
    printf("[COUNTERS] node adds: %lld (%lld failed CASs)\n", (&slist_lfree)->adds, (&slist_lfree)->fail);  // counts the add for every level
    (&slist_lfree)->fail = 0;
    #endif


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
    printf("Removing %d nodes from lock free skip list (%d levels) with %d threads took: %fs\n", times/2, max_level, t, toc - tic);
    #ifdef COUNTERS
    printf("[COUNTERS] node removes: %lld (%lld failed CASs)\n", (&slist_lfree)->adds, (&slist_lfree)->fail);
    (&slist_lfree)->fail = 0;
    #endif


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
    printf("Checking %d nodes for contains in lock free skip list (%d levels) with %d threads took: %fs\n", times, max_level, t, toc - tic);
    printf("[COUNTERS] nodes checked for contains: %lld\n", (&slist_lfree)->cons);

    free_skip_list_lfree(&slist_lfree);
    result.time_lfree = (toc - tic);

    // free random numbers
    free(numbers);

    return result;
}

bench_result small_bench(int t, int times, int max_level) {
    bench_result br = {.time_seq=0, .time_l=0, .time_lfree=0, .throughput_seq=0, .throughput_l=0, .throughput_lfree=0};

    if (t <= 0) t = omp_get_max_threads();
    if (max_level <= 0) max_level = 3;

    omp_set_num_threads(t);

    // br = benchmark_separate(times, max_level, t);
    br = benchmark_random(times, 25, 25, max_level, t);
    
    return br;
}


