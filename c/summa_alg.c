// Alejandro Iregui Valcarcel
// Implementación del algoritmo SUMMA

#include <stdlib.h>
#include <stdio.h>
#include "mpi.h"
#include <math.h>
#include <string.h>

void prodMatrizLocal(float alfa, float *A, float *B, float *C, int m, int n, int k) {
    int i, j, l;
    for (i = 0; i < m; i++) {
        for (j = 0; j < n; j++) {
            for (l = 0; l < k; l++) {
                C[i*n+j] += alfa*A[i*k+l]*B[l*n+j];
            }
        }
    }
}

// Esta función permite utilizar scatterv y gatherv con un datatype modificado para conseguir enviar los elementos como queremos y no tener problemas con el stride. 
void my_send(int rows, int cols, int stride, int procs, float * matrix, float * buffer, int receive_send) {
	int elems[procs], pos[procs], size= (int)round(sqrt(procs));

	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			pos[i* size + j] = i* stride* rows + j* cols;
			elems[i* size + j] = 1;
		}
	}
	
	MPI_Datatype datatype, tmp;
	MPI_Type_vector(rows, cols, stride, MPI_FLOAT, &tmp);
	MPI_Type_create_resized(tmp, 0, sizeof(float), &datatype);
	MPI_Type_commit(&datatype);
	
	if (receive_send) 
		MPI_Gatherv(buffer, rows*cols, MPI_FLOAT, matrix, elems, pos, datatype, 0, MPI_COMM_WORLD);
    else 
		MPI_Scatterv(matrix, elems, pos, datatype, buffer, rows*cols, MPI_FLOAT, 0, MPI_COMM_WORLD);
		
	MPI_Type_free(&datatype);
	MPI_Type_free(&tmp);
}

