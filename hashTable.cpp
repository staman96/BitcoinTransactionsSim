#include <string.h>
#include <iostream>
#include "hashTable.h"

using namespace std;

/*-------------------------------------records-----------------------------------*/
records::records(char* id, transactionNode* root){
    int length = strlen(id);
    this->walletID = new char[length+1];
    strcpy(this->walletID,id);
    transactionsRoot =root;
    cout << "New record was created." << endl;
}

transactionNode*  records::getRoot(){
    return this->transactionsRoot;
}

char* records::getID(){
    return this->walletID;
}

void records::printTransactions(int index){
    cout << "Printing transactions from record " << index
        << " with wallet id " << this->walletID << endl;
    this->transactionsRoot->PrintList();
}

records::~records(){
    delete[] this->walletID;
    delete this->transactionsRoot;
}


/*-------------------------------------bucketNode---------------------------------*/
bucketNode::bucketNode(unsigned short buckSize):bucketSize(buckSize){
    this->overflowNode = NULL;
    this->recs = NULL;
    this->recCounter = 0;
    this->setcurrentSize(); 
    cout << "New bucket created." << endl; 
}

void bucketNode::setcurrentSize(){
    this->currentSize =  (sizeof(unsigned short)*3) + sizeof(bucketNode*) 
        + sizeof(records**) + (sizeof(records)*this->recCounter)
        + (sizeof(records*)*this->recCounter);
}

bool bucketNode::enoughBuckbytes(int bucksize){
    int minSize = 0;
    /*calculates minimun size*/
    minSize =  (sizeof(unsigned short)*3) + sizeof(bucketNode*) + 
        sizeof(records**) + sizeof(records*) + sizeof(records);
    if (bucksize >= minSize){
        cout << bucksize << "b is enough bucket size." << endl;
        return true;
    }
    else{
        cout << "Minimum bucket size is " << minSize << endl;
        return false;
    }
}

transactionNode* bucketNode::get(char* id){
    bucketNode *temp = this;
    /*first accesses each bucket from list*/
    while(temp != NULL){
        for(int i = 0; i< temp->recCounter; i++){
            if (strcmp(temp->recs[i]->getID(),id) == 0){
                /*function returns root transaction node*/
                return this->recs[i]->getRoot();
            }
        }
        temp = temp->overflowNode;
    }
    /*if program wont exit means theres no record of that id*/
    return NULL;
}

void bucketNode::addRecord(char* id,transactionNode *root){
    this->setcurrentSize();
    /*if there is enough space in bucketNode add record*/
    if (this->bucketSize - this->currentSize >= sizeof(records) + sizeof(records*)){
        this->recCounter++;

        /*make new array with one extra slot*/
        records** temp = new records*[this->recCounter];

        /*initialize new node(the last one)*/
        records* newRec = new records(id,root);

        /*pass the reference of new node into array @ last pos*/
        temp[this->recCounter-1]  = newRec;

        /*make new array to point to old elements*/
        for (int i = 0; i<this->recCounter-1;i++){
            temp[i] = this->recs[i];
        }

        /*free old array pointers if recs is inialized*/
        if (this->recs != NULL)
            delete [] this->recs;

        /*make recs pointing to new array*/
        this->recs = temp;
    }
    /*else go to next bucket to add record, allocate bucket if needed*/
    else{
        if (this->overflowNode == NULL) {
            this->overflowNode = new bucketNode(this->bucketSize);
        }
        this->overflowNode->addRecord(id,root);
    }
}

void bucketNode::print(int length){
    cout << "BUCKET No " << length << endl;
    length++;
    for (int i = 0;i < this->recCounter; i++){
        this->recs[i]->printTransactions(i);
    }
    if (this->overflowNode != NULL)
        this->overflowNode->print(length);
}

bucketNode::~bucketNode(){
    cout << "Destroying bucket." << endl;
    for (int i = 0;i < this->recCounter; i++){
        delete this->recs[i];
    }
    delete [] recs;
    if (this->overflowNode != NULL)
        delete this->overflowNode;
}


/*------------------------------------hashtable------------------------------------*/
hashTable::hashTable(int len, int bucksize):length(len){
    this->prime = 631;
    this->bucketSize  = (unsigned short)bucksize;

    /*initialing array while not creating new buckets*/
    this->bucketsArray = new bucketNode*[this->length];
    for (int i = 0;i<this->length;i++){
        this->bucketsArray[i] = NULL;
    }
    cout << "A hash table with " << this->length << " buckets was created." << endl;
}

void hashTable::put(char* id,transactionNode *transaction){
    /*hashes  id*/
    int index = hash(id);

    /*makes new bucket if its not initialized*/
    if(this->bucketsArray[index] == NULL){
        this->bucketsArray[index] = new bucketNode(this->bucketSize);
    }
    this->bucketsArray[index]->addRecord(id,transaction);
}

transactionNode* hashTable::get(char* id){
    int index = hash(id);

    /*makes new bucket if its not initialized and returns null because 
    *there won't be a record inside*/
    if(this->bucketsArray[index] == NULL){
        this->bucketsArray[index] = new bucketNode(this->bucketSize);
        return NULL;
    }
    /*else calls to search the bucket*/
    else return this->bucketsArray[index]->get(id);
}

int hashTable::hash(char* id){
    int asciiSum = 0,step = 0;
    /*asciiSuming the ascii values*/
    for(int i = 0;i < strlen(id);i++){
	    asciiSum += id[i];
    }

    /*modding with prime number if bigger than hashTable length*/
    if(this->prime > this->length){
        asciiSum = asciiSum%this->prime;
    }
    return asciiSum%this->length;
}

void hashTable::printToCheck(){
    for (int i = 0; i < this->length; i++){
        if(this->bucketsArray[i] != NULL){
            cout << endl << "Printing data from position " << i << endl;
            this->bucketsArray[i]->print(0);
        }
    }
}

hashTable::~hashTable(){
    cout <<  "Destroying hash table." << endl;
    for (int i = 0; i < this->length; i++){
        if(this->bucketsArray[i] != NULL){
            delete this->bucketsArray[i];
        }
    }
    delete [] this->bucketsArray;
}


