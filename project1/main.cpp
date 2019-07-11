#include <ctime>
#include "functions.hpp"


#define EXIT 0

using namespace std;


int main(int argc, char *argv[]){
    //DEFINE ARGUMENTS
    if (argc%2 == 0){
        cerr << "Wrong argument input..." << endl;
        return ERROR;
    }
    char *argument1=NULL, *argument2=NULL, *a=NULL, *t=NULL, *v=NULL, *h1=NULL, *h2=NULL, *b=NULL;
    char flag = 0;
    for (int i=1; i<argc; i++){
        if (!strcmp("-a", argv[i])){
            if ((i+1 >= argc) || (argv[i+1][0] == '-') || (a != NULL)){
                cerr << "Wrong argument input..." << endl;
                return ERROR;   
            }
            a=argv[i+1];
            i++;
        }
        else if (!strcmp("-t", argv[i])){
            if ((i+1 >= argc) || (argv[i+1][0] == '-') || (t != NULL)){
                cerr << "Wrong argument input..." << endl;
                return ERROR;   
            }
            t=argv[i+1];
            i++;
        }
        else if (!strcmp("-v", argv[i])){
            if ((i+1 >= argc) || (argv[i+1][0] == '-') || (v != NULL)){
                cerr << "Wrong argument input..." << endl;
                return ERROR;
            }
             v=argv[i+1];
             i++;
        }
        else if (!strcmp("-h1", argv[i])){
            if ((i+1 >= argc) || (argv[i+1][0] == '-') || (h1 != NULL)){
                cerr << "Wrong argument input..." << endl;
                return ERROR;
            }
            h1=argv[i+1];
            i++;
        }
        else if (!strcmp("-h2", argv[i])){
            if ((i+1 >= argc) || (argv[i+1][0] == '-') || (h2 != NULL)){
                cerr << "Wrong argument input..." << endl;
                return ERROR;
            }
            if (argv[i+1][0] == '-'){
                cerr << "Wrong argument input..." << endl;
                return ERROR;
            }
            if (h2 != NULL){
                cerr << "Wrong argument input..." << endl;
                return ERROR;
            }
            h2=argv[i+1];
            i++;
        }
        else if (!strcmp("-b", argv[i])){
            if ((i+1 >= argc) || (argv[i+1][0] == '-') || (b != NULL)){
                cerr << "Wrong argument input..." << endl;
                return ERROR;
            }
            b=argv[i+1];
            i++;
        }
        else {
            cerr << "Wrong argument input..." << endl;
            return ERROR;
        }
    }
    if (a == NULL || t == NULL || v == NULL || h1 == NULL || h2 == NULL || b == NULL){
        cerr << "Wrong argument input..." << endl;
        return ERROR;
    }
    int val = atoi(v);
    int sendVal = atoi(h1);
    int recVal = atoi(h2);
    int bucketSize = atoi(b)/sizeof(hashNode);
    if(bucketSize == 0){
        cerr << "Error: BucketSize too small... min size of bucket " << sizeof(hashNode) << endl;
        return ERROR;
    }
    int maxTranscId = 0;
    int maxYear = 0;
    int maxMonth = 0;
    int maxDay = 0;
    int maxHour = 0;
    int maxMin = 0;
    //open Input Files

    FILE * fp = NULL;
    char * line = NULL;
    const char s[3] = " \n";
    char *token;

    UserList users;
    TranscList transcactions;
    BitcoinTreeList bitcoins;
    userNode * tmpUser;

    HashList senderHashTable[sendVal];
    HashList receiverHashTable[recVal];

    size_t n = 0;
    ssize_t nread;
    
    //parse Balance File
    fp = fopen(a, "r");
    if (fp == NULL) {
      	perror("fopen");
      	return ERROR;
   	}
    while ((nread = getline(&line, &n, fp)) != -1) {
        
        //get the first token
        //get username
        token = strtok(line, s);
        //add user
        tmpUser = users.addUser(token);
        // walk through other tokens
        token = strtok(NULL, s);
        //get bitcoins if there r any

        while( token != NULL ) {
            int id = atoi(token);
        
            //add to users wallet with val
            tmpUser->wallet.addBitInfo(id, val);
            //add it to bitcoinList
            //intitialize lists and trees
            if( bitcoins.addBitTree(id, tmpUser->wallet.walletId, val, NULL) == NULL){
                cerr << "Invalid Balance File!" << endl;
                return ERROR;
            }
            if( bitcoins.findBitTree(id) == NULL){ 
                cerr << "Invalid Balance File!" << endl;
                return ERROR;
            }
            token = strtok(NULL, s);
        }
		
	}
	if (line != NULL){
		free(line);
	}
	fclose(fp);
    cout << "User size is : " << users.size << ", Bitcoins size is : " << bitcoins.size << endl;
    fp = fopen(t, "r");
    if (fp == NULL) {
      	perror("fopen");
      	return ERROR;
   	}
    line = NULL;
    while ((nread = getline(&line, &n, fp)) != -1) {
		int trVal;
        userNode * tmpS, * tmpR;
        transcNode * tmpT;
        char * date, * times, *trId;
        
        //get transcId
        token = strtok(line, s);
        //check for correct format
        if (token == NULL){
            cerr << "Wrong Format in Transcaction File" << endl;
            return ERROR;
        }
        trId = new char[strlen(token) + 1];
        strcpy(trId, token);
        //get sender
        token = strtok(NULL, s);
        //check for correct format
        if (token == NULL){
            cerr << "Wrong Format in Transcaction File" << endl;
            return ERROR;
        }
        if ((tmpS = users.findUserByName(token)) == NULL){
            cerr << "User in Transcaction File doesn't exist!" << endl;
            return ERROR;
        }
        

        //get receiver
        token = strtok(NULL, s);
        //check for correct format
        if (token == NULL){
            cerr << "Wrong Format in Transcaction File" << endl;
            return ERROR;
        }
        if ((tmpR = users.findUserByName(token)) == NULL){
            cerr << "User in Transcaction File doesn't exist!" << endl;
            return ERROR;
        }
        //get val
        token = strtok(NULL, s);
        //check for correct format
        if (token == NULL){
            cerr << "Wrong Format in Transcaction File" << endl;
            return ERROR;
        }
        trVal = atoi(token);
        //get date
        token = strtok(NULL, s);
        //check for correct format
        if (token == NULL){
            cerr << "Wrong Format in Transcaction File" << endl;
            return ERROR;
        }
        date = new char[strlen(token)+ 1];
        strcpy(date, token);
        //get time
        token = strtok(NULL, s);
        //check for correct format
        if (token == NULL){
            cerr << "Wrong Format in Transcaction File" << endl;
            return ERROR;
        }
        times = new char[strlen(token)+ 1];
        strcpy(times, token);

        //add transcaction
        //if not enough money or same user transcaction
        if ((tmpS->wallet.val - trVal < 0) || (tmpS->wallet.walletId == tmpR->wallet.walletId)){
            //ignore
            cerr << "Invalid transcaction " << trId << endl;
            free(trId);
            free(date);
            free(times);
            continue;
        }
        if ((tmpT = transcactions.addTranscNode(trId, tmpS, tmpR, trVal, date, times)) == NULL){
            cerr << "Invalid Transcaction File -- Same transcaction id" << endl;
            return ERROR;
        }
        //execute transcaction
        execTranscaction(tmpT, bitcoins);
        //update hashTables
        senderHashTable[hashFunction(tmpS->wallet.walletId, sendVal)].addData(bucketSize, tmpS, tmpT);    
        receiverHashTable[hashFunction(tmpR->wallet.walletId, recVal)].addData(bucketSize, tmpR, tmpT);
        if (maxTranscId < atoi(trId)){
            maxTranscId = atoi(trId);
        }
        free(trId);
        free(date);
        free(times);
    }

    if(line != NULL){
        free(line);
    }
    fclose(fp);
    
    int dayM = 0;
    int monthM = 0;
    int yearM = 0;
    int hoursM = 0;
    int minsM = 0;
    const char del[4] = "-/\n";
    const char del2 [3] = ":\n";
    if (transcactions.size == 0){
        cout << "No Transactions aquired from TransFile" << endl;
    }
    else{
        cout << "Transaction File read successfully with " << transcactions.size << " Transactions"<< endl;
        //get last date and time
        transcNode * last = transcactions.getLast();
        
        char * maxDate = new char[strlen(last->date)+1];
        strcpy(maxDate, last->date);
        token = strtok(maxDate, del);
        dayM = atoi(token);

        token = strtok(NULL, del);
        monthM = atoi(token);

        token = strtok(NULL, del);
        yearM = atoi(token);

        
        char * maxTime = new char[strlen(last->time)+1];
        strcpy(maxTime, last->time);

        token = strtok(maxTime, del2);
        hoursM = atoi(token);

        token = strtok(NULL, del2);
        minsM = atoi(token);

        delete maxDate;
        delete maxTime;
    }
    
    
    //read cmds from keyboard
    line = NULL;
    cout << endl << "Insert a command: " << endl;
    while ((nread = getline(&line, &n, stdin)) != -1){
        
        char * tmpLine = new char[strlen(line) + 1];
        strcpy(tmpLine,line);
        token = strtok(tmpLine, s);
        if (!strcmp(token, "/requestTransaction")){
            requestTransaction(users, minsM, hoursM, dayM, monthM, yearM, maxTranscId, transcactions, bitcoins, senderHashTable, receiverHashTable, sendVal, recVal, bucketSize);
        }
        else if(!strcmp(token, "/requestTransactions")){
            requestTransactions(line, users, minsM, hoursM, dayM, monthM, yearM, maxTranscId, transcactions, bitcoins, senderHashTable, receiverHashTable, sendVal, recVal, bucketSize);
        }
        else if(!strcmp(token, "/findEarnings")){
        	findEarnings(users, receiverHashTable, recVal);
        }
        else if(!strcmp(token, "/findPayments")){
        	findPayments(users, senderHashTable, sendVal);
        }
        else if(!strcmp(token, "/walletStatus")){
            walletStatus(users);
        }
        else if(!strcmp(token, "/bitCoinStatus")){
            bitcoinStatus(bitcoins);
        }
        else if(!strcmp(token, "/traceCoin")){
            traceCoin(bitcoins);
        }
        else if(!strcmp(token, "/exit")){
            delete tmpLine;
            break;
        }
        else{
            cerr << "Command not found!" << endl;
        }
        delete tmpLine;
        cout << endl << "Insert a command: " << endl;  
    }
    if(line != NULL){
        free(line);
    }

    return EXIT;
}
