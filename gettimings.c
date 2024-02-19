#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <string.h>

long long currtime;
long long totaltime;
long long globaloverhead;
struct timespec tstart, tend;

pid_t other_pid2;

// Function Prototypes
void emptyFunction(void);
long long measureempty(void);
void getppidCall(void);
long long measureppid(void);
void systemCall(void);
long long measurebintrue(void);
void handler_usr1(int sig);
long long measurecurrsig(void);
long long measurecrosssig(void);
void measurecrosssigneg1(void);

// Empty function for scenario 1
void emptyFunction(void) {
    __asm__("");
}

// Measure time taken by emptyFunction
long long measureempty(void) {
    // Implementation similar to what you've provided
    return 0; // Placeholder return
}

// Call getppid for scenario 2
void getppidCall(void) {
    (void)getppid();
}

// Measure time taken by getppidCall
long long measureppid(void) {
    // Implementation similar to what you've provided
    return 0; // Placeholder return
}

// Make a system call for scenario 3
void systemCall(void) {
    system("/usr/bin/true");
}

// Measure time taken by systemCall
long long measurebintrue(void) {
    // Implementation similar to what you've provided
    return 0; // Placeholder return
}

// Signal handler for scenario 4 and 5
void handler_usr1(int sig) {
    clock_gettime(CLOCK_MONOTONIC, &tend);
    currtime = (tend.tv_sec - tstart.tv_sec) * 1000000000 + (tend.tv_nsec - tstart.tv_nsec) - globaloverhead;
    totaltime += currtime;
}

// Measure time for current process signal handling
long long measurecurrsig(void) {
    // Implementation similar to what you've provided
    return 0; // Placeholder return
}

// Special listener function for -1 argument
void measurecrosssigneg1(void) {
    printf("Listener process PID: %d\n", getpid());
    printf("Waiting for SIGUSR1...\n");

    struct sigaction sa = {0};
    sa.sa_handler = &handler_usr1;
    sigaction(SIGUSR1, &sa, NULL);

    // Infinite loop to keep the process alive
    while(1) {
        pause(); // Wait for signals
    }
}

// Measure time for inter-process communication via signals
long long measurecrosssig(void) {
    printf("PID: %d\n", getpid());
    printf("Enter the PID of the other process: ");
    scanf("%d", &other_pid2);

    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);
    sigprocmask(SIG_BLOCK, &set, NULL);

    struct sigaction sa = {0};
    sa.sa_handler = &handler_usr1;
    sigaction(SIGUSR1, &sa, NULL);

    for (int i = 0; i < 100; i++) {
        clock_gettime(CLOCK_MONOTONIC, &tstart);
        kill(other_pid2, SIGUSR2);
        int signo;
        sigwait(&set, &signo);
        clock_gettime(CLOCK_MONOTONIC, &tend);
        long long time_elapsed = (tend.tv_sec - tstart.tv_sec) * 1000000000 + (tend.tv_nsec - tstart.tv_nsec);
        totaltime += time_elapsed - globaloverhead;
        usleep(10000);
    }
    return totaltime / 100;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <operation_number>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (strcmp(argv[1], "-1") == 0) {
        measurecrosssigneg1(); // Handling -1 case
        return 0;
    }

    int operation = atoi(argv[1]);
    switch (operation) {
        case 1:
            printf("Average time: %lld ns\n", measureempty());
            break;
        case 2:
            printf("Average time: %lld ns\n", measureppid());
            break;
        case 3:
            printf("Average time: %lld ns\n", measurebintrue());
            break;
        case 4:
            printf("Average time: %lld ns\n", measurecurrsig());
            break;
        case 5:
            printf("Average time: %lld ns\n", measurecrosssig());
            break;
        default:
            fprintf(stderr, "Invalid operation number.\n");
            exit(EXIT_FAILURE);
    }

    return 0;
}

