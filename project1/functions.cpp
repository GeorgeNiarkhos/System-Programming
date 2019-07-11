#include "functions.hpp"

int hashFunction(int walletId, int val){
    return walletId % val;
}

void execTranscaction(transcNode * transc, BitcoinTreeList &bitcoins){
    int val = transc->val;
    int height;
    bitcoinTreeNode * bitTree;

    //exec transc
    //go through sender bitcoinList
    
    bitTree = bitcoins.findBitTree(transc->sender->wallet.bitcoins.first->bitcoinId);
    //if first bitcoin amount is sufficient
    if (transc->sender->wallet.bitcoins.first->val >= val){
        
        height = bitcoins.findBitTreeHeight(transc->sender->wallet.bitcoins.first->bitcoinId);
        //bfs in first bitcoin
        BitNodeList list ;
        for (int i=1; i<=height; i++){
            bitcoins.findUserAtLevel(bitTree->bitcoinTree, i, transc->sender->wallet.walletId, list);
        }
        bitcoinNodeList * temp = list.first;
        while((temp != NULL) && (val != 0)){
            //if node val is sufficient
            if(temp->node->val > val){
                //right --reciever
                temp->node->right = new bitcoinNode;
                temp->node->right->walletId = transc->receiver->wallet.walletId;
                temp->node->right->val = val;
                temp->node->right->transc = transc;
                temp->node->right->left = NULL;
                temp->node->right->right = NULL;
                //left --leftovers
                temp->node->left = new bitcoinNode;
                temp->node->left->walletId = transc->sender->wallet.walletId;
                temp->node->left->val = temp->node->val - val;
                temp->node->left->transc = NULL; //no transcaction made in this node's creation
                temp->node->left->left = NULL;
                temp->node->left->right = NULL;    
                val = 0;
            }
            else{
                //only one leaf with previous node amount
                temp->node->right = new bitcoinNode;
                temp->node->right->walletId = transc->receiver->wallet.walletId;
                temp->node->right->val = temp->node->val;
                temp->node->right->transc = transc;
                temp->node->right->left = NULL;
                temp->node->right->right = NULL;
                val -= temp->node->val;
            }
            temp = temp->next;
        }
        transc->sender->wallet.addBitInfo(transc->sender->wallet.bitcoins.first->bitcoinId, -1 * transc->val);
        transc->receiver->wallet.addBitInfo(transc->sender->wallet.bitcoins.first->bitcoinId, transc->val);
    }
    else {
        int oldVal = val;
        //get first bitcoin info and tree
        bitcoinInfo * tmpInfo = transc->sender->wallet.bitcoins.first;
        while((val != 0) && (tmpInfo != NULL)){
            //if bitcoinVal is sufficient
            if (tmpInfo->val == 0){
                tmpInfo = tmpInfo->next;
                bitTree = bitcoins.findBitTree(tmpInfo->bitcoinId);
                continue;
            }
            //go to next bitcoin until transc is completed
            BitNodeList list ;
            height = bitcoins.findBitTreeHeight(tmpInfo->bitcoinId);
            for (int i=1; i<=height; i++){
                bitcoins.findUserAtLevel(bitTree->bitcoinTree, i, transc->sender->wallet.walletId, list);
            }
            bitcoinNodeList * temp = list.first;    
            while((temp != NULL) && (val != 0)){
	            //if node val is sufficient
	            if(temp->node->val > val){
	                //right --reciever
	                temp->node->right = new bitcoinNode;
	                temp->node->right->walletId = transc->receiver->wallet.walletId;
	                temp->node->right->val = val;
	                temp->node->right->transc = transc;
	                temp->node->right->left = NULL;
	                temp->node->right->right = NULL;
	                //left --leftovers
	                temp->node->left = new bitcoinNode;
	                temp->node->left->walletId = transc->sender->wallet.walletId;
	                temp->node->left->val = temp->node->val - val;
	                temp->node->left->transc = NULL; //no transcaction made in this node's creation
	                temp->node->left->left = NULL;
	                temp->node->left->right = NULL;    
	                val = 0;
	            }
	            else{
	                //only one leaf with previous node amount
	                temp->node->right = new bitcoinNode;
	                temp->node->right->walletId = transc->receiver->wallet.walletId;
	                temp->node->right->val = temp->node->val;
	                temp->node->right->transc = transc;
	                temp->node->right->left = NULL;
	                temp->node->right->right = NULL;
	                val -= temp->node->val;
	            }
            	temp = temp->next;
        	}
            transc->receiver->wallet.addBitInfo(tmpInfo->bitcoinId, oldVal - val);
            transc->sender->wallet.addBitInfo(tmpInfo->bitcoinId, val - oldVal);  
        	oldVal = val;
        	tmpInfo = tmpInfo->next;
            if (tmpInfo == NULL){
                break;
            }
            bitTree = bitcoins.findBitTree(tmpInfo->bitcoinId);
        }
    }
    
}

uint64_t convertDateToSec(int mins, int hours, int days, int months, int years){
	uint64_t secs;
	secs = mins*60 + hours*3600 + days*86400 + months*2592000 + 31104000*years;
    return secs; 
}

void printUserInfo(UserList & users){
    userNode * tmpUser = users.first;
    for (int i=0; i<users.size; i++){
        cout << tmpUser->name << " : " << tmpUser->wallet.val << " --- " ;
        bitcoinInfo * tmpB = tmpUser->wallet.bitcoins.first;
        for(int j=0; j<tmpUser->wallet.bitcoins.size; j++){
            cout << tmpB->bitcoinId << "/" << tmpB->val << " ";
            tmpB = tmpB->next;
        }
        cout << endl;
        tmpUser = tmpUser->next;
    }
    return;
}

void printTransInfo(TranscList & transcactions, BitcoinTreeList & bitcoins){
    transcNode * tmpT = transcactions.first;
    for (int i=0; i<transcactions.size; i++){
        cout << tmpT->transcId << " " << tmpT->sender->name << " " << tmpT->receiver->name << " " << tmpT->val << " " << tmpT->date << " " << tmpT->time << " " << endl;
        tmpT = tmpT->next;
    }
    cout << "BitCoinTrees: " << endl;
    bitcoinTreeNode * tempL = bitcoins.first;
    for (int i=0; i<bitcoins.size; i++){
        cout << endl;
        cout << tempL->bitcoinId << "/" << tempL->bitcoinTree->walletId << "/" << tempL->bitcoinTree->val << endl;
        bitcoins.printBfs(tempL->bitcoinTree);
        tempL = tempL->next;
    }
    return;
}

