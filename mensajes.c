#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "mensajes.h"



int obtenerVariablesEntorno()
{
	//Obtenemos las variables de entorno y se guardan en las variables globales
    ip_tuplas = getenv("IP_TUPLAS");
    if (ip_tuplas == NULL){
        printf("Variable IP_TUPLAS no definida\n");
        return -1;
    }

    port_tuplas = getenv("PORT_TUPLAS");
    if (port_tuplas == NULL){
        printf("Variable PORT_TUPLAS no definida\n");
        return -1;
    }
    
    return 0;
}

char * obtenerIP() {
	//Devuelve la IP
    return ip_tuplas;
}

char * obtenerPuerto() {
	//Devuelve el puerto
    return port_tuplas;
}

int sendMessage(int socket, char * buffer, int len)
{
	int r;
	int l = len;
		
	do {	
		r = write(socket, buffer, l);
		l = l -r;
		buffer = buffer + r;
	} while ((l>0) && (r>=0));
	
	if (r < 0)
		return (-1);   /* fail */
	else
		return(0);	/* full length has been sent */
}

int recvMessage(int socket, char *buffer, int len)
{
	int r;
	int l = len;
		

	do {	
		r = read(socket, buffer, l);
		l = l -r ;
		buffer = buffer + r;
	} while ((l>0) && (r>=0));
	
	if (r < 0)
		return (-1);   /* fallo */
	else
		return(0);	/* full length has been receive */
}