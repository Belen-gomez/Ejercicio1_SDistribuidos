#include "mensajes.h"
#include "claves.h"
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdint.h>

#define MAX_SIZE 32
#define MAXSIZE_STRING 256

int init(){
    //Variables del socket
    int sd;
    struct sockaddr_in server_addr;
    struct hostent *hp;

    //Variables del mensaje
	int32_t op;
	int err;
    uint32_t res;

    //Funcion para obtener las variables de entorno
    err = obtenerVariablesEntorno();
    if (err == -1){
        printf("Error en las variables de entorno");
        return -1;
    }

    //Creación del socket TCP
    sd = socket(AF_INET, SOCK_STREAM, 0);
	if (sd == 1) {
		printf("Error en socket\n");
		return -1;
	}

    //Obtención de la dirección IP
    hp = gethostbyname (obtenerIP());
	if (hp == NULL) {
		printf("Error en gethostbyname\n");
		return -1;
	}

    //Inicialización de la estructura del servidor
    bzero((char *)&server_addr, sizeof(server_addr));
    memcpy (&(server_addr.sin_addr), hp->h_addr, hp->h_length);
   	server_addr.sin_family  = AF_INET;

    //Obtención del puerto. Se utiliza strtol para el manejo de errores
   	char *endptr;
    long int puerto = strtol(obtenerPuerto(), &endptr, 10);
    if (*endptr != '\0') {
        // Error: La cadena no es un número válido
        printf("Error: La cadena no es un número válido\n");
    } else if (puerto < 0 || puerto > UINT32_MAX) {
        // Error: Puerto fuera de rango
        printf("Error: Puerto fuera de rango\n");
    } else {
        // Conversión exitosa, puerto contiene el valor numérico
        server_addr.sin_port = htons((int32_t)puerto);
    }

    // se establece la conexión
   	err = connect(sd, (struct sockaddr *) &server_addr,  sizeof(server_addr));
	if (err == -1) {
		printf("Error en connect\n");
		return -1;
	}

    //Como la operación es un entero se convierte a network byte order
    op = htonl(0);

    err = sendMessage(sd, (char *) &op, sizeof(op)); // envía op
	if (err == -1){
		printf("Error en envio\n");
		return -1;
	}
    err = recvMessage(sd, (char *) &res, sizeof(uint32_t));     // recibe la respuesta
	if (err == -1){
		printf("Error en recepcion\n");
		return -1;
	}
    
    //Se cierra el socket
    close(sd);

    //Se convierte la respuesta a host byte order
    res = ntohl(res);
    return res;

}

