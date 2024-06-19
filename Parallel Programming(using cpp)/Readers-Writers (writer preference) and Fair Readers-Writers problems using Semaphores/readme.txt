
Input file(for both Programs):
    (as given in problem statement)
- needs to be named 'inp-params.txt'
- It contains (space or newline seperated in this same order)
    - nw: the number of writer threads, 
    - nr: the number of reader threads, 
    - kw: the number of times each writer thread tries to enter the CS, 
    - kr: the number of times each reader thread tries to enter the CS, 
    - μCS: Average Critical Section Delay of Exponential Distribution
    - μRem: Average Remainder Section Delay of Exponential Distribution


Executing:
- Readers-Writers (writer preference)
    - g++ rw-CS22BTECH11061.cpp
    - ./a.out
    - we get the output file
- Fair Readers-Writers
    - g++ frw-CS22BTECH11061.cpp
    - ./a.out
    - we get the output file


Output Files:
- one output file for each Algorithm
- named RW-log.txt and FairRW-log.txt
- each output file consists of 
    - Execution Log
    - Average and Worst Reader and Writer Time(At the end of the File)