/*
Grupo 1.4
Souto Baña, Julio Manuel - j.soutob DNI: 78809733
Iregui Valcarcel, Alejandro - a.ireguiv DNI: 53309657 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <pwd.h>
#include <time.h>
#include <dirent.h>
#include <stdlib.h>
#include <grp.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include "list.h"
#include <fcntl.h>

#define LEERCOMPLETO ((ssize_t)-1)

#define TRUE  1
#define FALSE 0

//funciones de utilidad
char TipoFichero (mode_t m){
	switch (m&S_IFMT) { /*and bit a bit con los bits de formato,0170000 */
	case S_IFSOCK: return 's'; /*socket */
	case S_IFLNK: return 'l'; /*symbolic link*/
	case S_IFREG: return '-'; /* fichero normal*/
	case S_IFBLK: return 'b'; /*block device*/
	case S_IFDIR: return 'd'; /*directorio */
	case S_IFCHR: return 'c'; /*char device*/
	case S_IFIFO: return 'p'; /*pipe*/
	default: return '?'; /*desconocido, no deberia aparecer*/
	}
}


char * ConvierteModo (mode_t m, char *permisos){
	strcpy (permisos,"---------- ");
	permisos[0]=TipoFichero(m);
	if (m&S_IRUSR) permisos[1]='r';
	if (m&S_IWUSR) permisos[2]='w';
	if (m&S_IXUSR) permisos[3]='x';
	if (m&S_IRGRP) permisos[4]='r';
	if (m&S_IWGRP) permisos[5]='w';
	if (m&S_IXGRP) permisos[6]='x';
	if (m&S_IROTH) permisos[7]='r';	
	if (m&S_IWOTH) permisos[8]='w';
	if (m&S_IXOTH) permisos[9]='x';
	if (m&S_ISUID) permisos[3]='s';
	if (m&S_ISGID) permisos[6]='s';
	if (m&S_ISVTX) permisos[9]='t';
return permisos;}


int TrocearCadena (char* cadena, char * trozos[], const char* coincidencia){
	int i= 1;
	
	if ((trozos[0]=strtok(cadena,coincidencia)) == NULL)
		return 0;
	
	while ((trozos[i]=strtok(NULL,coincidencia)) != NULL)
		i ++; 
	
	return i;
}


ssize_t EscribirFichero(char *fich, void *p, ssize_t n){
	 /*n=-1 indica que se lea todo*/

	ssize_t nleidos,tam=n;
	int df, aux;
	struct stat s;

	if ((df=open(fich, O_WRONLY | O_CREAT, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH))==-1 || stat (fich,&s)==-1)
		return ((ssize_t)-1);

	if ((nleidos=write(df,p, tam))==-1){
		aux=errno;
		close(df);
		errno=aux;
		return ((ssize_t)-1);
	}

	close (df);

	return (nleidos);
}

ssize_t LeerFichero(char *fich, void *p, ssize_t n){
	 /*n=-1 indica que se lea todo*/
	ssize_t nleidos,tam=n;
	int df, aux;
	struct stat s;

	if (stat (fich,&s)==-1 || (df=open(fich,O_RDONLY))==-1)
		return ((ssize_t)-1);

	if (n==LEERCOMPLETO)
		tam=(ssize_t) s.st_size;

	if ((nleidos=read(df,p, tam))==-1){
		aux=errno;
		close(df);
		errno=aux;
		return ((ssize_t)-1);
	}

	close (df);

	return (nleidos);
}	
////////////////comandos del shell//////////////////////////////
void pid(int numtokens, char * tokens[]){
	if (numtokens == 1){
		printf("PID: %d\n", getpid());
		printf("Este comando devuelve el PID del proceso ejecutando el shell\n");
		printf("opcion -p devuelve el Parent's PID\n");}
	else if (strcmp(tokens[1],"-p") == 0) 
		printf("Parent's PID: %d\n", getppid());
	else 
		printf("La opcion introducida no es valida\n");
			
	
}	

///////////////////////////////////////////////////////////////////////////////////////////
void chdirect(int numtokens, char * tokens[]){
	if (numtokens == 1){
		char cwd[50];
   		if (getcwd(cwd, sizeof(cwd)) != NULL)
       		printf("Current working dir: %s\n", cwd);}
	else if (strcmp(tokens[1],"direct") == 0){
		if (chdir(tokens[1]) == 0)
			printf("Se cambio el directorio de trabajo actual\n");
		else
			printf("Directorio no existente\n");
	}
	else
		printf("La opcion introducida no es valida\n");
		
}	

////////////////////////////////////////////////////////////////////////////////////////////
void fecha (void){
	time_t t = time(NULL);
    	struct tm tm = *localtime(&t);

    	printf("Fecha actual: %d-%d-%d\n", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
}

//////////////////////////////////////////////////////////////////////////////////////////
void hora(void){
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);

	printf("Hora actual: %d:%d:%d\n", tm.tm_hour, tm.tm_min, tm.tm_sec);
}

