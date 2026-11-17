#include "types.h"
#include "stat.h"
#include "user.h"

// Shared-memory spinlocks to demonstrate circular wait deadlock across fork()
struct lock {
    volatile uint locked;
};

struct shared_locks {
    struct lock lock1;
    struct lock lock2;
};

void acquire(struct lock *lk) {
    while(__sync_lock_test_and_set(&lk->locked, 1) != 0)
        ;
}

void release(struct lock *lk) {
    __sync_lock_release(&lk->locked);
}

int main(int argc, char *argv[]) {
    int addr = shm_get();
    if (addr < 0) {
        printf(1, "shm_get failed\n");
        exit();
    }
    struct shared_locks *shm = (struct shared_locks*)addr;
    shm->lock1.locked = 0;
    shm->lock2.locked = 0;

    int pid = fork();

    if (pid == 0) {
        // Child: Process B (acquires Lock2 then Lock1 -> opposite order!)
        printf(1, "Process B trying to acquire Lock2...\n");
        acquire(&shm->lock2);
        printf(1, "Process B acquired Lock2.\n");

        sleep(10); // Deliberate delay so Process A holds Lock1

        printf(1, "Process B trying to acquire Lock1...\n");
        acquire(&shm->lock1); // DEADLOCK HERE
        printf(1, "Process B acquired Lock1.\n");

        release(&shm->lock1);
        release(&shm->lock2);
        exit();
    } else {
        // Parent: Process A (acquires Lock1 then Lock2)
        printf(1, "Process A trying to acquire Lock1...\n");
        acquire(&shm->lock1);
        printf(1, "Process A acquired Lock1.\n");

        sleep(10); // Deliberate delay so Process B holds Lock2

        printf(1, "Process A trying to acquire Lock2...\n");
        acquire(&shm->lock2); // DEADLOCK HERE
        printf(1, "Process A acquired Lock2.\n");

        release(&shm->lock2);
        release(&shm->lock1);

        wait();
    }

    exit();
}
