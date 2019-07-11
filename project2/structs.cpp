#include "structs.hpp"

int IdList::isEmpty(){
    return !(this->size);
}

int IdList::findNewId(){
    if(!(this->isEmpty())){
        idNode * temp = this->first;
        for (int i=0; i<this->size; i++){
            if(temp->flag == 1){
                temp->flag = 0;     //set flag to visited
                return temp->id;
            }
            temp = temp->next;
        }
        return -1;  //no ids where added
    }
}

void IdList::resetFlags(){
    if(!(this->isEmpty())){
        idNode * temp = this->first;
        for (int i=0; i<this->size; i++){
            temp->flag = -1;
            temp = temp->next;
        }
    }
}

int IdList::deleteId(int id){
    if(!(this->isEmpty())){
        idNode * temp = this->first;
        for (int i=0; i<this->size; i++){
            if (i == (size - 1)) break;
            if (temp->next->id == id){     //delete id
                idNode * temp1 = temp->next->next;
                delete temp->next;
                temp->next = temp1;
                this->size--;
                return this->size;
            }
            temp = temp->next;
        }
        return -1;  //id doesnt exist;
    }
}

int IdList::findFFlag(){
    if(!(this->isEmpty())){
        idNode * temp = this->first;
        for (int i=0; i<this->size; i++){
            if(temp->flag == -1){
                int id = temp->id;
                this->deleteId(id);
                return id;
            }
            temp = temp->next;
        }
        return -1;  //no ids where deleted
    }
}

idNode * IdList::addId(int id){ //return pointer to added node
    if (this->isEmpty()){
        this->first = new idNode;   //fisrt id is always selfID
        this->first->next = NULL;
        this->first->id = id;
        this->first->flag = 1;
        this->size++;
        return this->first;
    }
    else{
        idNode * temp = this->first;
        for (int i=0; i<this->size; i++){
            if (temp->id == id){     //id alreday exists
                temp->flag = 0;
                return NULL;
            }
            if (i == (size - 1)) break;
            temp = temp->next;
        }
        temp->next = new IdNode;
        temp->next->next = NULL;
        temp->next->id = id;
        temp->next->flag = 1;
        this->size++;
        return temp->next;
    }
}