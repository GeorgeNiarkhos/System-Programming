#include "dataStructs.hpp"

int BitNodeList::isEmpty(){
    return !(this->size);
}

void BitNodeList::addNode(bitcoinNode * node){
    if(this->isEmpty()){
        this->first = new bitcoinNodeList;
        this->first->next = NULL;
        this->first->node = node;
        this->last = this->first;
        this->size++;
    }
    else{
        this->last->next = new bitcoinNodeList;
        this->last->next->next = NULL;
        this->last->next->node = node;
        this->last = this->last->next;
        this->size++;
    }
    return;
}

int BitcoinList::isEmpty(){
    return !(this->size);
}

int BitcoinList::addBitcoinInfo(int val, int bitcoinId){ //returns 0 for update and 1 for insert
    if (this->isEmpty()){
        this->first = new bitcoinInfo;
        this->first->val = val;
        this->first->next = NULL;
        this->first->bitcoinId = bitcoinId;
        this->size++;
        return 1;
    }
    bitcoinInfo * temp = this->first;
    for (int i=0; i<this->size; i++){
        if (temp->bitcoinId == bitcoinId){
            temp->val += val;
            if (temp->val < 0){
                return ERROR;
            }
            return 0;
        }
        if (i == (size - 1)) break;
        temp = temp->next;
    }
    temp->next = new bitcoinInfo;
    temp->next->val = val;
    temp->next->next = NULL;
    temp->next->bitcoinId = bitcoinId;
    this->size++;
    return 1;
}

int BitcoinList::findBitTree(int bitcoinId){
    bitcoinInfo * temp = this->first;
    for (int i=0; i<this->size; i++){
        if (temp->bitcoinId == bitcoinId){
            return temp->val;
        }
        temp = temp->next;
    }
    return -1;  //empty list or not found
}    

int Wallet::addBitInfo(int bitcoinId, int val){
    this->val+= val;
    return this->bitcoins.addBitcoinInfo( val, bitcoinId);
}

int Wallet::findBitInfo(int bitcoinId){ //returns val
    return this->bitcoins.findBitTree(bitcoinId);
}


int UserList::isEmpty(){
    return !(this->size);
}

userNode * UserList::addUser(char * userName){ //returns added user pointer
    userNode * temp;
    if (this->isEmpty()){
        this->first = new userNode;
        this->first->name = new char[strlen(userName)+1];
        strcpy(this->first->name, userName); 
        this->first->next = NULL;
        this->first->wallet.walletId = this->size + 1;
        this->size++;
        return this->first;
    }
    temp = this->first;
    for (int i=0; i<this->size; i++){
        if (!strcmp(temp->name, userName)) return NULL;
        if (i == (size - 1)) break;
        temp = temp->next;
    }
    temp->next = new userNode;
    temp->next->name = new char[strlen(userName)+1];
    strcpy(temp->next->name, userName); 
    temp->next->next = NULL;
    temp->next->wallet.walletId = this->size + 1;
    this->size++;
    return temp->next;
}

userNode * UserList::findUserByWalletId(int walletId){
    userNode * temp=this->first;
    if (walletId > this->size || walletId < 0) return NULL;
    for (int i=1; i<walletId; i++){
        temp = temp->next;
    }
    return temp;
}

userNode * UserList::findUserByName(char * name){
    userNode * temp=this->first;
    for (int i=0; i<this->size; i++){
        if (!strcmp(temp->name, name)){
            return temp;
        }
        temp = temp->next;
    }    
    return NULL;
}


int TranscList::isEmpty(){
    return !(this->size);
}

transcNode * TranscList::addTranscNode(char * transcId, userNode * sender, userNode * receiver, int val, char * date, char * time){ //returns trans added
    transcNode * temp;
    if (this->isEmpty()) {
        this->first = new transcNode;
        this->first->val = val;
        this->first->next = NULL;
        this->first->sender = sender;
        this->first->receiver = receiver;
        this->first->transcId = new char[strlen(transcId)+1];
        strcpy(this->first->transcId, transcId);
        this->first->date = new char[strlen(date)+1];
        strcpy(this->first->date, date);
        this->first->time = new char[strlen(time)+1];
        strcpy(this->first->time, time);
        this->size++;
        return this->first; 
    }
    temp = this->first;
    for (int i=0; i<this->size; i++){
        if (!strcmp(transcId, temp->transcId)) return NULL;
        if (i == size - 1) break;
        temp = temp->next;
    }
    temp->next = new transcNode;
    //add new vals
    temp->next->val = val;
    temp->next->next = NULL;
    temp->next->sender = sender;
    temp->next->receiver = receiver;
    temp->next->transcId = new char[strlen(transcId)+1];
    strcpy(temp->next->transcId, transcId);
    temp->next->date = new char[strlen(date)+1];
    strcpy(temp->next->date, date);
    temp->next->time = new char[strlen(time)+1];
    strcpy(temp->next->time, time); 
    temp->next->next = NULL;
    this->size++; 
    return temp->next;
}

transcNode * TranscList::findTranscNode(char * transcId){
    transcNode * temp;
    temp = this->first;
    for (int i=0; i<this->size; i++){
        if (!strcmp(transcId, temp->transcId)) break;
        temp = temp->next;
    }
    return temp;
}

transcNode * TranscList::getLast(){
    if (this->isEmpty()){
        return NULL;
    }
    transcNode * temp = this->first;
    for (int i=1; i<this->size; i++){
        temp = temp->next;
    }
    return temp;
}


int TranscInfoList::isEmpty(){
    return !(this->size);
}

