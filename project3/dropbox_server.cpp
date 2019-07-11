 #include <stdio.h>
#include <string.h>
#include <poll.h>
#include <sys/wait.h> /* sockets */
#include <sys/types.h> /* sockets */
#include <sys/socket.h> /* sockets */
#include <netinet/in.h> /* internet sockets */
#include <netdb.h> /* gethostbyaddr */
#include <unistd.h> /* fork */
#include <stdlib.h> /* exit */
#include <ctype.h> /* toupper */
#include <signal.h> /* signal */

#include "client_list.hpp"

#define BUFFSIZE 512

using namespace std;

int sock;

void f1(int signum){
    cout << endl << "Aborting..." << endl;
    close(sock);
    exit(0);
}

int main(int argc, char *argv[]) {
    int port, newsock;
    struct sockaddr_in server, client;
    socklen_t clientlen;
    struct sockaddr *serverptr=(struct sockaddr *)&server;
    struct sockaddr *clientptr=(struct sockaddr *)&client;
    struct hostent *rem;
    ClientList list;
    if (argc != 3) {
        cerr << "Wrong argument input" << endl;
        exit(EXIT_FAILURE);
    }
    if (!strcmp(argv[1],"-p")){
        port = atoi(argv[2]);
    }
    else{
        cerr << "Wrong argument input" << endl;
        exit(EXIT_FAILURE);  
    }
    
    /* Create socket */
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("socket");
        exit(EXIT_FAILURE);
    }
    int enable = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
        perror("setsockopt(SO_REUSEADDR) failed");
    server.sin_family = AF_INET; /* Internet domain */
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(port); /* The given port */
    /* Bind socket to address */
    if (bind(sock, serverptr, sizeof(server)) < 0){
        perror("bind");
        exit(EXIT_FAILURE);
    }
    //SIG HANDLERS
    static struct sigaction act;
    act.sa_handler = f1;
    sigemptyset(&act.sa_mask);
    sigaction(SIGINT, &act, NULL);
    int ret;
    struct pollfd fds[1];
    fds[0].fd = sock;
    fds[0].events = POLLIN;
    int timeout_msecs = 5000000;
    /* Listen for connections */
    if (listen(sock, 5) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    printf("Listening for connections to port %d\n", port);
    char buf[BUFFSIZE];  //temporary
    while(1){
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
        
        clientlen = sizeof(client);
        /* accept connection */
        if ((newsock = accept(sock, clientptr, &clientlen))< 0){
            perror("accept");
            close(newsock);
            exit(EXIT_FAILURE);
        }
        /* Find client's name */
        if ((rem = gethostbyaddr((char *) &client.sin_addr.s_addr,sizeof(client.sin_addr.s_addr), client.sin_family))== NULL) {
            perror("gethostbyaddr");
            exit(EXIT_FAILURE);
        }
        printf("Accepted connection from %s\n", rem->h_name);
        if(read(newsock, buf, BUFFSIZE) <= 0){
            perror("read");
            close(newsock);
            exit(EXIT_FAILURE);
        }
        if(!strcmp("LOG_ON", buf)){
            cout << "LOG_ON" << endl;
            //read ip
            unsigned long netIp;
            int netPort;
            //read netIp
            if(read(newsock, &netIp, sizeof(unsigned long)) <= 0){
                perror("read");
                close(newsock);
                exit(EXIT_FAILURE);
            }
            //read netPort
            if(read(newsock, &netPort, sizeof(int)) <= 0){
                perror("read");
                close(newsock);
                exit(EXIT_FAILURE);
            }
            //add node
            if (list.addInfoNode(ntohl(netIp) , ntohs(netPort)) != NULL ){
                //no error
                strcpy(buf, "OK");
                if(write(newsock, buf, BUFFSIZE) <= 0){
                    perror("write");
                    close(newsock);
                    exit(EXIT_FAILURE);
                }
                //read GET CLIENTS
                if(read(newsock, buf, BUFFSIZE) <= 0){
                    perror("read");
                    close(newsock);
                    exit(EXIT_FAILURE);
                }
                if (strcmp(buf, "GET_CLIENTS")){
                    cerr << "protocol error, read : " << buf << " pslp " << strlen(buf) << " " << buf[0] << " ll "<< buf[1] << endl;
                    close(newsock);
                    exit(EXIT_FAILURE);
                }
                //write client list
                strcpy(buf, "CLIENT_LIST");
                if(write(newsock, buf, BUFFSIZE) <= 0){
                    perror("write");
                    close(newsock);
                    exit(EXIT_FAILURE);
                }
                //write size
                int csize = list.size-1;
                if(write(newsock, &csize, sizeof(int)) <= 0){
                    perror("write");
                    close(newsock);
                    exit(EXIT_FAILURE);
                }
                if (csize != 0){
                    //if not first
                    infoNode * temp = list.first;
                    while(temp != NULL){
                        //if diff transmit
                        if(temp->IP != ntohl(netIp) || temp->port != ntohs(netPort)){
                            //write IP
                            int nip = htonl(temp->IP);
                            int nport = htons(temp->port);
                            if(write(newsock, &nip, sizeof(unsigned long)) <= 0){
                                perror("write");
                                close(newsock);
                                exit(EXIT_FAILURE);
                            }
                            //write port
                            if(write(newsock, &nport, sizeof(int)) <= 0){
                                perror("write");
                                close(newsock);
                                exit(EXIT_FAILURE);
                            }
                        }
                        temp=temp->next;
                    }
                    close(newsock);
                    //notify other clients
                    cout << "Notifying other clients..." << endl;
                    temp = list.first;
                    while(temp != NULL){
                        //if diff transmit
                        if(temp->IP != ntohl(netIp) || temp->port != ntohs(netPort)){
                            int tempSock;
                            struct sockaddr_in tempServer;
                            struct sockaddr *tempServerptr = (struct sockaddr*)&tempServer;
                            struct hostent *rem;
                            //establish connection
                            /* Create socket for client*/
                            if ((tempSock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
                                perror("socket");
                                exit(EXIT_FAILURE);
                            }
                            int nip = htonl(temp->IP);
                            int nport = htons(temp->port);
                            tempServer.sin_addr.s_addr = nip;
                            tempServer.sin_port = nport;
                            tempServer.sin_family = AF_INET;
                            /* Initiate connection */
                            if (connect(tempSock  , tempServerptr, sizeof(server)) < 0){
                                perror("connect");
                                exit(EXIT_FAILURE);
                            }
                            //write USER ON
                            strcpy(buf, "USER_ON");
                            if(write(tempSock, buf, BUFFSIZE) <= 0){
                                perror("write");
                                close(tempSock);
                                exit(EXIT_FAILURE);
                            }
                            //write IP
                            if(write(tempSock, &netIp, sizeof(unsigned long)) <= 0){
                                perror("write");
                                close(tempSock);
                                exit(EXIT_FAILURE);
                            }
                            //write port
                            if(write(tempSock, &netPort, sizeof(int)) <= 0){
                                perror("write");
                                close(tempSock);
                                exit(EXIT_FAILURE);
                            }
                            close(tempSock);
                            cout << "Notified client " << ntohs(netPort) << endl;
                        }
                        temp=temp->next;
                    }
                }
                
            }
            else{
                //duplicate error
                strcpy(buf, "ERROR");
                if(write(newsock, buf, BUFFSIZE) <= 0){
                    perror("write");
                    exit(EXIT_FAILURE);
                }
                close(newsock); /* parent closes socket to client */
            }
            list.print();
        }
        else if(!strcmp("LOG_OFF", buf)){
            cout << "LOG_OFF" << endl;
            //find client by address
            infoNode *temp = list.findInfoNodebyAddress(ntohl(client.sin_addr.s_addr));
            if(temp != NULL){
                //write OK
                strcpy(buf, "OK");
                if(write(newsock, buf, BUFFSIZE) <= 0){
                    perror("write");
                    close(newsock);
                    exit(EXIT_FAILURE);
                }
                close(newsock);
                unsigned long tip = htonl(temp->IP);
                int tport = htons(temp->port);
                //delete node
                cout << "Notifying other clients for exit" << endl;
                cout << "Remove is : " << list.deleteNodebyAddress(ntohl(client.sin_addr.s_addr)) << endl;
                //notify other users (if any)
                
                if(list.isEmpty() != 1){
                    infoNode *temp1 = list.first;
                    while(temp1 != NULL){
                        cout << "Trying to notify..." << endl;
                        int tempSock;
                        struct sockaddr_in tempServer;
                        struct sockaddr *tempServerptr = (struct sockaddr*)&tempServer;
                        struct hostent *rem;
                        //establish connection
                        /* Create socket for client*/
                        if ((tempSock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
                            perror("socket");
                            exit(EXIT_FAILURE);
                        }
                        int nip = htonl(temp1->IP);
                        int nport = htons(temp1->port);
                        tempServer.sin_addr.s_addr = nip;
                        tempServer.sin_port = nport;
                        tempServer.sin_family = AF_INET;
                        /* Initiate connection */
                        if (connect(tempSock  , tempServerptr, sizeof(server)) < 0){
                            perror("connect");
                            exit(EXIT_FAILURE);
                        }
                        //write USER ON
                        strcpy(buf, "USER_OFF");
                        if(write(tempSock, buf, BUFFSIZE) <= 0){
                            perror("write");
                            close(tempSock);
                            exit(EXIT_FAILURE);
                        }
                        //write IP
                        if(write(tempSock, &tip, sizeof(unsigned long)) <= 0){
                            perror("write");
                            close(tempSock);
                            exit(EXIT_FAILURE);
                        }
                        //write port
                        if(write(tempSock, &tport, sizeof(int)) <= 0){
                            perror("write");
                            close(tempSock);
                            exit(EXIT_FAILURE);
                        }
                        close(tempSock);
                        cout << "Notified " << ntohs(nport) << endl;
                        temp1=temp1->next;
                        
                    }   
                }
                list.print();
            }
            else{
                //write ERROR
                cout << "Client not found" << endl;
                strcpy(buf, "ERROR_IP_PORT_NOT_FOUND_IN_LIST");
                if(write(newsock, buf, BUFFSIZE) <= 0){
                    perror("write");
                    close(newsock);
                    exit(EXIT_FAILURE);
                }
                close(newsock);
            }
        }
        else{
            cerr << "protocol error, read : " << buf << " pslp " << strlen(buf) << endl;
            exit(EXIT_FAILURE);
        }
        
    }
}