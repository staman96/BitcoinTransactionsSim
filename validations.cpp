#include <iostream>
#include <fstream>
#include <string.h>
#include <cstdlib>
#include <time.h>
#include "bitcoin.h"
#include "wallet.h"
#include "transaction.h"
#include "validations.h"
#include "hashTable.h"

using namespace std;

validations::validations(bitcoins *b,transactions *tr,wallets *w, hashTable *send,hashTable *receive)
:bs(b),transfers(tr),wl(w),senders(send),receivers(receive){
    cout << "Validating tools initialized." << endl;
}

bool validations::validTransaction(char* transID,char* senderID,char* receiverID,double value,char* datestr,char* timestr){
    /*checks if there's is another transaction with same id*/
    if (this->transfers->uniqueTrID(transID) == false){
        cout << "Transaction ID: " << transID << " already exists...transaction is being cancelled." << endl;
        return false;
    }
    /*checks if sender exists*/
    if (this->wl->uniqueWl(senderID) == true){
        cout << "Sender ID: " << senderID << " doesn't exist...transaction is being cancelled." << endl;
        return false;
    }
    /*checks if receiver exists*/
    if (this->wl->uniqueWl(receiverID) == true){
        cout << "Receiver ID: "<< receiverID << " doesn't exist...transaction is being cancelled." << endl;
        return false;
    }
    /*checks if sender has enough balance*/
    if (this->wl->getWallet(senderID)->getBalance() < value){
        cout << "Sender: " << senderID << " doesn't have enough balance...transaction is being cancelled." << endl;
        return false;
    }
    /*checks the timeline of transaction*/
    if (this->transfers->newerTime(datestr, timestr) == false){
        cout << "Transaction is older than last transaction...transaction is being cancelled." << endl;
        return false;
    }
    /*checks if sender and receiver is the same user*/
    if (strcmp(receiverID,senderID) == 0){
        cout << "Receiver ID: "<< receiverID << " doesn't exist...transaction is being cancelled." << endl;
        return false;
    }
    return true;
}

bool validations::transactionsFromFile(char* file){
    /*initializing temporary data*/
    char* pch;
    size_t length = 512;
    char line[length];
    char* data[6];
    for (int i = 0;i < 6;i++){
        data[i] = NULL;
    }
    /*opening file to read*/
    FILE* tractsfile = fopen(file, "r");

    /*check if file is open*/
    if (tractsfile != NULL){
        /*reads line*/
        while ( fgets(line, length, tractsfile) != NULL){   
            /*fills array with transaction's arguments*/
            pch = strtok(line," ,\n\t");
            data[0] = pch;
            for (int i = 1;i < 6;i++){
                pch = strtok (NULL, " ,\n\t");
                data[i] = pch;
            }

            /*validating transaction*/
            if(this->validTransaction(data[0],data[1],data[2],atof(data[3]),data[4],data[5])){
                /*make transaction*/
                this->makeTransaction(data[0],data[1],data[2],atof(data[3]),data[4],data[5]);
            }
        }
        /*close file*/
        fclose(tractsfile);
    }
    else {
        cout << "Unable to open transactions file..." << endl;
        return false;
    }
    cout << "Transactions completed." << endl;
    return true;
}

void validations::makeTransaction(char* trID,char* sender,char* receiver,double value,char* date,char* timestr){
    transactionNode *tempNode = NULL;
    cout << "***************************************************" << endl;
    /*initiating valid transaction*/
    transactionData *temptrans = new transactionData(trID,wl->getWallet(sender),wl->getWallet(receiver),value,date,timestr);
    /*adds transaction to transactions array*/
    this->transfers->addTransaction(temptrans);
    /*updating wallets and moving bitcoins*/
    this->transactionMaster(temptrans);
    /*add transaction to hashtables*/

        /*sender*/
    tempNode = this->senders->get(temptrans->getSender()->getWalletID());
    if (tempNode == NULL){
        tempNode = new transactionNode(temptrans);
        /*puts root node and makes a record for sender id*/
        this->senders->put(temptrans->getSender()->getWalletID(),tempNode);
    }
    else{
        /*adds node to list*/
        tempNode->addTransaction(temptrans);
    }

        /*receiver*/
    tempNode = this->receivers->get(temptrans->getReceiver()->getWalletID());
    if (tempNode == NULL){
        tempNode = new transactionNode(temptrans);
        /*puts root node and makes a record for receiver id*/
        this->receivers->put(temptrans->getReceiver()->getWalletID(),tempNode);
    }
    else{
        /*adds node to list*/
        tempNode->addTransaction(temptrans);
    }
        
    cout << "***************************************************" << endl;
}

