#include "bitcoin.h"
#include <iostream>
#include "wallet.h"
#include <string.h>

using  namespace std;

    /*----------------------------wallet--------------------------*/

    wallet::wallet(char* id){
        this->balance = 0;
        this->bcCounter = 0;
        this->bitcoins = NULL;
        int name = strlen(id);
        this->walletID = new char[name+1];
        strcpy(this->walletID, id);
        cout << "A wallet was constructed with id: " << this->walletID << endl;
    }

    double wallet::getBalance(){
        return this->balance;
    }

    char *wallet::getWalletID(){
        return this->walletID;
    }

    /*it is called to add new bitcoin*/
    void wallet::addBitcoinN(bitcoinNode *bcN){
        
        this->bcCounter++;

        /*make new array with one extra slot*/
        bitcoinNode **temp = new bitcoinNode*[this->bcCounter];

        /*pass the reference of new node into array @ last pos*/
        temp[this->bcCounter-1] = bcN;

        /*make new array to point to old elements*/
        for (int i = 0; i<this->bcCounter-1;i++){
            temp[i] = this->bitcoins[i];
        }

        /*free old space*/
        if(this->bitcoins != NULL)
            delete[] this->bitcoins;
   

        /*make bitcoins pointing to new array*/
        this->bitcoins = temp;

        /*updating balance*/
        double val = temp[this->bcCounter-1]->getBitcoin()->getBcValue();
        this->balance += val;


    }

    
    bitcoinNode** wallet::getBCsValued(double value,int &length){
        /*if bitcoin value is 0 it doesnt put the bitcoin in array,
        also at every transaction must remove bitcoinNOdes from array 
        and add new ones updated if there is remainders*/
        length = 0;
        double sum = 0;
        int i = this->bcCounter-1;

        /*find the bitcoins needed to complete requested value*/
        while(sum < value){
            length++;
            sum += this->bitcoins[i]->getBitcoin()->getBcValue();
            i--;
        }

        /*make new arrays with desirable lengths*/
        bitcoinNode **returnBCs = new bitcoinNode*[length];
        bitcoinNode **temp = new bitcoinNode*[this->bcCounter-length];

        /*make new arrays to point to old elements*/
        for (i = 0; i < this->bcCounter-length; i++){
            temp[i] = this->bitcoins[i];
        }

        for (i = this->bcCounter-length; i < this->bcCounter; i++){
            /*updating balance*/
            this->balance -= this->bitcoins[i]->getBitcoin()->getBcValue();
            returnBCs[i-this->bcCounter + length] = this->bitcoins[i];
        }

        /*updating quantity of bitcoins*/
        this->bcCounter = this->bcCounter - length;

        /*free old space*/
        delete[] this->bitcoins;

        /*make bitcoins pointing to new array*/
        this->bitcoins = temp;

        return returnBCs;
        
    }

    void wallet::printdata(){
        cout << this->walletID << " has " << this->bcCounter << 
        " bitcoin parts with total balance of: " << this->balance; 
    }

    wallet::~wallet(){
        cout << "A wallet was destroyed with " << this->bcCounter 
        << " bitcoins." << endl;
        delete[] this->walletID; 
        delete[] this->bitcoins;
    }


    /*--------------------------------all wallets--------------------------------*/

    wallets::wallets(){
        cout << "Wallets array initialized." << endl;
        this->walletCounter = 0;
        this->walletsArray = NULL;
    }

    void wallets::addWallet(wallet *wl){
        this->walletCounter++;

        /*make new array with one extra slot*/
        wallet **temp = new wallet*[this->walletCounter];
        

        /*pass the pointer of the new wallet into array @ last pos*/
        temp[this->walletCounter-1] = wl;
        

        /*make new array to point to old elements*/
        for (int i = 0; i<this->walletCounter-1;i++){
            temp[i] = this->walletsArray[i];
        }

        /*free old space*/
        if(this->walletsArray != NULL)
            delete[] this->walletsArray;        

        /*make bitcoinroots pointing to new array*/
        this->walletsArray = temp;
    }

    wallet* wallets::getWallet(char* id){
        if (id == NULL) return NULL;
        for (int i = 0; i<this->walletCounter; i++){
            wallet *currentWallet = this->walletsArray[i];
            if (strcmp(currentWallet->getWalletID(),id) == 0)
                return currentWallet;
        }
        /*returns NULL if wallet with specific id doens't exist*/
        return NULL;
    }

    bool wallets::uniqueWl(char* id){
        for (int i = 0; i<this->walletCounter; i++){
            if (strcmp(this->walletsArray[i]->getWalletID(),id) == 0)
                return false;
        }
        return true;
    }

    void wallets::printForCheck(){
        for (int i = 0; i<this->walletCounter; i++){
            wallet *currentWallet = this->walletsArray[i];
            cout << "Wallet[" << i << "] = " ;
            currentWallet->printdata();
            cout << endl;
        }
    }

    wallets::~wallets(){
        cout << "Wallets array destroyed!" << endl;
        for (int i = 0; i<this->walletCounter; i++){
            if (this->walletsArray[i] != NULL)
                delete this->walletsArray[i];
        }
        delete[] this->walletsArray;
    }