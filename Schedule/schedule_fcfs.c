#include <stdio.h>
#include <stdlib.h>
#include "task.h"
#include "list.h"
#include "cpu.h"
#include "schedulers.h"

struct node *tasks = NULL;

void add(char *name, int priority, int burst) {
    Task *newTask = malloc(sizeof(Task));
    newTask->name = name;
    newTask->priority = priority;
    newTask->burst = burst;
    insert(&tasks, newTask);

}

void pick_NextTask(){
    if (!tasks)
        return;

    Task *task = tasks->task;
    delete(&tasks, task);
    pick_NextTask();
    run(task, task -> burst);
}

void schedule() {
    pick_NextTask();
}