void requestTransaction(UserList & users, int& minsM, int& hoursM, int& dayM, int& monthM, int& yearM, int& maxTranscId, TranscList& transcactions, BitcoinTreeList& bitcoins, HashList* senderHashTable, HashList* receiverHashTable, int sendVal, int recVal, int bucketSize){
    const char del[4] = "-/\n";
    const char del2 [3] = ":\n";
    const char s[3] = " \n";
    //get sender
    char * token;
    token = strtok(NULL, s);
    if (token == NULL){
        cerr << "Not enough arguments in cmd" << endl;
        return;
    }
    char * senderId = new char[strlen(token) + 1];
    strcpy(senderId, token);
    userNode * senderT = users.findUserByName(senderId);
    if (senderT == NULL){
        cerr << "Sender user doesn't exist! Invalid Transaction" << endl;
        delete senderId;
        return;
    }
    //get receiver
    token = strtok(NULL, s);
    if (token == NULL){
        cerr << "Not enough arguments in cmd" << endl;
        delete senderId;
        return;
    }
    char * receiverId = new char[strlen(token) + 1];
    strcpy(receiverId, token);
    userNode * receiverT = users.findUserByName(receiverId);
    if (receiverT == NULL){
        cerr << "Receiver user doesn't exist! Invalid Transaction" << endl;
        delete senderId;
        delete receiverId;
        return;
    }
    //get val
    token = strtok(NULL, s);
    if (token == NULL){
        cerr << "Not enough arguments in cmd" << endl;
        delete senderId;
        delete receiverId;
        return;
    }
    int amount = atoi(token);
    //check for date and time
    token = strtok(NULL, s);
    //check for suficient amount
    if (senderT->wallet.val < amount){
        cerr << "Not enough sender's wallet amount! Invalid Transaction" << endl;
        delete senderId;
        delete receiverId;
        return;
    }
    if (senderT->wallet.walletId == receiverT->wallet.walletId){
        cerr << "Same User! Invalid Transaction" << endl;
        delete senderId;
        delete receiverId;
        return;
    }
    if (token == NULL){
        //no date and time
        time_t now = time(0);
        tm *ltm = localtime(&now);
        char * date = new char[DATESIZE + 1];
        char * tim = new char[TIMESIZE + 1];
        minsM = ltm->tm_min;
        hoursM = ltm->tm_hour;
        dayM = ltm->tm_mday;
        monthM = 1 + ltm->tm_mon;
        yearM = 1900 + ltm->tm_year;
        sprintf(date, "%d-%d-%d", dayM, monthM, yearM);
        sprintf(tim, "%d:%d", hoursM, minsM);

        char * newtrId = new char[BUFFER];
        maxTranscId++;
        sprintf(newtrId, "%d", maxTranscId);
        
        transcNode * tempTran;
        tempTran = transcactions.addTranscNode(newtrId, senderT, receiverT, amount, date, tim);
        execTranscaction(tempTran, bitcoins);

        senderHashTable[hashFunction(senderT->wallet.walletId, sendVal)].addData(bucketSize, senderT, tempTran);    
        receiverHashTable[hashFunction(receiverT->wallet.walletId, recVal)].addData(bucketSize, receiverT, tempTran);
        
        cout << "Transaction with new id " << newtrId << " executed successfully!" << endl;

        delete date;
        delete tim;
        delete senderId;
        delete receiverId;
        delete newtrId;

    }
    else{
        //get date
        char * date = new char[strlen(token) + 1];
        strcpy(date, token);
        //check for time
        token = strtok(NULL, s);
        if (token == NULL){
            cerr << "Not enough arguments in cmd" << endl;
            delete senderId;
            delete receiverId;
            delete date;
            return;
        }
        char * tim = new char[strlen(token) + 1];
        strcpy(tim, token);
        //check for valid date and time
        //compute date
        char * dateT = new char[strlen(date) + 1];
        strcpy(dateT, date);
        token = strtok(dateT, del);
        int day = atoi(token);
        token = strtok(NULL, del);
        int month = atoi(token);
        token = strtok(NULL, del);
        int year = atoi(token);
        //compute time
        char * timT = new char[strlen(tim) + 1];
        strcpy(timT, tim);
        token = strtok(timT, del2);
        int hour = atoi(token);
        token = strtok(NULL, del2);
        int min = atoi(token);
        if (convertDateToSec(minsM, hoursM, dayM, monthM, yearM) <= convertDateToSec(min, hour, day, month, year) ){
            //add to transcaction list
            char * newtrId = new char[BUFFER];
            maxTranscId++;
            sprintf(newtrId, "%d", maxTranscId);
            
            transcNode * tempTran;
            tempTran = transcactions.addTranscNode(newtrId, senderT, receiverT, amount, date, tim);
            execTranscaction(tempTran, bitcoins);

            senderHashTable[hashFunction(senderT->wallet.walletId, sendVal)].addData(bucketSize, senderT, tempTran);    
            receiverHashTable[hashFunction(receiverT->wallet.walletId, recVal)].addData(bucketSize, receiverT, tempTran);
            
            cout << "Transaction with new id " << newtrId << " executed successfully!" << endl; 
            minsM = min;
            hoursM = hour;
            dayM = day;
            monthM = month;
            yearM = year;
            delete date;
            delete tim;
            delete timT;
            delete dateT;
            delete senderId;
            delete receiverId;
            delete newtrId;               
        }
        else{
            //skip
            cerr << "Transaction not valid by date and time" << endl;
            delete senderId;
            delete receiverId;
            delete date;
            delete tim;
            delete timT;
            delete dateT;
            return;
        }
    }
    return;
}