int set_value(int key,char *value1, int N_value2, double *V_value2){

    //Primero se comprueba que el vector no sea demasiado grande o que el tamaño sea negativo
    if(N_value2>MAX_SIZE){
        perror("Vector demasiado grande");
        return -1;

    }
    if(N_value2<0){
        perror("Argumento N negativo");
        return -1;

    }

    //Se comprueba que la cadena no sea demasiado grande
    if(strlen(value1) > MAXSIZE_STRING){
        perror("Cadena demasiado grande");
        return -1;
    }
    
    //Variables del socket
    int sd;
    struct sockaddr_in server_addr;
    struct hostent *hp;    

    //Variables del mensaje   
	int32_t op;
	int err;
    uint32_t res;

    //Funcion para obtener las variables de entorno
    err = obtenerVariablesEntorno();
    if (err == -1){
        printf("Error en las variables de entorno");
        return -1;
    }

    //Creación del socket TCP
    sd = socket(AF_INET, SOCK_STREAM, 0);
	if (sd == 1) {
		printf("Error en socket\n");
		return -1;
	}

    //Obtención de la dirección IP
    hp = gethostbyname (obtenerIP());
	if (hp == NULL) {
		printf("Error en gethostbyname\n");
		return -1;
	}

    //Inicialización de la estructura del servidor
    bzero((char *)&server_addr, sizeof(server_addr));
    memcpy (&(server_addr.sin_addr), hp->h_addr, hp->h_length);
   	server_addr.sin_family  = AF_INET;

    //Obtención del puerto. Se utiliza strtol para el manejo de errores
   	char *endptr;
    long int puerto = strtol(obtenerPuerto(), &endptr, 10);
    if (*endptr != '\0') {
        // Error: La cadena no es un número válido
        printf("Error: La cadena no es un número válido\n");
    } else if (puerto < 0 || puerto > UINT32_MAX) {
        // Error: Puerto fuera de rango
        printf("Error: Puerto fuera de rango\n");
    } else {
        // Conversión exitosa, puerto contiene el valor numérico
        server_addr.sin_port = htons((int32_t)puerto);
    }

    // se establece la conexión
   	err = connect(sd, (struct sockaddr *) &server_addr,  sizeof(server_addr));
	if (err == -1) {
		printf("Error en connect\n");
		return -1;
	}

    //Como la operación, la clave y N_value2 son un enteros se convierten a network byte order
    op = htonl(1);
    int clave = htonl(key);
    int N = htonl(N_value2);

    //se copia el valor de la cadena
    char valor1[MAXSIZE_STRING];
    strcpy(valor1, value1);
 
    err = sendMessage(sd, (char *) &op, sizeof(op)); // envía op
	if (err == -1){
		printf("Error en envio de op\n");
        close(sd);
		return -1;
	}
    err = sendMessage(sd, (char *) &clave, sizeof(clave)); // se envía la clave
	if (err == -1){
		printf("Error en envio de clave\n");
        close(sd);
		return -1;
	}
    err = sendMessage(sd, (char *) &valor1, sizeof(valor1)); // se envía la cadena
	if (err == -1){
		printf("Error en envio de valor1\n");
        close(sd);
		return -1;
	}
    err = sendMessage(sd, (char *) &N, sizeof(N)); // envía N
	if (err == -1){
		printf("Error en envio de valor1\n");
        close(sd);
		return -1;
	}

    //Se envía el vector valor por valor
    for (int i = 0; i< N_value2; i++){
        err = sendMessage(sd, (char *) &V_value2[i], sizeof(V_value2[i])); // envía valor2[i]
        if (err == -1){
            printf("Error en envio de valor2 %d\n", i);
            close(sd);
            return -1;
	    }
    }

    err = recvMessage(sd, (char *) &res, sizeof(uint32_t));     // recibe la respuesta
	if (err == -1){
		printf("Error en recepcion\n");
        close(sd);
		return -1;
	}

    //Se cierra el socket
    close(sd);

    //Se convierte la respuesta a host byte order
    res = ntohl(res);
    return res;

}

