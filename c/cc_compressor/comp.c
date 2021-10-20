#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <pthread.h>
#include "compress.h"
#include "chunk_archive.h"
#include "queue.h"
#include "options.h"

#define CHUNK_SIZE (1024*1024)
#define QUEUE_SIZE 10

#define COMPRESS 1
#define DECOMPRESS 0

struct thread_info {
	pthread_t       thread_id;        // id returned by pthread_create()
	int             thread_num;       // application defined thread #
};

struct args {
	int   tr_num;       // Nº THREAD
	int   *it;
	pthread_mutex_t *m_it; 
	chunk chunk_t;
	queue *in;
	queue *out;
	chunk (*process)(chunk);
};

struct args_in_c {
	int size;
	int num_chunks;
	int fd;
	queue *in;
};

struct args_out_c {
	archive arch;
	int num_chunks;
	queue *out;
};

struct args_in_d {
	archive arch;
	queue *in;
};

struct args_out_d {
	archive arch;
	int fd;
	queue *out;
};

void *read_chunk_c(void *ptr){
	struct args_in_c *arg = ptr;
	chunk ch;
	int i, offset;
	
	for(i=0; i<arg->num_chunks; i++) {
		
		ch = alloc_chunk(arg->size);
		
        offset = lseek(arg->fd, 0, SEEK_CUR);
		
        ch->size   = read(arg->fd, ch->data, arg->size);
        ch->num    = i;
        ch->offset = offset;
        
        q_insert(*arg->in, ch); 
	}
	return NULL;
}

void *write_chunk_c(void * ptr){
	struct args_out_c *arg = ptr;
	chunk ch;
	int i;
	
	for(i=0; i<arg->num_chunks; i++) {
		ch = q_remove(*arg->out);
		add_chunk(arg->arch, ch);
		free_chunk(ch);	
    }
    return NULL;
}

void *read_chunk_d(void * ptr){
	struct args_in_d *arg = ptr;
	chunk ch;
	int i;
	
	for(i=0; i<chunks(arg->arch); i++) {
        ch = get_chunk(arg->arch, i);
        q_insert(*arg->in, ch);
    }
    return NULL;
}

void *write_chunk_d (void * ptr){
	struct args_out_d *arg = ptr;
	chunk ch;
	int i;
	
	for(i=0; i<chunks(arg->arch); i++) {
        ch = q_remove(*arg->out);
        lseek(arg->fd, ch->offset, SEEK_SET);
        write(arg->fd, ch->data, ch->size);
        free_chunk(ch);
    }
    return NULL;
}

// take chunks from queue in, run them through process (compress or decompress), send them to queue out
void *worker(void *ptr) {
	struct args *arg = ptr;
    chunk ch, res;
    int cont;
    
    while(1) {
		pthread_mutex_lock(arg->m_it);
		if(*arg->it == 0){
			pthread_mutex_unlock(arg->m_it);
			break;
		}
		cont = *arg->it;
		*arg->it =	*arg->it -1 ;
		pthread_mutex_unlock(arg->m_it);
		
		printf("\nTHREAD Nº%d PROCESANDO EN LA ITERACION: %d",arg->tr_num,cont);
        ch = q_remove(*(arg->in));

        res = arg->process(ch);
        res->num = ch->num;
        free_chunk(ch);
        
        q_insert(*(arg->out), res);
    }
    return NULL;
}

// Compress file taking chunks of opt.size from the input file,
// inserting them into the in queue, running them using a worker,
// and sending the output from the out queue into the archive file
void comp(struct options opt) {
    int fd, chunks, i, iterations;
	pthread_mutex_t *m_iterations; 
	struct thread_info *threads;
	pthread_t read_t, write_t;
	struct args *arg;
	struct args_in_c arg_in_c; 
	struct args_out_c arg_out_c; 
    struct stat st;
    char comp_file[256];
    archive ar;
    queue in, out;
   
    
    if((fd=open(opt.file, O_RDONLY))==-1) {
        printf("Cannot open %s\n", opt.file);
        exit(0);
    }
    
    threads = malloc(sizeof(struct thread_info) * opt.num_threads);
	arg = malloc(sizeof(struct args) * opt.num_threads);	
	m_iterations = malloc(sizeof(pthread_mutex_t));
    
	if (threads == NULL || arg==NULL || m_iterations == NULL ){
		printf("Not enough memory\n");
		exit(1);
	}
	pthread_mutex_init(m_iterations, NULL);//inicializamos el mutex de las iteraciones
	
    fstat(fd, &st);
    chunks = st.st_size/opt.size+(st.st_size % opt.size ? 1:0);
    iterations = chunks; //variable iterations que reparte los chunks entre los threads
        
    if(opt.out_file) {
        strncpy(comp_file,opt.out_file,255);
    } else {
        strncpy(comp_file, opt.file, 255);
        strncat(comp_file, ".ch", 255);
    }
    
    ar = create_archive_file(comp_file);
	
    in  = q_create(QUEUE_SIZE);
    out = q_create(QUEUE_SIZE);

    // read input file and send chunks to the in queue
    // prepare the arguments for the thread
	arg_in_c.size = opt.size;
	arg_in_c.num_chunks = chunks;
	arg_in_c.fd = fd;
	arg_in_c.in = &in;
	
	if ( 0 != pthread_create(&read_t, NULL,
		read_chunk_c,&arg_in_c)) {
		printf("Error");
		exit(1);
	}

    // compression of chunks from in to out
    for (i = 0; i < opt.num_threads; i++) {
		arg[i].tr_num = i;
		arg[i].it = &iterations;
		arg[i].m_it = m_iterations;
		
		arg[i].in = &in;
		arg[i].out = &out;
		arg[i].process = zcompress;
		
		if ( 0 != pthread_create(&threads[i].thread_id, NULL,
					 worker,&arg[i])) {
			printf("Could not create thread #%d", i);
			exit(1);
		}
    }
      
    // send chunks to the output archive file
	// prepare the arguments for the thread
    arg_out_c.arch = ar;
    arg_out_c.num_chunks = chunks;
    arg_out_c.out = &out;
    
    if ( 0 != pthread_create(&write_t, NULL,
					 write_chunk_c,&arg_out_c)) {
			printf("Error");
			exit(1);
		}
	
	//wait for threads
    for (i = 0; i < opt.num_threads; i++)
		pthread_join(threads[i].thread_id, NULL);	
	pthread_join(read_t, NULL);
	pthread_join(write_t, NULL);
		
    free(threads);
    free(arg);
    pthread_mutex_destroy(m_iterations);
    free(m_iterations);
    close_archive_file(ar);
    close(fd);
    q_destroy(in);
    q_destroy(out);
}


