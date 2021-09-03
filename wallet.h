#ifndef wallet_h
#define wallet_h
#include "bitcoin.h"
#include <iostream>
#include <string.h>

class bitcoinNode;
class bitcoin;

class wallet{
    private:
    char* walletID;
    double balance;
    bitcoinNode **bitcoins;
    int bcCounter;

    public:
    wallet(char *id);

    double getBalance();

    char *getWalletID();

    /*it is called to add new bitcoin or a new leaf of an existing(buyer)*/
    void addBitcoinN(bitcoinNode *bcN);

    /*it is called after a transaction to return  bitcoins in value*/
    bitcoinNode** getBCsValued(double value,int &length);

    /*Prints wallet data..needs new line*/
    void printdata();

    ~wallet();
};

class wallets{
    private:
    wallet **walletsArray;
    int walletCounter;

    public:
    /*constructor of wallets array*/
    wallets();

    /*adds new wallet*/
    void addWallet(wallet *wl);

    /*returns wallet*/
    wallet* getWallet(char* id);

    /*checks if the wallet id is unique*/
    bool uniqueWl(char* id);
    
    /*prints data for check/debug*/
    void printForCheck();

    /*Destructor*/
    ~wallets();
};
#endif