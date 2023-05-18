/////////////////////////////////////////////////////////////////////////////////////
/* SKIP LIST LOCK */
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

typedef struct Node_l{
    struct Node_l** nexts;
    struct Node_l** prevs;
    omp_nest_lock_t lock;
    int key;
    int value;
    unsigned int level;
    bool marked;
    bool fullylinked;
}Node_l;


typedef struct Skip_list_l{
    Node_l* header;
    Node_l* tail;
    unsigned int max_level;
    unsigned int* random_seeds;
}Skip_list_l;


typedef struct Window_l{
    Node_l* pred;
    Node_l* curr;
}Window_l;


/*
    Initializes skip list slist with tail and header,
    where both have an array of lists next and prev 
    that are max_level long and point to each other    
*/
bool init_skip_list_l(Skip_list_l* slist, int max_level);

/* 
    Initializes node with key, value and level and allocates memory for
    the node itselfs and the prevs and nexts arrays
*/
bool init_node_l(Node_l** node, int key, int value, unsigned int level);

/* 
    must be called before using function random_level_generator,
    sets the seeds for all threads, is called in init_skip_list_l
*/
bool init_random_l(Skip_list_l* slist);

/* 
    returns random level count, 
    probability: level 0 = 100%, level 1 = 50%, level 2 = 25% ...
*/
unsigned int random_level_generator_l(Skip_list_l* slist);

/*
    initializes prev and next array by using malloc and making an array that
    is level size big with Node_l* entries
*/
bool init_prevs_nexts_l(Node_l*** prevs, Node_l*** nexts, unsigned int level);

/*
    Unlocks all the locks from prevs[0] upto prevs[level]
*/
void unlock_prevs_l(Node_l** prevs, unsigned int level);

// Melvin
bool add_skip_list_l(Skip_list_l* slist, int key, int value);

// Christopher
bool remove_skip_list_l(Skip_list_l* slist, int key);

// Thomas
int find_skip_list_l(Skip_list_l* slist, int key, Node_l** prevs, Node_l** nexts);

// Thomas
bool validate_skip_list_l(Window_l w, int l);

/*
    returns true if the key is in the skip list slist, otherwise false
*/
bool contains_skip_list_l(Skip_list_l* slist, int key);


/*
    frees up memory for node
*/
void free_node_l(Node_l* node);

/*
    frees up memory for skip list
*/
void free_skip_list_l(Skip_list_l* slist);


/*
    frees up memory for window
*/
void free_window_l(Window_l* w);

/*
    prints the skip list slist in the format (key, value)
*/
void print_skip_list_l(Skip_list_l* slist);

/*
    compares keys from slist_seq to slist_l, if not same error message
    is printed out otherwise success message is printed out
*/
void compare_results_l(Skip_list_seq* slist_seq, Skip_list_l* slist_l);