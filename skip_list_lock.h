/////////////////////////////////////////////////////////////////////////////////////
/* SKIP LIST LOCK */
/////////////////////////////////////////////////////////////////////////////////////

#ifndef OMP
#define OMP
#include <omp.h>
#endif


typedef struct Node_lock{
    struct Node_lock** next;
    struct Node_lock** prev;
    int key;
    int value;
    int level;
    omp_nest_lock_t lock;
    bool marked;
    bool fullylinked;
}Node_lock;


typedef struct Skip_list_lock{
    Node_lock* header;
    Node_lock* tail;
    int max_level;
}Skip_list_lock;


typedef struct Window_lock{
    Node_lock* pred;
    Node_lock* curr;
}Window_lock;


// Melvin
bool add_skip_list_lock(int key, int value);

// Christopher
bool remove_skip_list_lock(int key);

// Thomas
Window_lock find_skip_list_lock(int key);

// Thomas
bool validate_skip_list_lock(Window_lock w);

// Melvin
bool contains_skip_list_lock(int key);