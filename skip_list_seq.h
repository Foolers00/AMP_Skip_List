/////////////////////////////////////////////////////////////////////////////////////
/* SKIP LIST SEQUENTIAL */
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


#define FRACTION (1.0/2.0)

typedef struct Node_seq{
    struct Node_seq** nexts;
    struct Node_seq** prevs;
    int key;
    int value;
    unsigned int level;
}Node_seq;

typedef struct Window_seq{
    Node_seq* pred;
    Node_seq* curr;
}Window_seq;


typedef struct Skip_list_seq{
    Node_seq* header;
    Node_seq* tail;
    unsigned int max_level;
}Skip_list_seq;



/* 
    Initializes skip list slist with tail and header,
    where both have an array of lists next and prev 
    that are max_level long and point to each other 
*/
bool init_skip_list_seq(Skip_list_seq* slist, unsigned int max_level);


/* 
    Initializes node with key, value and level and allocates memory for
    the node itselfs and the prevs and nexts array
*/
bool init_node_seq(Node_seq** node, int key, int value, unsigned int level);

/* 
    adds the value to the skip list slist with the key,
    if there is already a node with key it will be modified
    with value
*/
bool add_skip_list_seq(Skip_list_seq* slist, int key, int value);

/* 
    removes the node with key if it is in the skip list
    slist from all levels
*/
bool remove_skip_list_seq(Skip_list_seq* slist, int key);

/*
     if key is found in the Skip list slist then true is returned
     otherwise false
*/
bool contains_skip_list_seq(Skip_list_seq* slist, int key);
/*
    finds the node with key if it is in the list otherwise 
    returns NULL and in the process makes a proto 
    array that is max_level which can be used to insert a node
    with key if it is not found
*/
Node_seq* find_skip_list_seq(Skip_list_seq* slist, int key, Node_seq** proto);

/* 
    finds node within a specific level with key, if not found 
    it returns the node before it would insert the node with key
    a w double must be also given over to increase the search time,
*/
Node_seq* find_list_seq(Skip_list_seq* slist, unsigned int level, int key, Window_seq* w);

/* 
    must be called before using function random_level_generator,
    sets the seed, is called in init_skip_list_l
*/
void init_random_seq();

/* 
    returns random level count, 
    probability: level 0 = 100%, level 1 = 50%, level 2 = 25% ...
*/
int random_level_generator_seq(unsigned int max_level);

/*
    frees up memory for node
*/
void free_node_seq(Node_seq* node);

/*
    frees up memory for skip list
*/
void free_skip_list_seq(Skip_list_seq* slist);


/*
    frees up memory for window
*/
void free_window_seq(Window_seq* w);

/*
    prints the skip list slist in the format (key, value)
*/
void print_skip_list_seq(Skip_list_seq* slist);