void validations::transactionMaster(transactionData *transaction){
    /*sender data before transaction*/
    cout << "Sender data before transaction:" << endl;
    transaction->getSender()->printdata();
    cout << endl;

    /*receiver data before transaction*/
    cout << "Receiver data before transaction:" << endl;
    transaction->getReceiver()->printdata();
    cout << endl;

    bitcoinNode** BCsForTransfer;
    int length;
    BCsForTransfer = transaction->getSender()->getBCsValued(transaction->getValue(), length);
    double value = transaction->getValue(), valueForTrans = 0.0;
    bitcoinNode *temp;

    for (int i = length-1; i > -1; i--){
        value -= BCsForTransfer[i]->getBitcoin()->getBcValue();

        cout << endl;
        BCsForTransfer[i]->getBitcoin()->print();
        cout << endl;

        if (value < 0){
            valueForTrans = BCsForTransfer[i]->getBitcoin()->getBcValue() + value;
            
            /*sender------------->*/
            /*making new right branch of current bitcoinNode with the leftover of bitcoin value*/
            temp = BCsForTransfer[i]->addrightBcNode(transaction,valueForTrans);
            
        }
        else
        {
            valueForTrans = BCsForTransfer[i]->getBitcoin()->getBcValue();
        }
        

        /*----------->receiver*/
        /*making new left branch of current bitcoinNode and adds bitcoinNode to receiver*/
        temp = BCsForTransfer[i]->addleftBcNode(transaction,valueForTrans);
        
        
    }
    /*sender data after transaction*/
    cout << "Sender data after transaction:" << endl;
    transaction->getSender()->printdata();
    cout << endl;

    /*receiver data after transaction*/
    cout << "Receiver data after transaction:" << endl;
    transaction->getReceiver()->printdata();
    cout << endl;

    /*deleting temporary array*/
    delete[] BCsForTransfer;

}

bool validations::tracecoin(char* line){
    char* pch;
    /*gets next word*/
    pch = strtok(NULL," ,\t\n");
    int id = atoi(pch);
    /*checks if id consists of numbers only*/
    if (id ==  0)
        return false;
    
    /*checks if id exists*/
    if (bs->uniqueBc(id) == false){
        cout << "/tracecoin " << id << endl;
        bs->getBitcoinRoot(id)->traceCoin();
    }
    else
        cout << "Bitcoin with id " << id << " doesn't exist." << endl;
    return true;
    fflush;
}

bool validations::bitCoinStatus(char* line){
    int noOfTransactions = 0;
    char* pch;
    pch = strtok(NULL," ,\t\n");
    int id = atoi(pch);
    /*checks if id consists of numbers only*/
    if (id ==  0)
        return false;
    
    /*checks if id exists*/
    if (bs->uniqueBc(id) == false){
        bs->getBitcoinRoot(id)->transactionsPart(noOfTransactions);
        cout << id << "\t" << noOfTransactions << "\t" 
        << bs->getBitcoinRoot(id)->getUnspent() << endl;
    }
    else
        cout << "Bitcoin with id " << id << " doesn't exist." << endl;
    return true;
    fflush;
}

void validations::printWalletStatus(char* line){
    char* pch;
    pch = strtok(NULL," ,\t\n");
    if (wl->uniqueWl(pch)){
        cout << "Wallet with id " << pch << " doesn't exist." << endl;
    }
    else
    {
        cout << "Wallet with id " << pch << " has " << 
        wl->getWallet(pch)->getBalance() << " of bitcoin balance" << endl;
    }
}

