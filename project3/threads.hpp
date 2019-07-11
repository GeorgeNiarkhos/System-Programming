#include <pthread.h>
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <time.h>
#include <stdio.h>
#include <arpa/inet.h> /* inet_ntoa */
#include <sys/types.h> /* sockets */
#include <sys/socket.h> /* sockets */
#include <netinet/in.h> /* internet sockets */
#include <unistd.h> /* read, write, close */
#include <string.h>
#include <netdb.h> /* gethostbyaddr */

#ifndef CLIENT_LIST_H
#define CLIENT_LIST_H
#include "client_list.hpp"
#endif

#ifndef BUFFSIZE
#define BUFFSIZE 512
#endif


using namespace std;

#define PATHSIZE 512

class Job {
    
    public:
        unsigned long ip;
        int port;
        char * pathname;
        time_t version;

        Job(){
            this->pathname = NULL;
        }

        Job(unsigned long IP, int Port){
            this->ip = IP;
            this->port = Port;
            this->pathname = NULL;
        }

        Job(unsigned long IP, int Port, char* pathn, time_t ver) : ip(IP), port(Port), version(ver){
            if(pathn != NULL){
                this->pathname = new char[strlen(pathn)+1];
                strcpy(this->pathname, pathn);
            }
        }

        ~Job(){
            if(this->pathname != NULL){
                delete[] this->pathname;
            }
        }
        
        void execute(ClientList* list);
        void getFileListJob();
        void getFileJob();
    
};

class JobBuffer{
public:
    Job ** cbuf;
    int start;
    int end;
    int size;
    int maxSize;
    
    JobBuffer(int bufSize) : maxSize(bufSize){
        this->start = 0;
        this->end = 0;
        this->size = 0;
        this->cbuf = new Job* [bufSize];
    };
    ~JobBuffer(){
        delete[] this->cbuf;
    };

    int addJob(Job * job);
    int removeJob();
    Job * getStart();
};

// Definition of thread function
void* threadFunction();

class JobScheduler {

    public:
    // Jobs queue
    JobBuffer* buff;
    ClientList* list;
    // Thread pool
    pthread_t * threads;
    // Mutexes etc.
    pthread_mutex_t mutex;
    pthread_mutex_t jobMutex;
    pthread_cond_t nonempty;
    pthread_cond_t empty;
    pthread_cond_t nonfull;
    int jobCounter;
    int threadNum;

    // Constructor and destructor methods
    inline JobScheduler(){
        // default
        // this->threads = new pthread_t[threadNum];
        pthread_mutex_init(&this->mutex, NULL);
        pthread_mutex_init(&this->jobMutex, NULL);
        pthread_cond_init(&this->nonempty, NULL);
        pthread_cond_init(&this->empty, NULL);
        pthread_cond_init(&this->nonfull, NULL);
        this->buff = NULL;
        this->list = NULL;
        jobCounter = 0;
    }

    inline ~JobScheduler() {
        delete[] this->threads;
        delete this->buff;
    }

    // Initializes the JobScheduler with the number of open threads.
    // Returns true if everything done right false else.
    int init(int nthreads, int buffSize, ClientList* list);
    // Free all resources that the are allocated by JobScheduler
    // Returns true if everything done right false else.
    int destroy();
    // Wait for all threads to finish their job
    void barrier();

    // Job methods
    // Schedule a job for execution by threads.
    void schedule(Job* job);
    // Get a job from the queue
    Job * getJob();
    ClientList* getList();
};

extern JobScheduler scheduler;