void requestTransactions(char * line, UserList & users, int& minsM, int& hoursM, int& dayM, int& monthM, int& yearM, int& maxTranscId, TranscList& transcactions, BitcoinTreeList& bitcoins, HashList* senderHashTable, HashList* receiverHashTable, int sendVal, int recVal, int bucketSize){
    const char del[4] = "-/\n";
    const char del2 [3] = ":\n";
    const char s[3] = " \n";
    char * token;
    FILE *fp = NULL;
    token = strtok(NULL, s);
    if (token == NULL){
        cerr << "Not enough arguments in cmd" << endl;
        return;
    }

    //if semicol on input "last" char -- Hard code way
    if( line[strlen(line)-2] == ';'){
        size_t n1 = 0;
        ssize_t nread1 = 0;
        char * line1 = NULL;
        char s1[5];
        s1[0] = ' ';
        s1[1] = '\n';
        s1[2] = ';';
        s1[3] = 13;
        s1[4] = '\0';
        //need to parse tmpline
        //get sender
        char * senderId = new char[strlen(token) + 1];
        strcpy(senderId, token);
        userNode * senderT = users.findUserByName(senderId);
        if (senderT == NULL){
            cerr << "Sender user doesn't exist! Invalid Transaction" << endl;
            delete senderId;
            return;
        }
        //get receiver
        token = strtok(NULL, s);
        if (token == NULL){
            cerr << "Not enough arguments in cmd" << endl;
            delete senderId;
            return;
        }
        char * receiverId = new char[strlen(token) + 1];
        strcpy(receiverId, token);
        userNode * receiverT = users.findUserByName(receiverId);
        if (receiverT == NULL){
            cerr << "Receiver user  " << token << " doesn't exist! Invalid Transaction" << endl;
            delete senderId;
            delete receiverId;
            return;
        }
        //get val
        token = strtok(NULL, s);
        if (token == NULL){
            cerr << "Not enough arguments in cmd" << endl;
            delete senderId;
            delete receiverId;
            return;
        }
        int amount = atoi(token);
        //check for date and time
        token = strtok(NULL, s);
        //check for suficient amount
        if (senderT->wallet.val < amount){
            cerr << "Not enough sender's wallet amount! Invalid Transaction" << endl;
            delete senderId;
            delete receiverId;
            return;
        }
        if (senderT->wallet.walletId == receiverT->wallet.walletId){
            cerr << "Same User! Invalid Transaction" << endl;
            delete senderId;
            delete receiverId;
            return;
        }
        if (token == NULL){
            //no date and time
            time_t now = time(0);
            tm *ltm = localtime(&now);
            char * date = new char[DATESIZE + 1];
            char * tim = new char[TIMESIZE + 1];
            minsM = ltm->tm_min;
            hoursM = ltm->tm_hour;
            dayM = ltm->tm_mday;
            monthM = 1 + ltm->tm_mon;
            yearM = 1900 + ltm->tm_year;
            sprintf(date, "%d-%d-%d", dayM, monthM, yearM);
            sprintf(tim, "%d:%d", hoursM, minsM);

            char * newtrId = new char[BUFFER];
            maxTranscId++;
            sprintf(newtrId, "%d", maxTranscId);
            
            transcNode * tempTran;
            tempTran = transcactions.addTranscNode(newtrId, senderT, receiverT, amount, date, tim);
            execTranscaction(tempTran, bitcoins);

            senderHashTable[hashFunction(senderT->wallet.walletId, sendVal)].addData(bucketSize, senderT, tempTran);    
            receiverHashTable[hashFunction(receiverT->wallet.walletId, recVal)].addData(bucketSize, receiverT, tempTran);
            
            cout << "Transaction with new id " << newtrId << " executed successfully!" << endl;

            delete date;
            delete tim;
            delete senderId;
            delete receiverId;
            delete newtrId;

        }
        else{
            //get date
            char * date = new char[strlen(token) + 1];
            strcpy(date, token);
            //check for time
            token = strtok(NULL, s);
            if (token == NULL){
                cerr << "Not enough arguments in cmd" << endl;
                delete senderId;
                delete receiverId;
                delete date;
                return;
            }
            char * tim = new char[strlen(token) + 1];
            strcpy(tim, token);
            //check for valid date and time
            //compute date
            char * dateT = new char[strlen(date) + 1];
            strcpy(dateT, date);
            token = strtok(dateT, del);
            int day = atoi(token);
            token = strtok(NULL, del);
            int month = atoi(token);
            token = strtok(NULL, del);
            int year = atoi(token);
            //compute time
            char * timT = new char[strlen(tim) + 1];
            strcpy(timT, tim);
            token = strtok(timT, del2);
            int hour = atoi(token);
            token = strtok(NULL, del2);
            int min = atoi(token);
            if (convertDateToSec(minsM, hoursM, dayM, monthM, yearM) <= convertDateToSec(min, hour, day, month, year) ){
                //add to transcaction list
                char * newtrId = new char[BUFFER];
                maxTranscId++;
                sprintf(newtrId, "%d", maxTranscId);
                
                transcNode * tempTran;
                tempTran = transcactions.addTranscNode(newtrId, senderT, receiverT, amount, date, tim);
                execTranscaction(tempTran, bitcoins);

                senderHashTable[hashFunction(senderT->wallet.walletId, sendVal)].addData(bucketSize, senderT, tempTran);    
                receiverHashTable[hashFunction(receiverT->wallet.walletId, recVal)].addData(bucketSize, receiverT, tempTran);
                
                cout << "Transaction with new id " << newtrId << " executed successfully!" << endl; 
                minsM = min;
                hoursM = hour;
                dayM = day;
                monthM = month;
                yearM = year;
                delete date;
                delete tim;
                delete timT;
                delete dateT;
                delete senderId;
                delete receiverId;
                delete newtrId;               
            }
            else{
                //skip
                cerr << "Transaction not valid by date and time" << endl;
                delete senderId;
                delete receiverId;
                delete date;
                delete tim;
                delete timT;
                delete dateT;
                return;
            }
        }
        while ((nread1 = getline(&line1, &n1, stdin)) != -1){
            //get sender
            token = strtok(line1, s1);
            if (token == NULL){
                cerr << "Not enough arguments in request" << endl;
                continue;
            }
            char * senderId = new char[strlen(token) + 1];
            strcpy(senderId, token);
            userNode * senderT = users.findUserByName(senderId);
            if (senderT == NULL){
                cerr << "Sender user doesn't exist! Invalid Transaction" << endl;
                delete senderId;
                continue;
            }
            //get receiver
            token = strtok(NULL, s1);
            if (token == NULL){
                cerr << "Not enough arguments in request" << endl;
                delete senderId;
                continue;
            }
            char * receiverId = new char[strlen(token) + 1];
            strcpy(receiverId, token);
            userNode * receiverT = users.findUserByName(receiverId);
            if (receiverT == NULL){
                cerr << "Receiver user doesn't exist! Invalid Transaction" << endl;
                delete senderId;
                delete receiverId;
                continue;
            }
            //get val
            token = strtok(NULL, s1);
            if (token == NULL){
                cerr << "Not enough arguments in request" << endl;
                delete senderId;
                delete receiverId;
                continue;
            }
            int amount = atoi(token);
            //check for date and time
            token = strtok(NULL, s1);
            //check for suficient amount
            if (senderT->wallet.val < amount){
                cerr << "Not enough sender's wallet amount! Invalid Transaction" << endl;
                delete senderId;
                delete receiverId;
                continue;
            }
            if (senderT->wallet.walletId == receiverT->wallet.walletId){
                cerr << "Same User! Invalid Transaction" << endl;
                delete senderId;
                delete receiverId;
                continue;
            }
            if (token == NULL){
                //no date and time
                time_t now = time(0);
                tm *ltm = localtime(&now);
                char * date = new char[DATESIZE + 1];
                char * tim = new char[TIMESIZE + 1];
                minsM = ltm->tm_min;
                hoursM = ltm->tm_hour;
                dayM = ltm->tm_mday;
                monthM = 1 + ltm->tm_mon;
                yearM = 1900 + ltm->tm_year;
                sprintf(date, "%d-%d-%d", dayM, monthM, yearM);
                sprintf(tim, "%d:%d", hoursM, minsM);

                char * newtrId = new char[BUFFER];
                maxTranscId++;
                sprintf(newtrId, "%d", maxTranscId);
                
                transcNode * tempTran;
                tempTran = transcactions.addTranscNode(newtrId, senderT, receiverT, amount, date, tim);
                execTranscaction(tempTran, bitcoins);

                senderHashTable[hashFunction(senderT->wallet.walletId, sendVal)].addData(bucketSize, senderT, tempTran);    
                receiverHashTable[hashFunction(receiverT->wallet.walletId, recVal)].addData(bucketSize, receiverT, tempTran);               
                cout << "Transaction with new id " << newtrId << " executed successfully!" << endl;
                delete date;
                delete tim;
                delete senderId;
                delete receiverId;
                delete newtrId;

            }
            else{
                //get date
                char * date = new char[strlen(token) + 1];
                strcpy(date, token);
                //check for time
                token = strtok(NULL, s1);
                if (token == NULL){
                    cerr << "Not enough arguments in request" << endl;
                    delete senderId;
                    delete receiverId;
                    delete date;
                    continue;
                }
                char * tim = new char[strlen(token) + 1];
                strcpy(tim, token);
                //check for valid date and time
                //compute date
                char * dateT = new char[strlen(date) + 1];
                strcpy(dateT, date);
                token = strtok(dateT, del);
                int day = atoi(token);
                token = strtok(NULL, del);
                int month = atoi(token);
                token = strtok(NULL, del);
                int year = atoi(token);
                //compute time
                char * timT = new char[strlen(tim) + 1];
                strcpy(timT, tim);
                token = strtok(timT, del2);
                int hour = atoi(token);
                token = strtok(NULL, del2);
                int min = atoi(token);
                if (convertDateToSec(minsM, hoursM, dayM, monthM, yearM) <= convertDateToSec(min, hour, day, month, year) ){
                    //add to transcaction list
                    char * newtrId = new char[BUFFER];
                    maxTranscId++;
                    sprintf(newtrId, "%d", maxTranscId);
                    
                    
                    transcNode * tempTran;
                    tempTran = transcactions.addTranscNode(newtrId, senderT, receiverT, amount, date, tim);
                    execTranscaction(tempTran, bitcoins);

                    senderHashTable[hashFunction(senderT->wallet.walletId, sendVal)].addData(bucketSize, senderT, tempTran);    
                    receiverHashTable[hashFunction(receiverT->wallet.walletId, recVal)].addData(bucketSize, receiverT, tempTran);
                    
                    cout << "Transaction with new id " << newtrId << " executed successfully!" << endl; 
                    minsM = min;
                    hoursM = hour;
                    dayM = day;
                    monthM = month;
                    yearM = year;
                    delete date;
                    delete tim;
                    delete timT;
                    delete dateT;
                    delete senderId;
                    delete receiverId;
                    delete newtrId;               
                }
                else{
                    //skip
                    cerr << "Transaction not valid by date and time" << endl;
                    delete senderId;
                    delete receiverId;
                    delete date;
                    delete tim;
                    delete timT;
                    delete dateT;
                    continue;
                }
            }
        }
        if (line1 != NULL){
            free(line1);
        }
    }
    else {
        //input file
        fp = fopen(token, "r");
        if (fp == NULL) {
            perror("fopen");
            return;
        }
        size_t n1 = 0;
        ssize_t nread1 = 0;
        char * line1 = NULL;
        char s1[5];
        s1[0] = ' ';
        s1[1] = '\n';
        s1[2] = ';';
        s1[3] = 13;
        s1[4] = '\0';
        while ((nread1 = getline(&line1, &n1, fp)) != -1){
            //get sender
            token = strtok(line1, s1);
            if (token == NULL){
                cerr << "Not enough arguments in request" << endl;
                continue;
            }
            char * senderId = new char[strlen(token) + 1];
            strcpy(senderId, token);
            userNode * senderT = users.findUserByName(senderId);
            if (senderT == NULL){
                cerr << "Sender user doesn't exist! Invalid Transaction" << endl;
                delete senderId;
                continue;
            }
            //get receiver
            token = strtok(NULL, s1);
            if (token == NULL){
                cerr << "Not enough arguments in request" << endl;
                delete senderId;
                continue;
            }
            char * receiverId = new char[strlen(token) + 1];
            strcpy(receiverId, token);
            userNode * receiverT = users.findUserByName(receiverId);
            if (receiverT == NULL){
                cerr << "Receiver user doesn't exist! Invalid Transaction" << endl;
                delete senderId;
                delete receiverId;
                continue;
            }
            //get val
            token = strtok(NULL, s1);
            if (token == NULL){
                cerr << "Not enough arguments in request" << endl;
                delete senderId;
                delete receiverId;
                continue;
            }
            int amount = atoi(token);
            //check for date and time
            token = strtok(NULL, s1);
            //check for suficient amount
            if (senderT->wallet.val < amount){
                cerr << "Not enough sender's wallet amount! Invalid Transaction" << endl;
                delete senderId;
                delete receiverId;
                continue;
            }
            if (senderT->wallet.walletId == receiverT->wallet.walletId){
                cerr << "Same User! Invalid Transaction" << endl;
                delete senderId;
                delete receiverId;
                continue;
            }
            if (token == NULL){
                //no date and time
                time_t now = time(0);
                tm *ltm = localtime(&now);
                char * date = new char[DATESIZE + 1];
                char * tim = new char[TIMESIZE + 1];
                minsM = ltm->tm_min;
                hoursM = ltm->tm_hour;
                dayM = ltm->tm_mday;
                monthM = 1 + ltm->tm_mon;
                yearM = 1900 + ltm->tm_year;
                sprintf(date, "%d-%d-%d", dayM, monthM, yearM);
                sprintf(tim, "%d:%d", hoursM, minsM);

                char * newtrId = new char[BUFFER];
                maxTranscId++;
                sprintf(newtrId, "%d", maxTranscId);
                
                transcNode * tempTran;
                tempTran = transcactions.addTranscNode(newtrId, senderT, receiverT, amount, date, tim);
                execTranscaction(tempTran, bitcoins);

                senderHashTable[hashFunction(senderT->wallet.walletId, sendVal)].addData(bucketSize, senderT, tempTran);    
                receiverHashTable[hashFunction(receiverT->wallet.walletId, recVal)].addData(bucketSize, receiverT, tempTran);
                
                cout << "Transaction with new id " << newtrId << " executed successfully!" << endl;

                delete date;
                delete tim;
                delete senderId;
                delete receiverId;
                delete newtrId;

            }
            else{
                //get date
                char * date = new char[strlen(token) + 1];
                strcpy(date, token);
                //check for time
                token = strtok(NULL, s1);
                if (token == NULL){
                    cerr << "Not enough arguments in request123" << endl;
                    delete senderId;
                    delete receiverId;
                    delete date;
                    continue;
                }
                char * tim = new char[strlen(token) + 1];
                strcpy(tim, token);
                //check for valid date and time
                //compute date
                char * dateT = new char[strlen(date) + 1];
                strcpy(dateT, date);
                token = strtok(dateT, del);
                int day = atoi(token);
                token = strtok(NULL, del);
                int month = atoi(token);
                token = strtok(NULL, del);
                int year = atoi(token);
                //compute time
                char * timT = new char[strlen(tim) + 1];
                strcpy(timT, tim);
                token = strtok(timT, del2);
                int hour = atoi(token);
                token = strtok(NULL, del2);
                int min = atoi(token);
                if (convertDateToSec(minsM, hoursM, dayM, monthM, yearM) <= convertDateToSec(min, hour, day, month, year) ){
                    //add to transcaction list
                    char * newtrId = new char[BUFFER];
                    maxTranscId++;
                    sprintf(newtrId, "%d", maxTranscId);
                    
                    
                    transcNode * tempTran;
                    tempTran = transcactions.addTranscNode(newtrId, senderT, receiverT, amount, date, tim);
                    execTranscaction(tempTran, bitcoins);

                    senderHashTable[hashFunction(senderT->wallet.walletId, sendVal)].addData(bucketSize, senderT, tempTran);    
                    receiverHashTable[hashFunction(receiverT->wallet.walletId, recVal)].addData(bucketSize, receiverT, tempTran);
                    
                    cout << "Transaction with new id " << newtrId << " executed successfully!" << endl; 
                    minsM = min;
                    hoursM = hour;
                    dayM = day;
                    monthM = month;
                    yearM = year;
                    delete date;
                    delete tim;
                    delete timT;
                    delete dateT;
                    delete senderId;
                    delete receiverId;
                    delete newtrId;               
                }
                else{
                    //skip
                    cerr << "Transaction not valid by date and time" << endl;
                    delete senderId;
                    delete receiverId;
                    delete date;
                    delete tim;
                    delete timT;
                    delete dateT;
                    continue;
                }
            }   
        }
        if (line1 != NULL){
            free(line1);
        }
        fclose(fp);
    }
}