void validations::findEarnings(char* line){
    char* pch;
    pch = strtok(NULL," ,\t\n");
    transactionNode *tempTrNode = receivers->get(pch);
    /*checking for wallet id*/
    if (wl->uniqueWl(pch)){
        cout << "Wallet with id " << pch << " doesn't exist." << endl;
        return;
    }
    else if(tempTrNode == NULL){
        cout << "Wallet with id " << pch << " never received bitcoins." << endl;
        return;
    }
    else{
        char** durations = new char*[4];
        for (int i = 0;i<4;i++) durations[i] = NULL;
        int step = 0;

        /*getting all arguments left, max 4*/
        do{
            pch = strtok(NULL," ,\t");
            durations[step] = pch;
            step++;
        }while(pch != NULL && step < 4);

        /*checking arguments quantity*/
        int ntime = 2, ndate = 2;
        for (int i = 0;i<4;i++){
            if (durations[i] != NULL){
                for(int x = 0;x < strlen(durations[i]);x++){
	                if (durations[i][x] == '-'){
                        ndate--;
                        break;
                    }
                    else if(durations[i][x] == ':'){
                        ntime--;
                        break;
                    }
                }
            }
        }

        /*prints total earnings*/
        transactionNode *temp = tempTrNode;
        double earnings = 0;
        do{
            earnings += temp->getTransData()->getValue();
            temp = temp->getNextTransPtr();
        }while(temp != NULL);
        cout << tempTrNode->getTransData()->getReceiver()->getWalletID()
        << " has " << earnings << " of total earnings." << endl;

        /*categorizing arguments*/
        char *time1 = NULL, *time2 = NULL, *date1 = NULL, *date2 = NULL;
        if (ntime == 0 && ndate == 2){
            time1 = durations[0];
            time2 = durations[1];
        }
        else if (ntime == 0 && ndate == 0){
            time1 = durations[0];
            date1 = durations[1];
            time2 = durations[2];
            date2 = durations[3];
        }
        else if (ntime == 2 && ndate == 0){
            date1 = durations[0];
            date2 = durations[1];
        }
        else if (ntime == 2 && ndate == 2){
            //do nothing
        }
        else{
            cout << "Wrong arguments!" << endl;
            delete[] durations;
            return;
        }


        /*printing transactions meeting criteria*/
        temp = tempTrNode;
        do{
            if ( betweenTime1_2( time1,time2, temp->getTransData()->getTimestr() ) && 
                beforeAfterDate1_2( date1,date2, temp->getTransData()->getDatestr() ) ){
                    temp->getTransData()->printTransaction();
                    cout << endl;
            }
            temp = temp->getNextTransPtr();
        }while(temp != NULL);

        delete[] durations;
    }
}

void validations::findPayments(char* line){
    char* pch;
    pch = strtok(NULL," ,\t\n");
    transactionNode *tempTrNode = senders->get(pch);
    /*checking for wallet id*/
    if (wl->uniqueWl(pch)){
        cout << "Wallet with id " << pch << " doesn't exist." << endl;
        return;
    }
    else if(tempTrNode == NULL){
        cout << "Wallet with id " << pch << " never sent bitcoins." << endl;
        return;
    }
    else{
        char** durations = new char*[4];
        for (int i = 0;i<4;i++) durations[i] = NULL;
        int step = 0;

        /*getting all arguments left, max 4*/
        do{
            pch = strtok(NULL," ,\t");
            durations[step] = pch;
            step++;
        }while(pch != NULL && step < 4);

        /*checking arguments quantity*/
        int ntime = 2, ndate = 2;
        for (int i = 0;i<4;i++){
            if (durations[i] != NULL){
                for(int x = 0;x < strlen(durations[i]);x++){
	                if (durations[i][x] == '-'){
                        ndate--;
                        break;
                    }
                    else if(durations[i][x] == ':'){
                        ntime--;
                        break;
                    }
                }
            }
        }

        /*prints total payments*/
        transactionNode *temp = tempTrNode;
        double payments = 0;
        do{
            payments += temp->getTransData()->getValue();
            temp = temp->getNextTransPtr();
        }while(temp != NULL);
        cout << tempTrNode->getTransData()->getSender()->getWalletID()
        << " has " << payments << " of total payments." << endl;

        /*categorizing arguments*/
        char *time1 = NULL, *time2 = NULL, *date1 = NULL, *date2 = NULL;
        if (ntime == 0 && ndate == 2){
            time1 = durations[0];
            time2 = durations[1];
        }
        else if (ntime == 0 && ndate == 0){
            time1 = durations[0];
            date1 = durations[1];
            time2 = durations[2];
            date2 = durations[3];
        }
        else if (ntime == 2 && ndate == 0){
            date1 = durations[0];
            date2 = durations[1];
        }
        else if (ntime == 2 && ndate == 2){
            //do nothing
        }
        else{
            cout << "Wrong arguments!" << endl;
            delete[] durations;
            return;
        }


        /*printing transactions meeting criteria*/
        temp = tempTrNode;
        do{
            if ( betweenTime1_2( time1,time2, temp->getTransData()->getTimestr() ) && 
                beforeAfterDate1_2( date1,date2, temp->getTransData()->getDatestr() ) ){
                    temp->getTransData()->printTransaction();
                    cout << endl;
            }
            temp = temp->getNextTransPtr();
        }while(temp != NULL);

        delete[] durations;
    }
}

