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

tline *comandos; //Variable para introducir los comandos que entren 
int** crear_pipes(){
		int **p; //array de pipes
		int i;
		p = malloc(sizeof(int*)*comandos->ncommands-1);//reservamos el espacio en memoria   para abrir los pipes necesarios
		for(i=0; i<comandos->ncommands-1; i++){
			p[i]= malloc(sizeof(int)*2);
			pipe(p[i]);
		}
		return p;
}
	
void cerrar_pipes(int **p){
		int i;
		for(i=0; i<comandos->ncommands-1; i++){
			close(p[i][0]);
			close(p[i][1]);
			free(p[i]);
		}
		free(p);
}
void ejecutarComandos(void){
	int i;
	int fd;
	int *pids;
	int pid;
	int **pipes;
	int cmd = comandos ->ncommands;
	pids= malloc(sizeof(int)*comandos->ncommands);
	

//fin de redirección de datos>
	pipes = crear_pipes();

	for (i=0;i<comandos->ncommands;i++){
		pid = fork();
		if (pid < 0){//si es menos de 0 hay un error
			fprintf(stderr, "Error. %s\n", comandos->commands[i].filename);
			exit(1);
		}
		pids[i]=pid;
		if (pid == 0){
				//<inicio de redirección de datos
	if ((comandos->redirect_input!= NULL)&&(i==0)){
		fd=open(comandos->redirect_input,O_RDONLY); //si solo lees "open"
		dup2(fd,0);
		close(fd);

	}
	if ((comandos->redirect_output!= NULL)&&(i==cmd-1)){
		fd=creat(comandos->redirect_output,0644);//si vas a escribir "creat"
		dup2(fd,1);
		close(fd);

	} 
	if ((comandos->redirect_error!= NULL)&&(i==cmd-1)){
		fd=creat(comandos->redirect_error,0644);//si vas a escribir "creat"
		dup2(fd,2);
		close(fd);

	}	//es 0 si es el hijo
			if (cmd > 1){
				if(i==0){ //si es el primer comando ejecutando redirecciona la entrada del pipe para meter datos
					dup2(pipes[i][1],1);
				} else if(i==cmd-1){//si es el último comando redirecciona la salida del pipe para recibir datos
					dup2(pipes[i-1][0],0);
				} else {
					dup2(pipes[i][1],1);
					dup2(pipes[i-1][0],0);
				}
			cerrar_pipes(pipes);
			}
		execv(comandos->commands[i].filename,comandos->commands[i].argv);
		fprintf(stderr, "Error. %s\n", comandos->commands[i].filename);
		exit(1);
		}
	}
	cerrar_pipes(pipes);
}
	/*pipes = crear_pipes();
	for(i=0; i<comandos->ncommands; i++){
		pid= fork();
		if(pid<0){
			fprintf(stderr,"Error. fork\n");
			exit(1);
		}
		pids[i]=pid;
		if(pid==0){
			if(!comandos->background){
				signal(SIGINT, SIG_DFL);
				signal(SIGQUIT, SIG_DFL);
			}
			if((i==0) && (comandos->redirect_input!=NULL)){
				fd=open(comandos->redirect_input,O_RDONLY);
				if(fd<0){
					fprintf(stderr,"Error.%s \n",comandos->redirect_input);
					exit(1);
				}
				dup2(fd,0);
				close(fd);
			}
			if((i==comandos->ncommands-1) && (comandos->redirect_output!=NULL)){
				fd=creat(comandos->redirect_output,0644);
				if(fd<0){
					fprintf(stderr,"Error.%s \n",comandos->redirect_output);
					exit(1);
				}
				dup2(fd,1);
				close(fd);
			}
			if((i==comandos->ncommands-1) && (comandos->redirect_error!=NULL)){
				fd=creat(comandos->redirect_error,0644);
				if(fd<0){
					fprintf(stderr,"Error.%s /n",comandos->redirect_error);
					exit(1);
				}
				dup2(fd,2);
				close(fd);
			}
			if(cmd>1){
				if(i==0)
		    		dup2(pipes[i][1],1);
				else if(i==cmd-1){
					dup2(pipes[i-1][0],0);
				}
				else{
					dup2(pipes[i][1],1);
					dup2(pipes[i-1][0],0);

				}
				cerrar_pipes(pipes);
			}
			execv(comandos->commands[i].filename,comandos->commands[i].argv);
			fprintf(stderr, "Error. %s\n", comandos->commands[i].filename);
			exit(1);
		}
}
cerrar_pipes(pipes);
}
*/
int main(void) {
	char buffer[1024];
	while (!feof(stdin)){
		printf("msh> ");
		fgets(buffer,1024,stdin);
		comandos = tokenize(buffer);
		ejecutarComandos();
	}
}
