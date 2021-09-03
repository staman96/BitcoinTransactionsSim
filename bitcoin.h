#ifndef bitcoin_h
#define bitcoin_h
#include <string.h>
#include <iostream>
#include "wallet.h"
#include "transaction.h"

class wallet;
class transactionData;

class bitcoin{

    private:
    int bitcoinID;
    double value;
    wallet *wl;

    public:
    bitcoin(double val, int id, wallet* w);
    int getbitcoinID();
    bitcoin* getBitcoinPtr();
    double getBcValue();
    wallet *getWallet();
    void print();
    ~bitcoin();

};

class bitcoinNode{

    private:
    bitcoinNode *left, *right;
    bitcoin *bc;
    transactionData *tranfer;

    public:
    bitcoinNode(bitcoin *coin);/*used to initialize bitcoin node array, or receiver*/
    bitcoinNode(bitcoin *coin, transactionData *trans);/*for new left node only*/

    /*makes new left node to current and bitcoin data and returns it*/
    bitcoinNode* addleftBcNode(transactionData *trans, double valForTrans);

    /*makes new right node to current and bitcoin data and returns it*/
    bitcoinNode* addrightBcNode(transactionData *trans, double valForTrans);

    bitcoinNode* getLeftLeaf();
    bitcoinNode* getRightLeaf();

    /*recursively traversing tree and printing all the different 
    *transactions the specific bitcoin took part off
    */
    void traceCoin();

    /*returns unspent value*/
    double getUnspent();

    /*returns the no of transactions participated, noOfTrans must be 0 when called*/
    void transactionsPart(int &noOfTrans);

    void print(int &depth);

    bitcoinNode *getBitcoinNodePtr();
    bitcoin *getBitcoin();

    ~bitcoinNode();
};

class bitcoins{

    private:
    bitcoinNode **bitcoinRoots;
    int bcRootCounter;

    public:
    bitcoins();

    void addBitcoinRoot(bitcoin *bc);

    bitcoin *getBitcoin(int id);

    /*checks if bicoin id is unique,reutns true if it is*/
    bool uniqueBc(int id);

    /*returns the root node of bitcoin id*/
    bitcoinNode* getBitcoinRoot(int id);

    void printForCheck();

    ~bitcoins();
};

#endif