#ifndef hashTable_h
#define hashTable_h
#include <string.h>
#include "transaction.h"


class records
{
    private:
    transactionNode* transactionsRoot;
    char* walletID;
    
    public:
        /*initializing root node and wallet id*/
        records(char* id,transactionNode* root);

        /*returns root node*/
        transactionNode* getRoot();

        char* getID();

        void printTransactions(int index);

        ~records();
};

class bucketNode{
    private:
        bucketNode *overflowNode;
        unsigned short bucketSize, currentSize, recCounter;
        records **recs;
        
        /*when called calculates the current bucket size*/
        void setcurrentSize();
        

    public:
        bucketNode(unsigned short buckSize);

        /*recursively tries to add record with specific id*/
        void addRecord(char* id,transactionNode* root);

        /*checks if given bucket size is enough*/
        bool static enoughBuckbytes(int buckSize);

        /*returns root node*/
        transactionNode* get(char* id);

        void print(int length);

        ~bucketNode();

};

class hashTable{
    private:
        bucketNode **bucketsArray;
        int length,prime;
        unsigned short bucketSize;

        /*hash function*/
        int hash(char* id);

    public:
        hashTable(int len,int bucksize);

        /*puts new  key and passes 1st transaction node(root)*/
        void put(char* id,transactionNode *transaction);

        /*returns nullptr if no record exists with this wallet id*/
        transactionNode* get(char *wID);

        void printToCheck();
        
        ~hashTable();

};

#endif