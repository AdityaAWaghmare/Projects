#include<iostream>
#include<fstream>
#include<thread>
#include<semaphore.h>
#include<pthread.h>
#include<chrono>
#include<random>
#include<queue>

using namespace std;

int nw, nr, kw, kr, ucs, urem;//Input Parameters
ofstream out_file;//Output file Object to be used by all threads
int64_t **reader_time, **writer_time;//keeps time taken in each CS access(entry time - request time) for reader and writer threads

//--------------------------------------------------------------------------------------------------------------------------
void prepare_2d_arrays(){
    // Using dynamic memory allocation for 2D arrays
    writer_time = new int64_t*[nw];
    reader_time = new int64_t*[nr];

    for(int i = 0; i < nw; i++){
        writer_time[i] = new int64_t[kw];
    }

    for(int i = 0; i < nr; i++){
        reader_time[i] = new int64_t[kr];
    }
    
}

// Here Output is a delay value exponentially distributed with an
// Average value averageTime milli-seconds. These time delays aim to simulate these threads performing some complicated, time-consuming tasks.
int generateDelay(double averageTime) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::exponential_distribution<> dist(1.0 / averageTime);
    return static_cast<int>(dist(gen));
}

//binary semaphore for output file access
//only one thread can write to the file at a time
sem_t out_sem;
void write_output(string output){
    //aquiring the semaphore
    sem_wait(&out_sem);

    out_file << output;

    //releasing the semaphore
    sem_post(&out_sem);
}

//----------------------------------------------------------------------------------------------------------------------------
//Initialzations for Fair Readers Writers Problem

int readerCount = 0;//Number of readers in the CS

// all semaphores are binary semaphores
sem_t resourceLock;//Lock to the shared resource 
sem_t rcLock;//reader count variabe lock 
sem_t queueLock;//lock for the queue

//Waitlist Queue for threads
queue<int> waitlist;

//Atomically adds elements to the end of the queue
void enqueue(int id){
    //Aquire Queue Lock
    sem_wait(&queueLock);

    waitlist.push(id);

    //Release Queue Lock
    sem_post(&queueLock);
}

//Atomically removes the first element from the queue
void dequeue(){
    //Aquire Queue Lock
    sem_wait(&queueLock);

    waitlist.pop();

    //Release Queue Lock
    sem_post(&queueLock);
}

chrono::system_clock::time_point startTime;//Stores the start time of the program
//---------------------------------------------------------------------------------------------------------------------------
//WRITER THREAD ROUTINE

void writer(int id) {//id is the writer thread number

    //each writer thread will enter the CS kw times
    for (int i = 0; i <= kw; i++) {
        //get current system time
        auto rt = std::chrono::system_clock::now();
        //Convert to miliseconds Relative to Program Start Time
        auto reqTime = chrono::duration_cast<std::chrono::microseconds>(rt-startTime).count();//time relative to program start time

        string output = to_string(i) + "th CS request by Writer Thread " + to_string(id) + " at " + to_string(reqTime) + "\n";
        write_output(output);



        //----------------------------------------------------------------------------------------------------------------
        /*
         * Write your code for Readers Writers() and Fair Readers Writers() Using Semaphores here.
         */

        //<ENTRY SECTION>

        //Add itself to the waitlist queue
        enqueue(id);
        //wait till its turn
        while(waitlist.front() != id){//if not first in the queue
            //wait for turn
        }
        //This threads turn

        //Aquire resource lock
        sem_wait(&resourceLock);

        //Let next thread in the queue pass
        dequeue();


        //----------------------------------------------------------------------------------------------------------------

        //<CRITICAL SECTION>

        //get entry time relative to start time in miliseconds 
        auto ent = std::chrono::system_clock::now();
        auto enterTime = chrono::duration_cast<std::chrono::microseconds>(ent-startTime).count();
        
        output = to_string(i) + "th CS Entry by Writer Thread " + to_string(id) + " at " + to_string(enterTime) + "\n";
        write_output(output);

        //sleep for random time(exponential dist with ucs average time) in CS
        int randCSTime = generateDelay(ucs);
        // simulate a thread writing in CS
        this_thread::sleep_for(chrono::milliseconds(randCSTime));


        //----------------------------------------------------------------------------------------------------------------
        /*
         * Your code for the thread to exit the CS.
         */
        
        //<EXIT SECTION>

        //Release Resource Lock
        sem_post(&resourceLock);//Can be Aquired by next in line thread
 
        //----------------------------------------------------------------------------------------------------------------

        //get exit time relative to start time in miliseconds
        auto et = std::chrono::system_clock::now();
        auto exitTime = chrono::duration_cast<std::chrono::microseconds>(et-startTime).count();
        
        output = to_string(i) + "th CS Exit by Writer Thread " + to_string(id) + " at " + to_string(exitTime) + "\n";
        write_output(output);

        //<REMAINDER SECTION>

        // simulate a thread executing in Remainder Section
        int randRemTime = generateDelay(urem);
        this_thread::sleep_for(chrono::milliseconds(randRemTime));

        //save time taken to get entry since requested
        writer_time[id][i] = chrono::duration_cast<chrono::microseconds>(ent - rt).count();
    }
}

//-------------------------------------------------------------------------------------------------------------------------
//READER THREAD ROUTINE

