#include <time.h>
#include "transaction.h"
#include <iostream>
#include <string.h>
#include "wallet.h"
#include <cstdlib>
#include <cstdio>

using namespace std;
/*--------------------------------transaction data---------------------------------*/
    transactionData::transactionData(char *trID,wallet *send,wallet *receive,double val,char* d,char *tim)
    :value(val),sender(send),receiver(receive){
        int len;
        /*saving strings*/
        len = strlen(trID); this->transactionID = new char[len+1]; 
        strcpy(this->transactionID,trID); 
        
        len = strlen(d); this->date = new char[len+1]; 
        strcpy(this->date,d);
        
        len = strlen(tim); this->timestr = new char[len+1]; 
        strcpy(this->timestr,tim);

        this->t = transactions::convertTime(this->date, this->timestr);

        cout << "A transaction with id " << this->transactionID << " was made." << endl;
        this->printTransaction();
        cout << endl;
    }

    time_t transactionData::getTime(){
        return this->t;
    }

    char* transactionData::getTimestr(){
        return this->timestr;
    }

    char* transactionData::getDatestr(){
        return this->date;
    }

    wallet* transactionData::getSender(){
        return this->sender;
    }

    wallet* transactionData::getReceiver(){
        return this->receiver;
    }

    double transactionData::getValue(){
        return this->value;
    }

    char* transactionData::getTransID(){
        return this->transactionID;
    }

    void transactionData::printTransaction(){
        cout << this->transactionID << " ";
        
        cout << this->sender->getWalletID() << " " ;
        cout << this->receiver->getWalletID();
        cout << " " << this->value << " " << this->date << " " << this->timestr;
    }

    transactionData::~transactionData(){
        cout << "A transaction is being destroyed!" << endl;
        delete[] this->transactionID;
        delete[] this->date;
        delete[] this->timestr;
    }

/*------------------------------transaction lists------------------------------------*/

    transactionNode::transactionNode(transactionData* transData)
    :data(transData){
        this->next = NULL;
        cout << "A transaction node was made." << endl;
    }

    transactionNode* transactionNode::addTransaction(transactionData *transData){
        /*making new transactionNode*/ 
        transactionNode *node = new transactionNode(transData);

        /*looking for last node*/
        transactionNode *temp = this;
        while(temp->next != NULL){
            temp = temp->next;
        }

        /*seting last next = new*/ 
        temp->next = node;
        
        /*next of new node is null and has the transaction data from constructor*/
        
        /*returns nextptr*/
        return this->next;
    }

    transactionData* transactionNode::getTransData(){
        return this->data;
    }

    transactionNode* transactionNode::getNextTransPtr(){
        return this->next;
    }

    void transactionNode::PrintList(){
        this->data->printTransaction();
        cout << endl;
        if (this->next != NULL)
            this->next->PrintList();
    }

    transactionNode::~transactionNode(){
        cout << "A transaction node is being erased." << endl;
        if (this->next != NULL)
            delete next;
    }


/*------------------------------------all transactions array---------------------------------*/
transactions::transactions(){
    cout << "Transactions array is being initialized." << endl;
    this->alltransactions = NULL;
    this->trCounter = 0;
    this->recentTime = 0;
    this->timestr = new char[6];
    this->datestr = new char[11];
}

void transactions::addTransaction(transactionData* transaction){
        this->trCounter++;
        
        /*make new array with one extra slot*/
        transactionData **temp = new transactionData*[this->trCounter];

        /*pass the reference of new node into array @ last pos*/
        temp[this->trCounter-1] = transaction;

        /*make new array to point to old elements*/
        for (int i = 0; i<this->trCounter-1;i++){
            temp[i] = this->alltransactions[i];
        }
        
        /*free old space if its beeb allocated*/
        if(this->alltransactions != NULL)
            delete[] this->alltransactions;

        /*make alltransactions pointing to new array*/
        this->alltransactions = temp;

        /*update recent time*/
        if(this->recentTime < transaction->getTime())
            this->recentTime = transaction->getTime();
}

