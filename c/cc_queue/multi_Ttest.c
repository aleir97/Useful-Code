#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include "queue.h"
#include <pthread.h>


struct args{
	int n;
	queue *Q;
};

struct thread_info {
	pthread_t       thread_id;        // id returned by pthread_create()
	int             thread_num;       // application defined thread #
};

void *insertar(void *ptr){
	struct args *arg =  ptr;
	void * in;
	
	//usleep(1000000);
	in = (void*) &(arg->n);
	q_insert(*arg->Q, in);
	return NULL;
}

void *eliminar(void *ptr){
	struct args *arg = ptr;
	
	//usleep(5000);
	q_remove(*arg->Q);
	return NULL;
}

void test_queue(){
	int i, *res, elem, ntr1 = 100, ntr2 = 100;
	struct args *arg1, *arg2;
	struct thread_info *tr1, *tr2;
	queue my_Q;
	
	printf("Creating threads\n");
	tr1 = malloc(sizeof(struct thread_info) * ntr1); //tr1 ELIMINAR
	tr2 = malloc(sizeof(struct thread_info) * ntr2); // tr2 INSERTAR
	arg1 = malloc(sizeof(struct args) * ntr1); //tr1
 	arg2 = malloc(sizeof(struct args) * ntr2); // tr2
	
	my_Q = q_create(100); //100 elementos
	
	if (tr1 == NULL || arg1==NULL || tr2 == NULL || arg2==NULL) {
		printf("Not enough memory\n");
		exit(1);
	}

	//tr1
	for(i = 0; i<ntr1; i++){
		tr1[i].thread_num = i;
		
		arg1[i].Q = &my_Q;
		arg1[i].n = i; 
	
		if ((0 != pthread_create(&tr1[i].thread_id, NULL,
					 eliminar, &arg1[i]))) {
			printf("Could not create thread #%d", i);
			exit(1);	
		}
	}

	//tr2
	for (i = 0; i < ntr2; i++) {
		tr2[i].thread_num = i;
		
		arg2[i].Q = &my_Q;
		arg2[i].n = i; 
		
		if (0 != pthread_create(&tr2[i].thread_id, NULL,
					 insertar, &arg2[i])) {
			printf("Could not create thread #%d", i);
			exit(1);
		}
	}
	
	for (i = 0; i < ntr1; i++)
		pthread_join(tr1[i].thread_id, NULL);
		
	for (i = 0; i < ntr2; i++)
		pthread_join(tr2[i].thread_id, NULL);

	printf("\nNUMERO DE ELEMENTOS EN LA COLA: %d \n", q_elements(my_Q));
	elem = q_elements(my_Q);
	for (i = 0; i < elem; i++){
		res = (int*) q_remove(my_Q);
		printf(" %d ",*res);	
	}
	printf("\n");
}

int main(){
	test_queue();
	exit(0);
}