bool validations::betweenTime1_2(char* time1,char* time2, char* searchTime){
    /*if value is NULL then the arguments are missing*/
    if (time1 == NULL || time2 == NULL) return true;
    /*copying strings to avoid changing the originals*/
    char* timeA = new char[6];
    char* timeB = new char[6];
    char* timeS = new char[6];
    strcpy(timeA,time1); strcpy(timeB,time2); strcpy(timeS,searchTime);
    /*making string to double so i can compare*/
    timeA[2]= '.'; double A = atof(timeA);
    timeB[2]= '.'; double B = atof(timeB);
    timeS[2]= '.'; double S = atof(timeS);
    
    delete [] timeA;
    delete [] timeB;
    delete [] timeS;

    /*if transaction time is between times return true, else false*/
    if (A < S && S < B) return true;
    return false;
}
    
bool validations::beforeAfterDate1_2(char* date1,char* date2, char* timespan){
    /*if value is NULL then the arguments are missing*/
    if (date1 == NULL || date2 == NULL) return true;

    char* timeZero = "00:00";
    time_t before = transactions::convertTime(date1, timeZero);
    time_t after = transactions::convertTime(date2, timeZero);
    time_t between = transactions::convertTime(timespan, timeZero);

    /*checks if there is a transaction in the same day as 
    *the start and the end of duration given
    */
    if (before <= between && between <= after) return true;
    return false;
}

void validations::requestTransaction(char* line){
    char* pch;    
    char** arguments = new char*[5];
    double amount;
    for (int i = 0;i<5;i++) arguments[i] = NULL;
    int step = 0;

    /*getting all arguments left, max 5*/
    do{
        pch = strtok(NULL," ,\t\n;");
        arguments[step] = pch;
        step++;
    }while(pch != NULL && step < 5);

    /*checkinh*/
    if (wl->getWallet(arguments[0]) == NULL){
        cout << "Sender with id " << arguments[0] << " doesn't exist." << endl;
        delete[] arguments;
        return;
    }
    if (wl->getWallet(arguments[1]) == NULL){
        cout << "Receiver with id " << arguments[2] << " doesn't exist." << endl;
        delete[] arguments;
        return;
    }
    if(arguments[2] != NULL){
        amount = atof(arguments[2]);
    }
    else{
        cout << "Invalid amount argument was given." << endl;
        delete[] arguments;
        return;
    }

    /*checking if has date, time or both*/
    int ndate = 0, ntime = 0;
    step = 3;
    while(arguments[step] != NULL) {
        for(int x = 0;x < strlen(arguments[step]);x++){
            if (arguments[step][x] == '-'){
                ndate = 1;
                break;
            }
            else if(arguments[step][x] == ':'){
                ntime = 1;
                break;
            }
        }
        step++;
    }

    /*setting time and date depending on arguments*/
    char *datestr, *timestr;
    time_t t;
    if (ndate == 1 && ntime == 0){
        time(&t);
        datestr = arguments[3];
        timestr = transfers->extractTimestr(t);
        /*set correct time and date*/
        t = transactions::convertTime(datestr, timestr);
    }
    else if (ndate == 1 && ntime == 1) {
        datestr = arguments[3];
        timestr = arguments[4];
        /*set correct time and date*/
        t = transactions::convertTime(datestr, timestr);
    }
    else if(ndate = 0 && ntime == 1){
        time(&t);
        timestr = arguments[3];
        datestr = transfers->extractDatestr(t);
        /*set correct time and date*/
        t = transactions::convertTime(datestr, timestr);
    }
    else{
        time(&t);
        datestr = transfers->extractDatestr(t);
        timestr = transfers->extractTimestr(t);
    }

    /*extracts id from timestamp, there is only 1 transaction 
    *per minute so timestamp id is unique
    */
    char* trID = new char[12];
    sprintf(trID, "%u", t);
    
    /*check if transaction is valid*/
    if (this->validTransaction(trID,arguments[0],arguments[1],amount,datestr,timestr)){
        this->makeTransaction(trID,arguments[0],arguments[1],amount,datestr,timestr);
        cout << "Transaction was successful." << endl;
    }

    /*free allocated space*/
    delete[] trID;
    delete[] arguments;
}

void validations::requestTransactions(char* inputLine){
    
    size_t length = 512;
    char line[length];
    char* pch = strtok(NULL," ,\t\n;");

    /*trying opening file to read*/
    FILE* tractsfile = fopen(pch, "r");

    /*check if file is open*/
    if (tractsfile != NULL){
        cout << "just opened file" << endl;
        /*reads line*/
        while ( fgets(line, length, tractsfile) != NULL){ 
            /*trying to make transaction*/
            this->requestTransaction(line);
        }
        fclose(tractsfile);
        cout << endl << "Transactions from file completed." << endl << endl;
        return;
    }
    else{
        cout << "coudn't open file" << endl;
    }

}

validations::~validations(){
    cout << "Deleting validation tools..." << endl;
}