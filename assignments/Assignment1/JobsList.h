#ifndef JOBS_LIST_H
#define JOBS_LIST_H

#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>



typedef struct Node{
    pid_t data;
    struct Node * next;
    struct Node * previous;
} Node;

typedef struct LinkedList{
    struct Node * head;
    struct Node * tail;
} LinkedList;

typedef struct JobsList{
    struct LinkedList jobs;
    LinkedList hashmap[1024];
} JobsList;

JobsList initList();
Node * insertLinked(struct LinkedList list, pid_t data);
void insert(struct JobsList list, pid_t data);
void deleteList(struct LinkedList list);
void deleteNode(struct Node * node);


#endif
