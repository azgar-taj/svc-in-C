#include <assert.h>
#include "svc.h"
#include <stdio.h>
int main(int argc, char **argv) {
    helperx *helper = svc_init();

    // TODO: write your own tests hereq
    // Hint: you can use assert(EXPRESSION) if you want
    // e.g.  assert((2 + 3) == 5);
    printf("Commit attar: %s \n",svc_commit(helper,"banana"));
    
    printf("Add res: %d \n",svc_add(helper,"Hello.py"));
    printf("Add res: %d \n",svc_add(helper,"Hello.py"));
    //printf("Add res: %d \n",svc_add(helper,"Hello.txt"));
    printf("Add res: %d\n",svc_add(helper,"test.txt"));
    //printf("Add res: %d\n",svc_rm(helper,"Hello.txt"));
    printf("\nAdding branch : %d\n" ,svc_branch(helper,"ehllo"));
    printf("Commit attar: %s \n",svc_commit(helper,"Init commit"));
    printf("\nAdding branch : %d\n" ,svc_branch(helper,"ehllo"));
    
    printf("Commit attar: %s \n",svc_commit(helper,"Init commit"));
    
    print_commit(helper,"E57751");
    printf("\nAdd res: %d\n",svc_add(helper,"Hello.txt"));
    printf("Commit attar: %s \n",svc_commit(helper,"Added Hello"));
    printf("Add res: %d\n",svc_rm(helper,"test.txt"));
    printf("Commit attar: %s \n",svc_commit(helper,"Removed test"));
    print_commit(helper,"8FD");
    int n;
    char** branches = list_branches(helper,&n);
    printf("\n%d",n);
    int i = 0;
    for(i = 0; i < n; i++){
        printf("\nBranch revived : %s",*(branches+i));
    }
    printf("Crossed");
    branches = get_prev_commits(helper,get_commit(helper,"8FD"),&n);
    
    i = 0;
    for(i = 0; i < n; i++){
        printf("\nBranch revived : %s",*(branches+i));
    }
    //printf("Add res: %d\n",svc_rm(helper,"Hello.txt"));
    //printf("Add res: %d\n",svc_rm(helper,"test.txt"));
    //printf("Add res: %d\n",svc_rm(helper,"Hello.py"));
    //printf("Add res: %d\n",svc_rm(helper,"Hello.txt"));

    printf("\n%d",hash_file(helper,"Helo.py"));
    return 0;
}

