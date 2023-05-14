/////////////////////////////////////////////////////////////////////////////////////
/* SKIP LIST LOCK */
/////////////////////////////////////////////////////////////////////////////////////



typedef struct Node_lock{
    struct Node_lock** next;
    struct Node_lock** prev;
    int key;
    int value;
    int level;
    bool marked;
}Node_lock;


typedef struct Skip_list_lock{
    Node_lock* header;
    Node_lock* tail;
    int max_level;
}Skip_list_lock;


typedef struct Window{
    Node_lock* pred;
    Node_lock* curr;
}Window;


// Melvin
bool add_skip_list_lock(int key, int value);

// Christopher
bool remove_skip_list_lock(int key);

// Thomas
Window find_skip_list_lock(int key);

// Thomas
bool validate_skip_list_lock(Window w);

// Melvin
bool contains(int key);