int get_value(int key,char *value1, int *N_value, double *V_value2){
    //Variables del socket
    int sd;
    struct sockaddr_in server_addr;
    struct hostent *hp; 

    //Variables del mensaje      
	int32_t op;
	int err;
    uint32_t res;

    //Funcion para obtener las variables de entorno
    err = obtenerVariablesEntorno();
    if (err == -1){
        printf("Error en las variables de entorno");
        return -1;
    }

    //Creación del socket TCP
    sd = socket(AF_INET, SOCK_STREAM, 0);
	if (sd == 1) {
		printf("Error en socket\n");
		return -1;
	}

    //Obtención de la dirección IP
    hp = gethostbyname (obtenerIP());
	if (hp == NULL) {
		printf("Error en gethostbyname\n");
        close(sd);
		return -1;
	}

    //Inicialización de la estructura del servidor
    bzero((char *)&server_addr, sizeof(server_addr));
    memcpy (&(server_addr.sin_addr), hp->h_addr, hp->h_length);
   	server_addr.sin_family  = AF_INET;

    //Obtención del puerto. Se utiliza strtol para el manejo de errores
   	char *endptr;
    long int puerto = strtol(obtenerPuerto(), &endptr, 10);
    if (*endptr != '\0') {
        // Error: La cadena no es un número válido
        printf("Error: La cadena no es un número válido\n");
        close(sd);
    } else if (puerto < 0 || puerto > UINT32_MAX) {
        // Error: Puerto fuera de rango
        printf("Error: Puerto fuera de rango\n");
        close(sd);
    } else {
        // Conversión exitosa, puerto contiene el valor numérico
        server_addr.sin_port = htons((int32_t)puerto);
    }

    // se establece la conexión
   	err = connect(sd, (struct sockaddr *) &server_addr,  sizeof(server_addr));
	if (err == -1) {
		printf("Error en connect\n");
        close(sd);
		return -1;
	}

    //Como la operación y la clave son enteros se convierten a network byte order
    op = htonl(2);
    int clave = htonl(key);

    err = sendMessage(sd, (char *) &op, sizeof(op)); // envía op
	if (err == -1){
		printf("Error en envio de clave\n");
        close(sd);
		return -1;
	}
    err = sendMessage(sd, (char *) &clave, sizeof(clave)); // envía clave
	if (err == -1){
		printf("Error en envio de clave\n");
        close(sd);
		return -1;
	}

    //Variables para recibir la respuesta
    char valor1[MAXSIZE_STRING];
    int32_t N;

    err = recvMessage(sd, (char *) &res, sizeof(uint32_t));     // recibe la respuesta
	if (err == -1){
		printf("Error en recepcion\n");
        close(sd);
		return -1;
	}
    if (res==0){
    err = recvMessage(sd, (char *) &valor1, sizeof(valor1));     // recibe la cadena
	if (err == -1){
		printf("Error en recepcion\n");
        close(sd);
		return -1;
	}
    strcpy(value1, valor1);

    err = recvMessage(sd, (char *) &N, sizeof(uint32_t));     // recibe N
	if (err == -1){
		printf("Error en recepcion\n");
        close(sd);
		return -1;
	}

    N= ntohl(N);
    *N_value = N;
    //Cada elemento del vector se recibe uno a uno
    for (int i = 0; i< N; i++){
            err = recvMessage ( sd, (char *) &V_value2[i], sizeof(double));   // recibe V_value2[i]
            if (err == -1) {
                printf("Error en recepcion\n");
                close(sd);
                return -1;
            }
        }
    }
    close(sd);
    res = ntohl(res);
    return res;
}


