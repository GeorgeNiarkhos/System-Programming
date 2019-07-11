#ifndef DATA_STRUCTS_H
#define DATA_STRUCTS_H
#include "dataStructs.hpp"
#endif

typedef struct HashNode{
    userNode * user;
    TranscInfoList list;
}hashNode;

typedef struct HashBucket{
    hashNode * bucket;
    int index;  //points to next cell to be filled
    struct HashBucket * next;
}hashBucket;


class HashList{    
public:
    //data
    int size;
    hashBucket * first;
    //constuctor destructor
    HashList(){
        this->first = NULL;
        this->size = 0;
    }
    ~HashList() {
        hashBucket * temp = this->first;
        hashBucket * temp1;
        while(temp!=NULL){
            temp1 = temp;
            temp = temp->next;
            // Deleting bucket known error, for somereason cant delete array of objects hashNode
            // delete temp1->bucket;
            delete temp1;
        }
    }
    //member Functions
    int isEmpty();
    
    void addData(int bucketSize, userNode * user, transcNode * transc);

    hashNode * findUser(int walletId);
};