//this code is made by - Manas Patel;

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX_PROCESSES 10

// Frequency and voltage levels (simulated values)
float frequencies[] = {1.0, 1.5, 2.0, 2.5}; // GHz
float voltages[] = {0.9, 1.0, 1.1, 1.2};   // Volts
int num_levels = 4;

typedef struct {
    int pid;
    int arrival_time;
    int burst_time;
    int remaining_time;
    int completion_time;
    int waiting_time;
    int turnaround_time;
    float energy_consumed;
    float frequency;
    float voltage;
} Process;

void swap(Process *a, Process *b) {
    Process temp = *a;
    *a = *b;
    *b = temp;
}

void sort_by_arrival(Process processes[], int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (processes[j].arrival_time > processes[j + 1].arrival_time) {
                swap(&processes[j], &processes[j + 1]);
            }
        }
    }
}

// Select frequency & voltage level based on remaining workload and burst characteristics
void adjust_dvfs(Process *p) {
    int level = 0;
    if (p->remaining_time > 8) level = 3;
    else if (p->remaining_time > 5) level = 2;
    else if (p->remaining_time > 2) level = 1;
    p->frequency = frequencies[level];
    p->voltage = voltages[level];
}

void dvfs_scheduling(Process processes[], int n) {
    int time = 0, completed = 0;
    
    while (completed < n) {
        int min_index = -1;
        int min_burst = __INT_MAX__;
        
        for (int i = 0; i < n; i++) {
            if (processes[i].arrival_time <= time && processes[i].remaining_time > 0) {
                if (processes[i].remaining_time < min_burst) {
                    min_burst = processes[i].remaining_time;
                    min_index = i;
                }
            }
        }
        
        if (min_index == -1) {
            // If no process is available, move time to the next arrival time
            int next_arrival = __INT_MAX__;
            for (int i = 0; i < n; i++) {
                if (processes[i].remaining_time > 0 && processes[i].arrival_time > time) {
                    next_arrival = (processes[i].arrival_time < next_arrival) ? processes[i].arrival_time : next_arrival;
                }
            }
            time = next_arrival;
            continue;
        }
        
        adjust_dvfs(&processes[min_index]);
        int exec_time = processes[min_index].remaining_time;
        processes[min_index].remaining_time = 0;
        time += exec_time;
        processes[min_index].completion_time = time;
        processes[min_index].turnaround_time = processes[min_index].completion_time - processes[min_index].arrival_time;
        processes[min_index].waiting_time = processes[min_index].turnaround_time - processes[min_index].burst_time;
        processes[min_index].energy_consumed = pow(processes[min_index].voltage, 2) * exec_time; // Power ~ V^2 * t
        completed++;
    }
}

void display_metrics(Process processes[], int n) {
    printf("\nPID\tAT\tBT\tCT\tTAT\tWT\tFreq(GHz)\tVolt(V)\tEnergy\n");
    for (int i = 0; i < n; i++) {
        printf("%d\t%d\t%d\t%d\t%d\t%d\t%.1f\t\t%.1f\t\t%.2f\n", processes[i].pid, processes[i].arrival_time, processes[i].burst_time,
               processes[i].completion_time, processes[i].turnaround_time, processes[i].waiting_time,
               processes[i].frequency, processes[i].voltage, processes[i].energy_consumed);
    }
}

int main() {
    int n;
    Process processes[MAX_PROCESSES];
    
    printf("Enter number of processes: ");
    scanf("%d", &n);
    
    printf("Enter Arrival Time and Burst Time for each process:\n");
    for (int i = 0; i < n; i++) {
        processes[i].pid = i + 1;
        printf("Process %d: ", i + 1);
        scanf("%d %d", &processes[i].arrival_time, &processes[i].burst_time);
        processes[i].remaining_time = processes[i].burst_time;
    }
    
    sort_by_arrival(processes, n);
    dvfs_scheduling(processes, n);
    display_metrics(processes, n);
    
    return 0;
}            