bool transactions::newerTime(char* datestr, char* timestr){
    time_t testTime = convertTime(datestr,timestr);
    /*time can't be the same as the recent one, must ne later
    *due to the conflict it might occure at a bitcoin being transfered
    *at the same time*/
    if (testTime > this->recentTime){
        return true;
    }
    return false;
}

bool transactions::uniqueTrID(char* transactionID){
    for (int i = 0;i < this->trCounter;i++){
        if (strcmp(this->alltransactions[i]->getTransID(),transactionID) == 0)
            return false;
    }
    return true;
}

time_t transactions::convertTime(char* datestr,char* timestr){
    char * pch;
    struct tm timeparse;
    time_t longTime;
    if(datestr == NULL || timestr == NULL){
        /*use local tome for transaction*/
        time(&longTime);
        cout << "Local time was used." << endl;
        return longTime;
    }

    /*11 chars are enough for date and more than enough for time*/
    char s[11]; 
    strcpy(s,datestr);

    /*set date*/
    pch = strtok(s,"-");
    timeparse.tm_mday = atoi(pch);
    pch = strtok(NULL,"-");
    timeparse.tm_mon = atoi(pch)-1;
    pch = strtok(NULL,"-");
    timeparse.tm_year = atoi(pch)-1900;
    
    strcpy(s,timestr);

    /*set time*/
    pch = strtok(s,":");
    timeparse.tm_hour = atoi(pch);
    pch = strtok(NULL,":");
    timeparse.tm_min = atoi(pch);
    
    timeparse.tm_sec = 0;
    timeparse.tm_isdst = 0;

    longTime = mktime(&timeparse);

    return longTime;
}

char* transactions::extractTimestr(time_t ctime){
    /*if argument is null take current time*/
    if (ctime == 0)
        time(&ctime);
    struct tm *timeparse = localtime(&ctime);
    char *ascii;
    ascii = asctime(timeparse);
    for (int i = 0; i < 5; i++){
        this->timestr[i] = ascii[i+11];
    }
    return this->timestr;
}

char* transactions::extractDatestr(time_t ctime){
    /*if argument is null take current date*/
    if (ctime == 0)
        time(&ctime);
    struct tm *timeparse = localtime(&ctime);
    char *ascii;
    ascii = asctime(timeparse);
    /*add year to string*/
    for (int i = 0; i < 4; i++){
        this->datestr[i+6] = ascii[i+20];
    }
    /*add dashes*/
    this->datestr[2] = '-';
    this->datestr[5] = '-';

    /*add days*/
    char *str = new char[3];
    if (timeparse->tm_mday < 10){
        this->datestr[0] = '0';
        sprintf(str, "%d", timeparse->tm_mday);
        this->datestr[1] = str[0];
    }
    else{
        sprintf(str, "%d", timeparse->tm_mday);
        for (int i = 0; i < 2; i++){
            this->datestr[i] = str[i];
        }
    }

    /*add months*/
    int month = timeparse->tm_mon + 1;
    if (month < 10){
        this->datestr[3] = '0';
        sprintf(str, "%d", month);
        this->datestr[4] = str[0];
    }
    else{
        sprintf(str, "%d", month);
        for (int i = 0; i < 2; i++){
            this->datestr[i+3] = str[i];
        }
    }
    delete[] str;
    return this->datestr;
}

void transactions::printTransactions(){
    for (int i = 0; i<this->trCounter;i++){
        cout << "Transaction[" << i << "] --> ";
        this->alltransactions[i]->printTransaction();
        cout << endl; 
    }
}

transactions::~transactions(){
    cout << "Destroying all transactions..." << endl;
    delete[] this->timestr;
    delete[] this->datestr;
    for (int i = 0;i<this->trCounter;i++){
        delete this->alltransactions[i];
    }
    if(this->alltransactions != NULL)
        delete[] this->alltransactions;
}