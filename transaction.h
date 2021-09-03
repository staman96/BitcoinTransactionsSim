#ifndef transaction_h
#define transaction_h
#include <time.h>
#include "wallet.h"

class transactionData {
    private:
    double value;
    char *transactionID, *date, *timestr;
    wallet* sender, *receiver;
    time_t t;      

    public:
    /*from transactions file*/
    transactionData(char *trID,wallet *send,wallet *receive,double val,char* d,char *tim);
    /*for keyboard transactions*/
    transactionData(char *sendID, char *recID,double val,char* d,char *tim);

    /*functions to get individual values*/
    time_t getTime();

    char* getTimestr();

    char* getDatestr();

    wallet* getSender();

    wallet* getReceiver();

    double getValue();

    char* getTransID();

    /*printing all transaction data in one line with explanation*/
    void printTransaction();

    ~transactionData();
};

class transactionNode {
    private:
    transactionNode *next;
    transactionData *data;

    public:
    transactionNode(transactionData *transData);

    transactionNode* addTransaction(transactionData *transData);

    transactionData* getTransData();

    transactionNode* getNextTransPtr();

    void PrintList();

    ~transactionNode();
};

/*ftiaxnw pinaka wste na elegxw unique transction 
id alla kai gia th teleytaia wra,epishs oi destructors de 
ginetai na kanoyn kai oi 2(sender,receiver) delete[] to idio 
transaction node gt 8a pairnw error gia double free*/
class transactions{
    private:
    transactionData **alltransactions;
    int trCounter;
    time_t recentTime;
    char *timestr, *datestr;/*strings to retun, values change*/

    public:
    transactions();

    /*adds new transactions to array*/
    void addTransaction(transactionData *transaction);

    /*checks if given time is most recent*/
    bool newerTime(char* datestr, char* timestr);

    /*checks if given transaction id is unique*/
    bool uniqueTrID(char* transactionID);

    /*converts date and time from strings to time_t, 
    *if an argument is null takes local time */
    time_t static convertTime(char* datestr,char* timestr);

    /*extract string from ctime givev*/
    char *extractTimestr(time_t ctime);
    char *extractDatestr(time_t ctime);

    /*print for check values/debug*/
    void printTransactions();

    ~transactions();
};
#endif