//////////////////////////////////////////////////////////////////////////////////
void autores (char * tokens[], int numtokens){
	if (numtokens == 1){
			printf("Este comando imprime el nombre de los autores\n");
			printf("opcion -l sus logins\n");
			printf("opcion -n sus nombres\n");     	
		
		}else if (numtokens == 2){
				if (strcmp(tokens[1] ,"-l") == 0 )
					printf("a.ireguiv y j.soutob\n");
				else if (strcmp(tokens[1] ,"-n") == 0 )
					     printf("Julio Manuel y ALejandro\n");
				else printf("La opción que introdujo no es correcta\n");	     
		
		}else if (numtokens == 3) {
				if ( ((strcmp(tokens[1], "-l") == 0 ) & (strcmp(tokens[2],"-n") == 0)) || ((strcmp(tokens[2],"-l") == 0) & (strcmp(tokens[1], "-n") == 0)) ){
				printf("Julio Manuel y ALejandro\n");
				printf("a.ireguiv y j.soutob\n");
				}else printf("No se reconoce alguno de los comandos que  introdujo\n");	
		
		}else if (numtokens > 3 ){ 
				printf("No se reconocen las opciones introducidas\n");   
				printf("opcion -l sus logins\n");
				printf("opcion -n sus nombres\n");}  
}

//////////////////////////////////////////////////////
void create(char* tokens[],int numtokens){
	char permisos[11], cwd[1024], cwd_usr_req[50];
	mode_t mode_actual_dir;
	struct stat statbuf;
	uid_t uid = getuid();
	gid_t gid = getgid(); 
	
	//Comprobación de la existencia de opciones y número de trozos de la linea de entrada
	if (numtokens==1){
		printf("Este comando crea un archivo o un directorio dependiendo de la opcion\n");
		printf("Sintaxis:\n");
		printf("create -d folder (crea un directorio)\n");
		printf("create fich.txt (crea un fichero)\n");
		
	}else if (numtokens==2){
			  //Creación de ficheros de cualquier tipo
			  if (fopen(tokens[1],"ab+") == NULL)
				  printf("%s/n",strerror(errno));	  

	//Creación de ficheros con la opción -d	      
	}else if (numtokens==3){
			  if (strcmp(tokens[1] ,"-d") == 0 ){
				 
				  if (* tokens[2] != '/'){ //if que crea un directorio en el cwd cuando no indican ningún path
					  if (getcwd(cwd, sizeof(cwd)) != NULL){	
						  stat(cwd,&statbuf);
						  mode_actual_dir = statbuf.st_mode;
						  ConvierteModo(mode_actual_dir, permisos); //Extraemos el modo del cwd y rellenamos el string permisos
		
						  strcpy(cwd_usr_req,"/");
						  strcat(cwd_usr_req, tokens[2]); //cwd_usr_req es un string que contiene "/+ nombre del fichero que se quiere crear"	
					      
						  if (statbuf.st_uid == uid){ //Si el que inicio el proceso del shell = propietario de cwd...
							  if (permisos[2] == 'w'){
								  if (mkdir(strcat(cwd,cwd_usr_req),S_IRUSR | S_IWUSR | S_IXUSR) == -1)
									  printf("%s/n",strerror(errno));		
							  }else printf("cannot create %s: permission denied\n",tokens[2]);	
						  }else if (statbuf.st_gid == gid){ //Si el que inicio el proceso del shell esta en el mismo group que el cwd...
									if (permisos[5] == 'w'){
										if (mkdir(strcat(cwd,cwd_usr_req),S_IRUSR | S_IWUSR | S_IXUSR) == -1)
									        printf("%s/n",strerror(errno));		
									}else printf("cannot create %s: permission denied\n",tokens[2]);	
						  }else if (permisos[8] == 'w'){ //Si no se cumple lo anterior se miran los permisos de others del cwd
									if (mkdir(strcat(cwd,cwd_usr_req), S_IRUSR | S_IWUSR | S_IXUSR) == -1)
									    printf("%s\n",strerror(errno));		
							    }else printf("cannot create %s: permission denied\n",tokens[2]);	 
					  }else printf("CWD function overflowed the buffer\n");
				  
				  //Cuando nos indican el path: 		  
				  }else if (mkdir(tokens[2],777) == -1){
							  if (errno == 13)
							      printf("cannot create %s: permission denied\n",tokens[2]);	 
							  else printf("%s\n",strerror(errno));	 
					    }
			  }else printf("Opción no reconocida");
	}
}
//////////////////////////////////////////////////////////////////////////////////////
int borrarCarp(char * carpeta){
	//Función auxiliar para vaciar la carpeta que queremos borrar
	
	DIR *d = opendir(carpeta);
	struct dirent *p;
	char path[257];

	if (d == NULL)
		return 1;
	
	//Recorremos las entradas del directorio
	while ((p=readdir(d))){
		DIR *sub_d;
		FILE *file;
		//Discriminamos la entrada . del directorio 
        if ( ( (*(p->d_name) != '.')) || ((strlen(p->d_name) > 1) && (p->d_name[1] != '.'))){   
            sprintf(path, "%s/%s", carpeta, p->d_name);
            if( (sub_d = opendir(path))){   
               closedir(sub_d);
               borrarCarp(path);
			   rmdir(path);
            }   
            else if((file = fopen(path, "r"))){
                    fclose(file);
                    remove(path);
            }      
        }   
    }   
    remove(path);

	return 0;   
}

