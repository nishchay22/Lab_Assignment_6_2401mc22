#include "kernel/types.h"
#include "user/user.h"

// Simple semaphore implementation for xv6 simulation
struct sem {
    int value;
    struct lock lock;
};

void sem_init(struct sem *s, int val) {
    s->value = val;
    s->lock.locked = 0;
}

void sem_wait(struct sem *s) {
    while(1) {
        acquire(&s->lock);
        if (s->value > 0) {
            s->value--;
            release(&s->lock);
            break;
        }
        release(&s->lock);
        sleep(1);
    }
}

void sem_post(struct sem *s) {
    acquire(&s->lock);
    s->value++;
    release(&s->lock);
}

struct sem printer_sem; // 2 printers
struct sem scanner_sem; // 1 scanner
struct sem disk_sem;    // 2 disks

void process_task(int pid, int r1, int r2) {
    for (int i = 0; i < 3; i++) { // 3 iterations
        printf("Process P%d requesting resources (ordered)...\n", pid);
        
        // Resource ordering to avoid deadlock (0: printer, 1: scanner, 2: disk)
        if (r1 < r2) {
            if (r1 == 0) sem_wait(&printer_sem);
            if (r1 == 1) sem_wait(&scanner_sem);
            if (r2 == 1) sem_wait(&scanner_sem);
            if (r2 == 2) sem_wait(&disk_sem);
        } else {
            if (r2 == 0) sem_wait(&printer_sem);
            if (r2 == 1) sem_wait(&scanner_sem);
            if (r1 == 1) sem_wait(&scanner_sem);
            if (r1 == 2) sem_wait(&disk_sem);
        }
        
        printf("Process P%d GRANTED resources, starting work...\n", pid);
        sleep(5); // Simulate work
        
        printf("Process P%d releasing resources...\n", pid);
        if (r1 == 0 || r2 == 0) sem_post(&printer_sem);
        if (r1 == 1 || r2 == 1) sem_post(&scanner_sem);
        if (r1 == 2 || r2 == 2) sem_post(&disk_sem);
    }
}

int main(int argc, char *argv[]) {
    sem_init(&printer_sem, 2);
    sem_init(&scanner_sem, 1);
    sem_init(&disk_sem, 2);
    
    for (int i = 0; i < 5; i++) {
        int pid = fork();
        if (pid == 0) {
            // Assign different needs for each process
            int needs[5][2] = { {0, 1}, {1, 2}, {0, 2}, {0, 1}, {1, 2} }; 
            process_task(i, needs[i][0], needs[i][1]);
            exit(0);
        }
    }
    
    for(int i = 0; i < 5; i++) wait(0);
    printf("All processes completed successfully.\n");
    return 0;
}