void findEarnings(UserList & users, HashList* receiverHashTable, int recVal){
    const char del[4] = "-/\n";
    const char del2 [3] = ":\n";
    const char s[3] = " \n";
    char * token;
    token = strtok(NULL, s);
    if (token == NULL){
        cerr << "Not enough arguments in cmd" << endl;
        return;
    }
    userNode * tmpU = users.findUserByName(token);
    if (tmpU == NULL){
        cerr << "User " << token << " doesn't exist!" << endl;
        return;
    }
    int wid = tmpU->wallet.walletId;
    hashNode * tmpH = receiverHashTable[hashFunction(wid, recVal)].findUser(wid);
    if (tmpH == NULL){
        cout << "User has no earnings" << endl;
    }
    else{
        transcInfoNode * tmpTi = tmpH->list.first;
        token = strtok(NULL, s);
        if (token == NULL){
            //print all transc
            int sum = 0;
            for (int i=0; i<tmpH->list.size; i++){
                cout << tmpTi->transc->transcId << " " << tmpTi->transc->sender->name << " " << tmpTi->transc->receiver->name << " " << tmpTi->transc->val << " " << tmpTi->transc->date << " " << tmpTi->transc->time << endl;
                sum += tmpTi->transc->val;
                tmpTi = tmpTi->next;
            }
            if (sum == 0){
                cout << "User has no earnings" << endl;
            }
            else{
                cout << "Total earnings amount is: " << sum << endl;
            }
        }
        else{
            //print transc > date or time
            //if first arg is date
            if (strlen(token) > 5){
                //get date1
                char * date1 = new char[strlen(token) + 1];
                strcpy(date1, token); 
                //get next date
                token = strtok(NULL, s);
                if (token == NULL){
                    cerr << "Not enough arguments in cmd" << endl;
                    delete date1;
                    return;
                }
                char * date2 = new char[strlen(token) + 1];
                strcpy(date2, token);
                //print all transc between
                //compute date1
                token = strtok(date1, del);
                int day1 = atoi(token);
                token = strtok(NULL, del);
                int month1 = atoi(token);
                token = strtok(NULL, del);
                int year1 = atoi(token);
                //compute date2
                token = strtok(date2, del);
                int day2 = atoi(token);
                token = strtok(NULL, del);
                int month2 = atoi(token);
                token = strtok(NULL, del);
                int year2 = atoi(token);
                
                int sum = 0;
                for (int i=0; i<tmpH->list.size; i++){
                    //compute date
                    char * date = new char[strlen(tmpTi->transc->date) + 1];
                    strcpy(date, tmpTi->transc->date);
                    token = strtok(date, del);
                    int day = atoi(token);
                    token = strtok(NULL, del);
                    int month = atoi(token);
                    token = strtok(NULL, del);
                    int year = atoi(token);
                    if ((convertDateToSec(0, 0, day1, month1, year1) <= convertDateToSec(0, 0, day, month, year)) && (convertDateToSec(0, 0, day, month, year) <= convertDateToSec(0, 0, day2, month2, year2))){
                        cout << tmpTi->transc->transcId << " " << tmpTi->transc->sender->name << " " << tmpTi->transc->receiver->name << " " << tmpTi->transc->val << " " << tmpTi->transc->date << " " << tmpTi->transc->time << endl;
                        sum += tmpTi->transc->val;
                    }

                    tmpTi = tmpTi->next;
                    delete date;
                }
                if (sum == 0){
                    cout << "User has no earnings" << endl;
                }
                else{
                    cout << "Total earnings amount is: " << sum << endl;
                }
                delete date1;
                delete date2;
            }
            else {
                //get time1
                char * time1 = new char[strlen(token) + 1];
                strcpy(time1, token);
                token = strtok(NULL, s);
                if (token == NULL){
                    cerr << "Not enough arguments in cmd" << endl;
                    delete time1;
                    return;
                }
                //if next is date
                if (strlen(token) > 5){
                    //get date
                    char * date1 = new char[strlen(token) + 1];
                    strcpy(date1, token);
                    //compute next time and date
                    token = strtok(NULL, s);
                    if (token == NULL){
                        cerr << "Not enough arguments in cmd" << endl;
                        delete time1;
                        delete date1;
                        return;
                    }
                    char * time2 = new char[strlen(token) + 1];
                    strcpy(time2, token);
                    token = strtok(NULL, s);
                    if (token == NULL){
                        cerr << "Not enough arguments in cmd" << endl;
                        delete time1;
                        delete date1;
                        delete time2;
                        return;
                    }
                    char * date2 = new char[strlen(token) + 1];
                    strcpy(date2, token);

                    //compute date1
                    token = strtok(date1, del);
                    int day1 = atoi(token);
                    token = strtok(NULL, del);
                    int month1 = atoi(token);
                    token = strtok(NULL, del);
                    int year1 = atoi(token);
                    //compute date2
                    token = strtok(date2, del);
                    int day2 = atoi(token);
                    token = strtok(NULL, del);
                    int month2 = atoi(token);
                    token = strtok(NULL, del);
                    int year2 = atoi(token);
                    //compute time1
                    token = strtok(time1, del2);
                    int hour1 = atoi(token);
                    token = strtok(NULL, del2);
                    int min1 = atoi(token);
                    
                    //compute time2
                    token = strtok(time2, del2);
                    int hour2 = atoi(token);
                    token = strtok(NULL, del2);
                    int min2 = atoi(token);
                    

                    //print all transc between
                    int sum = 0;
                    for (int i=0; i<tmpH->list.size; i++){
                        //compute date
                        char * date = new char[strlen(tmpTi->transc->date) + 1];
                        strcpy(date, tmpTi->transc->date);
                        token = strtok(date, del);
                        int day = atoi(token);
                        token = strtok(NULL, del);
                        int month = atoi(token);
                        token = strtok(NULL, del);
                        int year = atoi(token);
                        //compute time
                        char * tim = new char[strlen(tmpTi->transc->time) + 1];
                        strcpy(tim, tmpTi->transc->time);
                        token = strtok(tim, del2);
                        int hour = atoi(token);
                        token = strtok(NULL, del2);
                        int min = atoi(token);
                        
                        if ((convertDateToSec(min1, hour1, day1, month1, year1) <= convertDateToSec(min, hour, day, month, year)) && (convertDateToSec(min, hour, day, month, year) <= convertDateToSec(min2, hour2, day2, month2, year2))){
                            cout << tmpTi->transc->transcId << " " << tmpTi->transc->sender->name << " " << tmpTi->transc->receiver->name << " " << tmpTi->transc->val << " " << tmpTi->transc->date << " " << tmpTi->transc->time << endl;
                            sum += tmpTi->transc->val;
                        }

                        tmpTi = tmpTi->next;
                        delete date;
                        delete tim;
                    }
                    if (sum == 0){
                        cout << "User has no earnings" << endl;
                    }
                    else{
                        cout << "Total earnings amount is: " << sum << endl;
                    }
                    delete date1;
                    delete date2;
                    delete time1;
                    delete time2;
                }
                else{
                    //get time2
                    char * time2 = new char[strlen(token) + 1];
                    strcpy(time2, token);
                    
                    //compute time1
                    token = strtok(time1, del2);
                    int hour1 = atoi(token);
                    token = strtok(NULL, del2);
                    int min1 = atoi(token);
                    //compute time2
                    token = strtok(time2, del2);
                    int hour2 = atoi(token);
                    token = strtok(NULL, del2);
                    int min2 = atoi(token);
                    

                    //print all transc between
                    int sum = 0;
                    for (int i=0; i<tmpH->list.size; i++){
                        //compute time
                        char * tim = new char[strlen(tmpTi->transc->time) + 1];
                        strcpy(tim, tmpTi->transc->time);
                        token = strtok(tim, del2);
                        int hour = atoi(token);
                        token = strtok(NULL, del2);
                        int min = atoi(token);
                        
                        if ((convertDateToSec(min1, hour1, 0, 0, 0) <= convertDateToSec(min, hour, 0, 0, 0)) && (convertDateToSec(min, hour, 0, 0, 0) <= convertDateToSec(min2, hour2, 0, 0, 0))){
                            cout << tmpTi->transc->transcId << " " << tmpTi->transc->sender->name << " " << tmpTi->transc->receiver->name << " " << tmpTi->transc->val << " " << tmpTi->transc->date << " " << tmpTi->transc->time << endl;
                            sum += tmpTi->transc->val;
                        }

                        tmpTi = tmpTi->next;
                        delete tim;
                    }
                    if (sum == 0){
                        cout << "User has no earnings" << endl;
                    }
                    else{
                        cout << "Total earnings amount is: " << sum << endl;
                    }
                    delete time1;
                    delete time2;
                }
            }
        }
    }    
}