void delete(int numtokens, char * tokens[]){
	
	char permisos[11];
	mode_t mode_actual_dir;
	struct stat statbuf;
	uid_t uid = getuid();
	gid_t gid = getgid(); 

	//Estructura que comprueba la existencia de opciones y del numero de trozos de la linea de entrada
	if (numtokens == 1){
			printf("Este comando borra un fichero vacio\n");
			printf("opcion -r para borrar un fichero y su contenido\n");		
		}		
	else if(numtokens == 2){
		if (strcmp(tokens[1],"-r") == 0)
			printf("Opcion no valida\n");
		else{ //Eliminado de ficheros
			stat(tokens[1],&statbuf);
			mode_actual_dir = statbuf.st_mode;
			ConvierteModo(mode_actual_dir, permisos);
			//Comprobación de permisos para la eliminación de ficheros
			if (statbuf.st_uid == uid){
				if (permisos[2] == 'w'){
					if (remove(tokens[1]) == 0)
						printf("El fichero fue eliminado\n");
					else
						printf("1El fichero no se ha podido eliminar\n");}
				else
					printf("cannot delete %s: permission denied\n",tokens[1]);}

			else if (statbuf.st_gid == gid){
				if (permisos[5] == 'w'){
					if (remove(tokens[1]) == 0)
						printf("El fichero fue eliminado\n");
					else
						printf("El fichero no se ha podido eliminar\n");}
				else
					printf("cannot delete %s: permission denied\n",tokens[1]);}

			else if (permisos[8] == 'w'){
				if (remove(tokens[1]) == 0)
					printf("El fichero fue eliminado\n");
				else
					printf("El fichero no se ha podido eliminar\n");}
			else
				printf("cannot delete %s: permission denied\n",tokens[1]);
		}
	}
	else if(numtokens == 3){ //Eliminado de directorios
		if (strcmp(tokens[1],"-r") == 0){
			stat(tokens[2],&statbuf);
			mode_actual_dir = statbuf.st_mode;
			ConvierteModo(mode_actual_dir, permisos);
			//Comprobación de los respectivos permisos para la eliminación de directorios
			if (statbuf.st_uid == uid){
				if (permisos[2] == 'w'){
					if (borrarCarp(tokens[2]) == 0){
						rmdir(tokens[2]);
						printf("El directorio fue eliminado\n");}
					else
						printf("El directotio no se ha podido eliminar\n");}
				else
					printf("cannot delete %s: permission denied\n",tokens[2]);}

			else if (statbuf.st_gid == gid){
				if (permisos[5] == 'w'){
					if (borrarCarp(tokens[2]) == 0){
						rmdir(tokens[2]);
						printf("El directorio fue eliminado\n");}
					else
						printf("El directorio no se ha podido eliminar\n");}
				else
					printf("cannot delete %s: permission denied\n",tokens[2]);}

			else if (permisos[8] == 'w'){
				if (borrarCarp(tokens[2]) == 0){
					rmdir(tokens[2]);
					printf("El directorio fue eliminado\n");}
				else
					printf("El fichero no se ha podido eliminar\n");}
			else
				printf("cannot delete %s: permission denied\n",tokens[2]);
					
		}
		else
			printf("Opcion no valida\n");				
		}		
	else
		printf("Opcion no valida\n");
}

