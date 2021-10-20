#include <stdlib.h>
#include <pthread.h>

// circular array
typedef struct _queue {
    int size;
    int used;
    int first;
    void **data;
	
	int w_cons;
	int w_prod;
	pthread_mutex_t *buff;
	pthread_cond_t *q_full;
	pthread_cond_t *q_empty;

} _queue;

#include "queue.h"

queue q_create(int size) {
    queue q = malloc(sizeof(_queue));
    
    q->size  = size;
    q->used  = 0;
    q->first = 0;
    q->data  = malloc(size*sizeof(void *));
    
    q->w_cons = 0;
    q->w_prod = 0;
    q->buff = malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(q->buff, NULL);
    
    q->q_full = malloc(sizeof(pthread_cond_t));
    pthread_cond_init( q->q_full, NULL);
    
    q->q_empty = malloc(sizeof(pthread_cond_t));
    pthread_cond_init( q->q_empty, NULL);
    
    return q;
}

int q_elements(queue q) {
    return q->used;
}

int q_insert(queue q, void *elem) {
	
	pthread_mutex_lock(q->buff);		
	while(q->used == q->size){
		q->w_prod ++;
		pthread_cond_wait(q->q_full,q->buff);		
	}

	q->data[(q->first+q->used) % q->size] = elem;    
	q->used++;
		
	if(q->w_cons > 0){
		pthread_cond_signal(q->q_empty);
		q->w_cons --;
	}
	
	pthread_mutex_unlock(q->buff);
	return 1;
}

void *q_remove(queue q) {
    void *res;
    
    pthread_mutex_lock(q->buff);
    
    while(q->used==0){
		q->w_cons++;
		pthread_cond_wait(q->q_empty,q->buff);
	}
    
    res = q->data[q->first];
    q->first = (q->first+1) % q->size;
    q->used--;
    
    if(q->w_prod > 0){
		pthread_cond_signal(q->q_full);
		q->w_prod--;
    }
    pthread_mutex_unlock(q->buff);
    
    return res;
}

void q_destroy(queue q) {
    free(q->data);
    free(q);
}
