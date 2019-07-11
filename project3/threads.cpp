#include "threads.hpp"

// used to signal thread termination
int work = 1;

using namespace std;

int JobBuffer::addJob(Job * job){
    if(this->size + 1 == this->maxSize){
        return -1;  //full error
    }
    //add job
    this->cbuf[this->end] = job;
    this->end = (this->end + 1) % this->maxSize;
    this->size++;
    return this->size;
}

int JobBuffer::removeJob(){
    if(this->size == 0){
        return -1;  //empty error
    }
    this->start = (this->start + 1) % this->maxSize;
    this->size--;
    return this->size;
}

Job * JobBuffer::getStart(){
    return this->cbuf[this->start];
}

void *threadFunction(void* arg) {
    // Function that every thread runs as soon
    // as it is created
    while ( work ) {
        
        Job* job = scheduler.getJob();
        job->execute(scheduler.getList());
        delete job;
        pthread_mutex_lock(&scheduler.mutex);
        scheduler.jobCounter--;
        pthread_cond_signal(&scheduler.nonfull);
        if ( scheduler.jobCounter == 0 ) {
            pthread_cond_signal(&scheduler.empty);
        }
        pthread_mutex_unlock(&scheduler.mutex);

    }
    // Exit as soon as jobs done
    pthread_exit(NULL);
}

int JobScheduler::init(int nthreads, int buffSize, ClientList* list) {
    // Create defined number of threads
    this->threads = new pthread_t[nthreads];
    this->buff = new JobBuffer(buffSize);
    this->threadNum = nthreads;
    this->list = list;
    // return their ids into the threads vector
    for(int i = 0; i < this->threadNum; i++) {
        pthread_create(&(this->threads[i]), NULL, threadFunction, NULL);
    }
    return 0;
}

int JobScheduler::destroy() {
    // Terminate all already executed threads
    work = 0;
    pthread_cond_broadcast(&this->nonempty);
    // Wait for them to exit
    for ( int i=0;i<this->threadNum;i++) {
        pthread_join(this->threads[i],NULL);
    }
    pthread_mutex_destroy(&this->mutex);
    pthread_cond_destroy(&this->nonempty);
    pthread_cond_destroy(&this->empty);
    pthread_cond_destroy(&this->nonfull);
    return 0;
}

void JobScheduler::barrier() {
    // Suspend execution until all threads complete their tasks.
    pthread_mutex_lock(&this->mutex);
    while ( this->jobCounter > 0 ) {
        pthread_cond_wait(&this->empty, &this->mutex);
    }  
    pthread_mutex_unlock(&this->mutex);

}

void JobScheduler::schedule(Job* job) {
    // Lock mutex to safely insert job
    pthread_mutex_lock(&this->mutex);
    if(this->buff->size == this->buff->maxSize){
        // Wait for empty space
        pthread_cond_wait(&this->nonfull, &this->mutex);
    }
    // Insert Job
    this->buff->addJob(job);
    if(this->buff->size < this->buff->maxSize){
        pthread_cond_signal(&this->nonfull);
    }
    pthread_cond_signal(&this->nonempty);
    this->jobCounter++;
    // Unlock mutex so that the process can continue
    pthread_mutex_unlock(&this->mutex);
}

Job * JobScheduler::getJob() {
    // Lock Mutex to safely access buff.
    pthread_mutex_lock(&this->mutex);
    // Wait fora a task to arrive.
    while ( this->buff->size <= 0 ) {

        pthread_cond_wait(&this->nonempty, &this->mutex);
        if ( work == 0 ) {
            // Exit in case thread gets termination signal
            pthread_mutex_unlock(&this->mutex);
            pthread_exit(0);
        }
        
    }
    // Get the highest priority Job.
    Job * job = this->buff->getStart();
    // Erase it from the buf.
    this->buff->removeJob(); 
    // Unlock mutex when done.
    pthread_mutex_unlock(&this->mutex);
    return job;
}

ClientList* JobScheduler::getList(){
    return this->list;
}

// Thread jobs
void Job::execute(ClientList* list){
    //check if user exists
    // Lock Mutex to safely access list.
    pthread_mutex_lock(&scheduler.mutex);
    if(list->findInfoNode(this->ip, this->port) == NULL){
        //NOT FOUND do nothing
        return;
    }
    // Unlock mutex when done.
    pthread_mutex_unlock(&scheduler.mutex);
    //check if pathname null
    if(this->pathname == NULL){
        //call getfileList
        this->getFileListJob();
        return;
    }
    else{
        //call getfile
        this->getFileJob();
        return;
    }
}

void Job::getFileListJob(){
    struct sockaddr_in client;
    struct sockaddr *clientptr=(struct sockaddr *)&client;
    int sock;
    char buf[BUFFSIZE];
    /* Create socket */
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("socket");
        return;
    }
    client.sin_family = AF_INET;
    client.sin_addr.s_addr = htonl(this->ip);
    client.sin_port = htons(this->port);
    /* Initiate connection */
    while (connect(sock, clientptr, sizeof(client)) < 0){
        perror("connect file list job");
        cout << this->ip << endl;
    }
    cout << "connected" << endl;
    //write GET_FILE_LIST
    strcpy(buf, "GET_FILE_LIST");
    if(write(sock, buf, BUFFSIZE) <= 0){
        perror("write");
        close(sock);
        return;
    }
    //read FILE_LIST
    if(read(sock, buf, BUFFSIZE) <= 0){
        perror("read");
        close(sock);
        return;
    }
    //read file list size
    int fsize;
    if(read(sock, &fsize, sizeof(int)) <= 0){
        perror("read");
        close(sock);
        return;
    }
    for(int i=0; i<fsize; i++){
        //read path
        if(read(sock, buf, BUFFSIZE) <= 0){
            perror("read");
            close(sock);
            return;
        }
        //read version
        time_t ver ;
        if(read(sock, &ver, sizeof(ver)) <= 0){
            perror("read");
            close(sock);
            return;
        }
        //add to jobs
        scheduler.schedule(new Job(this->ip, this->port, buf, ver));
    }
    close(sock);
    return;
}

