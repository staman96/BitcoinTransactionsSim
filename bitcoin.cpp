#include <string.h>
#include <iostream>
#include "bitcoin.h"
#include "wallet.h"
#include "transaction.h"

using namespace std;

    /*-----------------------------bitcoin data---------------------------------*/
    
    bitcoin::bitcoin(double val, int id, wallet* w):
    value(val),
    bitcoinID(id),
    wl(w){
        cout << "Bitcoin " << this->bitcoinID << " just constructed with value: " << this->value << endl;
    }

    int bitcoin::getbitcoinID(){
        return this->bitcoinID;
    }

    bitcoin* bitcoin::getBitcoinPtr(){
        return this;
    }

    wallet* bitcoin::getWallet(){
        return this->wl;
    }

    double bitcoin::getBcValue(){
        return this->value;
    }

    void bitcoin::print(){
        cout << "BitcoinID: "  << this->bitcoinID << " Value: " << this->value
        << " UserID: " << this->getWallet()->getWalletID();
    }

    bitcoin::~bitcoin(){
        cout << "Bitcoin destroyed!" << endl;
    }


    /*-----------------------------bitcoin node-------------------------------------*/
    bitcoinNode::bitcoinNode(bitcoin *coin){
        this->bc = coin;
        this->left = NULL;
        this->right = NULL;
        this->tranfer = NULL;
        /*adds this bitcoinNode to wallet*/
        this->bc->getWallet()->addBitcoinN(this);
        cout << "A new node created." << endl;
    }

    bitcoinNode::bitcoinNode(bitcoin *coin, transactionData *trans){
        this->bc = coin;
        this->left = NULL;
        this->right = NULL;
        this->tranfer = trans;
        /*adds this bitcoinNode to wallet*/
        this->bc->getWallet()->addBitcoinN(this);
        cout << "A new node created." << endl;
    }

    bitcoinNode* bitcoinNode::addleftBcNode(transactionData *trans, double valForTrans){
        cout  << "New left leaf" << endl;
        bitcoin *b = new bitcoin(valForTrans,this->getBitcoin()->getbitcoinID(),trans->getReceiver());
        this->left = new bitcoinNode(b,trans);
        return this->left;
    }

    bitcoinNode* bitcoinNode::addrightBcNode(transactionData *trans, double valForTrans){
        cout  << "New right leaf" << endl;
        double leftoverValue = this->getBitcoin()->getBcValue()-valForTrans;
        bitcoin *b = new bitcoin(leftoverValue, this->getBitcoin()->getbitcoinID(), trans->getSender());
        this->right = new bitcoinNode(b);
        return this->right;
    }

    void bitcoinNode::traceCoin(){
        if (this->tranfer != NULL){
            this->tranfer->printTransaction();
            cout << endl;
        }
        if (this->left != NULL)
            this->left->traceCoin();
        if (this->right != NULL)
            this->right->traceCoin();
    }

    double bitcoinNode::getUnspent(){
        double unspentValue = 0;
        if(this->right == NULL){
            if (this->left == NULL){
                unspentValue = this->getBitcoin()->getBcValue();
            }
        }
        else
        {
            unspentValue = this->right->getUnspent();
        }
        return unspentValue;

    }

    void bitcoinNode::transactionsPart(int &noOfTrans){
        if (this->tranfer != NULL){
            noOfTrans++;
        }
        if (this->left != NULL)
            this->left->transactionsPart(noOfTrans);
        if (this->right != NULL)
            this->right->transactionsPart(noOfTrans);
    }

    void bitcoinNode::print(int &depth){
        for (int i = 0; i< depth;i++) cout << "  ";
        this->bc->print();
        cout << endl;
        depth++;
        if(this->left != NULL){
            cout << "   " << "left" << "   ";
            this->left->print(depth);
        }
        if(this->right != NULL){ 
            cout << "   " << "right" << "   ";
            this->right->print(depth);
        }
        
    }

    /*return object pointer*/
    bitcoinNode *bitcoinNode::getBitcoinNodePtr(){
        return this;
    }

    /*returns bitcoin*/
    bitcoin* bitcoinNode::getBitcoin(){
        return this->bc;
    }

    bitcoinNode::~bitcoinNode(){
        cout << "Destroying node!" << endl;
        delete this->bc;
        if(this->left  != NULL)
        delete this->left;
        if(this->right != NULL)
        delete this->right;
        
    }


    /*-----------------------------all bitcoins------------------------------------*/
    /*einai dynamikos pinakas apo 
     *ta root twn bitcoinNodes
     */
    bitcoins::bitcoins(){
        cout << "Bitcoins array initialized." << endl;
        this->bcRootCounter = 0;
        this->bitcoinRoots = NULL;
    }
   
    void bitcoins::addBitcoinRoot(bitcoin *bc){
        this->bcRootCounter++;
        
        /*make new array with one extra slot*/
        bitcoinNode **temp = new bitcoinNode*[this->bcRootCounter];
    

        /*initialize new node(the last one)*/
        bitcoinNode *newNode = new bitcoinNode(bc);

        /*pass the reference of new node into array @ last pos*/
        temp[this->bcRootCounter-1] = newNode;

        /*make new array to point to old elements*/
        for (int i = 0; i<this->bcRootCounter-1;i++){
            temp[i] = this->bitcoinRoots[i];
        }
        
        /*free old space*/
        if(this->bitcoinRoots != NULL)
            delete[] this->bitcoinRoots;

        /*make bitcoinroots pointing to new array*/
        this->bitcoinRoots = temp;
        
    }

    /*returns bitcoin object with specific id*/
    bitcoin *bitcoins::getBitcoin(int id){

        for (int i = 0; i<this->bcRootCounter; i++){
            bitcoin *currentBitcoin = (bitcoinRoots[i])->getBitcoin();
            if (currentBitcoin->getbitcoinID() == id){
                return currentBitcoin;
            }
        }
        /*returns NULL if bitcoin with specific id doesn't exist*/
        return NULL;
    }

    /*function used for debug*/
    void bitcoins::printForCheck(){
        for (int i = 0; i<this->bcRootCounter; i++){
            // bitcoin *currentBitcoin = (this->bitcoinRoots[i])->getBitcoin();
            cout << "--------------------------------------------------" << endl;
            cout << "Bitcoin[" << i << "] = " ;
            // currentBitcoin->print();
            // cout << endl;
            int aman = 0;
            this->bitcoinRoots[i]->print(aman);
            //cout << endl; 
        }
    }
    
    bool bitcoins::uniqueBc(int id){
        for (int i = 0; i<this->bcRootCounter; i++){
            if(this->bitcoinRoots[i]->getBitcoin()->getbitcoinID() == id) 
                return false;
        }
        return true;
    }

    bitcoinNode* bitcoins::getBitcoinRoot(int id){
        int i;
        for (i = 0; i< this->bcRootCounter; i++){
            if (this->bitcoinRoots[i]->getBitcoin()->getbitcoinID() == id)
                return this->bitcoinRoots[i];
        }
        /*return NULL if bitcoin doesnt exist*/
        return NULL;
    }

    /*bitcoins distractor*/
    bitcoins::~bitcoins(){
        cout << "Bitcoins array destroyed!" << endl;
        for (int i = 0; i<this->bcRootCounter; i++){
            if (this->bitcoinRoots[i] != NULL)
                delete this->bitcoinRoots[i];
        }
        if (this->bitcoinRoots != NULL)
            delete [] this->bitcoinRoots;/*ok*/
        
    }