//////////////////////////////////////////////////////////////////////////////////////
void query(char* tokens[],int numtokens){
	char permisos[11], cwd[1024], date[50], name_link[1024];
	char * PATH;
	int i;
	mode_t mode_token;
	struct stat statbuf;
	struct passwd * pw;
	struct group * grp;
	ssize_t len;
	//Estructura que comprueba la existencia de opciones y del número de trozos de la linea introducida
	if (numtokens==1){
		printf("Este comando da información de los ficheron o directorios introducidos como argumentos\n");
		printf("Sintaxis:\n");
		printf("query nombre1 nombre2 nombre3\n");
	
	}else {
		for (i = 1; i < numtokens; i++){
			//Asignación de la variable PATH según lo introducido por el usuario
			if (*tokens[i] == '/'){ //Si nos da el PATH completo
				PATH = tokens[i];	
			}else if (getcwd(cwd,1024) != NULL){ //Si solo nos da 1 nombre suponemos que se refiere al CWD y concatenamos
					  strcat(cwd, "/");
					  strcat(cwd, tokens[i]);  		
					  PATH = cwd;	  
				  }else printf("CWD function overflowed the buffer\n");
		   
		    //Extramos la información del inodo 
		    if (lstat(PATH,&statbuf) != -1){
				mode_token = statbuf.st_mode;	
				ConvierteModo(mode_token, permisos);	
				
				pw = getpwuid(statbuf.st_uid);
				grp = getgrgid(statbuf.st_gid);
					
				printf("%lu ",statbuf.st_ino);
				printf("%s ",permisos);
				printf("%lu ",statbuf.st_nlink);
				printf("%s ",pw->pw_name);				
				printf("%s ",grp->gr_name);
				printf("%5d ",(signed int) statbuf.st_size);
				strftime(date, 50, "%b %d %H:%M ", localtime(&statbuf.st_mtime));
				printf("%s %s ",date, PATH);
				
				//Comprobación de link simbólico
				if (permisos[0] == 'l'){
				len = readlink(PATH,name_link,1024);
				name_link[len]='\0';
				printf("-> %s",name_link);
				}
			}else printf("%s\n",strerror(errno));	
			printf("\n");
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void cmd_list(char* tokens[],int numtokens, const int recursive){
	int n_op = FALSE, h_op = FALSE , r_op = FALSE;
	int cont_op = 0, cwd_bool = FALSE;
	
	char permisos[11], cwd[1024], date[50], name_link[1024], AUXPATH[1024], AUXPATH2[1024];
	char *PATH, *entry_tok[4], *trozos_PATH[1024];
	int i, ntrozos_PATH, show = TRUE;
	mode_t mode_token;
	struct stat statbuf;
	struct passwd *pw;
	struct group * grp;
	ssize_t len;
	DIR *dir;
	struct dirent *entry;
	
	//Esta estructura comprueba el número de opciones correctas introducidas por el usuario y cuando se refiere o no al CWD 
	if (numtokens == 1){
		cwd_bool = TRUE;
	}else if (numtokens == 2){
			  if (strcmp(tokens[1], "-n") == 0){
				  n_op = TRUE;
			  }else if (strcmp(tokens[1], "-h") == 0){
						h_op = TRUE;
			  }else if (strcmp(tokens[1], "-r") == 0)
						r_op = TRUE;
			    		
	}else if (numtokens == 3){
			  if ( (strcmp(tokens[1],"-n") == 0) || (strcmp(tokens[2],"-n") == 0))
				  n_op = TRUE;
			  if ( (strcmp(tokens[1],"-h") == 0) || (strcmp(tokens[2],"-h") == 0))	 
				  h_op = TRUE;	
			  if ( (strcmp(tokens[1],"-r") == 0) || (strcmp(tokens[2],"-r") == 0))	 
				  r_op = TRUE;	
	
	}else if (numtokens >= 4){
			  if ( (strcmp(tokens[1],"-n") == 0) || (strcmp(tokens[2],"-n") == 0) || (strcmp(tokens[3],"-n") == 0))
				  n_op = TRUE;
			  if ( (strcmp(tokens[1],"-h") == 0) || (strcmp(tokens[2],"-h") == 0) || (strcmp(tokens[3],"-h") == 0))	 
				  h_op = TRUE;	
			  if ( (strcmp(tokens[1],"-r") == 0) || (strcmp(tokens[2],"-r") == 0) || (strcmp(tokens[3],"-r") == 0))	 
				  r_op = TRUE;		
	
	}
		
    if (n_op == TRUE)
		cont_op ++;
	if (h_op == TRUE)
		cont_op ++;
	if (r_op == TRUE)
		cont_op ++;

	if ((numtokens == 2) && (cont_op == 1) ){
		//PATH CWD
		cwd_bool = TRUE;
    }else if ((numtokens == 3) && (cont_op == 2) ){
			  //PATH CWD
			  cwd_bool = TRUE;	
	}else if ((numtokens == 4) && (cont_op == 3) ){
			  //PATH CWD
			  cwd_bool = TRUE;
    }

	//Cuando el usuario introduce los nombres a mano...	
	if (cwd_bool == FALSE){	
		for(i = cont_op + 1; i < numtokens; i++){
			if (*tokens[i] == '/'){ //Asignación de la variable PATH cuando nos introduce la ruta el usuario
					PATH = tokens[i];	
			}else if (getcwd(cwd,1024) != NULL){//Cuando el usuario solo introduce un nombre suponemos que se refiere al CWD y concatenamos
					  strcat(cwd, "/");
					  strcat(cwd, tokens[i]);  		
					  PATH = cwd;	  
				  }else printf("CWD function overflowed the buffer\n");	

			//TROCEAMOS EL PATH Y LO GUARDAMOS EN AUXPATH
			strcpy(AUXPATH, PATH);
			ntrozos_PATH = TrocearCadena(AUXPATH, trozos_PATH, "/");

			//Comprobamos el ultimo trozo del PATH (sería lo que queremos listar) para saber si empieza por . o no
			if ( (ntrozos_PATH > 0) && (*trozos_PATH[ntrozos_PATH-1] == '.')){
				if (h_op == FALSE)
					show = FALSE;
			}

			//Extraemos la info del inodo
			if ( (show == TRUE) && (lstat(PATH,&statbuf) != -1)) {
				mode_token = statbuf.st_mode;	
				ConvierteModo(mode_token, permisos);	
				
				//Comprobación de la existencia de la opción -n
				if (n_op == FALSE){			
					pw = getpwuid(statbuf.st_uid);
					grp = getgrgid(statbuf.st_gid);
					
					printf("%lu ",statbuf.st_ino);
					printf("%s ",permisos);
					printf("%lu ",statbuf.st_nlink);
					printf("%s ",pw->pw_name);		
					printf("%s ",grp->gr_name);
				}else if (ntrozos_PATH == 0){
						  printf("%s ",PATH);	
					  }else	printf("%s ", trozos_PATH[ntrozos_PATH-1]);		
				
				printf("%ld ",statbuf.st_size);
				
				//Comprobación de la existencia de la opción -n
				if (n_op == FALSE){		
					strftime(date, 50, "%b %d %H:%M ", localtime(&statbuf.st_mtime));
					printf("%s %s ",date, PATH);
				
					if (permisos[0] == 'l'){
						len = readlink(PATH,name_link,1024);
						name_link[len]='\0';
						printf("-> %s",name_link);
						}
				}
				
				//Estructura que lista los contenidos de un directorio si está la opción -r
				if ( ( (r_op == TRUE) && (permisos[0] == 'd')) || ( (recursive == TRUE) && (permisos[0] == 'd'))){
					printf("\n");
					
					dir = opendir(PATH);
					
					while ( (entry = readdir(dir)) != NULL){
							//Descartamos las entradas . y la ..
							if ( (strcmp(entry->d_name, ".") != 0) && (strcmp(entry->d_name, "..") != 0) ){
								strcpy(AUXPATH, PATH);
								//Comprobación de que no estamos leyendo el directorio /
								if (ntrozos_PATH != 0)
									strcat(AUXPATH, "/");
								strcat(AUXPATH, entry->d_name);
								
								//Estructura con llamadas recursivas a list con cada entrada del directorio 
								if ( (h_op == TRUE) && (n_op == TRUE)){
									entry_tok[1] = (char*) malloc(5);
									strcpy(entry_tok[1],"-n");					
									entry_tok[2] = (char*) malloc(5);
									strcpy(entry_tok[2],"-h");
									entry_tok[3] = AUXPATH;
										  
									cmd_list(entry_tok,4,FALSE);
										  
									free(entry_tok[1]);
									free(entry_tok[2]);		
								
								}else if (h_op == TRUE){
										  entry_tok[1] = (char*) malloc(5);
										  strcpy(entry_tok[1],"-h");
										  entry_tok[2] = AUXPATH;
										  
										  cmd_list(entry_tok,3,FALSE);		
										  
										  free(entry_tok[1]);
								
								}else if (n_op == TRUE){
										  entry_tok[1] = (char*) malloc(5);
										  strcpy(entry_tok[1],"-n");
										  entry_tok[2] = AUXPATH;
									      
										  cmd_list(entry_tok,3,FALSE);	
									      
										  free(entry_tok[1]); 
								
								}else {
										entry_tok[1] = AUXPATH;	 
										cmd_list(entry_tok,2,FALSE);	
								}
							}
					}
				closedir(dir);
				}	
		
			}else if (lstat(PATH, &statbuf) == -1)
					  printf("%s",strerror(errno));	
			if (show == TRUE)
				printf("\n");
		}//FIN DEL FOR
     
	//Si cwd_bool == TRUE, es decir el usuario quiere que listemos los contenidos del CWD  
	}else if(getcwd(cwd,1024) != NULL){  		
			 int rec = FALSE;
			 if (r_op == TRUE)
			 	 rec= TRUE;

			 PATH = cwd;	   			
			 dir = opendir(PATH);
			 
			 while ( (entry = readdir(dir)) != NULL){
					//Descartamos el directorio . y el ..
					if ( (strcmp(entry->d_name, ".") != 0) && (strcmp(entry->d_name, "..") != 0) ){
						strcpy(AUXPATH2, PATH);
						strcat(AUXPATH2, "/");
						strcat(AUXPATH2, entry->d_name);
						
						//Estructura que con llamadas recursivas lista las entradas del CWD
						if ( (h_op == TRUE) && (n_op == TRUE)){
							entry_tok[1] = (char*) malloc(5);
							strcpy(entry_tok[1],"-n");					
							entry_tok[2] = (char*) malloc(5);
							strcpy(entry_tok[2],"-h");
							entry_tok[3] = AUXPATH2;
								  
							cmd_list(entry_tok,4,rec);
								  
							free(entry_tok[1]);
							free(entry_tok[2]);		
						
						}else if (h_op == TRUE){
								  entry_tok[1] = (char*) malloc(5);
								  strcpy(entry_tok[1],"-h");
								  entry_tok[2] = AUXPATH2;
								  
								  cmd_list(entry_tok,3,rec);		
								  
								  free(entry_tok[1]);
								  
						}else if (n_op == TRUE){
								  entry_tok[1] = (char*) malloc(5);
								  strcpy(entry_tok[1],"-n");
								  entry_tok[2] = AUXPATH2;
							      
								  cmd_list(entry_tok,3,rec);	
							      
								  free(entry_tok[1]);
							
						}else {
								entry_tok[1] = AUXPATH2;	 
								cmd_list(entry_tok,2,rec);	
						}				
					}
		     }
		     closedir(dir);	     		     
	}else printf("CWD function overflowed the buffer\n");	
			
}

//////////////////////////////////////////////////////////////////////////////////////////
list mem_list;

void initialize_list(){
	CreateEmptyList(&mem_list);
}


void * ObtenerMemoriaShmget (key_t clave, off_t tam){
void * p;
int aux,id,flags=0777, int_key;
struct shmid_ds s;
dato entrada;	
	
	if (tam) /*si tam no es 0 la crea en modo exclusivo */
		flags=flags | IPC_CREAT | IPC_EXCL;
	/*si tam es 0 intenta acceder a una ya creada*/
	
	if (clave==IPC_PRIVATE){
		/*no nos vale*/
		errno=EINVAL; 
		return NULL;}
		
	if ((id = shmget(clave, tam, flags))==-1)
		return (NULL);
	
	if ((p = shmat(id,NULL,0))==(void*) -1){
		aux=errno;
		/*si se ha creado y no se puede mapear*/
		if (tam)
		/*se borra */
			shmctl(id,IPC_RMID,NULL);
		
		errno=aux;
		return (NULL);}
	
	shmctl (id,IPC_STAT,&s);
	
	entrada.dirMemoria = p;
	entrada.fd = id;
	int_key = (int) clave;
	sprintf(entrada.info,"KEY %d", int_key);
	entrada.size = s.shm_segsz;
	entrada.time = time(NULL);
	strcpy(entrada.type,"shared");
	
	Insert(&mem_list, entrada);
	
	return (p);}


void Cmd_AllocCreateShared (char *arg[]){
key_t k;
off_t tam=0;

void *p;

	k=(key_t) atoi(arg[2]);
	if (arg[3]!=NULL)
		tam=(off_t) atoll(arg[3]);

	if ((p=ObtenerMemoriaShmget(k,tam))==NULL)
		perror ("Imposible obtener memoria shmget");	
	else
		printf ("Memoria de shmget de clave %d asignada en %p\n",k,p);
}

void * MmapFichero (char * fichero, int protection){
int df, map=MAP_PRIVATE,modo=O_RDONLY;
struct stat s;
void *p;
dato entrada;
	
	if (protection&PROT_WRITE)
		modo=O_RDWR;
	if (stat(fichero,&s)==-1 || (df=open(fichero, modo))==-1)
		return NULL;
	if ((p=mmap (NULL,s.st_size, protection,map,df,0))==MAP_FAILED)
		return NULL;

	entrada.dirMemoria = p;
	entrada.fd = df;
	entrada.time = time(NULL);
	strcpy(entrada.type,"mmap");
	strcpy(entrada.info,fichero);
	entrada.size = s.st_size;
	
	Insert(&mem_list, entrada);

	return p;
}


void Cmd_Mmap (char *arg[]){
char *perm;
void *p;
int protection=0;

	if ((perm=arg[3])!=NULL && strlen(perm)<4) {
		if (strchr(perm,'r')!=NULL) protection|=PROT_READ;
		if (strchr(perm,'w')!=NULL) protection|=PROT_WRITE;
		if (strchr(perm,'x')!=NULL) protection|=PROT_EXEC;
	}

	if ((p=MmapFichero(arg[2],protection))==NULL)
		perror ("Imposible mapear fichero");
	else
		printf ("fichero %s mapeado en %p\n", arg[2], p);
}


void MallocAssign(char * tokens[]){
	dato entrada;
	size_t tam = 0;
	
	tam = entrada.size = (size_t)strtoull(tokens[2],NULL,10);
	entrada.dirMemoria = malloc(tam);
	entrada.time = time(NULL);
	strcpy(entrada.type,"malloc");
	
	Insert(&mem_list, entrada);
	
	printf("Reservados %d en la dirección %p\n", entrada.size,entrada.dirMemoria);
	
}


void allocate (char * tokens[], int numtokens){
    int only_show = FALSE, op_malloc = FALSE, op_mmap = FALSE, op_share = FALSE;
   
    if (numtokens == 1){
        only_show = TRUE;

    }else if (numtokens >= 2){
              if (strcmp(tokens[1],"-malloc") == 0){
                  op_malloc = TRUE;
              }else if (strcmp(tokens[1],"-mmap") == 0){
                        op_mmap = TRUE;
              }else if ((strcmp(tokens[1],"-createshared") == 0)   ||   (strcmp(tokens[1],"-shared") == 0)){
                        op_share = TRUE;
              }else printf("OPCION NO RECONOCIDA\n");
    }

    if (only_show == TRUE){
        PrintAssign(mem_list);

    }else {
        if (op_malloc == TRUE){
            if (numtokens == 2 )
				PrintMalloc(mem_list);
            else
				MallocAssign(tokens);

        }else if (op_mmap == TRUE){
                  if (numtokens == 2 )
                     PrintMmap(mem_list);
                  else
					  Cmd_Mmap(tokens);
                    
        }else if (op_share == TRUE){
                  if (numtokens == 2 )
                      PrintShared(mem_list); 
                  else
					  Cmd_AllocCreateShared(tokens);
        }

    }
}

//////////////////////////////////////////////////////////////////////////////////////////
void MallocDeassign(char * tokens[]){
	int tam, pos;
	
	tam = atoi(tokens[2]);
	
	if ((pos = find(mem_list,tam) == -1) || (mem_list.fin == NULO)){
		printf("No se encuentra dicho bloque de tam %d\n",tam);	
		
		if (mem_list.fin != NULO){
			printf("Lista de ficheros disponibles:\n");
			PrintMalloc(mem_list);	
		}	
		
	}else{
		printf("Desasignando bloque...\n");
		free(mem_list.d[pos].dirMemoria);
		Delete(&mem_list,tam,NULL);
	}

}


void MmapDeassign (char* tokens[]){
	int pos;
	
	if ((pos = findFile(mem_list, tokens[2]) == -1) || (mem_list.fin == NULO)){
		printf("No se encuentra el fichero: %s\n", tokens[2]);	
		
		if (mem_list.fin != NULO){
			printf("Lista de ficheros disponibles:\n");
			PrintMmap(mem_list);
		}	
	
	}else{
		printf("Desasignando fichero...\n");
		
		if ((munmap(mem_list.d[pos].dirMemoria, mem_list.d[pos].size)== -1) || (close(mem_list.d[pos].fd) == -1))
			printf("%s",strerror(errno));
		else
			Delete(&mem_list,NULO,tokens[2]);
	}
}


void SharedDeassign (char* tokens[]){
	int i = 0, found = FALSE;
	char key[100];
	
	strcpy(key,"KEY ");
	strcat(key,tokens[2]);
	
	while (i < mem_list.fin){
		if (strcmp(mem_list.d[i].info, key) == 0){	
			printf("Desasignando el bloque con la %s\n",key);
			found = TRUE;
			
			if (shmdt(mem_list.d[i].dirMemoria)==-1){
				printf("Error al desasignar");
				return;
			}
				
			Delete(&mem_list, NULO, key);
			break;
		} 
		i++;
	}
	
	if (found == FALSE)
		printf("No se ha encontrado el bloque con la KEY asociada\n");
	
}


void AddrDeassign(char* tokens[]){
	void *dir;
	int i=0, found = FALSE;
	
	dir = (void*) strtoul(tokens[1],NULL,16);
	
	while(i < mem_list.fin){
		if (mem_list.d[i].dirMemoria == dir){
			found = TRUE;
			
			if (!strcmp(mem_list.d[i].type,"malloc")){
				free(mem_list.d[i].dirMemoria);
			    Delete(&mem_list,mem_list.d[i].size,NULL);
			    return;
			
		    }else if (!strcmp(mem_list.d[i].type,"shared")){
				 if (shmdt(mem_list.d[i].dirMemoria) == -1)
					printf("Error al desasignar esa direccion de memoria");
					
				 else
					Delete(&mem_list,NULO,mem_list.d[i].info);
					
				return;
			
		    }else if(!strcmp(mem_list.d[i].type,"mmap")){
				if ( (munmap(mem_list.d[i].dirMemoria, mem_list.d[i].size) == -1) || (close(mem_list.d[i].fd) == -1))
					printf("Error al desasignar esa direccion de memoria");
				
				else
					Delete(&mem_list, NULO,mem_list.d[i].info);
				
				return;
		    } 	
		}	
		i++;
	}
	
	if (found == FALSE){
		printf("No se ha encotrado esa dirección en la lista de direcciones\n");
		printf("\n");
		PrintAssign(mem_list);
	}

}


void deallocate (char * tokens[], int numtokens){
    int only_show = FALSE, op_malloc = FALSE, op_mmap = FALSE, op_share = FALSE, op_addr = FALSE;
   
    if (numtokens == 1){
        only_show = TRUE;

    }else if (numtokens >= 2){
              if (strcmp(tokens[1],"-malloc") == 0){
                  op_malloc = TRUE;
              }else if (strcmp(tokens[1],"-mmap") == 0){
                        op_mmap = TRUE;
              }else if ((strcmp(tokens[1],"-createshared") == 0)   ||   (strcmp(tokens[1],"-shared") == 0)){
                        op_share = TRUE;
              }else op_addr = TRUE;
    }

    if (only_show == TRUE){
        PrintAssign(mem_list);

    }else {
        if (op_malloc == TRUE){
            if (numtokens == 2 )
				PrintMalloc(mem_list);
            else
				MallocDeassign(tokens);

        }else if (op_mmap == TRUE){
                  if (numtokens == 2 )
                     PrintMmap(mem_list);
                  else
					  MmapDeassign(tokens);
                    
        }else if (op_share == TRUE){
                  if (numtokens == 2 )
                      PrintShared(mem_list); 
                  else
					  SharedDeassign(tokens);
        }else if (op_addr == TRUE){
				  AddrDeassign(tokens);
		}
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
void recursivefunction(int n, int numtokens)
{
	if (numtokens == 2){
		char automatico[2048];
		static char estatico[2048];
		printf ("parametro n:%d en %p\n",n,&n);
		printf ("array estatico en:%p \n",estatico);
		printf ("array automatico en %p\n",automatico);
		n--;

		if (n>0)
			recursivefunction(n, numtokens);
	}
	else
		printf("Numero de parametros invalido\n");
}

//////////////////////////////////////////////////////////////////////////////////////////
int global1, global2, global3;

void mem(char* command, char *tokens[], int numtokens){
	int local1,local2,local3;
	
	if (numtokens == 1){

		printf("Delete function address: %p\n", &delete);
		printf("Create function address: %p\n", &create);
		printf("List function address: %p\n", &cmd_list);
		printf("global1, global variable adrress: %p\n", &global1);
		printf("global2, global variable adrress: %p\n", &global2);
		printf("global3, global variable adrress: %p\n", &global3);
		printf("local1, local variable adrress: %p\n", &local1);
		printf("local2, local variable adrress: %p\n", &local2);
		printf("local3, local variable adrress: %p\n", &local3);
	
	}
	else
		printf("Numero de parametros invalido\n");
	
}

//////////////////////////////////////////////////////////////////////////////////////////
void readfile(char *tokens[], int numtokens){

	if (numtokens>=3){
		
		int size;
		char *p = (char*) strtoul(tokens[2], NULL, 16);
		char *fich = tokens[1];

		if (numtokens==3){
			size = LeerFichero(fich, p, LEERCOMPLETO);

			if (size < 0)
				printf("Segmentation Fault\n");
			else
				printf("%d bytes leidos del fichero, escritos en la direccion %p\n", size, p);
		
		}else if(numtokens==4){	
			ssize_t n = (ssize_t) atoi(tokens[3]);
			size = LeerFichero(fich, p, n);

			if (size < 0)
				printf("Segmentation fault\n");
			else
				printf("%d bytes leidos del fichero, escritos en la direccion %p\n", size, p);
		}else
			printf("Numero de parametros invalido\n");
	}
	else
		printf("Numero de parametros invalido\n");

}

//////////////////////////////////////////////////////////////////////////////////////////
void writefile(char *tokens[], int numtokens){

	if (numtokens>3){

		int size;
		char *p =(char*) strtoul(tokens[2],NULL,16);
		char *fich = tokens[1];
		ssize_t n = (size_t) atoi(tokens[3]);		

		if (numtokens == 4){
			if(access(fich, F_OK ) == -1 ) {
			// file doesn't exists	
				size = EscribirFichero(fich, p, n);

				if (size < 0)
					printf("Segmentation fault\n");

				else
					printf("%d bytes escritos de la direccion %p\n", size, p); 
			}else
				printf("El archivo ya existe, utilizar opcion -o\n");	
		
		}else if (numtokens == 5){
			if (strcmp(tokens[4], "-o") == 0){
				size = EscribirFichero(fich, p, n);

				if (size < 0)
					printf("Segmentation fault\n");

				else
					printf("%d bytes escritos de la direccion %p\n", size, p); 
			}else
				printf("Opcion no reocnocida");
		}else
			printf("Numero de parametros invalido\n");
	}else
		printf("Numero de parametros invalido\n");

}

//////////////////////////////////////////////////////////////////////////////////////////
void memdump(char *tokens[]){
	int i,j,cont = 25;
	char* dirc;
	unsigned char c;
	
	if (tokens[1]==NULL){
		printf("Direccion de memoria no especificada\n");
		return;
	
	}else{
		dirc = (char*)strtoul(tokens[1],NULL,16);
	
		if (tokens[2]!= NULL) 
			cont = atoi(tokens[2]);
	
		for (i=0; i<cont; i=i+25){//linea
			for (j=0; j<25; j++){//caracter
				c = *(dirc+(25*i)+j);
				if (c>31 && c<127)
					printf(" %c ",c);
			
				else
					printf("    ");
			
			}
			printf("\n");
		
			for(j=0; j<25; j++){//hexadecimal
				c = *(dirc+(25*i)+j);
				printf("%02x ", c);
			}
			printf("\n");
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void rmkey (char *tokens[]){
	int id;
	key_t cl;
	
	cl = (key_t) strtoul(tokens[1],NULL,10);
	
	if ((cl == IPC_PRIVATE) || (tokens[1] == NULL)){
		printf ("Clave no valida\n");
		return;
	}
	
	if ((id = shmget(cl,0,0666)) == -1){
		printf("%s", strerror(errno));
		return;
	}
	
	if (shmctl(id, IPC_RMID, NULL) == -1)
		printf("%s", strerror(errno));
	else
		printf("Clave borrada con exito\n");
}

//////////////////////////////////////////////////////////////////////////////////////////
void printPrompt (){
	printf("@ubuntu:~$ ");
}


char * readLine (char * command){ 
    gets(command);
		
	return command;}


int procCommand (char* command){
	char * tokens [100];
	int numtokens ;
	
	numtokens = TrocearCadena(command, tokens, " \n\t");

	//Estructura que controla las llamadas a los comandos de la shell
	if (strcmp(tokens[0],"autores") == 0)
		autores(tokens, numtokens);
	
	else if (strcmp(tokens[0],"pid") == 0)
		pid(numtokens, tokens);
			
	else if (strcmp(tokens[0],"chdir") == 0)
		chdirect(numtokens, tokens);
	
	else if (strcmp(tokens[0],"fecha") == 0)
		fecha();
	
	else if (strcmp(tokens[0],"hora") == 0)
		hora();
	
	else if	(strcmp(tokens[0],"create") == 0) 
		create(tokens, numtokens);

	else if (strcmp(tokens[0],"delete") == 0)
		delete(numtokens, tokens);
	
	else if	(strcmp(tokens[0],"query") == 0) 
		query(tokens, numtokens);

	else if	(strcmp(tokens[0],"list") == 0)
		cmd_list(tokens, numtokens, FALSE);   

	else if	(strcmp(tokens[0],"fin") == 0)
		return TRUE; 

	else if	(strcmp(tokens[0],"end") == 0)
		return TRUE;

	else if	(strcmp(tokens[0],"exit") == 0)
		return TRUE;  
	
	else if (strcmp(tokens[0],"allocate") == 0)
		allocate(tokens,numtokens);
	
	else if (strcmp(tokens[0],"deallocate") == 0) 
		deallocate(tokens, numtokens);

	else if	(strcmp(tokens[0],"recursivefunction") == 0)
		recursivefunction(strtol(tokens[1],NULL,10), numtokens);

	else if	(strcmp(tokens[0],"mem") == 0)
		mem(command, tokens, numtokens);

	else if	(strcmp(tokens[0],"memdump") == 0)   
		memdump(tokens);

	else if	(strcmp(tokens[0],"read") == 0)   
		readfile(tokens, numtokens);

	else if	(strcmp(tokens[0],"write") == 0)   
		writefile(tokens, numtokens);
	
	else if (strcmp(tokens[0],"rmkey") == 0)
		rmkey(tokens);

	return FALSE;
}


int main(){
    
    char promptLine [1024];
    int terminado = FALSE;
    
    initialize_list();
	//Bucle que procesa las lineas de comandos introducidos por los usuarios y que finalizará con los comandos exit, fin y end
    while (terminado != TRUE){
		printPrompt();
		readLine(promptLine);
		
		if (procCommand(promptLine) == TRUE)
			terminado = TRUE;
		}

    return 0;}
