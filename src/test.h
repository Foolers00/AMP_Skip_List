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


#ifndef SKIP_LIST_LOCK_FREE_IMPROVED
#define SKIP_LIST_LOCK_FREE_IMPROVED
#include "skip_list_lockf_improved.h"
#endif


#ifndef SKIP_LIST_LOCKF_PRED
#define SKIP_LIST_LOCKF_PRED
#include "skip_list_lockf_pred.h"
#endif



//////////////////////////////// 
// Test: sequential skip list //
////////////////////////////////
void test_seq_1();

//////////////////////////////// 
// Test: lock skip list ////////
////////////////////////////////

void test_lock_1();

void test_lock_2();

////////////////////////////////
// Test: lock free skip list ///
////////////////////////////////

void test_lockfree_1();

void test_lockfree_2();

/////////////////////////////////////////
// Test: lock free skip list improved ///
////////////////////////////////////////

void test_lockfree_improved_1();

/////////////////////////////////////
// Test: lock free skip list pred ///
/////////////////////////////////////

void test_lockfree_pred_1();

////////////////////////////////
// Test: additional methods ////
////////////////////////////////

bool compare_results(Skip_list_seq* slist_seq, Skip_list_l* slist_l, Skip_list_lfree* slist_lfree);



/*
    compares keys from slist_seq to slist_l, if not same error message
    is printed out otherwise success message is printed out
*/
bool compare_results_l(Skip_list_seq* slist_seq, Skip_list_l* slist_l);


/*
    compares keys from slist_seq to slist_lfree, if not same error message
    is printed out otherwise success message is printed out
*/
bool compare_results_lfree(Skip_list_seq* slist_seq, Skip_list_lfree* slist_lfree);


bool compare_results_lfree_improved(Skip_list_seq* slist_seq, Skip_list_lfree_improved* slist_lfree_improved);

bool compare_results_lfree_pred(Skip_list_seq* slist_seq, Skip_list_lfree_pred* slist_lfree_pred);




void random_array(int* numbers, int size);