void reader(int id) {//id is the reader thread number

    //each reader thread will enter the CS kr times
    for (int i = 0; i < kr; i++) {
        //get current system time
        auto rt = std::chrono::system_clock::now();
        //Convert to miliseconds Relative to Program Start Time
        auto reqTime = chrono::duration_cast<std::chrono::microseconds>(rt - startTime).count();
        
        string output = to_string(i) + "th CS request by Reader Thread " + to_string(id) + " at " + to_string(reqTime) + "\n";
        write_output(output);


        //----------------------------------------------------------------------------------------------------------------
        /*
         * Write your code for Readers Writers() and Fair Readers Writers() Using Semaphores here.
         */
        
        //<ENTRY SECTION>
        
        //add itself to the waitlist queue
        enqueue(id+nw);//to differentiate from writer threads
        //wait till its turn
        while(waitlist.front() != id+nw){//if not first in the queue
            //wait for turn
        }
        //This threads turn

        //Aquire Reader Count Lock
        sem_wait(&rcLock);
        //One more reader
        readerCount++;
        if (readerCount == 1){//if first reader
            //Aquire Resource Lock
            sem_wait(&resourceLock);
        }
        //Release Reader Count Lock
        sem_post(&rcLock);

        //Let next thread in the queue pass
        dequeue();

        //----------------------------------------------------------------------------------------------------------------

        //<CRITICAL SECTION>

        //get entry time relative to start time in miliseconds 
        auto ent = std::chrono::system_clock::now();
        auto enterTime = chrono::duration_cast<std::chrono::microseconds>(ent-startTime).count();
        output = to_string(i) + "th CS Entry by Reader Thread " + to_string(id) + " at " + to_string(enterTime) + "\n";
        write_output(output);

        // Simulate a thread reading from CS
        int randCSTime = generateDelay(ucs);
        this_thread::sleep_for(chrono::milliseconds(randCSTime));

        //----------------------------------------------------------------------------------------------------------------
        /*
         * Your code for the thread to exit the CS.
         */
        
        //<EXIT SECTION>

        //Aquire Reader Count Lock
        sem_wait(&rcLock);
        //One less reader
        readerCount--;
        if (readerCount == 0){//if last reader
            //Release Resource Lock
            sem_post(&resourceLock);
        }
        //Release Reader Count Lock
        sem_post(&rcLock);
        //----------------------------------------------------------------------------------------------------------------

        //get exit time relative to start time in miliseconds
        auto et = std::chrono::system_clock::now();
        auto exitTime = chrono::duration_cast<std::chrono::microseconds>(et-startTime).count();
        
        output = to_string(i) + "th CS Exit by Reader Thread " + to_string(id) + " at " + to_string(exitTime) + "\n";
        write_output(output);

        //<REMAINDER SECTION>

        // Simulate a thread executing in Remainder Section
        int randRemTime = generateDelay(urem);
        this_thread::sleep_for(chrono::milliseconds(randRemTime));

        //save time taken to get entry since requested
        reader_time[id][i] = chrono::duration_cast<chrono::microseconds>(ent - rt).count();
    }
}

//----------------------------------------------------------------------------------------------------------------------

int main(){
    //INPUT 

    //Opening input file and take input
    ifstream file("inp-params.txt");
    // Check if the file is open
    if(!file.is_open()){
        cerr << "Error opening input file." << endl;
        return 1;
    }

    file >> nw;
    file >> nr;
    file >> kw;
    file >> kr;
    file >> ucs;
    file >> urem;

    file.close();

//-----------------------------------------------------------------------------------------------

    //prepare output file
    out_file.open("FairRW-log.txt");
    // Check if the file is open
    if(!out_file.is_open()){
        cerr << "Error opening RW-log.txt file." << endl;
        return 1;
    }

    //initialize semaphores
    sem_init(&out_sem, 0, 1);
    sem_init(&resourceLock, 0, 1);
    sem_init(&rcLock, 0, 1);
    sem_init(&queueLock, 0, 1);

    //initialize 2D arrays
    prepare_2d_arrays();

    // Create an array to hold thread objects
    thread writerThreads[nw];
    thread readerThreads[nr];


//-----------------------------------------------------------------------------------------------
    //THREAD EXECUTION 

    //record Start time
    startTime = std::chrono::system_clock::now();
    // execute writer threads
    for(int id = 0; id < nw; id++){
        writerThreads[id] = thread(writer, id);
    }
    //execute reader threads
    for(int id = 0; id < nr; id++){
        readerThreads[id] = thread(reader, id);
    }

    // Join writer threads
    for(int id = 0; id < nw; id++){
        writerThreads[id].join();
    }
    //join reader threads
    for(int id = 0; id < nr; id++){
        readerThreads[id].join();
    }

//-----------------------------------------------------------------------------------------------
    //OUTPUT

    //Get average and worst case times for writer threads
    int64_t avg_writer_time = 0, worst_writer_time = 0;
    for(int i = 0; i < nw; i++){
        for(int j = 0; j < kw; j++){
            avg_writer_time += writer_time[i][j];
            if(worst_writer_time < writer_time[i][j]){
                worst_writer_time = writer_time[i][j];
            }
        }
    }
    out_file << "\n\nAverage time the writer threads take to enter the CS : " << avg_writer_time / (nw * kw) << " microseconds" << endl;
    out_file << "Worst time the writer threads take to enter the CS : " << worst_writer_time << " microseconds" << endl;

    //Get average and worst case times for reader threads
    int64_t avg_reader_time = 0, worst_reader_time = 0;
    for(int i = 0; i < nr; i++){
        for(int j = 0; j < kr; j++){
            avg_reader_time += reader_time[i][j];
            if(worst_reader_time < reader_time[i][j]){
                worst_reader_time = reader_time[i][j];
            }
        }
    }
    out_file << "\n\nAverage time the reader threads take to enter the CS : " << avg_reader_time / (nr * kr) << " microseconds" << endl;
    out_file << "Worst time the reader threads take to enter the CS : " << worst_reader_time << " microseconds" << endl;


    //close output file
    out_file.close();
    
}