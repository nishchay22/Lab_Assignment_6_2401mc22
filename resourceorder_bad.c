#include "kernel/types.h"
#include "user/user.h"

// Mocking locks for xv6 user space simulation
struct lock {
    uint locked;
};

void acquire(struct lock *lk) {
    while(__sync_lock_test_and_set(&lk->locked, 1) != 0);
}

void release(struct lock *lk) {
    __sync_lock_release(&lk->locked);
}

struct lock lock1;
struct lock lock2;

int main(int argc, char *argv[]) {
    lock1.locked = 0;
    lock2.locked = 0;

    int pid = fork();

    if (pid == 0) {
        // Child: Process B
        printf("Process B trying to acquire Lock2...\n");
        acquire(&lock2);
        printf("Process B acquired Lock2.\n");
        
        sleep(10); // Deliberate delay
        
        printf("Process B trying to acquire Lock1...\n");
        acquire(&lock1); // DEADLOCK HERE
        printf("Process B acquired Lock1.\n");
        
        release(&lock1);
        release(&lock2);
        exit(0);
    } else {
        // Parent: Process A
        printf("Process A trying to acquire Lock1...\n");
        acquire(&lock1);
        printf("Process A acquired Lock1.\n");
        
        sleep(10); // Deliberate delay
        
        printf("Process A trying to acquire Lock2...\n");
        acquire(&lock2); // DEADLOCK HERE
        printf("Process A acquired Lock2.\n");
        
        release(&lock2);
        release(&lock1);
        
        wait(0);
    }
    
    return 0;
}
