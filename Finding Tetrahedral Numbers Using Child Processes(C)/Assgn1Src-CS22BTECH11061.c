#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include<limits.h>

//--------------------------------------------------------------------------------------------------------------------------------

/*ceiling function*/
int get_ceil(float f){
    if(f > (int)f) return (int)f + 1;
    else return (int)f;
}

/*Function returns the pth tetrahedral number*/
long long pth_tetrahedral(long long p){
    return (p * (p + 1) * (p + 2)) / 6;
}

/*p_max is the max value in binary search for tetrahedral numbers,
it is greatest tetrahedral less than equal to INT_MAX */
int p_max = 1;

/*function to set p_max value*/
void set_p_max(){
    p_max = 1;
    while( pth_tetrahedral(p_max+1) < INT_MAX ) p_max++;
}

/*This function uses binary search to check if a number is tetrahedral.
Returns true if a number is tetrahedral, else false*/
bool ifTetrahedral(int num){
    //tetrahedral numbers are used as sorted array
    int start = 1;//first tetrahedral number index
    int end = p_max;//index of greatest tetrahedral number less than equal to INT_MAX

    //binary search
    while(start <= end){//search end condition
        int middle = start + (end - start)/2;//find middle element

        if(pth_tetrahedral(middle) == num){//if num found return true for present/ tetrahedral number
            return true;
        }
        else if(pth_tetrahedral(middle) < num){//check only right half
            start = middle + 1;
        }
        else{//check only left half
            end = middle - 1;
        }

    }

    return false;//when not found
}

//-------------------------------------------------------------------------------------------------------------------------------

/*function to retreive N and K values from input file(name is "input")*/
void get_input(int *n, int *k){
    FILE *input_file = fopen("input", "r");//open input file
    if(input_file == NULL){//when error
        perror("Error opening file named \"input\"\n");
        exit(0);//terminate process
    }

    //retreive N and K on last line of input file
    char line[100];
    while(fgets(line, sizeof(line), input_file) != NULL){
        char *token = strtok(line, " ");
        (*n) = atoi(token);

        token = strtok(NULL, " ");
        (*k) = atoi(token);
    }

    fclose(input_file);
}

/*creates custom output file pointer*/
FILE *create_output_file(char name[]){
    FILE *file = fopen(name, "w");
    if(file == NULL){
        printf("Error in creating %s\n", name);
    }
    return file;
}

//-------------------------------------------------------------------------------------------------------------------------------

int main(){
    //initialize p_max for later use(description above)
    //needed for ifTetrahedral function initialization/working
    set_p_max();

    int n, k;
    //get n and k values from input file
    get_input(&n, &k);

    //N and K constraints
    if(n <= 0 || k <= 0){
        printf("N and K both must be greater than 0\n");
        return 0;
    }

    //initializing variables used for creating shared memory buffer
    int size = get_ceil( (float)n/k );//size of each shared memory buffer
    int shm_fd;
    bool *ptr;//bool type ptr to check if each number is tetrahedral or not

    //---------------------------------------------------------------------------------------------------------------------

    // creating k child processes
    pid_t pid;
    for(int child = 0; child < k; child++){
        //create child process
        pid = fork();

        if(pid < 0){//error handling
            printf("Error creating child %d\n", child);
        } 
        else if(pid == 0){// child process
            //create shared memory buffer name with child number suffix
            char temp_name[100];
            snprintf(temp_name, sizeof(temp_name), "%s%d", "shared_memory_buffer", child);

            //create output file name with child number suffix
            char output_file_name[100];
            snprintf(output_file_name, sizeof(output_file_name), "%s%d", "OutPut", (child + 1));

            //create shared memory
            shm_fd = shm_open(temp_name, O_CREAT | O_RDWR, 0666);
            ftruncate(shm_fd, size);
            ptr = mmap(0, size, PROT_WRITE, MAP_SHARED, shm_fd, 0);

            //store result in shared memory and output file
            FILE *file = create_output_file(output_file_name);
            for(int j = 0; j < size; j++){
                int current = k * j + child + 1;//child i gets numbers c, such that c%k = (i+1) for i = 0 to K-2
                                                //for i = K-1 we get numbers c such that c%k = 0
                if (current > n) break;

                //store result in shared memory
                ptr[j] = ifTetrahedral(current);

                //storing result in output file
                if (ptr[j] == true) {
                    fprintf(file, "%d: a tetrahedral number\n", current);
                } else {
                    fprintf(file, "%d: Not a tetrahedral number\n", current);
                }
            }
            fclose(file);
            exit(0);//terminate child when work done
        }
        //as child is terminated, it makes sure that child does not create any child process
        //only parent creates child process
    }

    //-----------------------------------------------------------------------------------------------------

    // parent process
    for(int i = 0; i < k; i++) wait(NULL); // wait for each process to end

    //create ouptut file
    FILE *file = create_output_file("OutMain");
    for(int child = 0; child < k; child++){//for each child
        //get name of shared memory buffer
        char temp_name[100];
        snprintf(temp_name, sizeof(temp_name), "%s%d", "shared_memory_buffer", child);

        //access shared memory
        shm_fd = shm_open(temp_name, O_RDWR, 0666);
        ptr = mmap(0, size, PROT_READ, MAP_SHARED, shm_fd, 0);

        //write process name
        fprintf(file, "P%d: ", child+1);
        for(int j = 0; j < size; j++){//for each number in number set assigned to child
            if (k * j + child + 1 > n) break;
            if(ptr[j] == true){//if tetrahedral
                fprintf(file, "%d ", (k * j + child + 1));//write in output file
            }
        }

        fprintf(file, "\n");
        shm_unlink(temp_name);//unlink memory when done
    }

    fclose(file);

    return 0;
}