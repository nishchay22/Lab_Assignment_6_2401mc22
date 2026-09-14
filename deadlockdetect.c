#include "kernel/types.h"
#include "user/user.h"

#define N 5 // Number of processes
#define M 3 // Number of resource instances (types)

int Allocation[N][M] = {
    {0, 1, 0},
    {2, 0, 0},
    {3, 0, 3},
    {2, 1, 1},
    {0, 0, 2}
};

int Request[N][M] = {
    {0, 0, 0},
    {2, 0, 2},
    {0, 0, 0},
    {1, 0, 0},
    {0, 0, 2}
};

int Available[M] = {0, 0, 0};

int wait_for_graph[N][N];

void build_wait_for_graph() {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            wait_for_graph[i][j] = 0;
        }
    }
    
    for (int i = 0; i < N; i++) { // P_i requesting
        for (int r = 0; r < M; r++) {
            if (Request[i][r] > 0) {
                // Find who holds resource r
                for (int j = 0; j < N; j++) {
                    if (Allocation[j][r] > 0 && i != j) {
                        wait_for_graph[i][j] = 1;
                    }
                }
            }
        }
    }
}

int visited[N];
int recStack[N];
int cycle_found = 0;
int path[N];
int path_len = 0;

void print_cycle(int start_node, int current_node) {
    printf("Deadlock detected! Cycle: ");
    int i = 0;
    while(path[i] != start_node) i++;
    for (; i < path_len; i++) {
        printf("P%d -> ", path[i]);
    }
    printf("P%d\n", current_node);
}

int dfs(int v) {
    visited[v] = 1;
    recStack[v] = 1;
    path[path_len++] = v;

    for (int i = 0; i < N; i++) {
        if (wait_for_graph[v][i]) {
            if (!visited[i]) {
                if (dfs(i)) return 1;
            } else if (recStack[i]) {
                cycle_found = 1;
                print_cycle(i, i);
                return 1;
            }
        }
    }
    
    recStack[v] = 0;
    path_len--;
    return 0;
}

void check_deadlock() {
    for (int i = 0; i < N; i++) {
        visited[i] = 0;
        recStack[i] = 0;
    }
    cycle_found = 0;
    path_len = 0;

    for (int i = 0; i < N; i++) {
        if (!visited[i]) {
            if (dfs(i)) return; // Stop at first cycle found
        }
    }
    printf("No deadlock detected.\n");
}

int main(int argc, char *argv[]) {
    printf("--- Deadlock Detection using RAG ---\n");
    
    // Scenario 1: Current hardcoded state
    printf("\nScenario 1: Testing with given Request and Allocation matrices\n");
    build_wait_for_graph();
    check_deadlock();

    // Scenario 2: Induce Deadlock (Circular wait P1 -> P3 -> P4 -> P1)
    printf("\nScenario 2: Inducing a circular wait deadlock (P1->P3, P3->P4, P4->P1)\n");
    
    // Set up a clear circular wait for 3 processes
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            wait_for_graph[i][j] = 0;
        }
    }
    wait_for_graph[1][3] = 1;
    wait_for_graph[3][4] = 1;
    wait_for_graph[4][1] = 1;
    
    check_deadlock();

    return 0;
}
