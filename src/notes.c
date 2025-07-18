#include "../include/notes.h"
#include <dirent.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#define TEXT_BUFFER_SIZE 4096
int notes_load_all(const char * dir_name){
    DIR*dirp;
    dirp=opendir(dir_name);
    if(dirp==NULL){
        return -1;
    }
    struct dirent *dir;
    int fd_dirp=dirfd(dirp);
    char buffer[TEXT_BUFFER_SIZE];
    int fd;
 
    while((dir=readdir(dirp))!=NULL){
        if(*dir->d_name!='.'){
        printf("%s\n",dir->d_name);    
        if((fd=openat(fd_dirp,dir->d_name,O_RDONLY))==-1){
            return -1;
        } 
        if(read(fd,buffer,TEXT_BUFFER_SIZE)==-1){
            return -1;
        } 
        buffer[TEXT_BUFFER_SIZE]=0;
        printf("%s\n",buffer);
    }
    }
    closedir(dirp);
    return 0;
}

int notes_save(const char *filename, const char *text,const char*dir_name){
    DIR*dirp;
    dirp=opendir(dir_name);
    if(dirp==NULL){
        return -1;
    }
    int fd;
    if((fd=openat(dirfd(dirp),filename,O_CREAT | O_WRONLY,S_IRWXU))==-1){
            return -1;
    } 
    if(write(fd,text,strlen(text))==-1){
            return -1;
    } 
    closedir(dirp);
    return 0;
}
// Load a single note
char *notes_load(const char *filename,const char *dir_name){
    DIR*dirp;
    if(NULL==(dirp=opendir(dir_name))){
        return NULL;
    }
    int fd;
    if((fd=openat(dirfd(dirp),filename,O_RDONLY))==-1){
        close(fd);
        closedir(dirp);
        return NULL;
    }
    static char buffer[TEXT_BUFFER_SIZE];
    int n;
    if((n=read(fd,buffer,TEXT_BUFFER_SIZE-1))==-1){
        close(fd);
        closedir(dirp);
        return NULL;
    }
    buffer[n]=0;
    close(fd);
    closedir(dirp);
    return buffer;
}

// List all note filenames
int notes_list(char ***filenames, int *count,const char *dir_name){
    DIR*dirp;
    *count=0;
    char names[100][100];
    dirp=opendir(dir_name);
    if(dirp==NULL){
        return -1;
    }
    struct dirent *dir;
    while((dir=readdir(dirp))!=NULL){
        if(*dir->d_name!='.'){
            strcpy(names[*count],dir->d_name);
            printf("%s |",names[*count]);

            *count+=1;
        }
    }
    
  
    
    (*filenames)=malloc(sizeof(char*)**count);
    
    for(int i=0;i<*count; i++){
        (*filenames)[i]=malloc(sizeof(char)*strlen(names[i]));
        strcpy((*filenames)[i],names[i]);
        printf("%s || ",(*filenames)[i]);
    }
    closedir(dirp);
    return 0;
}    
/*
// Delete a note
int notes_delete(const char *filename);                                                                         */
