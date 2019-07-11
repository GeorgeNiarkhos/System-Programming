#ifndef DATA_STRUCTS_H
#define DATA_STRUCTS_H
#include "dataStructs.hpp"
#endif

#include "hash.hpp"


#define DATESIZE 10
#define TIMESIZE 10
#define BUFFER 50

int hashFunction(int walletId, int val);

void execTranscaction(transcNode * transc, BitcoinTreeList &bitcoins);

uint64_t convertDateToSec(int mins, int hours, int days, int months, int years);

void printUserInfo(UserList & users);

void printTransInfo(TranscList & transcactions, BitcoinTreeList & bitcoins);

void requestTransaction(UserList & users, int& minsM, int& hoursM, int& dayM, int& monthM, int& yearM, int& maxTranscId, TranscList& transcactions, BitcoinTreeList& bitcoins, HashList* senderHashTable, HashList* receiverHashTable, int sendVal, int recVal, int bucketSize);

void requestTransactions(char * line, UserList & users, int& minsM, int& hoursM, int& dayM, int& monthM, int& yearM, int& maxTranscId, TranscList& transcactions, BitcoinTreeList& bitcoins, HashList* senderHashTable, HashList* receiverHashTable, int sendVal, int recVal, int bucketSize);

void findEarnings(UserList & users, HashList* receiverHashTable, int recVal);

void findPayments(UserList & users, HashList* senderHashTable, int sendVal);

void walletStatus(UserList & users);

void bitcoinStatus(BitcoinTreeList& bitcoins);

void traceCoin(BitcoinTreeList& bitcoins);