void Job::getFileJob(){
    struct sockaddr_in client;
    struct sockaddr *clientptr=(struct sockaddr *)&client;
    struct hostent *rem;
    int sock;
    ofstream f;
    char buf[BUFFSIZE];
    char * buff;
    char str[BUFFSIZE];
    /* Create socket */
    if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("socket");
        return;
    }
    client.sin_family = AF_INET;
    client.sin_addr.s_addr = htonl(this->ip);
    client.sin_port = htons(this->port);
    struct hostent *host_entry;
    //Find my address
    if (gethostname(buf, sizeof(buf)) == -1){
        perror("gethostname");
        return;
    }
    if ((host_entry = gethostbyname(buf)) == NULL) {
        perror("gethostbyname");
        return;
    }
    
    /* Find client's name */
    if ((rem = gethostbyaddr((char *) &client.sin_addr.s_addr,sizeof(client.sin_addr.s_addr), client.sin_family))== NULL) {
        perror("gethostbyaddr");
        return;
    }
    sprintf(str, "%s/%s/%s", host_entry->h_name, rem->h_name, this->pathname);
    FILE *file;
    time_t ver;
    //check if file exists
    if (file = fopen(str, "r")){
        fclose(file);
        ver = this->version;
    }
    else{
        ver = 0;
    }
    /* Initiate connection */
    if (connect(sock, clientptr, sizeof(client)) < 0){
        perror("connect get file job");
        return;
    }
    //write GET_FILE
    strcpy(buf, "GET_FILE");
    if(write(sock, buf, BUFFSIZE) <= 0){
        perror("write");
        close(sock);
        return;
    }
    //write pathname
    strcpy(buf, this->pathname);
    if(this->pathname == NULL){
        cerr << "error" << endl;
    }
    if(write(sock, buf, BUFFSIZE) <= 0){
        perror("write");
        close(sock);
        return;
    }
    //write version
    if(write(sock, &ver, sizeof(ver)) <= 0){
        perror("write");
        close(sock);
        return;
    }
    //read answer
    if(read(sock, buf, BUFFSIZE) <= 0){
        perror("read");
        close(sock);
        return;
    }
    if(!strcmp(buf, "FILE_SIZE")){
        //make path dirs
        
        char * token1, *token2;
        token1 = strtok(str, "/");
        token2 = strtok(NULL, "/");
        //AT LEAST ONE DIR
        char * inpath1, * inpath2;
        //MAKE FIRST DIR
        inpath1 = new char[strlen(token1) + 1];
        sprintf(inpath1, "%s", token1);
        mkdir(inpath1, S_IRWXU | S_IRWXG | S_IROTH);
        token1 = token2;
        token2 = strtok(NULL, "/");
        while(token2 != NULL){
            //MORE DIRECTORIES                       
            inpath2 = new char[strlen(inpath1) + strlen(token1) + 2];
            sprintf(inpath2, "%s/%s", inpath1, token1);
            mkdir(inpath2, S_IRWXU | S_IRWXG | S_IROTH);
            delete[] inpath1;
            inpath1 = new char[strlen(inpath2) + 1];
            strcpy(inpath1, inpath2);
            delete[] inpath2;
            token1 = token2;
            token2 = strtok(NULL, "/");
        }
        //OPEN FILE
        inpath2 = new char[strlen(inpath1) + strlen(token1) + 2];
        sprintf(inpath2, "%s/%s", inpath1, token1);
        f.open(inpath2, ofstream::out);
        delete[] inpath1;
        delete[] inpath2;
        
        //read latest version
        if(read(sock, &ver, sizeof(ver)) <= 0){
            perror("read");
            close(sock);
            return;
        }
        this->version = ver;
        //read file size
        long fsize;
        if(read(sock, &fsize, sizeof(fsize)) <= 0){
            perror("read");
            close(sock);
            return;
        }
        if(fsize == 0) return;
        //read FILE
        long bsize = BUFFSIZE;
        if (fsize > bsize){
            //NOT ONE read
            int tsize = fsize;
            while(tsize > bsize){
                buff = new char[bsize];
                if (read(sock, buff, bsize) != bsize) {
                    //ERROR
                    delete[] buff;
                    f.close();
                    return;
                }
                f.write(buff,bsize);
                tsize-=bsize;
                delete[] buff;
            }
            //LAST read
            buff = new char[bsize];
            
            if (read(sock, buff, bsize) != bsize) {
                //ERROR
                delete[] buff;
                f.close();
                return;
            }
            f.write(buff,tsize);
            delete[] buff;
        }
        else{
            //ONE read
            buff = new char[bsize];
            if (read(sock, buff, bsize) != bsize) {
                //ERROR
                delete[] buff;
                f.close();
                return;
            }
            f.write(buff, fsize);
            delete[] buff;
        }
        f.close();
    }



}
