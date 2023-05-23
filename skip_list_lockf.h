/////////////////////////////////////////////////////////////////////////////////////
/* SKIP LIST LOCK FREE */
/////////////////////////////////////////////////////////////////////////////////////

#ifndef STDLIB
#define STDLIB
#include <stdlib.h>
#endif

#ifndef STDIO
#define STDIO
#include <stdio.h>
#endif

#ifndef TIME
#define TIME
#include <time.h>
#endif

#ifndef BOOL
#define BOOL
#include <stdbool.h>
#endif

#ifndef LIMITS
#define LIMITS
#include <limits.h>
#endif

#ifndef STRING
#define STRING
#include <string.h>
#endif

#ifndef OMP
#define OMP
#include <omp.h>
#endif

#ifndef SKIP_LIST_SEQ
#define SKIP_LIST_SEQ
#include "skip_list_seq.h"
#endif

#define FRACTION (1.0/2.0)

struct Atomic_markable_reference;

typedef struct Node_lfree{
    struct Atomic_markable_reference** nexts;
    omp_nest_lock_t lock;
    int key;
    int value;
    unsigned int level;
    volatile bool fullylinked;
}Node_lfree;

typedef struct Skip_list_lfree{
    Node_lfree* header;
    Node_lfree* tail;
    unsigned int max_level;
    unsigned int* random_seeds;
}Skip_list_lfree;


typedef struct Window_l{
    Node_lfree* pred;
    Node_lfree* curr;
}Window_l;

typedef struct Atomic_markable_reference {
    bool marked;
    Node_lfree* ref;
} Atomic_markable_reference;


/*
    Initializes skip list slist with tail and header,
    where both have an array of lists next and prev
    that are max_level long and point to each other
*/
bool init_skip_list_lfree(Skip_list_lfree* slist, int max_level);

/*
    Initializes node with key, value and level and allocates memory for
    the node itselfs and the prevs and nexts arrays
*/
bool init_node_lfree(Node_lfree** node, int key, int value, unsigned int level);

/*
    must be called before using function random_level_generator,
    sets the seeds for all threads, is called in init_skip_list_lfree
*/
bool init_random_lfree(Skip_list_lfree* slist);

/*
    returns random level count,
    probability: level 0 = 100%, level 1 = 50%, level 2 = 25% ...
*/
unsigned int random_level_generator_lfree(Skip_list_lfree* slist);

/*
    Unlocks all the locks from prevs[0] upto prevs[level]
*/
void unlock_preds_lfree(Node_lfree* preds[], unsigned int level);

// Melvin
bool add_skip_list_lfree(Skip_list_lfree* slist, int key, int value);

// Christopher
bool remove_skip_list_lfree(Skip_list_lfree* slist, int key);

// Thomas
int find_skip_list_lfree(Skip_list_lfree* slist, int key, Node_lfree* preds[], Node_lfree* succs[]);

// Thomas
bool validate_skip_list_lfree(Window_l w, int l);

/*
    returns true if the key is in the skip list slist, otherwise false
*/
bool contains_skip_list_lfree(Skip_list_lfree* slist, int key);

/*
    frees up memory for node
*/
void free_node_lfree(Node_lfree* node);

/*
    frees up memory for skip list
*/
void free_skip_list_lfree(Skip_list_lfree* slist);

/*
    frees up memory for window
*/
void free_window_lfree(Window_l* w);

/*
    prints the skip list slist in the format (key, value)
*/
void print_skip_list_lfree(Skip_list_lfree* slist);

/*
    compares keys from slist_seq to slist_lfree, if not same error message
    is printed out otherwise success message is printed out
*/
void compare_results_lfree(Skip_list_seq* slist_seq, Skip_list_lfree* slist_lfree);