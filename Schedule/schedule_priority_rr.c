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
void sort_by_priority() {
    struct node *sorted = NULL;
    struct node *current = tasks;

    while (current != NULL) {
        struct node *next = current->next;

        if (sorted == NULL || sorted->task->priority <= current->task->priority) {
            current->next = sorted;
            sorted = current;
        } else {
            struct node *temp = sorted;
            while (temp->next != NULL && temp->next->task->priority > current->task->priority) {
                temp = temp->next;
            }
            current->next = temp->next;
            temp->next = current;
        }

        current = next;
    }
    tasks = sorted;
}

void pick_NextTask(){
    if (tasks == NULL) {
        return;
    }
    while (tasks){
        struct node *current = tasks;
        while(current != NULL) {
            Task *currentTask = current->task;
            int executionTime = (currentTask->burst < QUANTUM) ? currentTask->burst : QUANTUM;
            run(currentTask, executionTime);
            currentTask->burst -= executionTime;

            if (currentTask->burst <= 0) {
                struct node *temp = current;
                current = current->next;
                delete(&tasks, temp->task);
                free(temp->task);
                free(temp);
            } else {
                if(current->next != NULL && current->task->priority == current->next->task->priority){
                    current = current->next;
                }
                else {
                    break;
                }
            }
        }
    }
}

void schedule() {
    sort_by_priority();
    pick_NextTask();
}