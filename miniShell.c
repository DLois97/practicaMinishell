#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include "parser.h"
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

// *comandos; //Variable para introducir los comandos que entren 
int** crear_pipes(int n){
		int **p; //array de pipes
		int i;
		p = malloc(sizeof(int*)*n-1);//reservamos el espacio en memoria   para abrir los pipes necesarios
		for(i=0; i<n-1; i++){
			p[i]= malloc(sizeof(int)*2);
			pipe(p[i]);
		}
		return p;
}
	
void cerrar_pipes(int **p,int n){
		int i;
		for(i=0; i<n-1; i++){
			close(p[i][0]);
			close(p[i][1]);
			free(p[i]);
		}
		free(p);
}
void ejecutarComandos(tline *comandos){
	int i;
	int fd;
	int pid;
	int **pipes;
	int ncmd = comandos ->ncommands;
	pipes = crear_pipes(ncmd);

	for (i=0;i<comandos->ncommands;i++){
		pid = fork();
		if (pid < 0){//si es menos de 0 hay un error
			fprintf(stderr, "Error al ejecutar el fork. %s\n", comandos->commands[i].filename);
			exit(1);
		}
		if (pid == 0){ // el pid es 0 si es el hijo
			//<inicio de redirección de datos
			if ((comandos->redirect_input!= NULL)&&(i==0)){
				fd=open(comandos->redirect_input,O_RDONLY); //si solo lees "open"
				dup2(fd,0);
				close(fd);

			}
			if ((comandos->redirect_output!= NULL)&&(i==ncmd-1)){
				fd=creat(comandos->redirect_output,0644);//si vas a escribir "creat"
				dup2(fd,1);
				close(fd);

			} 
			if ((comandos->redirect_error!= NULL)&&(i==ncmd-1)){
				fd=creat(comandos->redirect_error,0644);//si vas a escribir "creat"
				dup2(fd,2);
				close(fd);

			}
			//fin de redirección de datos>
			if (ncmd > 1){
				if(i==0){ //si es el primer comando ejecutando redirecciona la entrada del pipe para meter datos
					dup2(pipes[i][1],1);
				} else if(i==ncmd-1){//si es el último comando redirecciona la salida del pipe para recibir datos
					dup2(pipes[i-1][0],0);
				} else {
					dup2(pipes[i][1],1);
					dup2(pipes[i-1][0],0);
					}
				cerrar_pipes(pipes,ncmd);
				}
			execv(comandos->commands[i].filename,comandos->commands[i].argv);
			fprintf(stderr, "Error al ejecutar el comando. %s\n", comandos->commands[i].filename);
			exit(1);
		}
	}
	cerrar_pipes(pipes,ncmd);
}
void my_cd(tline *comandos){
	char * directoryHome;
	int d;
	if(comandos->commands[0].argc==1){
		directoryHome=getenv("HOME");
		d=chdir(directoryHome);
		if(d<0)
			fprintf(stderr,"Error al cambiar de directorio.%s \n",directoryHome);
	}
	else if(comandos->commands[0].argc==2){
		d=chdir(comandos->commands[0].argv[1]);
		if(d<0)
			fprintf(stderr,"Error al cambiar de directorio. %s \n", comandos->commands[0].argv[1]);
	} else {
		fprintf(stderr,"Error al introducir los argumentos. %s \n", comandos->commands[0].argv[1]);
	}
}
int main(void) {
	signal(SIGINT,SIG_IGN);
	signal(SIGQUIT,SIG_IGN);
	char buffer[1024];
	tline *comandos;
	while (!feof(stdin)){
		printf("msh> ");
		fgets(buffer,1024,stdin);
		comandos = tokenize(buffer);
		if(buffer[0]== '\n') //para que siga ejecutando el código cuando hay un salto de linea
			continue;
		if (strcmp(comandos->commands[0].argv[0],"cd")==0){
			my_cd(comandos);
		} else {
			ejecutarComandos(comandos);
		}
	}
}
