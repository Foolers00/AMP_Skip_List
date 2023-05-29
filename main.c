#ifndef SKIP_LIST_SEQ
#define SKIP_LIST_SEQ
#include "skip_list_seq.h"
#endif

#ifndef SKIP_LIST_LOCK
#define SKIP_LIST_LOCK
#include "skip_list_lock.h"
#endif

#ifndef SKIP_LIST_LOCK_FREE
#define SKIP_LIST_LOCK_FREE
#include "skip_list_lockf.h"
#endif


bool compare_results(Skip_list_seq* slist_seq, Skip_list_l* slist_l, Skip_list_lfree* slist_lfree);


int main(){
    omp_set_num_threads(5);
    int num_of_threads = 5; 

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
    #pragma omp parallel num_threads(num_of_threads)
    {
        #pragma omp for
        for(int i = 0; i < 5; i++){
            add_skip_list_l(&slist_l, numbers[i], numbers[i]);
        }
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
    // Test: lock free skip list ///
    ////////////////////////////////
    Skip_list_lfree slist_lfree;

    init_skip_list_lfree(&slist_lfree, 10);

    // add
    #pragma omp parallel num_threads(num_of_threads)
    {
        #pragma omp for
        for(int i = 0; i < 5; i++){
            add_skip_list_lfree(&slist_lfree, numbers[i], numbers[i]);
        }
    }

    print_skip_list_lfree(&slist_lfree);

    // remove
    remove_skip_list_lfree(&slist_lfree, 3);
    print_skip_list_lfree(&slist_lfree);

    // contains
    fprintf(stdout, "4 is contained: %s\n", contains_skip_list_lfree(&slist_lfree, 4) ? "true" : "false");
    fprintf(stdout, "3 is contained: %s\n", contains_skip_list_lfree(&slist_lfree, 3) ? "true" : "false");
    add_skip_list_lfree(&slist_lfree, numbers[2], numbers[2]);


    compare_results(&slist_seq, &slist_l, &slist_lfree);


    //////////////////////////////// 
    // FREE ////////////////////////
    ////////////////////////////////

    free_skip_list_seq(&slist_seq);
    free_skip_list_l(&slist_l);


    return 0;
}

bool compare_results(Skip_list_seq* slist_seq, Skip_list_l* slist_l, Skip_list_lfree* slist_lfree) {
    Node_seq* node_seq;
    Node_l* node_l;
    Node_lfree* node_lfree;

    node_seq = slist_seq->header->nexts[0];
    node_l = slist_l->header->nexts[0];
    node_lfree = getpointer(slist_lfree->header->nexts[0]);

    while (node_seq->nexts[0] && node_l->nexts[0] && getpointer(node_lfree->nexts[0])) {
        if (node_seq->key != node_l->key || node_l->key != node_lfree->key || node_seq->key != node_lfree->key) {
            fprintf(stdout, "Comparison Failed: %d, %d and %d are not all the same.\n",
                    node_seq->key, node_l->key, node_lfree->key);
            return false;
        }
        node_seq = node_seq->nexts[0];
        node_l = node_l->nexts[0];
        node_lfree = getpointer(node_lfree->nexts[0]);
    }

    if (node_seq->nexts[0] || node_l->nexts[0] || getpointer(node_lfree->nexts[0])) {
        fprintf(stdout, "Comparison Failed: Lists are not the same length\n");
        return false;
    }

    fprintf(stdout, "Comparison Succeeded\n");
    return true;
}

