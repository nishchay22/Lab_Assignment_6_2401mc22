#include "types.h"
#include "stat.h"
#include "user.h"

// Resource semaphore IDs backed by xv6 kernel semaphores
#define SEM_PRINTER 0 // 2 printers
#define SEM_SCANNER 1 // 1 scanner
#define SEM_DISK    2 // 2 disks
#define SEM_PRINT   3 // Console output lock

void process_task(int pid, int r1, int r2) {
    int i;
    int first  = (r1 < r2) ? r1 : r2;
    int second = (r1 < r2) ? r2 : r1;

    for (i = 0; i < 3; i++) { // 3 iterations
        sem_wait(SEM_PRINT);
        printf(1, "Process P%d requesting resources (ordered: R%d -> R%d)...\n", pid, first, second);
        sem_post(SEM_PRINT);

        // Acquire resources in strict global order: Printer (0) < Scanner (1) < Disk (2)
        sem_wait(first);
        sem_wait(second);

        sem_wait(SEM_PRINT);
        printf(1, "Process P%d GRANTED resources, starting work...\n", pid);
        sem_post(SEM_PRINT);

        sleep(5); // Simulate work

        sem_wait(SEM_PRINT);
        printf(1, "Process P%d releasing resources...\n", pid);
        sem_post(SEM_PRINT);

        sem_post(second);
        sem_post(first);
    }
}

int main(int argc, char *argv[]) {
    int i;
    sem_init(SEM_PRINTER, 2);
    sem_init(SEM_SCANNER, 1);
    sem_init(SEM_DISK,    2);
    sem_init(SEM_PRINT,   1);

    int needs[5][2] = { {0, 1}, {1, 2}, {0, 2}, {0, 1}, {1, 2} };

    for (i = 0; i < 5; i++) {
        int pid = fork();
        if (pid == 0) {
            process_task(i, needs[i][0], needs[i][1]);
            exit();
        }
    }

    for (i = 0; i < 5; i++)
        wait();

    printf(1, "All processes completed successfully.\n");
    exit();
}
