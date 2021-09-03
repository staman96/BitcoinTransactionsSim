#ifndef validations_h
#define validations_h
#include "bitcoin.h"
#include "wallet.h"
#include "transaction.h"
#include "hashTable.h"
#include <string.h>
#include <iostream>
#include <time.h>

class validations{
    private:
        bitcoins *bs;
        transactions *transfers;
        wallets *wl;
        hashTable *senders, *receivers;
    public:
    validations(bitcoins *b,transactions *tr,wallets *w,hashTable *send,hashTable *receive);

    /*makes the necessary checks and returns true or false depending if transaction is valid or not*/
    bool validTransaction(char* transID,char* senderID,char* receiverID,double value,char* datestr,char* timestr);

    /*adds to bitcoin network transactions from files*/
    bool transactionsFromFile(char* file);

    /*calls functions to update and create objects of validated transaction*/
    void makeTransaction(char* trID,char* sender,char* receiver,double value,char* date,char* timestr);

    /*function responsible of materializing transactions*/
    void transactionMaster(transactionData *transaction);

    /*calls tracecoin function in bicoins*/
    bool tracecoin(char* line);

    /*responsible of gathering and printing bitcoin status data*/
    bool bitCoinStatus(char* line);

    /*gets and prints wallet status*/
    void printWalletStatus(char* line);

    /*prints earnings of wallet*/
    void findEarnings(char* line);

    /*prints payments of wallet*/
    void findPayments(char* line);

    /*functions check timeline and date respectively as reffered in piazza,
    *used in findEarnings and findPayments
    */
    bool betweenTime1_2(char* time1,char* time2, char* searchTime);
    bool beforeAfterDate1_2(char* date1,char* date2, char* timespan);

    /*checks if the args are valid and then tries to make one transaction*/
    void requestTransaction(char* line);

    /*calls request Transaction to check transaction from each line*/
    void requestTransactions(char* inputLine);

    ~validations();

};

#endif