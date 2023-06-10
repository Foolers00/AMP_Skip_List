/////////////////////////////////////////////////////////////////////////////////////
/* SKIP LIST LOCK FREE IMPROVED */
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

#ifndef ATOMIC
#define ATOMIC
#include <stdatomic.h>
#endif

#define FRACTION (1.0/2.0)
#define COUNTERS

#ifdef COUNTERS
#define INC(_c) ((_c)++)
#else
#define INC(_c)
#endif


typedef struct Node_lfree_improved{
    struct Node_lfree_improved** nexts;
    int key;
    int value;
    int level;
}Node_lfree_improved;

typedef struct Skip_list_lfree_improved{
    Node_lfree_improved* header;
    Node_lfree_improved* tail;
    int max_level;
    unsigned int* random_seeds;

    #ifdef COUNTERS
    unsigned long long adds, rems, cons, trav, fail, rtry;
    #endif
}Skip_list_lfree_improved;


#define CAS_improved(_a,_e,_d) atomic_compare_exchange_weak(_a,_e,_d)

#define UNMARK_MASK_improved ~1
#define MARK_BIT_improved 0x0000000000001

#define getpointer_improved(_markedpointer)  ((Node_lfree_improved*)(((long)_markedpointer) & UNMARK_MASK_improved))
#define ismarked_improved(_markedpointer)    ((((long)_markedpointer) & MARK_BIT_improved) != 0x0)
#define setmark_improved(_markedpointer)     ((Node_lfree_improved*)(((long)_markedpointer) | MARK_BIT_improved))


/*
    Initializes skip list slist with tail and header,
    where both have an array of lists next and prev
    that are max_level long and point to each other
*/
bool init_skip_list_lfree_improved(Skip_list_lfree_improved* slist, int max_level, int num_of_threads);

/*
    Initializes node with key, value and level and allocates memory for
    the node itselfs and the prevs and nexts arrays
*/
bool init_node_lfree_improved(Node_lfree_improved** node, int key, int value, int level);

/*
    must be called before using function random_level_generator,
    sets the seeds for all threads, is called in init_skip_list_lfree_improved
*/
bool init_random_lfree_improved(Skip_list_lfree_improved* slist, int num_of_threads);

/*
    returns random level count,
    probability: level 0 = 100%, level 1 = 50%, level 2 = 25% ...
*/
unsigned int random_level_generator_lfree_improved(Skip_list_lfree_improved* slist);


// Melvin
bool add_skip_list_lfree_improved(Skip_list_lfree_improved* slist, int key, int value);

// Christopher
bool remove_skip_list_lfree_improved(Skip_list_lfree_improved* slist, int key);

// Thomas
int find_skip_list_lfree_improved(Skip_list_lfree_improved* slist, int key, Node_lfree_improved* preds[], Node_lfree_improved* succs[]);

/*
    returns true if the key is in the skip list slist, otherwise false
*/
bool contains_skip_list_lfree_improved(Skip_list_lfree_improved* slist, int key);

/*
    frees up memory for node
*/
void free_node_lfree_improved(Node_lfree_improved* node);

/*
    frees up memory for skip list
*/
void free_skip_list_lfree_improved(Skip_list_lfree_improved* slist);

/*
    prints the skip list slist in the format (key, value)
*/
void print_skip_list_lfree_improved(Skip_list_lfree_improved* slist);

/*
    prints the marked nodes in slist
*/
void print_marked_skip_list_lfree_improved(Skip_list_lfree_improved* slist);
