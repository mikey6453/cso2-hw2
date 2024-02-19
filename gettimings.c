#define _XOPEN_SOURCE 700
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

long long current_time;
long long total_time;
long long global_overhead;
struct timespec time_start, time_end;

pid_t other_pid1;
pid_t other_pid2;

__attribute__((noinline)) void empty_function(void) {
    __asm__("");
    return;
}

long long measure_empty(void) {
    struct timespec start, end;
    int repetition = 100;

    clock_gettime(CLOCK_MONOTONIC, &start);
    for(int i = 0; i < repetition; i++) {

    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    long long overhead = (end.tv_sec - start.tv_sec) * 1000000000 + (end.tv_nsec - start.tv_nsec);

    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < repetition; i++) {
        empty_function();
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    long long execution = (end.tv_sec - start.tv_sec) * 1000000000 + (end.tv_nsec - start.tv_nsec);

    long long total_ns = execution - overhead;

    return total_ns / repetition;
}

long long measure_parent_pid(void) {
    struct timespec start, end;
    int repetition = 100;

    clock_gettime(CLOCK_MONOTONIC, &start);
    for(int i = 0; i < repetition; i++) {

    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    long long overhead = (end.tv_sec - start.tv_sec) * 1000000000 + (end.tv_nsec - start.tv_nsec);

    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < repetition; i++) {
        getppid();
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    long long execution = (end.tv_sec - start.tv_sec) * 1000000000 + (end.tv_nsec - start.tv_nsec);

    long long total_ns = execution - overhead;

    return total_ns / repetition;
}

long long measure_bin_true(void) {
    struct timespec start, end;
    int repetition = 100;

    clock_gettime(CLOCK_MONOTONIC, &start);
    for(int i = 0; i < repetition; i++) {

    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    long long overhead = (end.tv_sec - start.tv_sec) * 1000000000 + (end.tv_nsec - start.tv_nsec);

    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < repetition; i++) {
        system("/usr/bin/true");
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    long long execution = (end.tv_sec - start.tv_sec) * 1000000000 + (end.tv_nsec - start.tv_nsec);

    long long total_ns = execution - overhead;

    return total_ns / repetition;
}

__attribute__((noinline)) void handle_signal_usr1(int id) {
    clock_gettime(CLOCK_MONOTONIC, &time_end);
    long long execution = (time_end.tv_sec - time_start.tv_sec) * 1000000000 + (time_end.tv_nsec - time_start.tv_nsec);
    current_time = execution - global_overhead;
    total_time += current_time;
    __asm__("");
}

long long measure_current_signal(void) {
    int repetition = 100;

    clock_gettime(CLOCK_MONOTONIC, &time_start);

    clock_gettime(CLOCK_MONOTONIC, &time_end);
    global_overhead = (time_end.tv_sec - time_start.tv_sec) * 1000000000 + (time_end.tv_nsec - time_start.tv_nsec);

    struct sigaction act;
    act.sa_handler = &handle_signal_usr1;
    sigaction(SIGUSR1, &act, NULL);

    for (int i = 0; i < repetition; i++) {
        clock_gettime(CLOCK_MONOTONIC, &time_start);
        raise(SIGUSR1); 
        usleep(10000);  
    }

    return total_time / repetition;
}

long long measure_cross_signal(void) {
    int repetition = 100;

    clock_gettime(CLOCK_MONOTONIC, &time_start);

    clock_gettime(CLOCK_MONOTONIC, &time_end);
    global_overhead = (time_end.tv_sec - time_start.tv_sec) * 1000000000 + (time_end.tv_nsec - time_start.tv_nsec);

    printf("PID: %d\n", getpid());
    printf("Enter the PID of the other process: ");
    scanf("%d", &other_pid2);

    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);
    sigprocmask(SIG_BLOCK, &set, NULL);

    for (int i = 0; i < repetition; i++) {
        clock_gettime(CLOCK_MONOTONIC, &time_start);
        kill(other_pid2, SIGUSR2);
        int signo;
        if(sigwait(&set, &signo) == 0){
            if(signo == SIGUSR1){
                clock_gettime(CLOCK_MONOTONIC, &time_end);
                long long execution = (time_end.tv_sec - time_start.tv_sec) * 1000000000 + (time_end.tv_nsec - time_start.tv_nsec);
                current_time = execution - global_overhead;
                total_time += current_time;
            }
        }
        usleep(10000);  
    }

    return total_time / repetition;
}

void handle_signal_usr2_negative1(int id) {
    kill(other_pid1, SIGUSR1);
    __asm__("");
}

long long measure_cross_signal_negative1(void) {
    struct sigaction act;
    act.sa_handler = &handle_signal_usr1; 
    sigaction(SIGUSR1, &act, NULL);  

    act.sa_handler = &handle_signal_usr2_negative1;
    sigaction(SIGUSR2, &act, NULL);  

    printf("PID: %d\n", getpid());
    printf("Enter the PID of the other process: ");
    scanf("%d", &other_pid1);

    while(1) {
        sleep(1);
    }    
}


int main(int argc, char *argv[]) {
    if (atoi(argv[1]) == 1) {
        long long time = measure_empty();
        printf("%lli\n", time);
    }
    if (atoi(argv[1]) == 2) {
        long long time = measure_parent_pid();;
        printf("%lli\n", time);
    }
    if (atoi(argv[1]) == 3) {
        long long time = measure_bin_true();
        printf("%lli\n", time);
    }
    if (atoi(argv[1]) == 4) {
        long long time = measure_current_signal();
        printf("%lli\n", time);
    }
    if (atoi(argv[1]) == 5) {
        long long time = measure_cross_signal();
        printf("%lli\n", time);
    }
    if (atoi(argv[1]) == -1) {
        measure_cross_signal_negative1();
    }

    return 0;
}

