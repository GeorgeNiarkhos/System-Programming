#include <stdio.h>
#include <poll.h>
#include <arpa/inet.h> /* inet_ntoa */
#include <sys/types.h> /* sockets */
#include <sys/socket.h> /* sockets */
#include <netinet/in.h> /* internet sockets */
#include <unistd.h> /* read, write, close */
#include <netdb.h> /* gethostbyaddr */
#include <stdlib.h> /* exit */
#include <string.h> /* strlen */
#include <signal.h> /* signal */


#include "threads.hpp"
#include "functions.hpp"

using namespace std;

int flag;

JobScheduler scheduler;

void f1(int signum){
    cout << endl << "Exiting..." << endl;
    flag = 1;
}

int main(int argc, char *argv[]) {
    int servPort, socks, newsock, mysock, i, portNum, netPortNum, threadNum, bSize;
    unsigned long myNetAddress;
    char buf[BUFFSIZE];
    socklen_t incominglen;
    struct hostent *host_entry;
    struct sockaddr_in server, host, incoming;
    struct sockaddr *serverptr = (struct sockaddr*)&server;
    struct sockaddr *hostptr=(struct sockaddr *)&host;
    struct hostent *rem;
    //DEFINE ARGUMENTS
    if (argc%2 == 0){
        cerr << "Wrong argument input..." << endl;
        exit(EXIT_FAILURE);
    }
    char *d=NULL, *p=NULL, *w=NULL, *sp=NULL, *sip=NULL, *b=NULL;
    for (int i=1; i<argc; i++){
        if (!strcmp("-d", argv[i])){
            if ((i+1 >= argc) || (argv[i+1][0] == '-') || (d != NULL)){
                cerr << "Wrong argument input..." << endl;
                exit(EXIT_FAILURE);
            }
            d=argv[i+1];
            i++;
        }
        else if (!strcmp("-p", argv[i])){
            if ((i+1 >= argc) || (argv[i+1][0] == '-') || (p != NULL)){
                cerr << "Wrong argument input..." << endl;
                exit(EXIT_FAILURE);
            }
            p=argv[i+1];
            i++;
        }
        else if (!strcmp("-w", argv[i])){
            if ((i+1 >= argc) || (argv[i+1][0] == '-') || (w != NULL)){
                cerr << "Wrong argument input..." << endl;
                exit(EXIT_FAILURE);
            }
             w=argv[i+1];
             i++;
        }
        else if (!strcmp("-sp", argv[i])){
            if ((i+1 >= argc) || (argv[i+1][0] == '-') || (sp != NULL)){
                cerr << "Wrong argument input..." << endl;
                exit(EXIT_FAILURE);
            }
            sp=argv[i+1];
            i++;
        }
        else if (!strcmp("-sip", argv[i])){
            if ((i+1 >= argc) || (argv[i+1][0] == '-') || (sip != NULL)){
                cerr << "Wrong argument input..." << endl;
                exit(EXIT_FAILURE);
            }
            sip=argv[i+1];
            i++;
        }
        else if (!strcmp("-b", argv[i])){
            if ((i+1 >= argc) || (argv[i+1][0] == '-') || (b != NULL)){
                cerr << "Wrong argument input..." << endl;
                exit(EXIT_FAILURE);
            }
            b=argv[i+1];
            i++;
        }
        else {
            cerr << "Wrong argument input..." << endl;
            exit(EXIT_FAILURE);
        }
    }
    if (d == NULL || p == NULL || w == NULL || sp == NULL || sip == NULL || b == NULL){
        cerr << "Wrong argument input..." << endl;
        exit(EXIT_FAILURE);
    }
    //SIG HANDLERS
    static struct sigaction act;
    act.sa_handler = f1;
    sigemptyset(&act.sa_mask);
    sigaction(SIGINT, &act, NULL);
    flag = 0;
    //ASSIGN ARGS VALS
    portNum = atoi(p);
    netPortNum = htons(portNum);
    threadNum = atoi(w);
    bSize = atoi(b);
    servPort = atoi(sp); /*Convert port number to integer*/
    ClientList* list = new ClientList;
    FileNameList * pathList = new FileNameList();
    char * addPath = NULL;
    int pr = getFileList(d, *pathList, d, addPath);
    pathList->print();
    delete pathList;
    //Start threads
    scheduler.init(threadNum, bSize, list);
    /* Create socket for server*/
    if ((socks = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        delete list;
        perror("socket");
        exit(EXIT_FAILURE);
    }
    //Find my address
    if (gethostname(buf, sizeof(buf)) == -1){
        delete list;
        perror("gethostname");
        exit(EXIT_FAILURE);
    }
    if ((host_entry = gethostbyname(buf)) == NULL) {
        delete list;
        perror("gethostbyname");
        exit(EXIT_FAILURE);
    }
    myNetAddress = (*((struct in_addr*) host_entry->h_addr)).s_addr;
    /* Find server address */
    if ((rem = gethostbyname(sip)) == NULL) {
        delete list;
        perror("gethostbyname");
        exit(EXIT_FAILURE);
    }
    
    server.sin_family = AF_INET; /* Internet domain */
    memcpy(&server.sin_addr, rem->h_addr, rem->h_length);
    server.sin_port = htons(servPort); /* Server port */
    /* Initiate connection */
    if (connect(socks, serverptr, sizeof(server)) < 0){
        delete list;
        perror("connect");
        scheduler.destroy();
        exit(EXIT_FAILURE);
    }
    printf("Connecting to %s port %d\n", sip, servPort);
    //LOG ON
    strcpy(buf, "LOG_ON");
    cout << buf << endl;
    if (write(socks, buf, BUFFSIZE) < 0){
        perror("write");
        delete list;
        close(socks);
        exit(EXIT_FAILURE);
    }
    if (write(socks, &myNetAddress, sizeof(long)) < 0){
        delete list;
        perror("write");
        close(socks);
        exit(EXIT_FAILURE);
    }
    
    if (write(socks, &netPortNum, sizeof(int)) < 0){
        delete list;
        perror("write");
        close(socks);
        exit(EXIT_FAILURE);
    }
    //read answer
    if(read(socks, buf, BUFFSIZE) <= 0){
        delete list;
        perror("read");
        close(socks);
        exit(EXIT_FAILURE);
    }
    if(!strcmp(buf, "OK")){
        //GET CLIENTS
        //write answer
        strcpy(buf, "GET_CLIENTS");
        if (write(socks, buf, BUFFSIZE) < 0){
            perror("write");
            delete list;
            close(socks);
            exit(EXIT_FAILURE);
        }
        //read answer
        if(read(socks, buf, BUFFSIZE) <= 0){
            perror("read");
            delete list;
            close(socks);
            exit(EXIT_FAILURE);
        }
        if(strcmp(buf, "CLIENT_LIST")){
            cerr << "protocol error, read : " << buf << endl;
            delete list;
            close(socks);
            exit(EXIT_FAILURE);  
        }
        //read size
        int size;
        if(read(socks, &size, sizeof(int)) <= 0){
            perror("read");
            delete list;
            close(socks);
            exit(EXIT_FAILURE);
        }
        if(size != 0){
            //add to list
            for(int i=0; i<size; i++){
                unsigned long nip;
                int nport;
                //read IP
                if(read(socks, &nip, sizeof(unsigned long)) <= 0){
                    perror("read");
                    delete list;
                    close(socks);
                    exit(EXIT_FAILURE);
                }
                //read PORT
                if(read(socks, &nport, sizeof(int)) <= 0){
                    perror("read");
                    delete list;
                    close(socks);
                    exit(EXIT_FAILURE);
                }
                nip = ntohl(nip);
                nport = ntohs(nport);
                //add info to struct
                list->addInfoNode(nip, nport);
                //schedule job
                scheduler.schedule(new Job(nip, nport));
            }
        }
        close(socks);
    }
    else if(!strcmp(buf, "ERROR")){
        //  duplicate error
        cerr << "Duplicate Error " << endl;
        delete list;
        close(socks);
        exit(EXIT_FAILURE);
    }
    else{
        cerr << "Protocol Error " << endl;
        delete list;
        close(socks);
        exit(EXIT_FAILURE);
    }
    /* Create socket */
    if ((mysock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        delete list;
        perror("socket");
        exit(EXIT_FAILURE);
    }
    int enable = 1;
    if (setsockopt(mysock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
        perror("setsockopt(SO_REUSEADDR) failed");
    host.sin_family = AF_INET; /* Internet domain */
    host.sin_addr.s_addr = htonl(INADDR_ANY);
    host.sin_port = htons(portNum); /* The given port */
    /* Bind socket to address */
    if (bind(mysock, hostptr, sizeof(host)) < 0){
        delete list;
        perror("bind");
        exit(EXIT_FAILURE);
    }
    /* Listen for connections */
    if (listen(mysock, 5) < 0) {
        delete list;
        perror("listen");
        exit(EXIT_FAILURE);
    }
    int ret;
    struct pollfd fds[1];
    fds[0].fd = mysock;
    fds[0].events = POLLIN;
    int timeout_msecs = 500;
    //...
    printf("Listening for connections to port %d\n", portNum);
    while(1){
        if(flag){
            /* Create socket for server*/
            if ((socks = socket(AF_INET, SOCK_STREAM, 0)) < 0){
                perror("socket");
                //destroy threads
                scheduler.destroy();
                delete list;
                exit(EXIT_FAILURE);
            }
            /* Initiate connection */
            if (connect(socks, serverptr, sizeof(server)) < 0){
                delete list;
                //destroy threads
                scheduler.destroy();
                perror("connect123");
                exit(EXIT_FAILURE);
            }
            //LOG OFF
            strcpy(buf, "LOG_OFF");
            if (write(socks, buf, BUFFSIZE) < 0){
                perror("write");
                delete list;
                //destroy threads
                scheduler.destroy();
                close(socks);
                exit(EXIT_FAILURE);
            }
            //READ ANSWER
            if (read(socks, buf, BUFFSIZE) < 0){
                perror("read");
                delete list;
                //destroy threads
                scheduler.destroy();
                close(socks);
                exit(EXIT_FAILURE);
            }
            if(!strcmp("OK", buf)){
                delete list;
                //destroy threads
                scheduler.destroy();
                exit(EXIT_SUCCESS);
            }
            else{
                cerr << buf << " returned from server" << endl;
                delete list;
                //destroy threads
                scheduler.destroy();
                exit(EXIT_FAILURE);
            }
            
            
        }
        ret = poll(fds, 1, timeout_msecs);
        if(ret > 0) {
            if(fds[0].revents == POLLIN){
                //if there is somthing to read
            }
            else{
                cerr << "poll not read! "  << fds[0].revents  << " " << POLLHUP << endl;
                continue;
            }
        }
        else{
            continue;
        }
        struct sockaddr_in incoming;
        struct sockaddr *incomingptr=(struct sockaddr *)&incomingptr;
        incominglen = sizeof(incoming);
        /* accept connection */
        if ((newsock = accept(mysock, incomingptr, &incominglen)) < 0){
            perror("accept");
            close(newsock);
            delete list;
            exit(EXIT_FAILURE);
        }
        if(read(newsock, buf, BUFFSIZE) <= 0){
            perror("read");
            close(newsock);
            delete list;
            exit(EXIT_FAILURE);
        }
        if(!strcmp(buf, "USER_ON")){
            cout << "USER_ON" << endl;
            //read ip
            unsigned long netIp;
            int netPort;
            //read netIp
            if(read(newsock, &netIp, sizeof(unsigned long)) <= 0){
                perror("read");
                close(newsock);
                delete list;
                exit(EXIT_FAILURE);
            }
            //read netPort
            if(read(newsock, &netPort, sizeof(int)) <= 0){
                perror("read");
                close(newsock);
                delete list;
                exit(EXIT_FAILURE);
            }
            //add node
            list->addInfoNode(ntohl(netIp) , ntohs(netPort));
            scheduler.schedule(new Job(ntohl(netIp), ntohs(netPort)));
            list->print();
        }
        else if(!strcmp(buf, "USER_OFF")){
            //USER_OFF
            cout << "USER_OFF" << endl;
            //read ip
            unsigned long netIp;
            int netPort;
            //read netIp
            if(read(newsock, &netIp, sizeof(unsigned long)) <= 0){
                perror("read");
                close(newsock);
                delete list;
                exit(EXIT_FAILURE);
            }
            //read netPort
            if(read(newsock, &netPort, sizeof(int)) <= 0){
                perror("read");
                close(newsock);
                delete list;
                exit(EXIT_FAILURE);
            }
            //delete node
            list->deleteNodebyAddress(ntohl(netIp));
            list->print();
        }
        else if(!strcmp(buf, "GET_FILE_LIST")){
            //write FILE_LIST
            FileNameList * pathList = new FileNameList();
            getFileList(d, *pathList, d, addPath);
            strcpy(buf, "FILE_LIST");
            if(write(newsock, buf, BUFFSIZE) <= 0){
                perror("write");
                close(newsock);
                delete list;
                exit(EXIT_FAILURE);
            }
            //Write file list size
            int fsize = pathList->size;
            if(write(newsock, &fsize, sizeof(int)) <= 0){
                perror("write");
                close(newsock);
                delete list;
                exit(EXIT_FAILURE);
            }
            if(fsize != 0){
                fileNameNode * temp = pathList->first;
                for(int i=0; i<fsize; i++){
                    //write path
                    if(write(newsock, temp->path, BUFFSIZE) <= 0){
                        perror("write");
                        close(newsock);
                        delete list;
                        exit(EXIT_FAILURE);
                    }
                    //write version
                    time_t ver = temp->version;
                    if(write(newsock, &ver, sizeof(ver)) <= 0){
                        perror("write");
                        close(newsock);
                        delete list;
                        exit(EXIT_FAILURE);
                    }
                    temp = temp->next;
                }
            }
            delete pathList;
        }
        else if(!strcmp(buf, "GET_FILE")){
            FileNameList * pathList = new FileNameList();
            getFileList(d, *pathList, d, addPath);
            //read pathname
            if(read(newsock, buf, BUFFSIZE) <= 0){
                perror("read");
                close(newsock);
                delete list;
                exit(EXIT_FAILURE);
            }
            //read version
            time_t ver;
            if(read(newsock, &ver, sizeof(ver)) <= 0){
                perror("read");
                close(newsock);
                delete list;
                exit(EXIT_FAILURE);
            }
            //check if filename exists
            fileNameNode * temp;
            if((temp = pathList->findFileNameNode(buf)) == NULL){
                cout << "file path " << buf << " not found" << endl;
                //file not found
                strcpy(buf, "FILE_NOT_FOUND");
                if(write(newsock, buf, BUFFSIZE) <= 0){
                    perror("write");
                    close(newsock);
                    delete list;
                    exit(EXIT_FAILURE);
                }
                
            }
            //check if latest version
            else if(temp->version == ver){
                //file up to date
                strcpy(buf, "FILE_UP_TO_DATE");
                if(write(newsock, buf, BUFFSIZE) <= 0){
                    perror("write");
                    close(newsock);
                    delete list;
                    exit(EXIT_FAILURE);
                }
            }
            //if file is changed
            else{
                //write FILE_SIZE string
                strcpy(buf, "FILE_SIZE");
                if(write(newsock, buf, BUFFSIZE) <= 0){
                    perror("write");
                    close(newsock);
                    delete list;
                    exit(EXIT_FAILURE);
                }
                //write version
                ver = temp->version;
                if(write(newsock, &ver, sizeof(ver)) <= 0){
                    perror("write");
                    close(newsock);
                    delete list;
                    exit(EXIT_FAILURE);
                }
                //write file size
                //write file
                sprintf(buf,"%s/%s", d, temp->path);
                writeFunc(buf, newsock, BUFFSIZE);
            }
            delete pathList;

        }
        else{
            flag=1; //exit
        }

        
    }
}
