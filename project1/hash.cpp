#include "hash.hpp"

int HashList::isEmpty(){
    return !(this->size);
}

void HashList::addData(int bucketSize, userNode * user, transcNode * transc){
    if (this->isEmpty()){
        this->first = new hashBucket;
        this->first->index = 1;
        this->first->next = NULL;
        this->first->bucket = new hashNode[bucketSize];
        this->first->bucket[0].user = user;
        this->first->bucket[0].list.addTranscInfo(transc);
        this->size++;
    }
    else {
        hashNode * node = this->findUser(user->wallet.walletId);
        if(node == NULL){
            hashBucket * temp = this->first;
            for (int i=1; i<this->size; i++){
                temp = temp->next;    
            }
            //if bucket full
            if (temp->index == bucketSize){
                //create new bucket
                temp->next = new hashBucket;
                temp->next->index = 1;
                temp->next->next = NULL;
                temp->next->bucket = new hashNode[bucketSize];
                temp->next->bucket[0].user = user;
                temp->next->bucket[0].list.addTranscInfo(transc);
                this->size++;
            }
            else{
                temp->bucket[temp->index].user = user;
                temp->bucket[temp->index].list.addTranscInfo(transc);
                temp->index++;
            }
        }
        else{
            node->list.addTranscInfo(transc);
        }
    }
    return;
}

hashNode * HashList::findUser(int walletId){
    if(this->isEmpty()) return NULL;
    hashBucket * temp = this->first;
    for (int i=0; i<this->size; i++){
        for(int j=0; j<temp->index; j++){
            if (temp->bucket[j].user->wallet.walletId == walletId){
                return &(temp->bucket[j]);
            }
        }
        temp = temp->next;
    }
    return NULL;
}