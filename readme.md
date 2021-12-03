Manolas Stamatios DIT Operating Systems 2019 Project 1

General:

The execution command is: 

    ./runelection -i input_output/18records.csv -o input_output/output.csv -n 3 
* -i is the input file (voter records).
* -o is the output file (optional, it creates the file output.csv in input_output folder automatically).
* -n is the number of changes needed to re-initialize bloom filter (default value is 5).
* The compilation is done with the 'make' command, and 'make clean' deletes all executables and .ο files.
* I also used Valgrind to check for memory leaks with the command:
    
    make clean && make && ./runelection -i input_output/18records.csv -o input_output/output.csv -n 3
    
 
After input data are given and processed to eliminate duplicates, the user has some instuctions from stdin, all instructions are followed with ":" and then the input data.

   * search bloom filter for key
   
    lbf key:
   * search red black tree for entry and print it
   
    lrb key:
   * insert new voter
   
    ins record:
   * search data structures for entry and print it
   
    find key:
   * delete entry from red black tree
   
    delete key:
   * vote key voter
   
    vote key:
   * vote keys voters from file
   
    load fileofkeys:
   * print all voters and total votes
   
    voted:
   * print all votes and from specific  postal code
   
    voted postcode:
   * print percentages of votes completed per postal code
   
    votedperpc:


NOTES:
A bloom filter and a RedBlack tree are the main data structures used.
