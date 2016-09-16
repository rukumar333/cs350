#include "JobsList.h"

int lengthHash = 1024;

JobsList initList(){
    JobsList jobsList;
    jobsList.jobs.head = NULL;
    jobsList.jobs.tail = NULL;
    int i = 0;
    while(i < lengthHash){
	jobsList.hashmap[i].head = NULL;
	jobsList.hashmap[i].tail = NULL;
	++ i;
    }
    return jobsList;
}

pid_t findPID(JobsList * jobsList, pid_t data){
    int hash = data % lengthHash;
    NodeN * it = jobsList->hashmap[hash].head;
    while(it != NULL){
	if(it->data->data == data){
	    return data;
	}
	it = it->next;
    }
    return -1;
}

pid_t deletePID(JobsList * jobsList, pid_t data){
    int hash = data % lengthHash;
    NodeN * it = jobsList->hashmap[hash].head;
    while(it != NULL){
	if(it->data->data == data){
	    deleteNodePID(&jobsList->jobs, it->data);
	    deleteNodeN(&jobsList->hashmap[hash], it);
	    return data;
	}
	it = it->next;
    }
    return -1;    
}

void insert(JobsList * jobsList, pid_t data, char * command){
    NodePID * inserted = insertNodePID(&(jobsList->jobs), data, command);
    NodePID * it = jobsList->jobs.head;
    int hash = data % lengthHash;
    insertNodeN(&(jobsList->hashmap[hash]), inserted);
}

void deleteJobsList(JobsList * jobsList){
    deleteListPID(&(jobsList->jobs));
    int i = 0;
    while(i < lengthHash){
	if(jobsList->hashmap[i].head != NULL){
	    deleteListN(&(jobsList->hashmap[i]));
	}
	++ i;
    }
}

void listJobs(JobsList * jobsList){
    printf("List of background processes:\n");
    NodePID * it = jobsList->jobs.head;
    int status;
    while(it != NULL){
	pid_t pid = waitpid(it->data, &status, WNOHANG);
	if(pid < 0){
	    printf("Error in listJobs()\n");
	    return;
	}
	if(pid == 0){
	    printf("%s with PID %d Status: RUNNING\n", it->command, it->data);
	}else{
	    printf("%s with PID %d Status: FINISHED\n", it->command, it->data);
	    pid_t dpid = deletePID(jobsList, it->data);
	    if(dpid == -1){
		printf("Error in deleting finished job from data structure.\n");
	    }
	}
	it = it->next;
    }
}

NodePID * insertNodePID(struct LinkedListPID * list, pid_t data, char * command){
    if(list->head == NULL){
	list->head = (NodePID *)malloc(sizeof(struct NodePID));
	list->tail = list->head;
	list->head->previous = NULL;
	list->head->next = NULL;
    }else{
	list->tail->next = (NodePID *)malloc(sizeof(struct NodePID));
	list->tail->next->previous = list->tail;
	list->tail = list->tail->next;
    }
    list->tail->next = NULL;
    list->tail->data = data;
    unsigned char lengthCommand = 0;
    unsigned char i = 0;
    while(*(command + i) != '\0'){
	++ lengthCommand;
	++ i;
    }
    list->tail->command = (char *)malloc(sizeof(char) * lengthCommand + 1);
    i = 0;
    while(*(command + i) != '\0'){
	*(list->tail->command + i) = *(command + i);
	++ i;
    }
    *(list->tail->command + i) = '\0';
    return list->tail;
}

NodeN * insertNodeN(struct LinkedListN * list, NodePID * data){
    if(list->head == NULL){
	list->head = (NodeN *)malloc(sizeof(struct NodeN));
	list->tail = list->head;
	list->head->previous = NULL;
	list->head->next = NULL;
    }else{
	list->tail->next = (NodeN *)malloc(sizeof(struct NodeN));
	list->tail->next->previous = list->tail;
	list->tail = list->tail->next;
    }
    list->tail->next = NULL;
    list->tail->data = data;
    return list->tail;    
}

void deleteListPID(LinkedListPID * list){
    NodePID * current = list->head;
    NodePID * temp;
    while(current != NULL){
	temp = current->next;
	free(current);
	current = temp;
    }
    list->head = NULL;
    list->tail = NULL;
}

void deleteListN(LinkedListN * list){
    NodeN * current = list->head;
    NodeN * temp;
    while(current != NULL){
	temp = current->next;
	free(current);
	current = temp;
    }
    list->head = NULL;
    list->tail = NULL;
}

void deleteNodePID(LinkedListPID * list, NodePID * node){
    /* if(node == list->head && node == list->head){ */
    /* 	list->head = NULL; */
    /* 	list->tail = NULL; */
    /* } */
    /* if(node == list->head){ */
    /* 	if(node == list->tail){ */
    /* 	    list->head = NULL; */
    /* 	    list->tail = NULL;    */
    /* 	}else{ */
	    
    /* 	} */
    /* } */
    if(node->previous != NULL){	
	node->previous->next = node->next;
	if(node == list->tail){
	    list->tail = node->previous;
       }
    }
    if(node->next != NULL){
	node->next->previous = node->previous;	
	if(node == list->head){
	    list->head = node->next;
	}
    }
    if(node->previous == NULL && node->next == NULL){
    	list->head = NULL;
    	list->tail = NULL;
    }
    free(node->command);
    free(node);
}

void deleteNodeN(LinkedListN * list, NodeN * node){
    /* if(node->previous == NULL && node->next == NULL){ */
    /* 	list->head = NULL; */
    /* 	list->tail = NULL; */
    /* } */
    /* if(node->previous != NULL){ */
    /* 	node->previous->next = node->next; */
    /* } */
    /* if(node->next != NULL){ */
    /* 	node->next->previous = node->previous;	 */
    /* } */
    if(node->previous != NULL){	
	node->previous->next = node->next;
	if(node == list->tail){
	    list->tail = node->previous;
	}
    }
    if(node->next != NULL){
	node->next->previous = node->previous;	
	if(node == list->head){
	    list->head = node->next;
	}
    }
    if(node->previous == NULL && node->next == NULL){
    	list->head = NULL;
    	list->tail = NULL;
    }
    free(node);
}
