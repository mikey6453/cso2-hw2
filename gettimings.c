#define _XOPEN_SOURCE 700

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <time.h>
# include <signal.h>

long long time_diff(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) * 1000000000LL + (end.tv_nsec - start.tv_nsec);
}


void empty_function() { // scenario 1
    // TODO
}


void signal_handler(int signum) {
    // TODO
}


int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <scenario_number>\n", argv[0]);
        return 1;
    }

    int scenario = atoi(argv[1]);
    long long start, end;

    switch (scenario) {
        case 1:
            // Measure empty function call
            break;
        case 2:
            // Measure getppid call
            break;
        case 3:
            // Measure system("/bin/true")
            break;
        case 4:
            // Measure sending a signal to current process
            break;
        case 5:
            // Measure signal exchange between processes
            break;
        default:
            fprintf(stderr, "Invalid scenario number.\n");
            return 1;
    }

    return 0;
}
