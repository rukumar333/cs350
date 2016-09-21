#ifndef JOBS_LIST_H
#define JOBS_LIST_H

#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

typedef struct NodePID{
    pid_t data;
    char * command;
    /*
      status == 0 then command running
      status == 1 then command finished
     */
    char status;
    struct NodePID * next;
    struct NodePID * previous;
} NodePID;

typedef struct LinkedListPID{
    struct NodePID * head;
    struct NodePID * tail;
} LinkedListPID;

typedef struct NodeN{
    struct NodePID *  data;
    struct NodeN * next;
    struct NodeN * previous;
} NodeN;

typedef struct LinkedListN{
    struct NodeN * head;
    struct NodeN * tail;
} LinkedListN;

typedef struct JobsList{
    struct LinkedListPID jobs;
    LinkedListN hashmap[1024];
} JobsList;

JobsList initList();
void insert(struct JobsList * jobsList, pid_t data, char * command);
pid_t findPID(struct JobsList * jobsList, pid_t data);
pid_t deletePID(struct JobsList * jobsList, pid_t data);
void deleteJobsList(struct JobsList * jobsList);
void listJobs(struct JobsList * jobsList);
char setPIDStatus(struct JobsList * jobsList, pid_t data, char status);

NodePID * insertNodePID(struct LinkedListPID * list, pid_t data, char * command);
NodeN * insertNodeN(struct LinkedListN * list, NodePID * data);

void deleteListPID(struct LinkedListPID * list);
void deleteListN(struct LinkedListN * list);

void deleteNodePID(struct LinkedListPID * list, struct NodePID * node);
void deleteNodeN(struct LinkedListN * list, struct NodeN * node);


#endif
