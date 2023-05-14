#ifndef SKIP_LIST_SEQ
#define SKIP_LIST_SEQ
#include "skip_list_seq.h"
#endif



int main(){
    Skip_list_seq slist;
    init_skip_list_seq(&slist, 10);
    
    int numbers[] = {1, 2, 3, 4, 5};
    for(int i = 0; i < 5; i++){
        add_skip_list_seq(&slist, numbers[i], numbers[i]);
    }
    print_skip_list_seq(&slist);
    remove_skip_list_seq(&slist, 3);
    print_skip_list_seq(&slist);
    fprintf(stdout, "4 is contained: %s\n", contains_skip_list_seq(&slist, 4) ? "true" : "false");
    fprintf(stdout, "3 is contained: %s\n", contains_skip_list_seq(&slist, 3) ? "true" : "false");
    free_skip_list_seq(&slist);
    return 0;
}