transcInfoNode * TranscInfoList::addTranscInfo(transcNode * transc){ //return pointer to added node
    if (this->isEmpty()){
        this->first = new transcInfoNode;
        this->first->next = NULL;
        this->first->transc = transc;
        this->size++;
        return this->first;
    }
    else{
        transcInfoNode * temp = this->first;
        for (int i=0; i<this->size; i++){
            if (!strcmp(temp->transc->transcId, transc->transcId)){
                return NULL;
            }
            if (i == (size - 1)) break;
            temp = temp->next;
        }
        temp->next = new TranscInfoNode;
        temp->next->next = NULL;
        temp->next->transc = transc;
        this->size++;
        return temp->next;
    }
}


int BitcoinTreeList::isEmpty(){
    return !(this->size);
}

void BitcoinTreeList::deleteTree(bitcoinNode * node){
    if (node == NULL) return;  
  
    // first delete both subtrees
    deleteTree(node->left);  
    deleteTree(node->right);  
      
    // then delete the node
    delete(node);
}

bitcoinNode * BitcoinTreeList::addBitTree(int bitcoinId, int walletId, int val, struct TranscactionNode * transc){
    bitcoinTreeNode * temp = this->first;
    if (this->isEmpty()){
        this->first = new BitcoinTreeNode;
        this->first->bitcoinId = bitcoinId;
        this->first->next = NULL;
        this->first->bitcoinTree = new bitcoinNode;
        this->first->bitcoinTree->walletId = walletId;
        this->first->bitcoinTree->val = val;
        this->first->bitcoinTree->transc = transc;
        this->first->bitcoinTree->left = NULL;
        this->first->bitcoinTree->right = NULL;
        this->size++;
        return this->first->bitcoinTree;
    }
    for (int i=0; i<size; i++){
        if (temp->bitcoinId == bitcoinId){
            return NULL; // bitcoins tree already exists
        }
        else if(i == size-1){
            break;
        }
        temp = temp->next;
    }
    temp->next = new BitcoinTreeNode;
    temp->next->bitcoinId = bitcoinId;
    temp->next->next = NULL;
    temp->next->bitcoinTree = new bitcoinNode;
    temp->next->bitcoinTree->walletId = walletId;
    temp->next->bitcoinTree->val = val;
    temp->next->bitcoinTree->transc = transc;
    temp->next->bitcoinTree->left = NULL;
    temp->next->bitcoinTree->right = NULL;
    this->size++;
    return temp->next->bitcoinTree;
}

bitcoinTreeNode * BitcoinTreeList::findBitTree(int bitcoinId){
    if(this->isEmpty()) return NULL;
    bitcoinTreeNode * temp = this->first;
    for (int i=0; i<size; i++){
        if (temp->bitcoinId == bitcoinId){
            return temp;
        }
        temp = temp->next;
    }
    return NULL;
}

int BitcoinTreeList::height(bitcoinNode * node){
    if (node==NULL) 
        return 0; 
    else{ 
        /* compute the height of each subtree */
        int lheight = this->height(node->left); 
        int rheight = this->height(node->right); 
        /* use the larger one */
        if (lheight > rheight){ 
            return(lheight+1);
        } 
        else{
            return(rheight+1);
        } 
    } 
} 

void BitcoinTreeList::findUserAtLevel(bitcoinNode * node, int level, int walletId, BitNodeList &list){ 
    if (node == NULL) 
        return; 
    if (level == 1){ 
        //if walletId is leaf
        if ((node->walletId == walletId) && (node->left == NULL) && (node->right == NULL)){
            //add to list
            list.addNode(node);
        }
        return;
    }
    else if (level > 1){ 
        this->findUserAtLevel(node->left, level-1, walletId, list); 
        this->findUserAtLevel(node->right, level-1, walletId, list); 
    }
}

int BitcoinTreeList::findTranscNum(bitcoinNode * node){
    if (node==NULL) 
        return 0; 
    else{ 
        //if no transc add 0
        if (node->transc == NULL){
            return 0 + this->findTranscNum(node->left) + this->findTranscNum(node->right);
        }
        //else add 1
        else{
            return 1 + this->findTranscNum(node->left) + this->findTranscNum(node->right);
        }
    }
}

void BitcoinTreeList::findTranscAtLevel(bitcoinNode * node, int level, TranscInfoList &list){ 
    if (node == NULL) 
        return; 
    if (level == 1){ 
        //if trans != NULL
        if (node->transc != NULL){
            //add to list
            list.addTranscInfo(node->transc);
        }
        return;
    }
    else if (level > 1){ 
        this->findTranscAtLevel(node->left, level-1, list); 
        this->findTranscAtLevel(node->right, level-1, list); 
    }
}

int BitcoinTreeList::findNotSpent(bitcoinNode * node){
    //go to most left node if there exists and return val else return 0
    if (node->left == NULL){
        if (node->right == NULL){
            return node->val;
        }
        return 0;   
    }
    return this->findNotSpent(node->left);
    
}

int BitcoinTreeList::findBitTreeHeight(int bitcoinId){
    return this->height(this->findBitTree(bitcoinId)->bitcoinTree);
}

//Debug tree prints
void BitcoinTreeList::printBfs(bitcoinNode * root){ 
    int h = height(root); 
    int i;
    if (root == NULL) return;
    for (i=1; i<=h; i++){ 
        printAtLevel(root, i);
        cout << endl;
    } 
} 

void BitcoinTreeList::printAtLevel(bitcoinNode * root, int level){ 
    if (root == NULL){ 
        return;
    } 
    if (level == 1){ 
        cout << root->walletId << "/" << root->val << " " ;
    } 
    else if (level > 1){ 
        printAtLevel(root->left, level-1);
        printAtLevel(root->right, level-1); 
    } 
} 
