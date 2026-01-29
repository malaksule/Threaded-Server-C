#ifndef __REQUEST_H__
#define __REQUEST_H__
#include "queue.h"



typedef struct Threads_stats{
    pthread_t thread;
    int id;
    int stat_req;
	int dynm_req;
	int total_req;
} * threads_stats; 

/*typedef struct threadInfo{
    pthread_t thread;
    int id;
    int request_count;
    int dynamic_count;
    int static_count;
} ThreadInfo;*/


// handle a request
void requestHandle(int fd, struct timeval arrival, struct timeval dispatch, threads_stats t_stats);


//  Returns True/False if realtime event
int getRequestMetaData(int fd);

#endif
