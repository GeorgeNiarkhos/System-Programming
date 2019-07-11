#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <stdio.h>
#include <stdint.h>
#include <errno.h>

#define ERROR 10

using namespace std;

struct TranscactionNode;

typedef struct BitcoinNode{ //tree node
	int walletId; // or pointer to user node
	int val;
	struct TranscactionNode * transc;   //pointer to transc
	struct BitcoinNode * left;	// left child
	struct BitcoinNode * right;	// right child
} bitcoinNode;	

typedef struct BitcoinTreeNode{ //node that points to root
	int bitcoinId;
	bitcoinNode * bitcoinTree;
	struct BitcoinTreeNode * next;
} bitcoinTreeNode;

typedef struct BitcoinNodeList  //list of nodes usen in execTransc
{
    bitcoinNode * node;
    struct BitcoinNodeList * next;
}bitcoinNodeList;

typedef struct BitcoinInfo{ //info of bitcoins used in wallet
    int bitcoinId;
    int val;
    struct BitcoinInfo * next;
}bitcoinInfo;

class BitNodeList{  //list of bitcoinNodes pointers

public:
    //data
    int size;
    bitcoinNodeList * first;
    bitcoinNodeList * last;
    //constuctor destructor
    BitNodeList(){
        this->size = 0;
        this->first = NULL;
        this->last = NULL;
    }
    ~BitNodeList(){
        bitcoinNodeList * temp = this->first;
        bitcoinNodeList * temp1;
        for(int i=0; i<this->size; i++){
            temp1 = temp;
            temp = temp->next;
            delete temp1;
        }
    }
    //member Functions
    int isEmpty();
    
    void addNode(bitcoinNode * node);
};


class BitcoinList{  //list with bitcoinInfo val and id for users wallet

public:
    //data
    bitcoinInfo * first;
    int size;
    //constuctor destructor
    BitcoinList() {
        this->size = 0;
        this->first = NULL;
    }
    ~BitcoinList() {
        bitcoinInfo * temp = this->first;
        bitcoinInfo * temp1;
        for(int i=0; i<this->size; i++){
            temp1 = temp;
            temp = temp->next;
            delete temp1;
        }
    }
    //member Functions
    int isEmpty();
    
    int addBitcoinInfo(int val, int bitcoinId);
    
    int findBitTree(int bitcoinId);
};

class Wallet{

public:
    //data
    int walletId;
    int val;
	BitcoinList bitcoins;
    //constuctor destructor
    Wallet() {
    	this->val = 0;
    }
    Wallet(int i):walletId(i){
        this->val = 0;
    }
    ~Wallet() { }
    //member Functions
    int addBitInfo(int bitcoinId, int val); //adds Bitcoin info 
    
    int findBitInfo(int bitcoinId); //returns bitcoin val
};

typedef struct UserNode{
	char * name;
	Wallet wallet;
	struct UserNode * next;

}userNode;

class UserList
{
public:
	//data
    int size;
	userNode * first;
    //constuctor destructor
	UserList(){
		this->size = 0;
		this->first = NULL;
	};
	~UserList(){
        userNode * temp = this->first;
        userNode * temp1;
        for(int i=0; i<this->size; i++){
            temp1 = temp;
            temp = temp->next;
            delete temp1->name;
            delete temp1;
        }
    };
    //member Functions
	int isEmpty();

	userNode * addUser(char * userName);

    userNode * findUserByWalletId(int walletId);

    userNode * findUserByName(char * name);
};

typedef struct TranscactionNode{
    char * transcId;
    userNode * sender;  //pointer
    userNode * receiver;    //pointer
    int val;
    char * date;
    char * time;
    struct TranscactionNode * next;   
}transcNode;


class TranscList{
public:
    //data
    int size;
    transcNode * first;
    //constuctor destructor
    TranscList(){
        this->size = 0;
        this->first = NULL;
    }
    ~TranscList(){
        transcNode * temp = this->first;
        transcNode * temp1;
        for(int i=0; i<this->size; i++){
            temp1 = temp;
            temp = temp->next;
            delete temp1->transcId;
            delete temp1->date;
            delete temp1->time;
            delete temp1;
        }
    }
    //member Functions
    int isEmpty();

    transcNode * addTranscNode(char * transcId, userNode * sender, userNode * receiver, int val, char * date, char * time);

    transcNode * findTranscNode(char * transcId);

    transcNode * getLast();
};

typedef struct TranscInfoNode{
    transcNode * transc;
    struct TranscInfoNode * next;
}transcInfoNode;


class TranscInfoList{   //list of pointers to transc
public:
    //data
    int size;
    transcInfoNode * first;
    //constuctor destructor
    TranscInfoList(){
        this->size = 0;
        this->first = NULL;
    }
    ~TranscInfoList(){
        transcInfoNode * temp = this->first;
        transcInfoNode * temp1;
        for(int i=0; i<this->size; i++){
            temp1 = temp;
            temp = temp->next;
            delete temp1;
        }
    }
    //member Functions
    int isEmpty();

    transcInfoNode * addTranscInfo(transcNode * transc);
};

class BitcoinTreeList{
public:
    //data
	int size;
	bitcoinTreeNode * first;
    //constuctor destructor
	BitcoinTreeList(){
		this->size = 0;
		this->first = NULL;
	};
	~BitcoinTreeList(){
        bitcoinTreeNode * temp = this->first;
        bitcoinTreeNode * temp1;
        for(int i=0; i<this->size; i++){
            temp1 = temp;
            temp = temp->next;
            this->deleteTree(temp1->bitcoinTree);
            delete temp1;
        }
    };
    //member Functions
	int isEmpty();

    void deleteTree(bitcoinNode * node);

	bitcoinNode * addBitTree(int bitcoinId, int walletId, int val, struct TranscactionNode * transc);

	bitcoinTreeNode * findBitTree(int bitcoinId);

    int height(bitcoinNode * node);

    void findUserAtLevel(bitcoinNode * node, int level, int walletId, BitNodeList &list);

    int findTranscNum(bitcoinNode * node);

    void findTranscAtLevel(bitcoinNode * node, int level, TranscInfoList &list);

    int findNotSpent(bitcoinNode * node);
    
    int findBitTreeHeight(int bitcoinId);

    //Debug staff
    void printBfs(bitcoinNode * root);

    void printAtLevel(bitcoinNode * root, int level);
};