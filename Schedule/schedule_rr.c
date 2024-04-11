#include <stdio.h>
#include <stdlib.h>
#include "task.h"
#include "list.h"
#include "cpu.h"
#include "schedulers.h"

struct node *tasks = NULL;
int countTasks = 0;

void add(char *name, int priority, int burst) {
    Task *newTask = malloc(sizeof(Task));
    newTask->name = name;
    newTask->priority = priority;
    newTask->burst = burst;
    countTasks++;
    insert(&tasks, newTask);
}
void inverse() {
    struct node *prev = NULL;
    struct node *current = tasks;
    struct node *next = NULL;

    while (current != NULL) {
        next = current->next;
        current->next = prev;
        prev = current;
        current = next;
    }
    tasks = prev;
}

void pick_NextTask(){
    while (tasks){
        struct node *current = tasks;
        while (current != NULL) {
            Task *currentTask = current->task;
            // Determine the time to execute the task (either time quantum or remaining burst)
            int executionTime = (currentTask->burst < QUANTUM) ? currentTask->burst : QUANTUM;
            run(currentTask, executionTime); // Run the task for the determined time
            currentTask->burst -= executionTime; // Update the remaining burst time

            // If the task is completed, delete it from the list
            if (currentTask->burst <= 0) {
                struct node *temp = current;
                current = current->next;
                delete(&tasks, temp->task);
                free(temp->task);
                free(temp);
            } else {
                current = current->next;
            }
        }
    }

}

void schedule() {
    inverse();
    pick_NextTask();
}