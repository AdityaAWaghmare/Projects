#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <mutex>
#include <chrono>
#include <cmath>
#include <iomanip> // for setw
#include <algorithm>
#include <pthread.h>
#include <atomic>

using namespace std;


int n, k, rowInc;//N, K and rowInc
int **a;//input array A
int **result;//Result array

int rowsAssigned = 0;//number of rows already assigned to threads or counter C for TAS, CAS, and BCAS

void prepare_2d_arrays(){
    // Using dynamic memory allocation for 2D arrays
    a = new int*[n];
    result = new int*[n];

    for(int i = 0; i < n; ++i){
        a[i] = new int[n];
        result[i] = new int[n];
    }
}

//computes one element of result square matrix
void matrix_mult(int row, int col){
    int r = 0;
    for(int i = 0; i < n; i++) {
        r += a[row][i] * a[i][col];
    }
    result[row][col] = r;
}

//computes one row of result square matrix
void matrix_row_mult(int row){
    for(int col = 0; col < n; col++) {
        matrix_mult(row, col);
    }
}

//---------------------------------------------------------------------------------------------------------------------------
//TAS

atomic_flag lock_tas = ATOMIC_FLAG_INIT;//TAS lock, intial value is unlocked

void dynamicChunks_TAS(int id){//id is thread id 

    while(true){//while not all rows are computed for square matrix

        //aquire TAS lock
        while (lock_tas.test_and_set(memory_order_acquire)) {
            // Spin until lock is acquired
        }

        /*CRITICAL SECTION*/

        //getting rows to work on
        int startRow = rowsAssigned;
        rowsAssigned += rowInc;

        //release lock or unlock
        lock_tas.clear(memory_order_release);

        /*REMAINDER SECTION*/

        //if assigned row is more than matrix rows, then break
        if(startRow >= n) break;

        //compute assigned rows
        for(int i = 0; i < rowInc && i < n; i++) {
            matrix_row_mult(startRow + i);
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------------
//CAS

atomic<bool> lock_cas(false);//CAS lock, initial value is unlocked

void dynamicChunks_CAS(int id){//id is thread id 

    while(true){//while not all rows are computed for square matrix

        //aquire CAS lock
        bool aquired = true;
        bool available = false;

        while (!lock_cas.compare_exchange_strong(available, aquired)){
            available = false;//to keep available variable value false
            // Spin until lock is acquired
        }

        /*CRITICAL SECTION*/
        
        //getting rows to work on
        int startRow = rowsAssigned;
        rowsAssigned += rowInc;

        //release lock
        lock_cas.store(available);

        /*REMAINDER SECTION*/

        //if assigned row is more than matrix rows, then break
        if(startRow >= n) break;

        //compute assigned rows
        for(int i = 0; i < rowInc && i < n; i++) {
            matrix_row_mult(startRow + i);
        }
    }
}

//-----------------------------------------------------------------------------------------------------------------------------
//Bounded CAS

bool *waiting;//waiting list of threads for critical section
atomic_flag lock_bcas = ATOMIC_FLAG_INIT;//BCAS lock, intial value is unlocked

void dynamicChunks_BCAS(int i){//i is thread id 

    while(true){//while not all rows are computed for square matrix

        //start waiting to aquire lock for critical section
        waiting[i] = true;
        bool key = true;

        //aquire lock
        while (waiting[i] && key){//either aquire lock ourselves or some other process assigns it to us
            key = lock_bcas.test_and_set(memory_order_acquire);
        }
        //stop waiting once lock aquired
        waiting[i] = false;


        /*CRITICAL SECTION*/
        
        //getting rows to work on
        int startRow = rowsAssigned;
        rowsAssigned += rowInc;
        
        //find which other process is waiting for lock
        int j = (i + 1) % k;
        while ((j != i) && !waiting[j]){
            j = (j + 1) % k;
        }
        //if no process waiting for lock release it
        if (j == i){
            //release lock
            lock_bcas.clear(memory_order_release);
        }
        //if some process waiting for lock, pass the lock to it
        else{
            waiting[j] = false;
        }
        
        /*REMAINDER SECTION*/

        //if assigned row is more than matrix rows, then break
        if(startRow >= n) break;

        //compute assigned rows
        for(int i = 0; i < rowInc && i < n; i++) {
            matrix_row_mult(startRow + i);
        }
    }
}

//-----------------------------------------------------------------------------------------------------------------------------
//Atomic Increment 

atomic<int> currentRow(0);//number of rows already assigned to threads or counter C for Atomic increment

void dynamicChunks_atomic_increment(int id){//id is thread id 

    while(true){//while not all rows are computed for square matrix

        //atomically getting rows to work on
        int startRow = currentRow.fetch_add(rowInc);

        /*REMAINDER SECTION*/

        //if assigned row is more than matrix rows, then break
        if(startRow >= n) break;

        //compute assigned rows
        for(int i = 0; i < rowInc && i < n; i++) {
            matrix_row_mult(startRow + i);
        }
    }
}

//-----------------------------------------------------------------------------------------------------------------------------
//resets the element of result matrix to 0 for testing purpose
void reset_result(){
    for(int i=0; i<n; i++){
        for(int j=0; j<n; j++) result[i][j] = 0;
    }
}


int main(){

    //Opening input file and take input
    ifstream file("inp.txt");
    // Check if the file is open
    if(!file.is_open()){
        cerr << "Error opening input file." << endl;
        return 1;
    }

    file >> n;
    file >> k;
    file >> rowInc;

    //create 2d arrays for use
    prepare_2d_arrays();

    //assign input values to array
    for(int row = 0; row < n; row++){
        for (int col = 0; col < n; col++) {
            file >> a[row][col];
        }
    }

    file.close();



    // Create an array to hold thread objects
    thread threads[k];

//----------------------------------------------------------------------------------------------------------------

//TAS
    
    //prepare output file for TAS
    ofstream out_file("out_tas.txt");
    // Check if the file is open
    if(!out_file.is_open()){
        cerr << "Error opening out_chunk.txt file." << endl;
        return 1;
    }

    reset_result();//sets values of result matrix to 0


    //initially rows assigned to threads are zero
    rowsAssigned = 0;

    //record start time
    auto start = chrono::high_resolution_clock::now();

    // execute threads
    for(int id = 0; id < k; id++){
        threads[id] = thread(dynamicChunks_TAS, id);
    }

    // Join threads (wait for them to finish)
    for(int id = 0; id < k; id++){
        threads[id].join();
    }
    //record end time
    auto end = chrono::high_resolution_clock::now();
    //get execution time
    auto time = chrono::duration_cast<chrono::microseconds>(end - start).count();

    //write the output
    out_file << "Time: "<< time << "\n\n";
    for(int row = 0; row < n; row++){
        for (int col = 0; col < n; col++) {
            out_file << setw(15) << result[row][col] << ' ';
        }
        out_file << endl;
    }

    out_file.close();
//----------------------------------------------------------------------------------------------------------------------------

//CAS
    
    //prepare output file for CAS
    out_file.open("out_cas.txt");
    // Check if the file is open
    if(!out_file.is_open()){
        cerr << "Error opening out_mixed.txt file." << endl;
        return 1;
    }
    reset_result();//sets values of result matrix to 0


    //initially rows assigned to threads are zero
    rowsAssigned = 0;

    //record start time
    start = chrono::high_resolution_clock::now();
    
    // execute threads
    for(int id = 0; id < k; id++){
        threads[id] = thread(dynamicChunks_CAS, id);
    }

    // Join threads (wait for them to finish)
    for(int id = 0; id < k; id++){
        threads[id].join();
    }
    //record end time
    end = chrono::high_resolution_clock::now();
    //get execution time
    time = chrono::duration_cast<chrono::microseconds>(end - start).count();

    //write the output
    out_file << "Time: "<< time << "\n\n";
    for(int row = 0; row < n; row++){
        for (int col = 0; col < n; col++) {
            out_file << setw(15) << result[row][col] << ' ';
        }
        out_file << endl;
    }

    out_file.close();

//-----------------------------------------------------------------------------------------------------------------------------

//Bounded CAS
    
    //prepare output file for BCAS
    out_file.open("out_bcas.txt");
    // Check if the file is open
    if(!out_file.is_open()){
        cerr << "Error opening out_mixed.txt file." << endl;
        return 1;
    }
    reset_result();//sets values of result matrix to 0


    //initially rows assigned to threads are zero
    rowsAssigned = 0;

    //for bcas
    waiting = new bool[k];
    for(int id=0; id<k; id++) waiting[id] = false; 

    //record start time
    start = chrono::high_resolution_clock::now();
    
    // execute threads
    for(int id = 0; id < k; id++){
        threads[id] = thread(dynamicChunks_BCAS, id);
    }

    // Join threads (wait for them to finish)
    for(int id = 0; id < k; id++){
        threads[id].join();
    }
    //record end time
    end = chrono::high_resolution_clock::now();
    //get execution time
    time = chrono::duration_cast<chrono::microseconds>(end - start).count();

    //write the output
    out_file << "Time: "<< time << "\n\n";
    for(int row = 0; row < n; row++){
        for (int col = 0; col < n; col++) {
            out_file << setw(15) << result[row][col] << ' ';
        }
        out_file << endl;
    }

    out_file.close();


//-----------------------------------------------------------------------------------------------------------------------------

//atomic increment
    
    //prepare output file for Atomic increment
    out_file.open("out_atomic_increment.txt");
    // Check if the file is open
    if(!out_file.is_open()){
        cerr << "Error opening out_mixed.txt file." << endl;
        return 1;
    }
    reset_result();//sets values of result matrix to 0


    //initially rows assigned to threads are zero
    currentRow = 0;

    //record start time
    start = chrono::high_resolution_clock::now();
    
    // execute threads
    for(int id = 0; id < k; id++){
        threads[id] = thread(dynamicChunks_atomic_increment, id);
    }

    // Join threads (wait for them to finish)
    for(int id = 0; id < k; id++){
        threads[id].join();
    }
    //record end time
    end = chrono::high_resolution_clock::now();
    //get execution time
    time = chrono::duration_cast<chrono::microseconds>(end - start).count();

    //write the output
    out_file << "Time: "<< time << "\n\n";
    for(int row = 0; row < n; row++){
        for (int col = 0; col < n; col++) {
            out_file << setw(15) << result[row][col] << ' ';
        }
        out_file << endl;
    }

    out_file.close();

//-----------------------------------------------------------------------------------------------------------------------------
    //free the resources

    // Deallocate memory
    for (int i = 0; i < n; ++i) {
        delete[] a[i];
        delete[] result[i];
    }
    delete[] a;
    delete[] result;
    delete[] waiting;

    return 0;
}
