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

typedef struct Node{
    struct Node** next;
    struct Node** prev;
    int key;
    int value;
    int level;
}Node;


typedef struct Skip_list{
    Node* header;
    Node* tail;
    int curr_level;
    int max_level;
}Skip_list;



/* 
    Initializes skip list slist with tail and header,
    where both have an array of lists next and prev t
    hat are max_level long and point to each other 
*/
void init_skip_list(Skip_list* slist, int max_level);

/* 
    adds the value to the skip list slist with the key,
    if there is already a node with key it will be modified
    with value
*/
void add_skip_list(Skip_list* slist, int key, int value);

/*
    finds the node with key if it is in the list otherwise 
    returns NULL and in the process makes a proto 
    array that is max_level which can be used to insert a node
    with key if it is not found
*/
Node* find_skip_list(Skip_list* slist, int key, Node** proto);

/* 
    finds node within a specific level with key, if not found 
    it returns the node before it would insert the node with key 
*/
Node* find_list(Skip_list* slist, int level, int key);

/* 
    must be called before using function random_level_generator,
    sets the seed
*/
void init_random();

/* 
    returns random level count, 
    probability: level 0 = 100%, level 1 = 50%, level 2 = 25% ...
*/
int random_level_generator(int max_level);

/*
    frees up memory for node
*/
void free_node(Node* node);

/*
    frees up memory for skip list
*/
void free_skip_list(Skip_list* slist);

/*
    prints the skip list slist in the format (key, value)
*/
void print_skip_list(Skip_list* slist);