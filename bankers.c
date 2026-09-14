#include "kernel/types.h"
#include "user/user.h"

#define n 5 // Number of processes
#define m 3 // Number of resource types

int Allocation[n][m] = {
    {0, 1, 0},
    {2, 0, 0},
    {3, 0, 2},
    {2, 1, 1},
    {0, 0, 2}
};

int Max[n][m] = {
    {7, 5, 3},
    {3, 2, 2},
    {9, 0, 2},
    {2, 2, 2},
    {4, 3, 3}
};

int Available[m] = {3, 3, 2};
int Need[n][m];

void compute_need() {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            Need[i][j] = Max[i][j] - Allocation[i][j];
        }
    }
}

int safety_algorithm(int *safe_seq) {
    int Work[m];
    int Finish[n];
    for (int i = 0; i < m; i++) Work[i] = Available[i];
    for (int i = 0; i < n; i++) Finish[i] = 0;

    int count = 0;
    while (count < n) {
        int found = 0;
        for (int p = 0; p < n; p++) {
            if (Finish[p] == 0) {
                int j;
                for (j = 0; j < m; j++)
                    if (Need[p][j] > Work[j])
                        break;
                
                if (j == m) {
                    for (int k = 0; k < m; k++)
                        Work[k] += Allocation[p][k];
                    safe_seq[count++] = p;
                    Finish[p] = 1;
                    found = 1;
                }
            }
        }
        if (found == 0) return 0; // System is not safe
    }
    return 1; // System is safe
}

void print_safe_sequence(int *safe_seq) {
    printf("Safe sequence: ");
    for (int i = 0; i < n; i++) {
        printf("P%d ", safe_seq[i]);
    }
    printf("\n");
}

void request_resources(int pid, int request[]) {
    printf("\n--- Process P%d requesting resources: %d %d %d ---\n", pid, request[0], request[1], request[2]);
    for (int i = 0; i < m; i++) {
        if (request[i] > Need[pid][i]) {
            printf("Error: Process has exceeded its maximum claim.\n");
            return;
        }
        if (request[i] > Available[i]) {
            printf("Process P%d must wait, resources not available.\n", pid);
            return;
        }
    }

    // Pretend to allocate
    for (int i = 0; i < m; i++) {
        Available[i] -= request[i];
        Allocation[pid][i] += request[i];
        Need[pid][i] -= request[i];
    }

    int safe_seq[n];
    if (safety_algorithm(safe_seq)) {
        printf("Request granted safely.\n");
        print_safe_sequence(safe_seq);
    } else {
        printf("Request denied - would lead to unsafe state.\n");
        // Rollback
        for (int i = 0; i < m; i++) {
            Available[i] += request[i];
            Allocation[pid][i] -= request[i];
            Need[pid][i] += request[i];
        }
    }
}

int main(int argc, char *argv[]) {
    printf("--- Banker's Algorithm Simulation ---\n");
    compute_need();

    int safe_seq[n];
    if (safety_algorithm(safe_seq)) {
        printf("Initial state is safe.\n");
        print_safe_sequence(safe_seq);
    } else {
        printf("Initial state is unsafe.\n");
        return 0;
    }

    // Scenario 1: Safe request
    int req1[3] = {1, 0, 2};
    request_resources(1, req1);

    // Scenario 2: Unsafe request
    int req2[3] = {3, 3, 0};
    request_resources(4, req2);

    return 0;
}
