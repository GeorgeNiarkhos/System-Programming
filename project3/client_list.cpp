#include "client_list.hpp"

using namespace std;

int ClientList::isEmpty(){
    return !(this->size);
}

infoNode * ClientList::addInfoNode(unsigned long ip, int port){
    if(this->isEmpty()){
        this->first = new infoNode;
        this->first->IP = ip;
        this->first->port = port;
        this->first->next = NULL;
        this->size++;
        return this->first;
    }
    else{
        infoNode * temp = this->first;
        while(1){
            if(temp->IP == ip && temp->port == port){
                //same info dont add
                return NULL;
            }
            if(temp->next == NULL) break;
            temp = temp->next;
        }
        temp->next = new infoNode;
        temp->next->IP = ip;
        temp->next->port = port;
        temp->next->next = NULL;
        this->size++;
        return temp;
    }
}

infoNode * ClientList::findInfoNode(unsigned long ip, int port){
    if(this->isEmpty()){
        return NULL;
    }
    else{
        infoNode * temp = this->first;
        while(temp != NULL){
            if(temp->IP == ip && temp->port == port){
                //same info return
                return temp;
            }
            temp = temp->next;
        }
        return NULL;
    }
}

infoNode * ClientList::findInfoNodebyAddress(unsigned long ip){
    if(this->isEmpty()){
        return NULL;
    }
    else{
        infoNode * temp = this->first;
        while(temp != NULL){
            if(temp->IP == ip){
                //same info return
                return temp;
            }
            temp = temp->next;
        }
        return NULL;
    }
}

int ClientList::deleteNodebyAddress(unsigned long ip){
    if(this->isEmpty()){
        return -1;
    }
    infoNode * temp;
    if(this->first->IP == ip){    
        if(this->size != 1){
            temp = this->first->next;
            delete this->first;
            this->first = temp;
        }
        else{
            delete this->first;
            this->first = NULL;
        }
        this->size--;
        return this->size;
    }
    else{
        if(this->size == 1){
            return -1;
        }
        temp = this->first;
        for (int i=0; i<this->size; i++){
            if (i == (size - 1)) break;
            if (temp->next->IP == ip){     //delete ip
                infoNode * temp1 = temp->next->next;
                delete temp->next;
                temp->next = temp1;
                this->size--;
                return this->size;
            }
            temp = temp->next;
        }
        return -1;  //ip doesnt exist;
    }
}

void ClientList::print(){
    if(this->isEmpty()){
        return;
    }
    infoNode * temp = this->first;
    for(int i=0; i<this->size; i++){
        cout << "IP: " << temp->IP << " port: " << temp->port << endl;
        temp=temp->next;
    }
}

//FileName

int FileNameList::isEmpty(){
    return !(this->size);
}

int FileNameList::addFileNameNode(char * pathname, time_t version){
    if(this->isEmpty()){
        if(pathname == NULL) return 1;
        this->first = new fileNameNode;
        this->first->path = new char[strlen(pathname) + 1];
        strcpy(this->first->path, pathname);
        this->first->version = version;
        this->first->next = NULL;
        this->size++;
        return 0;
    }
    else{
        if(pathname == NULL) return 1;
        fileNameNode * temp = this->first;
        while(1){
            if(temp->next == NULL) break;
            temp = temp->next;
        }
        temp->next = new fileNameNode;
        temp->next->path = new char[strlen(pathname) + 1];
        strcpy(temp->next->path, pathname);
        temp->next->version = version;
        temp->next->next = NULL;
        this->size++;
        return 0;
    }
}

fileNameNode * FileNameList::findFileNameNode(char * pathname){
    if(this->isEmpty()){
        return NULL;
    }
    else{
        if(pathname == NULL) return NULL;
        fileNameNode * temp = this->first;
        while(temp != NULL){
            if(!strcmp(pathname, temp->path)){
                //same info return
                return temp;
            }
            temp = temp->next;
        }
        return NULL;
    }
}

int FileNameList::deleteNodebyName(char * pathname){
    if(this->isEmpty()){
        return -1;
    }
    if(pathname == NULL) return -1;
    fileNameNode * temp;
    if(!strcmp(this->first->path, pathname)){    
        if(this->size != 1){
            temp = this->first->next;
            delete[] this->first->path;
            delete this->first;
            this->first = temp;
        }
        else{
            delete[] this->first->path;
            delete this->first;
            this->first = NULL;
        }
        this->size--;
        return this->size;
    }
    else{
        if(this->size == 1){
            return -1;
        }
        temp = this->first;
        for (int i=0; i<this->size; i++){
            if (i == (size - 1)) break;
            if (!strcmp(temp->next->path, pathname)){     //delete path
                fileNameNode * temp1 = temp->next->next;
                delete[] temp->next->path;
                delete temp->next;
                temp->next = temp1;
                this->size--;
                return this->size;
            }
            temp = temp->next;
        }
        return -1;  //ip doesnt exist;
    }
}

void FileNameList::print(){
    if(this->isEmpty()){
        return;
    }
    fileNameNode * temp = this->first;
    for(int i=0; i<this->size; i++){
        cout << "Path: " << temp->path << " version: " << temp->version << endl;
        temp=temp->next;
    }
}
    