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

#ifndef ATOMIC
#define ATOMIC
#include <stdatomic.h>
#endif

#define FRACTION (1.0/2.0)


typedef struct Node_lfree{
    struct Node_lfree** nexts;
    int key;
    int value;
    unsigned int level;
}Node_lfree;

typedef struct Skip_list_lfree{
    Node_lfree* header;
    Node_lfree* tail;
    unsigned int max_level;
    unsigned int* random_seeds;
}Skip_list_lfree;

#define CAS(_a,_e,_d) atomic_compare_exchange_weak(_a,_e,_d)

#define UNMARK_MASK ~1
#define MARK_BIT 0x0000000000001

#define getpointer(_markedpointer)  ((Node_lfree*)(((long)_markedpointer) & UNMARK_MASK))
#define ismarked(_markedpointer)    ((((long)_markedpointer) & MARK_BIT) != 0x0)
#define setmark(_markedpointer)     ((Node_lfree*)(((long)_markedpointer) | MARK_BIT))


/*
    Initializes skip list slist with tail and header,
    where both have an array of lists next and prev
    that are max_level long and point to each other
*/
bool init_skip_list_lfree(Skip_list_lfree* slist, int max_level, int num_of_threads);

/*
    Initializes node with key, value and level and allocates memory for
    the node itselfs and the prevs and nexts arrays
*/
bool init_node_lfree(Node_lfree** node, int key, int value, unsigned int level);

/*
    must be called before using function random_level_generator,
    sets the seeds for all threads, is called in init_skip_list_lfree
*/
bool init_random_lfree(Skip_list_lfree* slist, int num_of_threads);

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
    prints the skip list slist in the format (key, value)
*/
void print_skip_list_lfree(Skip_list_lfree* slist);

/*
    prints the marked nodes in slist
*/
void print_marked_skip_list_lfree(Skip_list_lfree* slist);
