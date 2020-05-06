#include <assert.h>
#include "svc.h"
#include <stdio.h>
int main(int argc, char **argv) {
    helperx *helper = svc_init();

    // TODO: write your own tests hereq
    // Hint: you can use assert(EXPRESSION) if you want
    // e.g.  assert((2 + 3) == 5);
    //printf("Commit result: %s \n",svc_commit(helper,"banana"));
    printf("Add FILE: %d \n",svc_add(helper,"hello.py")); 
    printf("MADE COMMIT : %s\n",svc_commit(helper,"Init commit"));
    //1printf("\nAdding branch : %d\n" ,svc_branch(helper,"branch1"));
    //1printf("REMOVED FILE hello.py\n",remove("hello.py"));
    printf("Add FILE: %d \n",svc_add(helper,"Hello.txt"));
    printf("Commit result: %s \n",svc_commit(helper,"Commit Two"));
    //1printf("\nAdding branch : %d\n" ,svc_branch(helper,"branch2"));
    //1printf("CHECKOUT %d\n",svc_checkout(helper,"new_branch"));
    printf("Add FILE: %d \n",svc_add(helper,"test.txt"));
    //printf("\nAdd res: %d\n",svc_add(helper,"Hello.txt"));
    printf("MADE Commit: %s \n",svc_commit(helper,"Commit Three"));
    print_commit(helper,"SomeID");
    printf("\n\n");
    print_commit(helper,"6F");
    printf("\n\n");
    print_commit(helper,"8FD");
    printf("\n\n");
    print_commit(helper,"BCFC40");
    int n;
    char** branches;
    int i = 0;    
    branches= list_branches(helper,&n);
    for(i = 0; i < n; i++){
        printf("\nBranches   : %s",(branches[i]));
    }

    printf("\n\n");
    branches = get_prev_commits(helper,get_commit(helper,"BCFC40"),&n);
    
    for(i = 0; i < n; i++){
        printf("\nPrev commits revived : %s",(branches[i]));
    }
    //printf("Add res: %d\n",svc_rm(helper,"Hello.txt"));
    //printf("Add res: %d\n",svc_rm(helper,"test.txt"));
    //printf("Add res: %d\n",svc_rm(helper,"Hello.py"));
    //printf("Add res: %d\n",svc_rm(helper,"Hello.txt"));

    //printf("\n%d",hash_file(helper,"Helo.py"));
    return 0;
}

