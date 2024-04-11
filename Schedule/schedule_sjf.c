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


void sort_by_burst() {
    struct node *sorted = NULL;
    struct node *current = tasks;

    while (current != NULL) {
        struct node *next = current->next;

        if (sorted == NULL || sorted->task->burst <= current->task->burst) {
            current->next = sorted;
            sorted = current;
        } else {
            struct node *temp = sorted;
            while (temp->next != NULL && temp->next->task->burst > current->task->burst) {
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

    while (tasks) {
        Task *currentTask = tasks->task;
        run(currentTask, currentTask->burst);
        delete(&tasks, currentTask);
    }
}

void schedule() {
    sort_by_burst();
    pick_NextTask();
}           