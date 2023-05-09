#ifndef SKIP_LIST_SEQ
#define SKIP_LIST_SEQ
#include "skip_list_seq.h"
#endif



int main(){
    Skip_list slist;
    init_skip_list(&slist, 10);
    
    int numbers[] = {1, 2, 3, 4, 5};
    for(int i = 0; i < 5; i++){
        add_skip_list(&slist, numbers[i], numbers[i]);
    }
    print_skip_list(&slist);
    remove_skip_list(&slist, 3);
    print_skip_list(&slist);
    free_skip_list(&slist);
    return 0;
}
