#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <mutex>
#include <chrono>
#include <cmath>
#include <iomanip> // for setw

using namespace std;

int n, k;//N and K
int **a;//input array A
int **result;//Result array

int p;//N/K


void prepare_2d_arrays(){
    // Using dynamic memory allocation for 2D arrays
    a = new int*[n];
    result = new int*[n];

    for(int i = 0; i < n; ++i){
        a[i] = new int[n];
        result[i] = new int[n];
    }
}

void matrix_mult(int row, int col){
    int r = 0;
    for(int i = 0; i < n; i++) {
        r += a[row][i] * a[i][col];
    }
    result[row][col] = r;
}

void matrix_row_mult(int row){
    for(int col = 0; col < n; col++) {
        matrix_mult(row, col);
    }
}

void matrix_diagonal_mult(int diagonal_row){
    int row = diagonal_row;;
    for(int col=0; col<n; col++){
        matrix_mult(row, col);
        row++;
        row %= n;
    }
}

void chunk(int id){
    for(int i = 0; i < p; i++) {
        matrix_row_mult(id * p + i);
    }
}

void mixed(int id){
    for(int i = 0; i < p; i++){
        matrix_row_mult(id + i * k);
    }
}

void diagonal_chunk(int id){
    for(int i = 0; i < p; i++){
        matrix_diagonal_mult(id * p + i);
    }
}

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
    p = ceil((double)n / k);

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

    //writing ouput to out.txt
    // out_file << "Chunks:\n\n";

    out_file << "Time: "<< time << "\n\n";
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

    //writing ouput to out.txt
    // out_file << "Mixed:\n\n";

    out_file << "Time: "<< time << "\n\n";
    for(int row = 0; row < n; row++){
        for (int col = 0; col < n; col++) {
            // out_file << result[row][col] << ' ';
            out_file << setw(15) << result[row][col] << ' ';
        }
        out_file << endl;
    }

    out_file.close();

//-----------------------------------------------------------------------------------------------------------------------------
    out_file.open("out_diagonal_chunk.txt");
    // Check if the file is open
    if(!out_file.is_open()){
        cerr << "Error opening out_diagonal_chunk.txt file." << endl;
        return 1;
    }
    reset_result();//sets values of result matrix to 0
    
//Diagonal Chunks

    //record start time
    start = chrono::high_resolution_clock::now();
    
    // execute threads
    for(int id = 0; id < k; id++){
        threads[id] = thread(diagonal_chunk, id);
    }

    // Join threads (wait for them to finish)
    for(int id = 0; id < k; id++){
        threads[id].join();
    }
    //record end time
    end = chrono::high_resolution_clock::now();
    //get execution time
    time = chrono::duration_cast<chrono::microseconds>(end - start).count();

    //writing ouput to out.txt
    // out_file << "Diagonal Chunks:\n\n";

    out_file << "Time: "<< time << "\n\n";
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