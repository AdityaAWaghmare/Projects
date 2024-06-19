#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"


int main() {
    int p[2]; // Pipe file descriptors
    //p[0] to read and p[1] to write

    // Create pipes
    if (pipe(p) < 0) {
        fprintf(2, "pipe failed\n");
        exit(1);
    }

    int pid = fork();

    if (pid < 0) {
        fprintf(2, "fork failed\n");
        exit(1);
    }

    if (pid == 0) {
        // Child process
        char msg[1];

        // Read byte from parent
        if (read(p[0], msg, 1) != 1) {
            fprintf(2, "read error\n");
            exit(1);
        }
        close(p[0]); // Close read end
        printf("%d: received ping\n", getpid());

        // Write same byte to parent
        if (write(p[1], msg, 1) != 1) {
            fprintf(2, "child write error\n");
            exit(1);
        }
        close(p[1]); // Close write 
        
        exit(0);
    } else {
        // Parent process
        char msg[1] = "a"; // Byte to send

        // Write byte to child
        if (write(p[1], msg, 1) != 1) {
            fprintf(2, "parent write error\n");
            exit(1);
        }
        wait(0); // Wait for child process to exit

        // Read byte from child
        if (read(p[0], msg, 1) != 1) {
            fprintf(2, "parent read error\n");
            exit(1);
        }
        printf("%d: received pong\n", getpid());

        close(p[0]); // Close read end
        //write close is delayed so that we can still read 
        close(p[1]); // Close write end
        exit(0);
    }

    exit(0);
}
