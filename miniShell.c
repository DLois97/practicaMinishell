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
int** crear_pipes(void){
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
	if (comandos->commands)[0].redirect_input != NULL){
		

	} 
	execv(comandos->commands[0].filename,comandos->commands[0].argv);
	fprintf(stderr, "Error. %s\n", comandos->commands[0].filename);
	exit(1);
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