int modify_value(int key, char *value1, int N_value2, double *V_value2){
    //Primero se comprueba que el vector no sea demasiado grande o que el tamaño sea negativo
    if(N_value2>MAX_SIZE){
        perror("Vector demasiado grande");
        return -1;

    }
    if(N_value2<0){
        perror("Argumento N negativo");
        return -1;

    }
    //Variables del socket
    int sd;
    struct sockaddr_in server_addr;
    struct hostent *hp;  

    //Variables del mensaje     
	int32_t op;
	int err;
    uint32_t res;

    //Funcion para obtener las variables de entorno
    err = obtenerVariablesEntorno();
    if (err == -1){
        printf("Error en las variables de entorno");
        return -1;
    }

    //Creación del socket TCP
    sd = socket(AF_INET, SOCK_STREAM, 0);
	if (sd == 1) {
		printf("Error en socket\n");
		return -1;
	}

    //Obtención de la dirección IP
    hp = gethostbyname (obtenerIP());
	if (hp == NULL) {
		printf("Error en gethostbyname\n");
		return -1;
	}

    //Inicialización de la estructura del servidor
    bzero((char *)&server_addr, sizeof(server_addr));
    memcpy (&(server_addr.sin_addr), hp->h_addr, hp->h_length);
   	server_addr.sin_family  = AF_INET;

    //Obtención del puerto. Se utiliza strtol para el manejo de errores
   	char *endptr;
    long int puerto = strtol(obtenerPuerto(), &endptr, 10);
    if (*endptr != '\0') {
        // Error: La cadena no es un número válido
        printf("Error: La cadena no es un número válido\n");
    } else if (puerto < 0 || puerto > UINT32_MAX) {
        // Error: Puerto fuera de rango
        printf("Error: Puerto fuera de rango\n");
    } else {
        // Conversión exitosa, puerto contiene el valor numérico
        server_addr.sin_port = htons((int32_t)puerto);
    }

    // se establece la conexión
   	err = connect(sd, (struct sockaddr *) &server_addr,  sizeof(server_addr));
	if (err == -1) {
		printf("Error en connect\n");
		return -1;
	}

    //Como la operación, la clave y N_value2 son enteros se convierten a network byte order
    op = htonl(3);
    int clave = htonl(key);
    int N = htonl(N_value2);

    //se copia el valor de la cadena
    char valor1[MAXSIZE_STRING];
    strcpy(valor1, value1);
         
    err = sendMessage(sd, (char *) &op, sizeof(op)); // envía op
	if (err == -1){
		printf("Error en envio de op\n");
        close(sd);
		return -1;
	}
    err = sendMessage(sd, (char *) &clave, sizeof(clave)); // envía clave
	if (err == -1){
		printf("Error en envio de clave\n");
        close(sd);
		return -1;
	}
    err = sendMessage(sd, (char *) &valor1, sizeof(valor1)); // envía cadena
	if (err == -1){
		printf("Error en envio de valor1\n");
        close(sd);
		return -1;
	}
    err = sendMessage(sd, (char *) &N, sizeof(N)); // envía N
	if (err == -1){
		printf("Error en envio de valor1\n");
        close(sd);
		return -1;
	}

    //Se envía el vector valor por valor
    for (int i = 0; i< N_value2; i++){
            err = sendMessage(sd, (char *) &V_value2[i], sizeof(V_value2[i])); // envía V_value2[i]
            if (err == -1){
                printf("Error en envio de valor2 %d\n", i);
                close(sd);
                return -1;
	    }
    }
    err = recvMessage(sd, (char *) &res, sizeof(uint32_t));     // recibe la respuesta
	if (err == -1){
		printf("Error en recepcion\n");
        close(sd);
		return -1;
	}

    //Se cierra el socket
    close(sd);

    //Se convierte la respuesta a host byte order
    res = ntohl(res);

    return res;
}

int delete_key(int key){
    //Variables del socket
    int sd;
    struct sockaddr_in server_addr;
    struct hostent *hp; 

    //Variables del mensaje      
	int32_t op;
	int err;
    uint32_t res;

    //Funcion para obtener las variables de entorno
    err = obtenerVariablesEntorno();
    if (err == -1){
        printf("Error en las variables de entorno");
        return -1;
    }

    //Creación del socket TCP
    sd = socket(AF_INET, SOCK_STREAM, 0);
	if (sd == 1) {
		printf("Error en socket\n");
		return -1;
	}

    //Obtención de la dirección IP
    hp = gethostbyname (obtenerIP());
	if (hp == NULL) {
		printf("Error en gethostbyname\n");
		return -1;
	}

    //Inicialización de la estructura del servidor
    bzero((char *)&server_addr, sizeof(server_addr));
    memcpy (&(server_addr.sin_addr), hp->h_addr, hp->h_length);
   	server_addr.sin_family  = AF_INET;

    //Obtención del puerto. Se utiliza strtol para el manejo de errores
   	char *endptr;
    long int puerto = strtol(obtenerPuerto(), &endptr, 10);
    if (*endptr != '\0') {
        // Error: La cadena no es un número válido
        printf("Error: La cadena no es un número válido\n");
    } else if (puerto < 0 || puerto > UINT32_MAX) {
        // Error: Puerto fuera de rango
        printf("Error: Puerto fuera de rango\n");
    } else {
        // Conversión exitosa, puerto contiene el valor numérico
        server_addr.sin_port = htons((int32_t)puerto);
    }

    // se establece la conexión
   	err = connect(sd, (struct sockaddr *) &server_addr,  sizeof(server_addr));
	if (err == -1) {
		printf("Error en connect\n");
		return -1;
	}

    //Como la operación y la clave son enteros se convierten a network byte order
    op = htonl(4);
    int clave = htonl(key);    

    err = sendMessage(sd, (char *) &op, sizeof(op)); // envía op
	if (err == -1){
		printf("Error en envio de op\n");
        close(sd);
		return -1;
	}
    err = sendMessage(sd, (char *) &clave, sizeof(clave)); // envía clave
	if (err == -1){
		printf("Error en envio de clave\n");
        close(sd);
		return -1;
	}
    err = recvMessage(sd, (char *) &res, sizeof(uint32_t));     // recibe la respuesta
	if (err == -1){
		printf("Error en recepcion\n");
        close(sd);
		return -1;
	}

    //Se cierra el socket
    close(sd);

    //Se convierte la respuesta a host byte order
    res = ntohl(res);
    return res;
}

