#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <string.h>

volatile sig_atomic_t signal_received = 0;

void emptyFunction() {
    __asm__("");
}

void getppidCall() {
    (void)getppid();
}

void systemCall() {
    system("/bin/true");
}

long long time_diff(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) * 1000000000LL + (end.tv_nsec - start.tv_nsec);
}

void measure_operation(void (*operation)()) {
    struct timespec start, end;
    const int repetitions = 100;
    long long overhead, execution, total_ns = 0;

    // Measure overhead
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < repetitions; ++i) {
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    overhead = time_diff(start, end);

    // Measure execution
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < repetitions; ++i) {
        operation();
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    execution = time_diff(start, end);

    total_ns = (execution - overhead) / repetitions;
    printf("Average time: %lld ns\n", total_ns);
}

void signalHandler(int sig) {
    signal_received = 1;
}

void setupSignalHandling() {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = signalHandler;
    sigaction(SIGUSR1, &sa, NULL);
}

void measureSignal() {
    setupSignalHandling();
    struct timespec start, end;
    long long total_ns;

    clock_gettime(CLOCK_MONOTONIC, &start);
    raise(SIGUSR1);
    while (!signal_received);
    clock_gettime(CLOCK_MONOTONIC, &end);

    total_ns = time_diff(start, end);
    printf("Signal handling time: %lld ns\n", total_ns);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <operation_number>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int operation = atoi(argv[1]);
    
    switch (operation) {
        case 1:
            measure_operation(emptyFunction);
            break;
        case 2:
            measure_operation(getppidCall);
            break;
        case 3:
            measure_operation(systemCall);
            break;
        case 4:
            measureSignal();
            break;
        default:
            fprintf(stderr, "Invalid operation number.\n");
            exit(EXIT_FAILURE);
    }

    return 0;
}