int main(int argc, char *argv[]) {
    int numprocs, sq_numprocs, rank, i, j, n, m, k, test;
    float alfa;

    MPI_Init(&argc, &argv);
    // Determinar el rango del proceso invocado
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    // Determinar el numero de procesos
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    sq_numprocs = (int) round(sqrt(numprocs));

    // Proceso 0 lee parámetros de entrada
    // Parámetro 1 -> n
    // Parámetro 2 -> alfa
    // Parámetro 3 -> booleano que nos indica si se desea imprimir matrices y vectores de entrada y salida
    if(!rank){
        if(argc>5){
            m= atoi(argv[1]);
            n= atoi(argv[2]);
			k= atoi(argv[3]);
            alfa= atof(argv[4]);
            test= atoi(argv[5]);
        
        }else { 
            printf("NUMERO DE PARAMETROS INCORRECTO\n");
            MPI_Finalize();
            return 0;
        }

        if ((m % sq_numprocs) || (n % sq_numprocs) || (k % sq_numprocs)){
            printf("TAMAÑO DE ALGUN PARAMETRO NO VALIDO !!\n");
            MPI_Finalize();
            return 0;

        }    
    }
    
    // Variables para PACK y UNPACK
	int TAM = 20, pos = 0;
	char buffer[TAM];

    float *A, *B, *C;
    if(!rank){ // Proceso 0 inicializa laS matrices y manda los parámetros
        A = (float *) malloc(m*k*sizeof(float));
		B = (float *) malloc(k*n*sizeof(float));
        C = (float *) malloc(m*n*sizeof(float));

        for(i=0; i<m; i++)
            for(j=0; j<k; j++)
                A[i*k+j] = 1+i+j;

        for(i=0; i<k; i++)
            for(j=0; j<n; j++)
                B[i*n+j] = 1+i+j;

        if(test){
            printf("\nMatriz A es...\n");
            for(i=0; i<m; i++){
                for(j=0; j<k; j++){
                    printf("%f ", A[i*k+j]);
                }
                printf("\n");
            }

            printf("\nMatriz B es...\n");
            for(i=0; i<k; i++){
                for(j=0; j<n; j++){
                    printf("%f ", B[i*n+j]);
                }
                printf("\n");
            }
            printf("\n");
        }

        MPI_Pack(&m, 1, MPI_INT, buffer, TAM, &pos, MPI_COMM_WORLD);
		MPI_Pack(&n, 1, MPI_INT, buffer, TAM, &pos, MPI_COMM_WORLD);
		MPI_Pack(&k, 1, MPI_INT, buffer, TAM, &pos, MPI_COMM_WORLD);
		MPI_Pack(&alfa, 1, MPI_FLOAT, buffer, TAM, &pos, MPI_COMM_WORLD);
    }

    MPI_Bcast(buffer, TAM, MPI_PACKED, 0, MPI_COMM_WORLD);
    
    if (rank){
        MPI_Unpack(buffer, TAM, &pos, &m, 1, MPI_INT, MPI_COMM_WORLD);
		MPI_Unpack(buffer, TAM, &pos, &n, 1, MPI_INT, MPI_COMM_WORLD);
		MPI_Unpack(buffer, TAM, &pos, &k, 1, MPI_INT, MPI_COMM_WORLD);
		MPI_Unpack(buffer, TAM, &pos, &alfa, 1, MPI_FLOAT, MPI_COMM_WORLD);
    }

    int mBlock= m /sq_numprocs, nBlock= n/sq_numprocs, kBlock= k/sq_numprocs;

    float *localA= calloc(mBlock*kBlock, sizeof(float));	
    float *localB= calloc(kBlock*nBlock, sizeof(float));
    float *localC= calloc(mBlock*nBlock, sizeof(float));

	float *bufA= calloc(mBlock*kBlock, sizeof(float));
    float *bufB= calloc(kBlock*nBlock, sizeof(float));

    // Variables para crear las topologias
	int dim[2]= {sq_numprocs, sq_numprocs}, period[2]= {0, 0}, coords[2];
	
    // Dimensiones para quedarnos con las filas del cartesiano
    int rows_dim[2]= {0, 1}; 
	MPI_Comm mesh, rows, cols;

    // Creamos la topología malla y luego la dividimos en filas y columnas
    MPI_Cart_create(MPI_COMM_WORLD, 2, dim, period, 0, &mesh);
	MPI_Cart_sub(mesh, rows_dim, &rows);

	int key= rank / sq_numprocs, color= rank % sq_numprocs;
	MPI_Comm_split(MPI_COMM_WORLD, color, key, &cols); 
	MPI_Cart_coords(mesh, rank, 2, coords);
    
    double t;
    MPI_Barrier(MPI_COMM_WORLD); // Barrera para garantizar una correcta medida de tiempo
    t = MPI_Wtime();
    
    my_send(mBlock, kBlock, k, numprocs, A, localA, 0);
	my_send(kBlock, nBlock, n, numprocs, B, localB, 0);

	// Cuerpo principal de SUMMA
	for (int it = 0; it < sq_numprocs; it++) {	
		if (it == coords[1]) {
			memcpy(bufA, localA, mBlock*kBlock*sizeof(float));
		}
		MPI_Bcast(bufA, mBlock*kBlock, MPI_FLOAT, it, rows);
		
		if (it == coords[0]) {
			memcpy(bufB, localB, kBlock*nBlock*sizeof(float));
		}
		MPI_Bcast(bufB, kBlock*nBlock, MPI_FLOAT, it, cols);

        prodMatrizLocal(alfa, bufA, bufB, localC, mBlock, nBlock, kBlock); 
	}
	
	// Gatherv de los resultados
	my_send(mBlock, nBlock, n, numprocs, C, localC, 1);
    t = MPI_Wtime()-t;

	fflush(NULL);
	MPI_Barrier(MPI_COMM_WORLD);

    if(!rank){
       if(test){

            printf("\nAl final matriz c es...\n");
                for(i=0; i<m; i++){
                    for(j=0; j<n; j++){
                        printf("%f ", C[i*n+j]);
                    }
                    printf("\n");
                }
            printf("\n");

            // Solo el proceso 0 calcula el producto y compara los resultados del programa secuencial con el paralelo
            float *testC = (float *) malloc(m*n*sizeof(float));
            for(i=0; i<m; i++){
                for(j=0; j<n; j++){
                    testC[i*n+j] = 0;
                }
            }

            prodMatrizLocal(alfa, A, B, testC, m, n, k);
            int errores = 0;
            for(i=0; i<m; i++){
                for(j=0; j<n; j++){
                    if (testC[i*n+j] != C[i*n+j]) {
                    errores++;
                    printf("\n Error en la posicion (%d,%d) porque %f != %f", i, j, testC[i*m+j], C[i*m+j]);
                    }
                }
            }

            printf("\n Errores en el producto de A y B -> %d \n", errores);
            free(testC);
        }		

        free(bufA);
		free(bufB);
		free(localA);
		free(localB);
		free(localC);
        free(A);
        free(B);
        free(C);
		MPI_Comm_free(&mesh);
		MPI_Comm_free(&rows);
		MPI_Comm_free(&cols);
    }
	
    // Barrera para que no se mezcle la impresión del tiempo con la de los resultados
    fflush(NULL);
    for (i = 0; i< numprocs; i++){
        MPI_Barrier(MPI_COMM_WORLD);
        if (rank == i)
            printf("\nProducto de A(%d,%d) y B(%d,%d). Tiempo de ejecucion del proceso: %d fue %.4lf \n", m, n, k, n, rank, t);
    }
    MPI_Finalize();
    return 0;
}
