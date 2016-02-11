//
//  main.c
//  multithread
//
//  Created by 俆銘村 on 2015/12/17.
//  Copyright © 2015年 Villager. All rights reserved.
//

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


/* this data is shared by the thread(s) */
char **ptr;
char **result;
int *count;
int length;
pthread_mutex_t lock;
pthread_t tid[3];
FILE *file = NULL;

void *runner(void *param); /* the thread */

int main(int argc, char *argv[])
{
    
    /* the thread identifier */
    pthread_attr_t attr;
    /* set of attributes for the thread */
    if (argc < 2) {
        fprintf(stderr,
                "usage: a.out <integer value>\n" );
        return -1;
    }
    
    /*initial array; ptr for 讀取檔案暫存 , result for 單詞 , count for 單詞計數*/
    
    //ptr=(char*)malloc(10*sizeof(int));
    ptr = malloc(20 * sizeof(char*));
    for (int i = 0; i < 20; i++)
        ptr[i] = malloc((10+1) * sizeof(char));
    
    result = malloc(1000000 * sizeof(char*));
    for (int i = 0; i < 1000000; i++)
        result[i] = malloc((10+1) * sizeof(char));
    
    
    count=(int*)malloc(1000000*sizeof(int));
    
    /* get the default attributes */
    pthread_attr_init(&attr);
    
    /* create the thread */
    int i = 0;
    int err;
    
    if (pthread_mutex_init(&lock, NULL) != 0)
    {
        printf("\n mutex init failed\n");
        return 1;
    }
    
    while(i < 3)
    {
        
        err = pthread_create(&(tid[i]), &attr, &runner,argv[i+1]);
        if (err != 0)
            printf("\ncan't create thread :[%s]", strerror(err));
        i++;
        
    }
    
    /* now wait for the thread to exit */
    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);
    pthread_join(tid[2], NULL);
    pthread_mutex_destroy(&lock);
    
   
    
    // sort the count_numbers
    int  swap_count;
    for (int j=0;j<length-1;j++) {
        for(int k=0;k<length-j-1;k++){
            if(count[k]<count[k+1]){
                strcpy(ptr[1],result[k]);
                strcpy(result[k],result[k+1]);
                strcpy(result[k+1],ptr[1]);
                swap_count = count[k];
                count[k]   = count[k+1];
                count[k+1] = swap_count;
            }
        }
    }
    // sort the result with same_count_number
    int  count_number=count[0],index=0,start=0;
    for (int a=start;a<length-1;a++) {
        if(count[a]!= count_number){
            count_number=count[a];
            index=a;
            for(int b=start;b<index-1;b++){
                for(int c=0;c<index-b-1;c++){
                    if(strcmp(result[c],result[c+1])>0){
                        strcpy(ptr[1],result[c]);
                        strcpy(result[c],result[c+1]);
                        strcpy(result[c+1],ptr[1]);
                    }
                }
            }
            start=a;

        }else{
            index=length;
            for(int b=start;b<index-1;b++){
                for(int c=0;c<index-b-1;c++){
                    if(strcmp(result[c],result[c+1])>0){
                        strcpy(ptr[1],result[c]);
                        strcpy(result[c],result[c+1]);
                        strcpy(result[c+1],ptr[1]);
                    }
                }
            }
        }
    }
    
    /*print result*/
    for(int f=0;f<length;f++){
        printf("%s: %d\n",result[f],count[f]);
    }
}
/* The thread will begin control in this function */
void *runner(void *param)
{
    pthread_mutex_lock(&lock);
    
    /*Open file whose path is passed as an argument */
    file = fopen( param, "r" );
    
    /* fopen returns NULL pointer on failure */
    if ( file == NULL) {
        printf("\nCould not open file");
    }
    else {
        while(!feof(file)){
            fscanf(file, "%s", ptr[0]);
            //if the string is only a character and not a english word then skip
            if(strlen(ptr[0])==1 && (ptr[0][0]<65 || ptr[0][0]==126)){
                continue;
            }
            
            //if any characher is not a english word then shift left the string
            for(int i=0;i<strlen(ptr[0]);i++){
                if(ptr[0][i]<65 || ptr[0][i]==126){
                    if(ptr[0][i]==39){
                        if(strcmp(&ptr[0][i+1],"s")==0 && strcmp(&ptr[0][i-1],"t")==0){
                            continue;
                        }
                    }
                    for(int j=i;j<strlen(ptr[0]);j++){
                        memcpy(&ptr[0][j],&ptr[0][j+1],1);
                    }
                    i=0;
                }
            }
            //double check if any non-engilsh word in the string
            if(ptr[0][0]<65){
                for(int j=0;j<strlen(ptr[0]);j++){
                    memcpy(&ptr[0][j],&ptr[0][j+1],1);
                }
            }

            

            //if last characher is not a english word then set \0
            while(ptr[0][strlen(ptr[0])-1]<65){
                strcpy(&ptr[0][strlen(ptr[0])-1],"\0");
            }
            
            //change all word to lowercase
            for(int i=0;i<=strlen(ptr[0]);i++){
                if(ptr[0][i]>=65 && ptr[0][i]<=90)
                        ptr[0][i]=ptr[0][i]+32;
            }
            
            if(strcmp(ptr[0],"\0")==0){
                continue;
            }
            
            
            //printf("scan: %s \n",ptr[0]);
            if(length==0){
                strcpy(result[length], ptr[0]);
                count[length]=1;
                length++;
            }
            else{
                int i;
                for(i=0;i<length;i++){
                    if(strcmp(result[i],ptr[0])==0){
                        count[i]++;
                        break;
                    }else if(i==length-1){
                        strcpy(result[length], ptr[0]);
                        count[length]=1;
                        length++;
                        break;
                    }
                }
            }
        }
        //printf("\nFile (%s) opened", param);
        /* Closing file */
        fclose(file);
    }
    
    pthread_mutex_unlock(&lock);
    pthread_exit(0);
}