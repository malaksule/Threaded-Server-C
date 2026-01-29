//21.1.25
#include "segel.h"
#include "request.h"
#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>
#include "queue.h"
// 
// server.c: A very, very simple web server
//
// To run:
//  ./server <portnum (above 2000)>
//
// Repeatedly handles HTTP requests sent to this port number.
// Most of the work is done within routines written in request.c
//



////Globals
threads_stats thread_arr;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
Queue running_requests ;
Queue waiting_requests ;
pthread_cond_t main_thread_cond;
pthread_cond_t worker_thread_cond;
int thread_num;
Queue vip_requests ;
int vip_in;
pthread_cond_t vip_thread_cond;

void getargs(int *port, int *thread_num, int *queue_size, char* schedalg ,int *max_size, int argc, char *argv[])
{
    if (argc < 5) {
        //fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(1);
    }
    *port = atoi(argv[1]);
    *thread_num = atoi(argv[2]);
    *queue_size = atoi(argv[3]);
    strcpy(schedalg, argv[4]);
    *max_size = -1;
    if(argc == 6 ){
        *max_size = atoi(argv[5]);
    }

    if((*port)<1023  || (*port)>65535 || (*thread_num)==0 || (*queue_size)==0){
        exit (1);
    }
}
int getThreadId(threads_stats arr,int size,pthread_t thread_id){
    for(int i=0 ;i<size;i++){
        if (arr[i].thread==thread_id){
            return arr[i].id;
        }
    }
    return -1;
}

void  *ThreadRoutine(void *args){
    while (1)
    {
        pthread_mutex_lock(&m);
        while (vip_in==1 || vip_requests->size>0 || waiting_requests->size==0) // make sure
        {
            //fprintf(stderr, "while.......\n");
            pthread_cond_wait(&worker_thread_cond, &m);
        }
        int id = getThreadId(thread_arr, thread_num, pthread_self());
        thread_arr[id].total_req++;
        Node curr_request =waiting_requests->head;
        int connfd = curr_request->fd;
        struct timeval dispach_time;
        struct timeval current_time;
        gettimeofday(&(current_time), NULL);
        timersub(&current_time, &(waiting_requests->head->arrival_time), &dispach_time);
        struct timeval copy_arrival_time = waiting_requests->head->arrival_time;
        Dequeue(waiting_requests);
        Enqueue(running_requests, connfd);
        pthread_mutex_unlock(&m);
        requestHandle(connfd, copy_arrival_time, dispach_time,&thread_arr[id] );
        Close(connfd);
        pthread_mutex_lock(&m);
        removeNode(running_requests, connfd);
        pthread_cond_signal(&main_thread_cond);
        pthread_mutex_unlock(&m);
    }
}
void  *ThreadRoutineVip(void *args){
    while (1)
    {
        pthread_mutex_lock(&m);
        while (vip_requests->size == 0) // make sure
        {
            pthread_cond_wait(&vip_thread_cond, &m);
        }
        int id = getThreadId(thread_arr, thread_num, pthread_self());
        thread_arr[id].total_req++;
        Node curr_request =vip_requests->head;
        int connfd = curr_request->fd;
        struct timeval dispach_time;
        struct timeval current_time;
        gettimeofday(&(current_time), NULL);
        timersub(&current_time, &(vip_requests->head->arrival_time), &dispach_time);
        struct timeval copy_arrival_time = vip_requests->head->arrival_time;
        Dequeue(vip_requests);
        vip_in=1;
        Enqueue(running_requests, connfd);
        pthread_mutex_unlock(&m);
        requestHandle(connfd, copy_arrival_time, dispach_time,&thread_arr[id] );
        Close(connfd);
        pthread_mutex_lock(&m);
        removeNode(running_requests, connfd); 
        vip_in=0;
        pthread_cond_signal(&main_thread_cond);
        pthread_mutex_unlock(&m);
    }
}
void overloadFunction(char* alg, int connfd, int* queue_size , int max_size){

     if (strcmp(alg, "block") == 0)
    {
        while (running_requests->size + waiting_requests->size+vip_requests->size >= *queue_size)
        {
            pthread_cond_wait(&main_thread_cond, &m);
        }
        Enqueue(waiting_requests, connfd);
        if (running_requests->size < thread_num)
        {
            pthread_cond_signal(&worker_thread_cond);
        }
    }
    else if (strcmp(alg, "dt") == 0){

        Close(connfd);
    }

    else if (strcmp(alg, "dh") == 0)
    {
        if (waiting_requests->size == 0)
        {
            //Close(connfd);
            ///heree
            Enqueue(waiting_requests, connfd);
            
            if (running_requests->size > 0)
            {
                pthread_cond_signal(&worker_thread_cond);
            }
        }
        else
        {
            int fd = waiting_requests->head->fd;
            Dequeue(waiting_requests);
            Close(fd);
            Enqueue(waiting_requests, connfd);
            if (running_requests->size < thread_num)
            {
                pthread_cond_signal(&worker_thread_cond);
            }
        }
    }

    else if (strcmp(alg, "bf") == 0){
        while (running_requests->size + waiting_requests->size+vip_requests->size > 0)
        {
            pthread_cond_wait(&main_thread_cond, &m);
        }
        Close(connfd);
    }
    /*else if (strcmp(alg, "dynamic") == 0){
        Close(connfd);
        if(*queue_size < max_size){
            (*queue_size)++;
        }
    }*/
     else if (strcmp(alg, "random") == 0)
     {
         if (waiting_requests->size == 0)
         {
             Close(connfd);
         }
         else
         {
             int num_to_drop = (int)(((waiting_requests->size) + 1) / 2);
             for(int i = 0; i < num_to_drop; i++) {
                 if(waiting_requests->size == 0) {
                     break;
                 }
                 int random_index = rand() % (waiting_requests->size);
                 int fd = DequeueIndex(waiting_requests, random_index);
                 Close(fd);
             }
             while(waiting_requests->size + running_requests->size+vip_requests->size >= *queue_size){//+vip_requests->size
                 pthread_cond_wait(&main_thread_cond,&m); ///make sure worker_thread_cond
             }
             Enqueue(waiting_requests, connfd);
            if(running_requests->size<thread_num){
                pthread_cond_signal(&worker_thread_cond);
            }
         }

     }

}

