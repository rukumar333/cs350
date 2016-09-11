#include "JobsList.h"

int lengthHash = 1024;

JobsList initList(){
    JobsList jobslist;
    jobslist.jobs.head = NULL;
    jobslist.jobs.tail = NULL;
    unsigned char i = 0;
    while(i < lengthHash){
	jobslist.hashmap[i].head = NULL;
	jobslist.hashmap[i].tail = NULL;
	++ i;
    }
}

void insert(JobsList list, pid_t data){
    Node * inserted = insertLinked(list.jobs, data);
    unsigned char hash = data % lengthHash;
    insert(list.hashmap[hash]
}

Node * insertLinked(LinkedList list, pid_t data){
    if(list.head == NULL){
	list.head = (Node *)malloc(sizeof(struct Node));
	list.tail = list.head;
	list.head->previous = NULL;
	list.head->next = NULL;
    }else{
	list.tail->next = (Node *)malloc(sizeof(struct Node));
	list.tail->next->previous = list.tail;
	list.tail = list.tail->next;
    }
    list.tail->next = NULL;
    list.tail->data = data;
    return list.tail;
}

void deleteList(LinkedList list){
    Node * current = list.head;
    Node * temp;
    while(current != NULL){
	temp = current->next;
	free(current);
	current = temp;
    }
}

void deleteNode(Node * node){
    node->previous->next = node->next;
    node->next->previous = node->previous;
    free(node);
}