int exist(int key){
    //Variables del socket
    int sd;
    struct sockaddr_in server_addr;
    struct hostent *hp;   

    //Variables del mensaje    
	int32_t op;
	int err;
    uint32_t res;

    //Funcion para obtener las variables de entorno
    err = obtenerVariablesEntorno();
    if (err == -1){
        printf("Error en las variables de entorno");
        return -1;
    }

    //Creación del socket TCP
    sd = socket(AF_INET, SOCK_STREAM, 0);
	if (sd == 1) {
		printf("Error en socket\n");
		return -1;
	}

    //Obtención de la dirección IP
    hp = gethostbyname (obtenerIP());
	if (hp == NULL) {
		printf("Error en gethostbyname\n");
		return -1;
	}

    //Inicialización de la estructura del servidor
    bzero((char *)&server_addr, sizeof(server_addr));
    memcpy (&(server_addr.sin_addr), hp->h_addr, hp->h_length);
   	server_addr.sin_family  = AF_INET;

    //Obtención del puerto. Se utiliza strtol para el manejo de errores
   	char *endptr;
    long int puerto = strtol(obtenerPuerto(), &endptr, 10);
    if (*endptr != '\0') {
        // Error: La cadena no es un número válido
        printf("Error: La cadena no es un número válido\n");
    } else if (puerto < 0 || puerto > UINT32_MAX) {
        // Error: Puerto fuera de rango
        printf("Error: Puerto fuera de rango\n");
    } else {
        // Conversión exitosa, puerto contiene el valor numérico
        server_addr.sin_port = htons((int32_t)puerto);
    }

    // se establece la conexión
   	err = connect(sd, (struct sockaddr *) &server_addr,  sizeof(server_addr));
	if (err == -1) {
		printf("Error en connect\n");
		return -1;
	}

    //Como la operación y la clave son enteros se convierten a network byte order
    op = htonl(5);
    int clave = htonl(key);   

    err = sendMessage(sd, (char *) &op, sizeof(op)); // envía op
	if (err == -1){
		printf("Error en envio de op\n");
        close(sd);
		return -1;
	}
    err = sendMessage(sd, (char *) &clave, sizeof(clave)); // envía clave
	if (err == -1){
		printf("Error en envio de clave\n");
        close(sd);
		return -1;
	}
    err = recvMessage(sd, (char *) &res, sizeof(uint32_t));     // recibe la respuesta
	if (err == -1){
		printf("Error en recepcion\n");
        close(sd);
		return -1;
	}

    //Se cierra el socket
    close(sd);

    //Se convierte la respuesta a host byte order
    res = ntohl(res);
    return res;
}