int main(int argc, char *argv[])
{
    int listenfd, connfd, port,queue_size,max_size,clientlen;

    char schedalg[10];
    struct sockaddr_in clientaddr;
    getargs(&port,&thread_num,&queue_size,schedalg,&max_size, argc, argv);
    
    running_requests = CreateQueue();
    waiting_requests = CreateQueue();
    vip_requests = CreateQueue();
    pthread_cond_init(&main_thread_cond,NULL);
    pthread_cond_init(&worker_thread_cond,NULL);
    pthread_cond_init(&vip_thread_cond,NULL);
    vip_in=0;
    listenfd = Open_listenfd(port);
     thread_arr= malloc(sizeof(struct Threads_stats)*(thread_num+1));
    for(int i=0; i<thread_num ;i++){
        thread_arr[i].id = i;
        thread_arr[i].total_req = 0;
        thread_arr[i].dynm_req = 0;
        thread_arr[i].stat_req = 0;
        pthread_create(&thread_arr[i].thread, NULL, ThreadRoutine,NULL);
    }

    thread_arr[thread_num].id = thread_num;
    thread_arr[thread_num].total_req = 0;
    thread_arr[thread_num].dynm_req = 0;
    thread_arr[thread_num].stat_req = 0;
    pthread_create(&thread_arr[thread_num].thread, NULL, ThreadRoutineVip,NULL);

    while (1)
    {
        clientlen = sizeof(clientaddr);
        connfd = Accept(listenfd, (SA *)&clientaddr, (socklen_t *)&clientlen);
        pthread_mutex_lock(&m);
        if (waiting_requests->size + running_requests->size+vip_requests->size >= queue_size)
        {
            if(getRequestMetaData(connfd)){
                Enqueue(vip_requests, connfd);
            }
            overloadFunction(schedalg,connfd,&queue_size,max_size);
        }

        else
        {
            if(getRequestMetaData(connfd)){
                Enqueue(vip_requests, connfd);
            }
            else {
                Enqueue(waiting_requests, connfd);
            }
            if (vip_requests->size >0)
            {
                pthread_cond_signal(&vip_thread_cond);
            }
            if (running_requests->size < queue_size)
            {
                pthread_cond_signal(&worker_thread_cond);
            }
        }
        pthread_mutex_unlock(&m);
    }

}


    


 
