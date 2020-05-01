#include "svc.h"
#include <stdio.h>



void cleanup(void *helper) {
    
}

int hash_file(void *helper, char *file_path) {
    // TODO: Implement
    int i;
    FILE *fp;
    fp = fopen(file_path,"r");
    int hash = 0;
    for(i = 0; i< strlen(file_path);i++){
        hash = (hash+file_path[i])%1000;
    }
    int ret = 0;
    while((ret = fgetc(fp))!=-1){
        hash = (hash+ret)%2000000000;
    }
    return hash;
}

char *svc_commit(void *helper, char *message) {
    // TODO: Implement
    
    return NULL;
}

void *get_commit(void *helper, char *commit_id) {
    // TODO: Implement
    return NULL;
}

char **get_prev_commits(void *helper, void *commit, int *n_prev) {
    // TODO: Implement
    return NULL;
}

void print_commit(void *helper, char *commit_id) {
    // TODO: Implement
}

int svc_branch(void *helper, char *branch_name) {
    // TODO: Implement
    return 0;
}

int svc_checkout(void *helper, char *branch_name) {
    // TODO: Implement
    return 0;
}

char **list_branches(void *helper, int *n_branches) {
    // TODO: Implement
    return NULL;
}

int svc_add(void *helper, char *file_name) {
    if(file_name == NULL) return -1;
    tracked_files *iter = helper->tracking;
    while(iter!=NULL){
        if(strcmp(iter->filename,file_name)==0)
            return -2;
        iter = iter->next;
    }
    iter = (tracked_files*)malloc(sizeof(tracked_files));
    strcpy(iter->filename,file_name);
    return hash_file(helper,file_name);
}

int svc_rm(void *helper, char *file_name) {
    helperx *helperc = (helperx*)helper;
    if(file_name ==NULL) return -1;
    tracked_files* iter  = helperc->tracking;
    if(iter == NULL) return -2;
    if(strcmp(iter->filename,file_name)==0){
        int hash = iter->hash;
        helperc->tracking = iter->next;
        return hash;
    }
    while(iter->next!=NULL){
        if(strcmp(iter->filename,file_name)==0){
            tracked_files* temp = iter->prev;
            iter->prev->next = iter->next;
            iter->next->prev = temp;
            return iter->hash;
        }
    }
    if(strcmp(iter->filename,file_name)==0){
        tracked_files* temp = iter->prev;
        helperc->tracking = iter->next;
        iter->prev->next = iter->next;
        return iter->hash;     
    }
    return -2;
}

int svc_reset(void *helper, char *commit_id) {
    // TODO: Implement
    return 0;
}

char *svc_merge(void *helper, char *branch_name, struct resolution *resolutions, int n_resolutions) {
    // TODO: Implement
    return NULL;
}