void findPayments(UserList & users, HashList* senderHashTable, int sendVal){
    const char del[4] = "-/\n";
    const char del2 [3] = ":\n";
    const char s[3] = " \n";
    char * token;
    token = strtok(NULL, s);
    if (token == NULL){
        cerr << "Not enough arguments in cmd" << endl;
        return;
    }
    userNode * tmpU = users.findUserByName(token);
    if (tmpU == NULL){
        cerr << "User " << token << " doesn't exist!" << endl;
        return;
    }
    int wid = tmpU->wallet.walletId;
    hashNode * tmpH = senderHashTable[hashFunction(wid, sendVal)].findUser(wid);
    if (tmpH == NULL){
        cout << "User has no payments" << endl;
    }
    else{
        transcInfoNode * tmpTi = tmpH->list.first;
        token = strtok(NULL, s);
        if (token == NULL){
            //print all transc
            int sum = 0;
            for (int i=0; i<tmpH->list.size; i++){
                cout << tmpTi->transc->transcId << " " << tmpTi->transc->sender->name << " " << tmpTi->transc->receiver->name << " " << tmpTi->transc->val << " " << tmpTi->transc->date << " " << tmpTi->transc->time << endl;
                sum += tmpTi->transc->val;
                tmpTi = tmpTi->next;
            }
            if (sum == 0){
                cout << "User has no payments" << endl;
            }
            else{
                cout << "Total payments amount is: " << sum << endl;
            }
        }
        else{
            //print transc > date or time
            //if first arg is date
            if (strlen(token) > 5){
                //get date1
                char * date1 = new char[strlen(token) + 1];
                strcpy(date1, token); 
                //get next date
                token = strtok(NULL, s);
                if (token == NULL){
                    cerr << "Not enough arguments in cmd" << endl;
                    delete date1;
                    return;
                }
                char * date2 = new char[strlen(token) + 1];
                strcpy(date2, token);
                //print all transc between
                //compute date1
                token = strtok(date1, del);
                int day1 = atoi(token);
                token = strtok(NULL, del);
                int month1 = atoi(token);
                token = strtok(NULL, del);
                int year1 = atoi(token);
                //compute date2
                token = strtok(date2, del);
                int day2 = atoi(token);
                token = strtok(NULL, del);
                int month2 = atoi(token);
                token = strtok(NULL, del);
                int year2 = atoi(token);
                
                int sum = 0;
                for (int i=0; i<tmpH->list.size; i++){
                    //compute date
                    char * date = new char[strlen(tmpTi->transc->date) + 1];
                    strcpy(date, tmpTi->transc->date);
                    token = strtok(date, del);
                    int day = atoi(token);
                    token = strtok(NULL, del);
                    int month = atoi(token);
                    token = strtok(NULL, del);
                    int year = atoi(token);
                    if ((convertDateToSec(0, 0, day1, month1, year1) <= convertDateToSec(0, 0, day, month, year)) && (convertDateToSec(0, 0, day, month, year) <= convertDateToSec(0, 0, day2, month2, year2))){
                        cout << tmpTi->transc->transcId << " " << tmpTi->transc->sender->name << " " << tmpTi->transc->receiver->name << " " << tmpTi->transc->val << " " << tmpTi->transc->date << " " << tmpTi->transc->time << endl;
                        sum += tmpTi->transc->val;
                    }

                    tmpTi = tmpTi->next;
                    delete date;
                }
                if (sum == 0){
                    cout << "User has no payments" << endl;
                }
                else{
                    cout << "Total payments amount is: " << sum << endl;
                }
                delete date1;
                delete date2;
            }
            else {
                //get time1
                char * time1 = new char[strlen(token) + 1];
                strcpy(time1, token);
                token = strtok(NULL, s);
                if (token == NULL){
                    cerr << "Not enough arguments in cmd" << endl;
                    delete time1;
                    return;
                }
                //if next is date
                if (strlen(token) > 5){
                    //get date
                    char * date1 = new char[strlen(token) + 1];
                    strcpy(date1, token);
                    //compute next time and date
                    token = strtok(NULL, s);
                    if (token == NULL){
                        cerr << "Not enough arguments in cmd" << endl;
                        delete time1;
                        delete date1;
                        return;
                    }
                    char * time2 = new char[strlen(token) + 1];
                    strcpy(time2, token);
                    token = strtok(NULL, s);
                    if (token == NULL){
                        cerr << "Not enough arguments in cmd" << endl;
                        delete time1;
                        delete date1;
                        delete time2;
                        return;
                    }
                    char * date2 = new char[strlen(token) + 1];
                    strcpy(date2, token);

                    //compute date1
                    token = strtok(date1, del);
                    int day1 = atoi(token);
                    token = strtok(NULL, del);
                    int month1 = atoi(token);
                    token = strtok(NULL, del);
                    int year1 = atoi(token);
                    //compute date2
                    token = strtok(date2, del);
                    int day2 = atoi(token);
                    token = strtok(NULL, del);
                    int month2 = atoi(token);
                    token = strtok(NULL, del);
                    int year2 = atoi(token);
                    //compute time1
                    token = strtok(time1, del2);
                    int hour1 = atoi(token);
                    token = strtok(NULL, del2);
                    int min1 = atoi(token);
                    
                    //compute time2
                    token = strtok(time2, del2);
                    int hour2 = atoi(token);
                    token = strtok(NULL, del2);
                    int min2 = atoi(token);
                    

                    //print all transc between
                    int sum = 0;
                    for (int i=0; i<tmpH->list.size; i++){
                        //compute date
                        char * date = new char[strlen(tmpTi->transc->date) + 1];
                        strcpy(date, tmpTi->transc->date);
                        token = strtok(date, del);
                        int day = atoi(token);
                        token = strtok(NULL, del);
                        int month = atoi(token);
                        token = strtok(NULL, del);
                        int year = atoi(token);
                        //compute time
                        char * tim = new char[strlen(tmpTi->transc->time) + 1];
                        strcpy(tim, tmpTi->transc->time);
                        token = strtok(tim, del2);
                        int hour = atoi(token);
                        token = strtok(NULL, del2);
                        int min = atoi(token);
                        
                        if ((convertDateToSec(min1, hour1, day1, month1, year1) <= convertDateToSec(min, hour, day, month, year)) && (convertDateToSec(min, hour, day, month, year) <= convertDateToSec(min2, hour2, day2, month2, year2))){
                            cout << tmpTi->transc->transcId << " " << tmpTi->transc->sender->name << " " << tmpTi->transc->receiver->name << " " << tmpTi->transc->val << " " << tmpTi->transc->date << " " << tmpTi->transc->time << endl;
                            sum += tmpTi->transc->val;
                        }

                        tmpTi = tmpTi->next;
                        delete date;
                        delete tim;
                    }
                    if (sum == 0){
                        cout << "User has no payments" << endl;
                    }
                    else{
                        cout << "Total payments amount is: " << sum << endl;
                    }
                    delete date1;
                    delete date2;
                    delete time1;
                    delete time2;
                }
                else{
                    //get time2
                    char * time2 = new char[strlen(token) + 1];
                    strcpy(time2, token);
                    
                    //compute time1
                    token = strtok(time1, del2);
                    int hour1 = atoi(token);
                    token = strtok(NULL, del2);
                    int min1 = atoi(token);
                    //compute time2
                    token = strtok(time2, del2);
                    int hour2 = atoi(token);
                    token = strtok(NULL, del2);
                    int min2 = atoi(token);
                    

                    //print all transc between
                    int sum = 0;
                    for (int i=0; i<tmpH->list.size; i++){
                        //compute time
                        char * tim = new char[strlen(tmpTi->transc->time) + 1];
                        strcpy(tim, tmpTi->transc->time);
                        token = strtok(tim, del2);
                        int hour = atoi(token);
                        token = strtok(NULL, del2);
                        int min = atoi(token);
                        
                        if ((convertDateToSec(min1, hour1, 0, 0, 0) <= convertDateToSec(min, hour, 0, 0, 0)) && (convertDateToSec(min, hour, 0, 0, 0) <= convertDateToSec(min2, hour2, 0, 0, 0))){
                            cout << tmpTi->transc->transcId << " " << tmpTi->transc->sender->name << " " << tmpTi->transc->receiver->name << " " << tmpTi->transc->val << " " << tmpTi->transc->date << " " << tmpTi->transc->time << endl;
                            sum += tmpTi->transc->val;
                        }

                        tmpTi = tmpTi->next;
                        delete tim;
                    }
                    if (sum == 0){
                        cout << "User has no payments" << endl;
                    }
                    else{
                        cout << "Total payments amount is: " << sum << endl;
                    }
                    delete time1;
                    delete time2;
                }
            }
        }
    }
}

