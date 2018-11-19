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
	int i = 0;
	int fd;
	//<inicio de redirección de datos
	if (comandos->redirect_output!= NULL){
		fd=open(comandos->redirect_input,O_RDONLY); //si solo lees "open"
		dup2(fd,0);

	}
	if (comandos->redirect_output!= NULL){
		fd=creat(comandos->redirect_output,0644);//si vas a escribir "creat"
		dup2(fd,1);

	} 
	if (comandos->redirect_output!= NULL){
		fd=creat(comandos->redirect_error,0644);//si vas a escribir "creat"
		dup2(fd,2);

	}
	//fin de redirección de datos>
	for (i;i<comandos->ncommands;i++){	
		execv(comandos->commands[i].filename,comandos->commands[i].argv);
		fprintf(stderr, "Error. %s\n", comandos->commands[i].filename);
		exit(1);
	}
}

int main(void) {
	char buffer[1024];
	while (!feof(stdin)){
		printf("msh> ");
		fgets(buffer,1024,stdin);
		comandos = tokenize(buffer);
		ejecutarComandos();
	}
}
