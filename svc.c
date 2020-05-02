#include "svc.h"
#include <stdio.h>



char *strrev(char *str)
{
      char *p1, *p2;

      if (! str || ! *str)
            return str;
      for (p1 = str, p2 = str + strlen(str) - 1; p2 > p1; ++p1, --p2)
      {
            *p1 ^= *p2;
            *p2 ^= *p1;
            *p1 ^= *p2;
      }
      return str;
}

int hash_file(void *helper, char *file_path) {
    // TODO: Implement
    int i;
    FILE *fp;
    if(file_path==NULL) return -1;
    fp = fopen(file_path,"r");
    int hash = 0;
    if(fp==NULL) return -2;
    for(i = 0; i< strlen(file_path);i++){
        hash = (hash+file_path[i])%1000;
        //printf("%d,",file_path[i]);
    }
    int ret = 0;
    while((ret = fgetc(fp))!=-1){
        hash = (hash+ret)%2000000000;
        //printf("%d,",ret);
    }
    return hash;
}
void cleanup(void *helper) {
    free(helper);
}
int svc_add(void *helper, char *file_name) {
    if(file_name == NULL) return -1;
    helperx *helperc = (helperx*)helper;
    tracked_files* iter = helperc->tracking;
    if(iter==NULL){
        //printf("%s in 1",file_name);
        helperc->tracking = (tracked_files*)malloc(sizeof(tracked_files));
        strcpy(helperc->tracking->filename,file_name);
        helperc->tracking->hash = hash_file(helper,file_name);
        return hash_file(helper,file_name);
    }
    while(iter->next!=NULL){
        //printf("%s in 2 ",iter->filename);
        if(strcmp(iter->filename,file_name)==0)
            return -2;
        iter = iter->next;
    }
    if(strcmp(iter->filename,file_name)==0) return -2;
    //tracked_files* req = iter->next;
    iter->next = (tracked_files*)malloc(sizeof(tracked_files));
    strcpy(iter->next->filename,file_name);
    iter->next->hash = hash_file(helper,file_name);
    iter->next->prev = iter;
    return hash_file(helper,file_name);
}
void *svc_init(void) {
    // TODO: Implement
    helperx *helper = (helperx*)malloc(sizeof(helperx));
    helper->branches = (branch*)malloc(sizeof(branch));
    strcpy(helper->branches->branch_name,"master");
    helper->head = helper->branches;
    helper->branches->down = NULL;
    helper->branches->up = NULL;
    return helper;
}
int svc_rm(void *helper, char *file_name) {
    helperx *helperc = (helperx*)helper;
    if(file_name ==NULL) return -1;
    tracked_files* iter  = helperc->tracking;
    if(iter == NULL) return -2;
    //printf("%s\n",file_name);
    if(strcmp(iter->filename,file_name)==0){
        //printf("%s in 2 : 1\n",iter->filename);
        int hash = iter->hash;
        helperc->tracking = iter->next;
        return hash;
    }
    while(iter->next!=NULL){
        //printf("stuch : %s ,%s ",iter->filename ,file_name);
        //printf(" %d \n",iter->hash);
        if(strcmp(iter->filename,file_name)==0){
            //printf("%s in 2 : 2\n",iter->filename);
            tracked_files* temp = iter->prev;
            //printf("%s",iter->next->filename);
            iter->prev->next = iter->next;
            iter->next->prev = temp;
            return iter->hash;
        }
        iter = iter->next;
    }
    if(strcmp(iter->filename,file_name)==0){
        //printf("%s in 2 : 3\n",iter->filename);
        int x = iter->hash;
        iter = iter->prev;
        iter->next = NULL;
        return x;     
    }
    return -2;
}
char *svc_commit(void *helper, char *message) {
    helperx *helperc = (helperx*)helper;
    if(message==NULL) return NULL;
    if(helperc->branches->latest_commit==NULL){
        if(helperc->tracking==NULL) return NULL;
        //printf("here1");
        helperc->branches->latest_commit = (commit*)malloc(sizeof(commit));
        commit* current_commit = helperc->branches->latest_commit;
        tracked_files* iter = helperc->tracking;
        change* current_changes = current_commit->changes;
        tracked_files* current_tracking =  current_commit->files;
        current_commit->changes = NULL;
        strcpy(current_commit->branch_name,helperc->branches->branch_name);
        while(iter!=NULL){
            //printf("here2");
            //printf("\nFile name : %s ",iter->filename);
            
            if(current_tracking==NULL){
                //printf("here3");
                current_commit->files = (tracked_files*)malloc(sizeof(tracked_files));
                strcpy(current_commit->files->filename,iter->filename);
                current_commit->files->hash = iter->hash;
                current_tracking =  current_commit->files;
                //printf("here3");
            }    
            else{
                //printf("here4");
                current_tracking->next = (tracked_files*)malloc(sizeof(tracked_files));
                strcpy(current_tracking->next->filename,iter->filename);
                current_tracking->next->hash = iter->hash;
                current_tracking->next->prev = current_tracking;
                current_tracking = current_tracking->next;
                //printf("here4");
            }
            if(hash_file(helper,iter->filename)==-2){
                //printf("%s",iter->filename);
                iter = iter->next;
                continue;    
            }
            if(current_commit->changes == NULL){
                //printf("here5");
                helperc->branches->latest_commit->changes = (change*)malloc(sizeof(change));
                strcpy(helperc->branches->latest_commit->changes->filename,iter->filename);
                helperc->branches->latest_commit->changes->curr_hash_value = hash_file(helper,iter->filename);
                current_changes = helperc->branches->latest_commit->changes;
                helperc->branches->latest_commit->changes->add_del_mod = 'a';
                
                //printf("here5");
            }
            else{
                //printf("here6");
                current_changes->next = (change*)malloc(sizeof(change));
                strcpy(current_changes->next->filename,iter->filename);
                current_changes->next->curr_hash_value = hash_file(helper,iter->filename);
                current_changes->next->add_del_mod = 'a';
                current_changes->next->prev = current_changes;
                current_changes = current_changes->next;
                //printf("here6");
            }
            //printf("here7");
            iter = iter->next;
        }
        //printf("File name : %c ",current_changes->add_del_mod);
        //printf("%d",helperc->branches->latest_commit->commit_id);
        //printf( " commit id :%d ",get_commit_id(current_changes,message));
        if(current_commit->changes == NULL){helperc->branches->latest_commit = NULL;  return NULL;}
        current_commit->commit_message = message;
        current_commit->commit_id = convert_to_hex(get_commit_id(current_commit->changes,message));
        strcpy(current_commit->branch_name,helperc->head->branch_name);
        //printf("Commit ID : %x\n",current_commit->commit_id);
        //printf("\nID in HEX: %s\n",convert_to_hex(current_commit->commit_id));
        return (current_commit->commit_id);
    }
    else{
        //printf("\nCOMMIT AFTER INIT]\n");
        if(helperc->tracking==NULL) return NULL;
        commit* prev_commit = helperc->head->latest_commit;
        prev_commit->next = (commit*)malloc(sizeof(commit));
        commit* current_commit = prev_commit->next;
        
        tracked_files* prev_tracking = prev_commit->files;
        change* current_changes = current_commit->changes;
        current_changes = NULL;
        tracked_files* tracking = helperc->tracking;
        tracked_files* current_tracking = NULL;
        char flag = 'a';
        while(tracking!=NULL){
            flag = 'a';
            //printf("\n%s \n",tracking->filename);
            while(prev_tracking!=NULL){
                //printf("comparing with : %s \n",prev_tracking->filename);
                if(strcmp(prev_tracking->filename,tracking->filename)==0){
                    flag = 'p';
                    //printf("FOUND : %s : %s \n",tracking->filename,prev_tracking->filename);
                    if(prev_tracking->hash == hash_file(helper,tracking->filename)){
                        //printf("Same Hash \n");
                        if(current_commit->files==NULL){
                            current_commit->files = (tracked_files*)malloc(sizeof(tracked_files));
                            strcpy(current_commit->files->filename , tracking->filename);
                            current_commit->files->hash = hash_file(helper,tracking->filename);
                            current_tracking = current_commit->files;
                        }
                        else{
                            current_tracking->next = (tracked_files*)malloc(sizeof(tracked_files));
                            strcpy(current_tracking->next->filename , tracking->filename);
                            current_tracking->next->hash = hash_file(helper,tracking->filename);
                            current_tracking->next->prev = current_tracking;
                            current_tracking = current_tracking->next;
                        }
                    }
                    else{
                        if(hash_file(helper,tracking->filename)==-2){
                            //printf("\n FILE REMOVED \n");
                            if(current_commit->files==NULL){
                                current_commit->files = (tracked_files*)malloc(sizeof(tracked_files));
                                strcpy(current_commit->files->filename , tracking->filename);
                                current_commit->files->hash = hash_file(helper,tracking->filename);
                                current_tracking = current_commit->files;
                            }
                            else{
                                current_tracking->next = (tracked_files*)malloc(sizeof(tracked_files));
                                strcpy(current_tracking->next->filename , tracking->filename);
                                current_tracking->next->hash = hash_file(helper,tracking->filename);
                                current_tracking->next->prev = current_tracking;
                                current_tracking = current_tracking->next;
                            }
                            if(current_commit->changes==NULL){
                                current_commit->changes = (change*)malloc(sizeof(change));
                                current_commit->changes->add_del_mod = 'd';
                                strcpy(current_commit->changes->filename,tracking->filename);
                                current_commit->changes->curr_hash_value = hash_file(helper,tracking->filename);
                                current_changes = current_commit->changes;
                            }
                            else{
                                current_changes->next = (change*)malloc(sizeof(change));
                                current_changes->next->add_del_mod = 'd';
                                strcpy(current_changes->next->filename,tracking->filename);
                                current_changes->next->curr_hash_value = hash_file(helper,tracking->filename);
                                current_changes->next->prev = current_changes;
                                current_changes = current_changes->next;
                            }
                        }
                        if(prev_tracking->hash == -2){
                            //printf("\n FILE ADDED TRACKING IN PREV \n");
                            if(current_commit->files==NULL){
                                current_commit->files = (tracked_files*)malloc(sizeof(tracked_files));
                                strcpy(current_commit->files->filename , tracking->filename);
                                current_commit->files->hash = hash_file(helper,tracking->filename);
                                current_tracking = current_commit->files;
                            }
                            else{
                                current_tracking->next = (tracked_files*)malloc(sizeof(tracked_files));
                                strcpy(current_tracking->next->filename , tracking->filename);
                                current_tracking->next->hash = hash_file(helper,tracking->filename);
                                current_tracking->next->prev = current_tracking;
                                current_tracking = current_tracking->next;
                            }
                            if(current_commit->changes==NULL){
                                current_commit->changes = (change*)malloc(sizeof(change));
                                current_commit->changes->add_del_mod = 'a';
                                strcpy(current_commit->changes->filename,tracking->filename);
                                current_commit->changes->curr_hash_value = hash_file(helper,tracking->filename);
                                current_changes = current_commit->changes;
                            }
                            else{
                                current_changes->next = (change*)malloc(sizeof(change));
                                current_changes->next->add_del_mod = 'a';
                                strcpy(current_changes->next->filename,tracking->filename);
                                current_changes->next->curr_hash_value = hash_file(helper,tracking->filename);
                                current_changes->next->prev = current_changes;
                                current_changes = current_changes->next;
                            }
                        }
                        else{
                            //printf("\n FILE MODIFIED \n");
                            if(current_commit->files==NULL){
                                current_commit->files = (tracked_files*)malloc(sizeof(tracked_files));
                                strcpy(current_commit->files->filename , tracking->filename);
                                current_commit->files->hash = hash_file(helper,tracking->filename);
                                current_tracking = current_commit->files;
                            }
                            else{
                                current_tracking->next = (tracked_files*)malloc(sizeof(tracked_files));
                                strcpy(current_tracking->next->filename , tracking->filename);
                                current_tracking->next->hash = hash_file(helper,tracking->filename);
                                current_tracking->next->prev = current_tracking;
                                current_tracking = current_tracking->next;
                            }
                            if(current_commit->changes==NULL){
                                current_commit->changes = (change*)malloc(sizeof(change));
                                current_commit->changes->add_del_mod = 'm';
                                strcpy(current_commit->changes->filename,tracking->filename);
                                current_commit->changes->curr_hash_value = hash_file(helper,tracking->filename);
                                current_commit->changes->prev_hash_value = prev_tracking->hash;
                                current_changes = current_commit->changes;
                            }
                            else{
                                current_changes->next = (change*)malloc(sizeof(change));
                                current_changes->next->add_del_mod = 'm';
                                strcpy(current_changes->next->filename,tracking->filename);
                                current_changes->next->curr_hash_value = hash_file(helper,tracking->filename);
                                current_changes->next->prev_hash_value = prev_tracking->hash;
                                current_changes->next->prev = current_changes;
                                current_changes = current_changes->next;
                            }
                        }
                    }
                }
                
                prev_tracking = prev_tracking->next;
            }
            if(flag == 'a'){
                //printf("\n FILE NOT TRACKING IN PREV \n");
                if(current_commit->files==NULL){
                    current_commit->files = (tracked_files*)malloc(sizeof(tracked_files));
                    strcpy(current_commit->files->filename , tracking->filename);
                    current_commit->files->hash = hash_file(helper,tracking->filename);
                    current_tracking = current_commit->files;
                }
                else{
                    current_tracking->next = (tracked_files*)malloc(sizeof(tracked_files));
                    strcpy(current_tracking->next->filename , tracking->filename);
                    current_tracking->next->hash = hash_file(helper,tracking->filename);
                    current_tracking->next->prev = current_tracking;
                    current_tracking = current_tracking->next;
                }
                if(current_commit->changes==NULL){
                    current_commit->changes = (change*)malloc(sizeof(change));
                    current_commit->changes->add_del_mod = 'a';
                    strcpy(current_commit->changes->filename,tracking->filename);
                    current_commit->changes->curr_hash_value = hash_file(helper,tracking->filename);
                    current_changes = current_commit->changes;
                }
                else{
                    current_changes->next = (change*)malloc(sizeof(change));
                    current_changes->next->add_del_mod = 'a';
                    strcpy(current_changes->next->filename,tracking->filename);
                    current_changes->next->curr_hash_value = hash_file(helper,tracking->filename);
                    current_changes->next->prev = current_changes;
                    current_changes = current_changes->next;
                }
            }
            prev_tracking = prev_commit->files;
            tracking = tracking->next;
        }
        if(current_commit->changes == NULL){current_commit->prev = NULL; prev_commit->next = NULL; return NULL;}
        current_commit->prev = prev_commit;
        helperc->head->latest_commit = current_commit;
        current_commit->commit_message = message;
        strcpy(current_commit->branch_name,helperc->head->branch_name);
        current_commit->commit_id = convert_to_hex(get_commit_id(current_commit->changes,message));
        return (current_commit->commit_id);
    }
    return NULL;
}
int get_commit_id(change *changes,char* message){
    //printf("here8");
            
    int id = 0;
    char * t = message;
    while(*t!='\0'){
        id = (id+*t)%1000;
        //printf("%c",*t);
        t++;
        
    }
    //printf("here9");
            
    change* iter = changes;
    while(iter!=NULL){
        //printf("\nmod value : %c\n",iter->add_del_mod);
        if(iter->add_del_mod == 'a'){
            id = id+376591;
        }
        if(iter->add_del_mod == 'd'){
            id = id+85973;
        }
        if(iter->add_del_mod == 'm'){
            id = id+9573681;
        }
        //printf("\nid in iterator : %d",id);
        char* fname = iter->filename;
        while(*fname!='\0'){
            //printf("%c",*fname);
            id = (id*(*fname%37))%15485863 + 1;
            fname++;
        }
        iter = iter->next;
    }
    //printf("\n final id : %d",id);
    return id;
}
char* convert_to_hex(int n){
    //printf("%d",n);
    char* hexaDeciNum = malloc(10); 
    //printf("alpha");
    // counter for hexadecimal number array 
    int i = 0; 
    while(n!=0) 
    {    
        // temporary variable to store remainder 
        int temp  = 0; 
          
        // storing remainder in temp variable. 
        temp = n % 16; 
          
        // check if temp < 10 
        if(temp < 10) 
        { 
            hexaDeciNum[i] = temp + 48; 
            i++; 
        } 
        else
        { 
            hexaDeciNum[i] = temp + 55; 
            i++; 
        } 
          
        n = n/16; 
    } 
      //printf("her\n \n");
    // printing hexadecimal number array in reverse order 
    //for(int j=i-1; j>=0; j--) 
    //    printf("%c",hexaDeciNum[j]);
    hexaDeciNum[i] = '\0'; 
    return strrev(hexaDeciNum);
}
void print_commit(void *helper, char *commit_id){
    commit* iter = (commit*)get_commit(helper,commit_id);
    if(iter==NULL) {printf("Invalid id"); return;}
    change* changes = iter->changes;
    tracked_files* tracks = iter->files;
    printf("commit id : %s [%s] : %s \n",iter->commit_id,iter->branch_name,iter->commit_message);
    while(changes!=NULL){
        if(changes->add_del_mod == 'a'){
            printf("+ %s\n",changes->filename);
        }
        if(changes->add_del_mod == 'd'){
            printf("- %s\n",changes->filename);
        }
        if(changes->add_del_mod == 'm'){
            printf("/ %s [%d --> %d]\n",changes->filename,changes->prev_hash_value,changes->curr_hash_value);
        }    
        changes = changes->next;    
    }
    int count = 0;
    while(tracks!=NULL){count++;tracks = tracks->next;}
    tracks = iter->files;
    printf("Tracked files (%d) : \n",count);
    while(tracks!=NULL){
        printf("[%10d] %s\n",tracks->hash,tracks->filename);
        tracks = tracks->next;
    }

}
void *get_commit(void *helper, char *commit_id){
    if(commit_id==NULL) return NULL;
    helperx* helperc = (helperx*)helper;
    branch* b_iter = helperc->branches;
    while(b_iter!=NULL){
        commit* c_iter = (commit*)b_iter->latest_commit;
        while(c_iter!=NULL){
            if(strcmp(commit_id,c_iter->commit_id)==0){
                printf("BRANCH NAME OF COMMIT : %s : %s\n",c_iter->commit_id,c_iter->branch_name);
                return c_iter;
            }
            c_iter = c_iter->prev;
        }
        b_iter = b_iter->down;
    }
    return NULL;
}
int svc_branch(void *helper, char *branch_name){
    helperx* helperc = (helperx*)helper;
    //printf("WE RE HERE\n");
    if(branch_name == NULL) return -1;
    char* iter  = branch_name;
    char flag = 'v';
    while(*iter!='\0'){
        //printf("%c",*iter);
        if(*iter>=47&&*iter<=57) {iter++;continue;}
        if(*iter>=65&&*iter<=90) {iter++;continue;}
        if(*iter>=97&&*iter<=122) {iter++;continue;}
        if(*iter == '_'||*iter == '-') {iter++;continue;}
        flag = 'i';
        break;
    }
    if(flag == 'i'){
        //printf("Invalid NAME\n");
        return -1;
    }
    //printf("NOT INVALID NAME");    
    
    branch* br_iter = helperc->branches;
    while(br_iter!=NULL){
        //printf("Comparing Branch names %s : %s\n",branch_name,br_iter->branch_name);
        if(strcmp(br_iter->branch_name,branch_name)==0)
            return -2;
        br_iter = br_iter->down;
    }
    //printf("PASSED SAME NAME\n");
    flag = 'a';
    tracked_files* tracking = helperc->tracking;
    commit* prev_commit = helperc->head->latest_commit;
    if(prev_commit == NULL){
        while (tracking!=NULL)
        {
            if(hash_file(helper,tracking->filename)!=-2){
                flag = 'c';
                break;
            }
            tracking = tracking->next;
        }
        
    }
    else{    
        tracked_files* prev_tracking = prev_commit->files;
        flag = 'a';
        while(tracking!=NULL){
            flag = 'a';
            //printf("File : %s\n",tracking->filename);
            while(prev_tracking!=NULL){
                //printf("comparing with File : %s\n",prev_tracking->filename);
                if(strcmp(tracking->filename,prev_tracking->filename)==0){
                    flag = 'f';
                    //printf("Found File : %s\n",prev_tracking->filename);
                    if(prev_tracking->hash != hash_file(helper,tracking->filename)){
                        flag = 'c';
                        //printf("There are changes : %s\n",prev_tracking->filename);
                        break;
                    }
                }
                prev_tracking = prev_tracking->next;
            }
            if(flag == 'c') break;
            if(flag == 'a') {    if(hash_file(helper,tracking->filename)!=-2)    {flag = 'c'; break;}}
            prev_tracking = prev_commit->files;
            tracking = tracking->next;
        }
    }
    if(flag == 'c'){
        //printf("There are changes");
        return -3;}
    br_iter = helperc->branches;
    while(br_iter->down!=NULL){br_iter = br_iter->down;}
    br_iter->down = (branch*)malloc(sizeof(branch));
    br_iter->down->up = br_iter;
    strcpy(br_iter->down->branch_name,branch_name);
    br_iter->down->latest_commit = helperc->head->latest_commit;
    br_iter->down->down = NULL;
    return 0;
}
int svc_checkout(void *helper, char *branch_name){
    helperx* helperc = (helperx*)helper;
    if(branch_name == NULL) return -1;
    branch* br_iter = (branch*)helperc->branches;
    char flag = 'i';
    while(br_iter!=NULL){
        if(strcmp(br_iter->branch_name,branch_name)==0){
            flag = 'v';
            break;
        }
        br_iter = br_iter->down;
    }
    if(flag == 'i') return -1;
    tracked_files* tracking = helperc->tracking;
    commit* prev_commit = helperc->head->latest_commit;
    tracked_files* prev_tracking = prev_commit->files;
    flag = 'a';
    while(tracking!=NULL){
        flag = 'a';
        while(prev_tracking!=NULL){
            if(strcmp(tracking->filename,prev_tracking->filename)==0){
                flag = 'f';
                if(prev_tracking->hash != hash_file(helper,tracking->filename)){
                    flag = 'c';
                    break;
                }
            }
            prev_tracking = prev_tracking->next;
        }
        if(flag == 'c') break;
        if(flag == 'a') {    if(hash_file(helper,tracking->filename)!=-2)    {flag = 'c'; break;}}
        prev_tracking = prev_commit->files;
        tracking = tracking->next;
    }
    if(flag == 'c'){    return -2;}
    helperc->head = br_iter;
    return 0;
}
char **list_branches(void *helper, int *n_branches){
    //printf("LIST BRANCHES\n");
    helperx* helperc  = (helperx*)helper;
    if(n_branches == NULL){return NULL;}
    branch* br_iter = helperc->branches;
    int n = 0;
    while(br_iter!=NULL){
        //printf("IN COUNTER : %s\n",br_iter->branch_name);
        n++;
        br_iter = br_iter->down;
    }
    
    char** iter = malloc(n * sizeof(char*));
    br_iter = helperc->branches;
    
    //printf("NOT NULL\n");
    int count = 0;
    while(br_iter!=NULL){
        //printf("\nBranch NAME: %s",br_iter->branch_name);
        //char* result = strdup(br_iter->branch_name);
        iter[count] = (br_iter->branch_name);
        //printf("\nBranch NAME in LIST: %s",iter[count]);
        count++;
        br_iter = br_iter->down;
        
    }
    *n_branches = count;
    
    return iter;
}
char **get_prev_commits(void *helper, void *commitx, int *n_prev){
    if(n_prev == NULL) return NULL;
    //printf("\nGET prev commites\n");
    commit* current_commit = (struct commit*)commitx;
    commit* iter = current_commit;
    //printf("current commit: %s\n",iter->commit_id);
    printf("current commit: %s\n",iter->branch_name);
    
    int n = 0;
    iter = iter->prev;
    while(iter!=NULL && (strcmp(current_commit->branch_name,iter->branch_name)==0)){
        n++;
        iter = iter->prev;
    }
    if(n == 0) return NULL;
    iter = current_commit ->prev;
    char** ls_iter = malloc(n * sizeof(char*));
    printf("current commit: %s\n",current_commit->branch_name);
    int count = 0;
    while(iter!=NULL && (strcmp(current_commit->branch_name,iter->branch_name)==0)){
        //printf("%s",iter->commit_id);
        ls_iter[count] =    (iter->commit_id);
        //printf("%s",iter->commit_id);
        printf("current lis: %s\n",ls_iter[count]);
        count++;
        iter = iter->prev;
    }
    //printf(" : OUT : ");
    *n_prev = n;
    
    return ls_iter;
}
int svc_reset(void *helper, char *commit_id){
    if(commit_id == NULL) return -1;
    commit* to_commit = get_commit(helper,commit_id);
    if(to_commit == NULL) return -2;
    helperx* helperc = (helperx*)helper;
    helperc->head->latest_commit = to_commit;
    strcpy(helperc->head->branch_name ,to_commit->branch_name);
    tracked_files* iter = to_commit->files;
    tracked_files* h_iter = helperc->tracking;
    helperc->tracking = NULL;
    while(iter!=NULL){
        if(helperc->tracking == NULL){
            helperc->tracking = (tracked_files*)malloc(sizeof(tracked_files));
            strcpy(helperc->tracking->filename,iter->filename);
            helperc->tracking->hash = iter->hash;
            h_iter = helperc->tracking;
        }
        else{
            h_iter->next = (tracked_files*)malloc(sizeof(tracked_files));
            strcpy(h_iter->next->filename,iter->filename);
            h_iter->next->hash = iter->hash;
            h_iter->next->prev = h_iter;
            h_iter = h_iter->next;
        }
        iter = iter->next;
    }
    return 0;
}
char *svc_merge(void *helper, char *branch_name, resolution *resolutions, int n_resolutions){
    if(branch_name==NULL){
        printf("Invalid name\n");
        return NULL;
    }
    helperx* helperc = (helperx*)helper;
    branch* iter = helperc->branches;
    if(strcmp(branch_name,helperc->head->branch_name)==0){
        printf("Cannot merge branch itself\n");
    }
    char f = 'n';
    while(iter!=NULL){
        if(strcmp(iter->branch_name,branch_name)==0){
            f = 'f';
            break;
        }
        iter = iter->down;
    }
    if(f == 'n'){
        printf("Branch Not found\n");
        return NULL;
    }
    tracked_files* tracking = helperc->tracking;
    commit* prev_commit = helperc->head->latest_commit;
    tracked_files* prev_tracking = prev_commit->files;
    f = 'a';
    while(tracking!=NULL){
        f = 'a';
        while(prev_tracking!=NULL){
            if(strcmp(tracking->filename,prev_tracking->filename)){
                f = 'f';
                if(prev_tracking->hash != hash_file(helper,tracking->filename)){
                    f = 'c';
                    break;
                }
            }
            prev_tracking = prev_tracking->next;
        }
        if(f == 'c') break;
        if(f == 'a') {    if(hash_file(helper,tracking->filename)!=-2)    {f = 'c'; break;}}
        prev_tracking = prev_commit->files;
        tracking = tracking->next;
    }
    if(f == 'c'){
        printf("Changes must committed\n");    
        return NULL;
    }
    printf("Merge Successful");
    commit* req_commit = (commit*)malloc(sizeof(commit));
    //tracked_files* tracks = req_commit->files;
    commit* other_commit =  iter->latest_commit;
    commit* this_commit = helperc->head->latest_commit;
    tracked_files* o_iter = other_commit->files;
    tracked_files* t_iter = this_commit->files;
    change* req_changes = req_commit->changes;
    req_commit->changes = NULL;
    f = 'n';
    int i= 0;
    while(o_iter!=NULL){
        while(t_iter!=NULL){
            if(strcmp(o_iter->filename,t_iter->filename)==0){
                if(o_iter->hash == t_iter->hash){
                    printf("H");
                }
                else{
                    for(i = 0; i < n_resolutions;i++){
                        if(strcmp(o_iter->filename,resolutions[i].file_name)){
                            if(req_commit->changes ==NULL){
                                req_commit->changes = (change*)malloc(sizeof(change));
                                req_commit->changes->add_del_mod = 'm';
                                strcpy(req_commit->changes->filename,o_iter->filename);
                                req_commit->changes->curr_hash_value = hash_file(helper,resolutions[i].resolved_file);
                                req_commit->changes->prev_hash_value = t_iter->hash;
                                req_changes = req_commit->changes;
                            }
                            else{
                                req_changes->next = (change*)malloc(sizeof(change));
                                req_changes->next->add_del_mod = 'm';
                                strcpy(req_changes->next->filename,o_iter->filename);
                                req_changes->next->curr_hash_value = hash_file(helper,resolutions[i].resolved_file);
                                req_changes->next->prev_hash_value = t_iter->hash;
                                req_changes->next->prev = req_changes;
                                req_changes = req_changes->next;
                            }
                        }
                    }
                }   
            }
        }
    }
    return NULL;
}