void walletStatus(UserList & users){
    const char s[3] = " \n";
    char * token;
    token = strtok(NULL, s);
    if (token == NULL){
        cerr << "Not enough arguments in cmd" << endl;
        return;
    }
    userNode * tmpU = users.findUserByName(token);
    if (tmpU == NULL){
        cerr << "User " << token << " doesn't exist!" << endl;
        return;
    }
    cout << "User wallet amount is: " << tmpU->wallet.val << endl;
}

void bitcoinStatus(BitcoinTreeList& bitcoins){
    const char s[3] = " \n";
    char * token;
    token = strtok(NULL, s);
    if (token == NULL){
        cerr << "Not enough arguments in cmd" << endl;
        return;
    }
    int bId = atoi(token);
    //... bfs and most left node
    bitcoinTreeNode * tree = bitcoins.findBitTree(bId);
    if (tree == NULL){
        cerr << "Bitcoin " << bId << " doesn't exist!" << endl;
        return;
    }
    bitcoinNode * tmpB = tree->bitcoinTree;
    cout << "Bitcoin " << bId << " used in " << bitcoins.findTranscNum(tmpB) << " Transactions ";
    cout << "and " << bitcoins.findNotSpent(tmpB) << "$ not used" << endl;
}

void traceCoin(BitcoinTreeList& bitcoins){
    const char s[3] = " \n";
    char * token;
    token = strtok(NULL, s);
    if (token == NULL){
        cerr << "Not enough arguments in cmd" << endl;
        return;
    }
    int bId = atoi(token);
    bitcoinTreeNode * tree = bitcoins.findBitTree(bId);
    if (tree == NULL){
        cerr << "Bitcoin " << bId << " doesn't exist!" << endl;
        return;
    }
    bitcoinNode * tmpB = tree->bitcoinTree;
    int height = bitcoins.height(tmpB);
    TranscInfoList list;
    int h;
    for (h=1; h<=height; h++){
        bitcoins.findTranscAtLevel(tmpB, h, list);
    }
    if (list.size == 0){
        cout << "Bitcoin " << bId << " has no transactions history" << endl;
    }
    else{
        cout << "Bitcoin " << bId << " history: " << endl;
        transcInfoNode * temp = list.first;
        for (int i=0; i<list.size; i++){
            cout << temp->transc->transcId << " " << temp->transc->sender->name << " " << temp->transc->receiver->name << " " << temp->transc->val << " " << temp->transc->date << " " << temp->transc->time << endl; 
            temp = temp->next;
        }
    }
}