#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <mutex>
#include <chrono>
#include <cmath>
#include <iomanip> // for setw


using namespace std;

int n, k, c, bt;//N, K, C, BT
int b;//b = K/C

int64_t *thread_exec_time;//stores execution time of each thread

int **a;//input array A
int **result;//Result array

int p;// p = N/K


void prepare_2d_arrays(){
    // Using dynamic memory allocation for 2D arrays
    a = new int*[n];
    result = new int*[n];

    for(int i = 0; i < n; ++i){
        a[i] = new int[n];
        result[i] = new int[n];
    }
}

//Gives single element of Square Matrix
void matrix_mult(int row, int col){
    int r = 0;
    for(int i = 0; i < n; i++) {
        r += a[row][i] * a[i][col];
    }
    result[row][col] = r;
}

//Gives Row of the Square Matrix
void matrix_row_mult(int row){
    for(int col = 0; col < n; col++) {
        matrix_mult(row, col);
    }
}

//Chunk Method Thread Routine
void chunk(int id){
    //Measuring Thread Execution time

    //record start time of thread execution
    auto start = chrono::high_resolution_clock::now();

    //Checking if this threads affinity should be set or not 
    if(id < bt && b!=0){
        //first BT threads are assigned to certain cores

        //to what core this thread associates 
        int core_id = id/b;//b threads for each core 

        // Set thread affinity on Linux (pthread_setaffinity_np)
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        CPU_SET(core_id, &cpuset);
        pthread_setaffinity_np(pthread_self(), sizeof(cpuset), &cpuset);
    }

    //Matrix Multiplication 
    for(int i = 0; i < p; i++) {
        matrix_row_mult(id * p + i);
    }

    //record end time
    auto end = chrono::high_resolution_clock::now();
    //get execution time
    auto time = chrono::duration_cast<chrono::microseconds>(end - start).count();

    //Save Thread Execution time in common memory array 
    thread_exec_time[id] = time;
}

//Mixed Method Thread Routine
void mixed(int id){
    //Measuring Thread Execution time

    //record start time
    auto start = chrono::high_resolution_clock::now();

    //Checking if this threads affinity should be set or not 
    if(id < bt && b!=0){
        //first BT threads are assigned to certain cores

        int core_id = id/b;//b threads for each core 

        // Set thread affinity on Linux (pthread_setaffinity_np)
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        CPU_SET(core_id, &cpuset);
        pthread_setaffinity_np(pthread_self(), sizeof(cpuset), &cpuset);
    }

    //Matrix Multiplication 
    for(int i = 0; i < p; i++){
        matrix_row_mult(id + i * k);
    }

    //record end time
    auto end = chrono::high_resolution_clock::now();
    //get execution time
    auto time = chrono::duration_cast<chrono::microseconds>(end - start).count();

    //Save Thread Execution time in common memory array 
    thread_exec_time[id] = time;
}


//Resets Result array Before Matrix Squaring
void reset_result(){
    for(int i=0; i<n; i++){
        for(int j=0; j<n; j++) result[i][j] = 0;
    }
}

//Returns Average Normal Thread Execution time
int64_t get_avg_nt_time(){
    int64_t avg_nt_time = 0;
    int nt_count = 0;//number of normal threads

    for(int id=0; id<k; id++){//for each thread
        if(id < bt && b!=0){
        }
        else{//if it is normal thread
            avg_nt_time += thread_exec_time[id];
            nt_count++;
        }
    }
    nt_count>0? avg_nt_time /= nt_count:0;//If Normal Thread count is non zero, only then we can divide to get avg time, otherwise it is zero already

    return avg_nt_time;
}

//Returns Average Core Bound Thread Execution Time
int64_t get_avg_cbt_time(){
    int64_t avg_cbt_time = 0;
    int cbt_count = 0;
    for(int id=0; id<k; id++){
        if(id < bt && b!=0){
            avg_cbt_time += thread_exec_time[id];
            cbt_count++;
        }
    }
    cbt_count>0? avg_cbt_time /= cbt_count:0;//If Core Bound Thread count is non zero, only then we can divide to get avg time, otherwise it is zero already

    return avg_cbt_time;
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
    p = ceil((double)n / k);
    file >> c;
    file >> bt;
    b = k>=c?k/c:1;// b = if K>=C then floor(K/C) Else it is 1

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

    //This array stores execution time of each thread
    thread_exec_time = new int64_t[k];
    
//----------------------------------------------------------------------------------------------------------------
    //prepare output file
    ofstream out_file("out_chunk.txt");
    // Check if the file is open
    if(!out_file.is_open()){
        cerr << "Error opening out_chunk.txt file." << endl;
        return 1;
    }

    reset_result();//sets values of result matrix to 0


//Chunks

    //record start time
    auto start = chrono::high_resolution_clock::now();

    // execute threads
    for(int id = 0; id < k; id++){
        threads[id] = thread(chunk, id);
    }

    // Join threads (wait for them to finish)
    for(int id = 0; id < k; id++){
        threads[id].join();
    }
    //record end time
    auto end = chrono::high_resolution_clock::now();
    //get execution time
    auto time = chrono::duration_cast<chrono::microseconds>(end - start).count();

    

    out_file << "Total Time: "<< time << endl;
    out_file << "Average Core Bound Thread time: "<< get_avg_cbt_time() << endl;
    out_file << "Average Normal Thread time: "<< get_avg_nt_time() << "\n\n";

    //Print Result Array
    for(int row = 0; row < n; row++){
        for (int col = 0; col < n; col++) {
            out_file << setw(15) << result[row][col] << ' ';
            // out_file << result[row][col] << ' ';
        }
        out_file << endl;
    }

    out_file.close();
//----------------------------------------------------------------------------------------------------------------------------
    out_file.open("out_mixed.txt");
    // Check if the file is open
    if(!out_file.is_open()){
        cerr << "Error opening out_mixed.txt file." << endl;
        return 1;
    }
    reset_result();//sets values of result matrix to 0


//Mixed

    //record start time
    start = chrono::high_resolution_clock::now();
    
    // execute threads
    for(int id = 0; id < k; id++){
        threads[id] = thread(mixed, id);
    }

    // Join threads (wait for them to finish)
    for(int id = 0; id < k; id++){
        threads[id].join();
    }
    //record end time
    end = chrono::high_resolution_clock::now();
    //get execution time
    time = chrono::duration_cast<chrono::microseconds>(end - start).count();

    

    out_file << "Total Time: "<< time << endl;
    out_file << "Average Core Bound Thread time: "<< get_avg_cbt_time() << endl;
    out_file << "Average Normal Thread time: "<< get_avg_nt_time() << "\n\n";
    
    //Print Result Array
    for(int row = 0; row < n; row++){
        for (int col = 0; col < n; col++) {
            // out_file << result[row][col] << ' ';
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

    return 0;
}
