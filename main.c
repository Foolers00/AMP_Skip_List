#ifndef SKIP_LIST_SEQ
#define SKIP_LIST_SEQ
#include "skip_list_seq.h"
#endif

#ifndef SKIP_LIST_LOCK
#define SKIP_LIST_LOCK
#include "skip_list_lock.h"
#endif



int main(){
    omp_set_num_threads(5);

    int numbers[] = {1, 2, 3, 4, 5};
    
    //////////////////////////////// 
    // Test: sequential skip list //
    ////////////////////////////////
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
    fprintf(stdout, "4 is contained: %s\n", contains_skip_list_seq(&slist_seq, 4) ? "true" : "false");
    fprintf(stdout, "3 is contained: %s\n", contains_skip_list_seq(&slist_seq, 3) ? "true" : "false");
    add_skip_list_seq(&slist_seq, numbers[2], numbers[2]);
    print_skip_list_seq(&slist_seq);

    




    //////////////////////////////// 
    // Test: lock skip list ////////
    ////////////////////////////////
    Skip_list_l slist_l;

    init_skip_list_l(&slist_l, 10);

    // add
    #pragma omp for 
    for(int i = 0; i < 5; i++){
        add_skip_list_l(&slist_l, numbers[i], numbers[i]);
    }

    print_skip_list_l(&slist_l);

    // remove
    remove_skip_list_l(&slist_l, 3);
    print_skip_list_l(&slist_l);

    // contains
    fprintf(stdout, "4 is contained: %s\n", contains_skip_list_l(&slist_l, 4) ? "true" : "false");
    fprintf(stdout, "3 is contained: %s\n", contains_skip_list_l(&slist_l, 3) ? "true" : "false");
    add_skip_list_l(&slist_l, numbers[2], numbers[2]);

    compare_results_l(&slist_seq, &slist_l);


    //////////////////////////////// 
    // FREE ////////////////////////
    ////////////////////////////////

    free_skip_list_seq(&slist_seq);
    free_skip_list_l(&slist_l);


    return 0;
}