// Decompress file taking chunks of opt.size from the input file,
// inserting them into the in queue, running them using a worker,
// and sending the output from the out queue into the decompressed file
void decomp(struct options opt) {
    int fd, i, iterations;
    struct thread_info *threads;
	pthread_t read_t, write_t;
	pthread_mutex_t *m_iterations; 
	struct args *arg;
    struct args_in_d arg_in_d; 
	struct args_out_d arg_out_d;
    char uncomp_file[256];
    archive ar;
    queue in, out;
    chunk ch;
    
    threads = malloc(sizeof(struct thread_info) * opt.num_threads);
	arg = malloc(sizeof(struct args) * opt.num_threads);
	m_iterations = malloc(sizeof(pthread_mutex_t));
	
	if (threads == NULL || arg == NULL || m_iterations == NULL ){
		printf("Not enough memory\n");
		exit(1);
	}
    pthread_mutex_init(m_iterations, NULL);//inicializamos el mutex de las iteraciones
    
    if((ar=open_archive_file(opt.file))==NULL) {
        printf("Cannot open archive file\n");
        exit(0);
    };
    
    if(opt.out_file) {
        strncpy(uncomp_file, opt.out_file, 255);
    } else {
        strncpy(uncomp_file, opt.file, strlen(opt.file) -3);
        uncomp_file[strlen(opt.file)-3] = '\0';
    }

    if((fd=open(uncomp_file, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH))== -1) {
        printf("Cannot create %s: %s\n", uncomp_file, strerror(errno));
        exit(0);
    } 

    in  = q_create(QUEUE_SIZE);
    out = q_create(QUEUE_SIZE);
	iterations = chunks(ar);
	
	// arguments for the thread that reads
    arg_in_d.arch = ar;
	arg_in_d.in = &in;
	if ( 0 != pthread_create(&read_t, NULL,
				 read_chunk_d, &arg_in_d)) {
		printf("Error");
		exit(1);
	}
    
    // decompress from in to out
    for (i = 0; i < opt.num_threads; i++) {
		arg[i].tr_num = i;
		arg[i].it = &iterations;
		arg[i].m_it = m_iterations;
		
		arg[i].in = &in;
		arg[i].out = &out;
		arg[i].process = zdecompress;
		
		if ( 0 != pthread_create(&threads[i].thread_id, NULL,
					 worker,&arg[i])) {
			printf("Could not create thread #%d", i);
			exit(1);
		}	
    }
   
	// arguments for the thread that writes
	arg_out_d.arch=ar;
    arg_out_d.fd=fd;
    arg_out_d.out=&out;
    
    if ( 0 != pthread_create(&write_t, NULL,
					 write_chunk_d,&arg_out_d)) {
		printf("Error");
		exit(1);
	}
    
    //wait for threads
    for (i = 0; i < opt.num_threads; i++)
		pthread_join(threads[i].thread_id, NULL);
	pthread_join(read_t, NULL);
	pthread_join(write_t, NULL);
		
    pthread_mutex_destroy(m_iterations);
    free(m_iterations);
    free(threads);
    free(arg);
    close_archive_file(ar);    
    close(fd);
    q_destroy(in);
    q_destroy(out);
}

int main(int argc, char *argv[]) {  
    struct options opt;
    
    opt.compress    = COMPRESS;
    opt.num_threads = 5;
    opt.size        = CHUNK_SIZE;
    opt.queue_size  = QUEUE_SIZE;
    opt.out_file    = NULL;
    
    read_options(argc, argv, &opt);
    
    if(opt.compress == COMPRESS) comp(opt);
    else decomp(opt);
	printf("\n");
}

    
    
