#include <iostream>
#include <string.h>

typedef struct ClientInfoNode{
    unsigned long IP;
    int port;
    struct ClientInfoNode * next;
}infoNode;

typedef struct FileNameNode{
    char* path;
    time_t version;
    struct FileNameNode * next;
}fileNameNode;

class ClientList{
    public:
    int size;
    infoNode * first;

    ClientList(){
        this->size = 0;
        this->first = NULL;
    }

    ~ClientList(){
        infoNode * temp = this->first;
        infoNode * temp1;
        for(int i=0; i<this->size; i++){
            temp1 = temp;
            temp = temp->next;
            delete temp1;
        }
    }

    int isEmpty();

    infoNode * addInfoNode(unsigned long ip, int port);

    infoNode * findInfoNode(unsigned long ip, int port);

    infoNode * findInfoNodebyAddress(unsigned long ip);

    int deleteNodebyAddress(unsigned long ip);

    void print();
};

class FileNameList{
    public:
    int size;
    fileNameNode * first;

    FileNameList(){
        this->size = 0;
        this->first = NULL;
    }

    ~FileNameList(){
        fileNameNode * temp = this->first;
        fileNameNode * temp1;
        for(int i=0; i<this->size; i++){
            temp1 = temp;
            temp = temp->next;
            delete[] temp1->path;
            delete temp1;
        }
    }

    int isEmpty();

    int addFileNameNode(char * pathname, time_t version);

    fileNameNode * findFileNameNode(char * pathname);

    int deleteNodebyName(char * pathname);

    void print();
};

