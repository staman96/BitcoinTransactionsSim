#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string.h>
#include "bitcoin.h"
#include "wallet.h"
#include "transaction.h"
#include "hashTable.h"
#include "validations.h"


using namespace std;

int main(int argc,char* argv[]) 
{   
    cout << endl;

    int bucketSize = 50,senderHashtable1NumOfEntries = 10 ,
    receiverHashtable1NumOfEntries = 10;
    double bitCoinValue = 100.0;
    char **bitCoinBalancesFile = NULL, **transactionsFile = NULL;             

    /*argument checking*/
    int i = 1;
    while(i<argc){
        if(strcmp(argv[i],"-a") == 0){
            //bitcoinbalancefile
            i++;
            bitCoinBalancesFile = &argv[i];
            //strcpy(bitCoinBalancesFile,argv[i]);
        }
        else if(strcmp(argv[i],"-t") == 0){
           //transactionsFile
           i++;
           transactionsFile = &argv[i]; 
        }
        else if (strcmp(argv[i], "-v") == 0) {
            //bitCoinValue
            i++;
            bitCoinValue = atof(argv[i]);
            cout << "Bitcoin value is: " << bitCoinValue << endl;
        }
        else if(strcmp(argv[i],"-h1") == 0){
           //senderHashtableNumOfEntries
           i++;
           senderHashtable1NumOfEntries = atoi(argv[i]);
           cout << "sender Hashtable1 Num Of Entries is: " << senderHashtable1NumOfEntries << endl;
        }
        else if (strcmp(argv[i],"-h2") == 0) {
            //receiverHashtableNumOfEntries
            i++;
            receiverHashtable1NumOfEntries = atoi(argv[i]);
            cout << "receiver Hashtable1 Num Of Entries is: " << receiverHashtable1NumOfEntries << endl;
        }
        else if (strcmp(argv[i],"-b") == 0) {
            //bucketSize
            i++;
            bucketSize = atoi(argv[i]);
            cout << "Bucket size is: " << bucketSize << endl;
        }
        else{
            cout << "Invalid parameter" << endl;
        }

        i++;

    }

    if (bitCoinBalancesFile == NULL){
        cout << "Bitcoin Balance file argument wasn't given." << endl
        << "Terminating process...";
        return 4;
    }

    /*checks if bucket size is enough*/
    if (!bucketNode::enoughBuckbytes(bucketSize)){
        cout << "The execution will continue with default bucket size of 50" << endl;
        bucketSize = 50;
    }

    wallets *wls = new wallets();
    bitcoins *bs = new bitcoins();
    transactions *trArray = new transactions();
    hashTable *senders = new hashTable(senderHashtable1NumOfEntries, bucketSize);
    hashTable *receivers = new hashTable(receiverHashtable1NumOfEntries, bucketSize);
    validations *vls = new validations(bs,trArray,wls,senders,receivers);

    /*temporary variables*/
    bitcoin *b;
    char * pch;
    wallet *wl;
    size_t length = 512;
    char line[length];/*its probably too much*/

    /*open file to read*/
    FILE* bcBalfile = fopen(*bitCoinBalancesFile,"r");
    
    /*check if file is open*/
    if (bcBalfile != NULL){
        /*reads file line by line*/
        while ( fgets(line, length, bcBalfile) != NULL ){  
            /*split line and getting read of extra chars like spaces and new lines*/
            pch = strtok(line," ,.\n\t");/*extracting name*/

            /*checks if wallet id is unique, if true makes new, if not add bitcoins to existing wallet*/
            if (wls->uniqueWl(pch)){    
                wl = new wallet(pch);/*making new wallet*/
                pch = strtok (NULL, " .,\n\t");/*1st bitcoin if user has bitcoins*/
                while (pch != NULL){
                    
                    /*checks if bitcoin id is unique, if true makes new*/
                    if (bs->uniqueBc(atoi(pch))){
                        b = new bitcoin(bitCoinValue, atoi(pch), wl);/*making new bitcoin*/
                        bs->addBitcoinRoot(b);/*adding the new bitcoin to the array*/
                    }
                    else{
                        cout << "Bitcoin " << pch << " already exists." << endl;
                        // return 5; /*stops the process if bitcoin exists*/
                    }
                    /*step & and checks for next bitcoin or exits*/
                    pch = strtok (NULL, " .,\n\t");
                }

                /*adding wallet to array of wallets*/
                wls->addWallet(wl);
            }
            else{
                wl = wls->getWallet(pch);
                cout << "User with wallet ID: " << pch << " already exists. The bitcoins are going to be added." << endl;
            }
            
        }
        fclose(bcBalfile);/*closing file*/
    }
    /*prints message if balances file wont found or couldnt be open ans terminates execution*/
    else {
        cout << "Unable to open bitCoinBalances file!!" << endl;
        cout << "Bitcoin network can't work without wallets and bitcoins being initialized." << endl;
        delete bs;
        delete wls;
        delete trArray;
        delete vls;
        delete senders;
        delete receivers;
        return 4;
    }
    cout << endl;

    /*making transactions from file*/
    vls->transactionsFromFile(*transactionsFile);

    /*user input*/
    streamsize inputLength = 512;
    char* input = new char[512];

    do{/*check for double \n*/
        cout << endl << "Waiting input..." << endl << "-/";
        cin.getline(input,inputLength);
    }while(strcmp(input,"\0") == 0);

    pch = strtok(input," ,.\n\t;");
    /*read stdin oso input != "exit"*/
    while(strcmp(pch,"exit") != 0){

        /*instuctions from stdin*/
        if(strcmp(pch,"traceCoin") == 0){
            if (vls->tracecoin(input) == false)
                cout << "false input!!! bitcoin IDs consists only of numbers" << endl;
        }
        else if(strcmp(pch,"bitCoinStatus") == 0){
            if (vls->bitCoinStatus(input) == false)
                cout << "false input!!! bitcoin IDs consists only of numbers" << endl;
        }
        else if (strcmp(pch,"walletStatus") == 0) {
            vls->printWalletStatus(input);
        }
        else if(strcmp(pch,"findPayments") == 0){
           vls->findPayments(input);
        }
        else if (strcmp(pch,"findEarnings") == 0) {
            vls->findEarnings(input);
        }
        else if (strcmp(pch,"requestTransaction") == 0) {
            vls->requestTransaction(input);
        }
        else if (strcmp(pch,"requestTransactions") == 0) {
            vls->requestTransactions(input);
        }
        else{
            cout << "Unknown Instuction" << endl;
        }


        /*step*/
        do{
            cout << endl << "Waiting input..." << endl << "-/";
            cin.getline(input,inputLength);
        }while(strcmp(input,"\0") == 0);
        pch = strtok(input," ,.\n\t;");
    }
    

    /*printing all data to check values*/
    wls->printForCheck();
    cout << endl;
    bs->printForCheck();
    trArray->printTransactions();
    cout << endl << "SENDERS HASHTABLE PRINT**************************" << endl;
    senders->printToCheck();
    cout << endl << "RECEIVERS HASHTABLE PRINT************************" << endl;
    receivers->printToCheck();
    
    /*free allocated space*/
    delete[] input;
    delete bs;
    delete wls;
    delete trArray;
    delete vls;
    delete senders;
    delete receivers;

    return 0;
}

