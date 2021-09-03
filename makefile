CC = g++

bitcoin: main.o wallet.o bitcoin.o transaction.o hashTable.o validations.o
	$(CC) -lm -g main.o wallet.o bitcoin.o transaction.o hashTable.o validations.o -o bitcoin

bitcoin.o: bitcoin.cpp bitcoin.h wallet.h transaction.h
	$(CC) -c -g bitcoin.cpp

wallet.o: wallet.cpp wallet.h bitcoin.h transaction.h
	$(CC) -c -g wallet.cpp

transaction.o: transaction.cpp transaction.h wallet.h
	$(CC) -c -g transaction.cpp

hashTable.o: hashTable.cpp hashTable.h transaction.h
	$(CC) -c -g hashTable.cpp

validations.o: validations.cpp validations.h bitcoin.h transaction.h wallet.h hashTable.h
	$(CC) -c -g validations.cpp

main.o: main.cpp wallet.h bitcoin.h transaction.h hashTable.h validations.h
	$(CC) -c -g main.cpp

clean:
	rm -rvf *.o bitcoin
