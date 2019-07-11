#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <signal.h>
#include <fstream>
#include <errno.h> 
#include <unistd.h>
#include <sys/wait.h>
#include <poll.h>

using namespace std;

typedef struct IdNode{
    int id;
    int flag;   //-1 not visited // 0 visited // 1 created
    struct IdNode * next;
}idNode;

class IdList{   //list of ids
public:
    //data
    int size;
    idNode * first;
    //constuctor destructor
    IdList(){
        this->size = 0;
        this->first = NULL;
    }
    ~IdList(){
        idNode * temp = this->first;
        idNode * temp1;
        for(int i=0; i<this->size; i++){
            temp1 = temp;
            temp = temp->next;
            delete temp1;
        }
    }
    //member Functions
    int isEmpty();

    void resetFlags();

    int deleteId(int id);

    int findFFlag();    //finds first false flag

    int findNewId();    //finds first new id // if none returns -1

    idNode * addId(int id);
};