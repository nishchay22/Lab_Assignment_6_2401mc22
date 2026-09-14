#include "kernel/types.h"
#include "user/user.h"

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
        // FIXED: Order is now Lock1 then Lock2
        printf("Process B trying to acquire Lock1...\n");
        acquire(&lock1);
        printf("Process B acquired Lock1.\n");
        
        sleep(10); 
        
        printf("Process B trying to acquire Lock2...\n");
        acquire(&lock2);
        printf("Process B acquired Lock2.\n");
        
        release(&lock2);
        release(&lock1);
        printf("Process B released both locks.\n");
        exit(0);
    } else {
        // Parent: Process A
        printf("Process A trying to acquire Lock1...\n");
        acquire(&lock1);
        printf("Process A acquired Lock1.\n");
        
        sleep(10);
        
        printf("Process A trying to acquire Lock2...\n");
        acquire(&lock2);
        printf("Process A acquired Lock2.\n");
        
        release(&lock2);
        release(&lock1);
        printf("Process A released both locks.\n");
        
        wait(0);
    }
    
    return 0;
}
