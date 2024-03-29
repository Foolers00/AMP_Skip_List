#ifndef TEST
#define TEST
#include "test.h"
#endif

#undef COUNTERS
// #define COUNTERS
// #define VERBOSE

// #ifdef COUNTERS
#undef INC
#define INC(_c) ((_c)++)
// #else
// #undef INC
// #define INC(_c)
// #endif

typedef struct bench_result {
    double time_seq;
    double time_l;
    double time_lfree;
    double time_lfree_improved;
    double time_lfree_pred;
    double throughput_seq;
    double throughput_l;
    double throughput_lfree;
    double throughput_lfree_improved;
    double throughput_lfree_pred;
    // struct counters reduced_counters;
} bench_result;

bench_result small_bench(int t, int times, int max_level, int percent_adds, int percent_rems);


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

    /////////////////////////////////////////
    // Test: lock free skip list improved ///
    ////////////////////////////////////////

    test_lockfree_pred_1();


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

bench_result benchmark_random(int n_ops, int percent_adds, int percent_rems, int max_level, int t) {
    bench_result result;
    int i;
    double exec_time;
    unsigned long long tops = 0;
    int keyrange = 10*n_ops;        // limit keys to 10 times the number of operations

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
    Skip_list_lfree_improved slist_lfree_improved;
    Skip_list_lfree_pred slist_lfree_pred;

    // init lists
    init_skip_list_seq(&slist_seq, max_level);
    init_skip_list_l(&slist_l, max_level, t);
    init_skip_list_lfree(&slist_lfree, max_level, t);
    init_skip_list_lfree_improved(&slist_lfree_improved, max_level, t);
    init_skip_list_lfree_pred(&slist_lfree_pred, max_level, t);

    // init numbers array
    int *numbers = (int*)malloc(sizeof(int)*n_ops);
    random_array(numbers, n_ops);
    for (i = 0; i < n_ops; i++) {
        numbers[i] %= keyrange;         // limit keys
    }

    // precompute operations as percentage 0-100
    int *operations = (int*)malloc(sizeof(int)*n_ops);
    random_array(operations, n_ops);
    for (i = 0; i < n_ops; i++) {
        operations[i] %= 100;
    }

    // prefill lists with some elements
    int rand_key;
    for (i = 0; i < n_ops/10; i++) {
        rand_key = rand() % keyrange;
        add_skip_list_seq(&slist_seq, rand_key, rand_key);
        add_skip_list_l(&slist_l, rand_key, rand_key);
        add_skip_list_lfree(&slist_lfree, rand_key, rand_key);
        add_skip_list_lfree_improved(&slist_lfree_improved, rand_key, rand_key);
        add_skip_list_lfree_pred(&slist_lfree_pred, rand_key, rand_key);
    }

    printf("=====Random mix throughput benchmark with: %d threads,%d levels,%d%% adds,%d%% rems=====\n", 
        t, max_level, percent_adds, percent_rems);
    printf("Type \t\tTime (ms) \tTotal ops \tThroughput (Kops/s)\n");

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

        #ifdef COUNTERS
        adds += slist_seq.adds;
        rems += slist_seq.rems;
        cons += slist_seq.cons;
        trav += slist_seq.trav;
        fail += slist_seq.fail;
        rtry += slist_seq.rtry;
        #endif
    }
    exec_time = toc-tic;
    result.time_seq = exec_time;
    result.throughput_seq = ((double)tops/exec_time)/1000;
    printf("SEQ \t\t%7.2f \t%llu \t\t%7.2f\n", exec_time*1000, tops, ((double)tops/exec_time)/1000);
    #ifdef VERBOSE
    printf(" ↳ adds=%llu rems=%llu cons=%llu trav=%llu fail=%llu rtry=%llu\n",
        adds, rems, cons, trav, fail, rtry);
    #endif
    tops = 0;
    #ifdef COUNTERS
    adds = 0; rems = 0; cons = 0; trav = 0; fail = 0; rtry = 0;
    #endif

    #pragma omp parallel num_threads(t) reduction(+:tops)
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
        #pragma omp barrier
        tops += ops;

        #ifdef COUNTERS
        adds += slist_l.adds;
        rems += slist_l.rems;
        cons += slist_l.cons;
        trav += slist_l.trav;
        fail += slist_l.fail;
        rtry += slist_l.rtry;
        #endif
    }
    exec_time = toc-tic;
    result.time_l = exec_time;
    result.throughput_l = ((double)tops/exec_time)/1000;
    printf("LOCK \t\t%7.2f \t%llu \t\t%7.2f\n", exec_time*1000, tops, ((double)tops/exec_time)/1000);
    #ifdef VERBOSE
    printf(" ↳ adds=%llu rems=%llu cons=%llu trav=%llu fail=%llu rtry=%llu\n",
        adds, rems, cons, trav, fail, rtry);
    #endif
    tops = 0;
    #ifdef COUNTERS
    adds = 0; rems = 0; cons = 0; trav = 0; fail = 0; rtry = 0;
    #endif


    #pragma omp parallel num_threads(t) reduction(+:tops)
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
        #pragma omp barrier
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
    printf("LFREE \t\t%7.2f \t%llu \t\t%7.2f\n", exec_time*1000, tops, ((double)tops/exec_time)/1000);
    #ifdef VERBOSE
    printf(" ↳ adds=%llu rems=%llu cons=%llu trav=%llu fail=%llu rtry=%llu\n",
        adds, rems, cons, trav, fail, rtry);
    #endif
    tops = 0;
    #ifdef COUNTERS
    adds = 0; rems = 0; cons = 0; trav = 0; fail = 0; rtry = 0;
    #endif


    #pragma omp parallel num_threads(t) reduction(+:tops)
    {
        int ops = 0;

        #pragma omp barrier
        tic = omp_get_wtime();
        #pragma omp for
        for (i = 0; i < n_ops; i++) {
            if (operations[i] < percent_adds) {
                add_skip_list_lfree_improved(&slist_lfree_improved, numbers[i], numbers[i]);
                INC(ops);
            } else if (operations[i] < percent_adds+percent_rems) {
                remove_skip_list_lfree_improved(&slist_lfree_improved, numbers[i]);
                INC(ops);
            } else {
                contains_skip_list_lfree_improved(&slist_lfree_improved, numbers[i]);
                INC(ops);
            }
        }
        toc = omp_get_wtime();
        #pragma omp barrier
        tops += ops;

        #ifdef COUNTERS
        adds += slist_lfree_improved.adds;
        rems += slist_lfree_improved.rems;
        cons += slist_lfree_improved.cons;
        trav += slist_lfree_improved.trav;
        fail += slist_lfree_improved.fail;
        rtry += slist_lfree_improved.rtry;
        #endif
    }
    exec_time = toc-tic;
    result.time_lfree_improved = exec_time;
    result.throughput_lfree_improved = ((double)tops/exec_time)/1000;
    printf("LFREE++\t\t%7.2f \t%llu \t\t%7.2f\n", exec_time*1000, tops, ((double)tops/exec_time)/1000);
    #ifdef VERBOSE
    printf(" ↳ adds=%llu rems=%llu cons=%llu trav=%llu fail=%llu rtry=%llu\n",
        adds, rems, cons, trav, fail, rtry);
    #endif
    tops = 0;
    #ifdef COUNTERS
    adds = 0; rems = 0; cons = 0; trav = 0; fail = 0; rtry = 0;
    #endif


    #pragma omp parallel num_threads(t) reduction(+:tops)
    {
        int ops = 0;

        #pragma omp barrier
        tic = omp_get_wtime();
        #pragma omp for
        for (i = 0; i < n_ops; i++) {
            if (operations[i] < percent_adds) {
                add_skip_list_lfree_pred(&slist_lfree_pred, numbers[i], numbers[i]);
                INC(ops);
            } else if (operations[i] < percent_adds+percent_rems) {
                remove_skip_list_lfree_pred(&slist_lfree_pred, numbers[i]);
                INC(ops);
            } else {
                contains_skip_list_lfree_pred(&slist_lfree_pred, numbers[i]);
                INC(ops);
            }
        }
        toc = omp_get_wtime();
        #pragma omp barrier
        tops += ops;

        #ifdef COUNTERS
        adds += slist_lfree_pred.adds;
        rems += slist_lfree_pred.rems;
        cons += slist_lfree_pred.cons;
        trav += slist_lfree_pred.trav;
        fail += slist_lfree_pred.fail;
        rtry += slist_lfree_pred.rtry;
        #endif
    }
    exec_time = toc-tic;
    result.time_lfree_pred = exec_time;
    result.throughput_lfree_pred = ((double)tops/exec_time)/1000;
    printf("LFREE++ PRED \t%7.2f \t%llu \t\t%7.2f\n", exec_time*1000, tops, ((double)tops/exec_time)/1000);
    #ifdef VERBOSE
    printf(" ↳ adds=%llu rems=%llu cons=%llu trav=%llu fail=%llu rtry=%llu\n",
        adds, rems, cons, trav, fail, rtry);
    #endif
    tops = 0;
    #ifdef COUNTERS
    adds = 0; rems = 0; cons = 0; trav = 0; fail = 0; rtry = 0;
    #endif

    free(numbers);
    free(operations);
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

    free_skip_list_lfree(&slist_lfree);
    result.time_lfree = (toc - tic);

    // free random numbers
    free(numbers);

    return result;
}

bench_result small_bench(int t, int times, int max_level, int percent_adds, int percent_rems) {
    bench_result br = 
        {.time_seq=0, .time_l=0, .time_lfree=0, .time_lfree_improved=0, .time_lfree_pred=0, 
        .throughput_seq=0, .throughput_l=0, .throughput_lfree=0, .throughput_lfree_improved=0, .throughput_lfree_pred=0};

    if (t <= 0) t = omp_get_max_threads();
    if (max_level <= 0) max_level = 3;

    omp_set_num_threads(t);

    // br = benchmark_separate(times, max_level, t);
    br = benchmark_random(times, percent_adds, percent_rems, max_level, t);
    
    return br;
}


