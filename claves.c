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

#define UINT32_MAX ((uint32_t)-1)


int init(){
    int sd;
    struct sockaddr_in server_addr;
    struct hostent *hp;       
	int32_t op;
	int err;
    uint32_t res;
    err = obtenerVariablesEntorno();
    if (err == -1){
        printf("Error en las variables de entorno");
        return -1;
    }

    sd = socket(AF_INET, SOCK_STREAM, 0);
	if (sd == 1) {
		printf("Error en socket\n");
		return -1;
	}
    hp = gethostbyname (obtenerIP());
	if (hp == NULL) {
		printf("Error en gethostbyname\n");
		return -1;
	}

    bzero((char *)&server_addr, sizeof(server_addr));
    memcpy (&(server_addr.sin_addr), hp->h_addr, hp->h_length);
   	server_addr.sin_family  = AF_INET;
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
    close(sd);
    res = ntohl(res);
    return res;

}

int set_value(int key,char *value1, int N_value2, double *V_value2){
    if(N_value2>32){
        perror("Vector demasiado grande");
        return -1;

    }
    if(N_value2<0){
        perror("Argumento N negativo");
        return -1;

    }
    int sd;
    struct sockaddr_in server_addr;
    struct hostent *hp;       
	int32_t op;
	int err;
    uint32_t res;
    err = obtenerVariablesEntorno();
    if (err == -1){
        printf("Error en las variables de entorno");
        return -1;
    }

    sd = socket(AF_INET, SOCK_STREAM, 0);
	if (sd == 1) {
		printf("Error en socket\n");
		return -1;
	}
    hp = gethostbyname (obtenerIP());
	if (hp == NULL) {
		printf("Error en gethostbyname\n");
		return -1;
	}

    bzero((char *)&server_addr, sizeof(server_addr));
    memcpy (&(server_addr.sin_addr), hp->h_addr, hp->h_length);
   	server_addr.sin_family  = AF_INET;
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
    op = htonl(1);
    int clave = htonl(key);
    char valor1[256];
    strcpy(valor1, value1);
    int N = htonl(N_value2);
    double valor2[32];      
    err = sendMessage(sd, (char *) &op, sizeof(op)); // envía op
	if (err == -1){
		printf("Error en envio de op\n");
        close(sd);
		return -1;
	}
    err = sendMessage(sd, (char *) &clave, sizeof(clave)); // envía op
	if (err == -1){
		printf("Error en envio de clave\n");
        close(sd);
		return -1;
	}
    err = sendMessage(sd, (char *) &valor1, sizeof(valor1)); // envía op
	if (err == -1){
		printf("Error en envio de valor1\n");
        close(sd);
		return -1;
	}
    err = sendMessage(sd, (char *) &N, sizeof(N)); // envía op
	if (err == -1){
		printf("Error en envio de valor1\n");
        close(sd);
		return -1;
	}
    for (int i = 0; i< N_value2; i++){
            valor2[i] = V_value2[i];
            err = sendMessage(sd, (char *) &valor2[i], sizeof(valor2[i])); // envía op
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
    close(sd);
    res = ntohl(res);
    return res;

}

int get_value(int key,char *value1, int *N_value, double *V_value2){
    int sd;
    struct sockaddr_in server_addr;
    struct hostent *hp;       
	int32_t op;
    char valor1[256];
    int32_t N;
    double valor2[32];
	int err;
    uint32_t res;
    err = obtenerVariablesEntorno();
    if (err == -1){
        printf("Error en las variables de entorno");
        return -1;
    }

    sd = socket(AF_INET, SOCK_STREAM, 0);
	if (sd == 1) {
		printf("Error en socket\n");
		return -1;
	}
    hp = gethostbyname (obtenerIP());
	if (hp == NULL) {
		printf("Error en gethostbyname\n");
        close(sd);
		return -1;
	}

    bzero((char *)&server_addr, sizeof(server_addr));
    memcpy (&(server_addr.sin_addr), hp->h_addr, hp->h_length);
   	server_addr.sin_family  = AF_INET;
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
    op = htonl(2);
    int clave = htonl(key);
    err = sendMessage(sd, (char *) &op, sizeof(op)); // envía op
	if (err == -1){
		printf("Error en envio de clave\n");
        close(sd);
		return -1;
	}
    err = sendMessage(sd, (char *) &clave, sizeof(clave)); // envía op
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
    if (res==0){
    err = recvMessage(sd, (char *) &valor1, sizeof(valor1));     // recibe la respuesta
	if (err == -1){
		printf("Error en recepcion\n");
        close(sd);
		return -1;
	}
    strcpy(value1, valor1);
    err = recvMessage(sd, (char *) &N, sizeof(uint32_t));     // recibe la respuest
	if (err == -1){
		printf("Error en recepcion\n");
        close(sd);
		return -1;
	}
    N= ntohl(N);
    *N_value = N;
    for (int i = 0; i< N; i++){
                err = recvMessage ( sd, (char *) &valor2[i], sizeof(double));   // recibe la operación
                if (err == -1) {
                    printf("Error en recepcion\n");
                    close(sd);
                    //res = -1;
                }
                V_value2[i] = valor2[i];
            }
    }
    close(sd);
    res = ntohl(res);
    return res;

}


int modify_value(int key, char *value1, int N_value2, double *V_value2){
    if(N_value2>32){
        perror("Vector demasiado grande");
        return -1;

    }
    if(N_value2<0){
        perror("Argumento N negativo");
        return -1;

    }
    int sd;
    struct sockaddr_in server_addr;
    struct hostent *hp;       
	int32_t op;
	int err;
    uint32_t res;
    err = obtenerVariablesEntorno();
    if (err == -1){
        printf("Error en las variables de entorno");
        return -1;
    }

    sd = socket(AF_INET, SOCK_STREAM, 0);
	if (sd == 1) {
		printf("Error en socket\n");
		return -1;
	}
    hp = gethostbyname (obtenerIP());
	if (hp == NULL) {
		printf("Error en gethostbyname\n");
		return -1;
	}

    bzero((char *)&server_addr, sizeof(server_addr));
    memcpy (&(server_addr.sin_addr), hp->h_addr, hp->h_length);
   	server_addr.sin_family  = AF_INET;
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
    op = htonl(3);
    int clave = htonl(key);
    char valor1[256];
    strcpy(valor1, value1);
    int N = htonl(N_value2);
    double valor2[32];      
    err = sendMessage(sd, (char *) &op, sizeof(op)); // envía op
	if (err == -1){
		printf("Error en envio de op\n");
        close(sd);
		return -1;
	}
    err = sendMessage(sd, (char *) &clave, sizeof(clave)); // envía op
	if (err == -1){
		printf("Error en envio de clave\n");
        close(sd);
		return -1;
	}
    err = sendMessage(sd, (char *) &valor1, sizeof(valor1)); // envía op
	if (err == -1){
		printf("Error en envio de valor1\n");
        close(sd);
		return -1;
	}
    err = sendMessage(sd, (char *) &N, sizeof(N)); // envía op
	if (err == -1){
		printf("Error en envio de valor1\n");
        close(sd);
		return -1;
	}
    for (int i = 0; i< N_value2; i++){
            valor2[i] = V_value2[i];
            err = sendMessage(sd, (char *) &valor2[i], sizeof(valor2[i])); // envía op
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
    close(sd);
    res = ntohl(res);
    return res;
}

int delete_key(int key){
    int sd;
    struct sockaddr_in server_addr;
    struct hostent *hp;       
	int32_t op;
	int err;
    uint32_t res;
    err = obtenerVariablesEntorno();
    if (err == -1){
        printf("Error en las variables de entorno");
        return -1;
    }

    sd = socket(AF_INET, SOCK_STREAM, 0);
	if (sd == 1) {
		printf("Error en socket\n");
		return -1;
	}
    hp = gethostbyname (obtenerIP());
	if (hp == NULL) {
		printf("Error en gethostbyname\n");
		return -1;
	}

    bzero((char *)&server_addr, sizeof(server_addr));
    memcpy (&(server_addr.sin_addr), hp->h_addr, hp->h_length);
   	server_addr.sin_family  = AF_INET;
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
    op = htonl(4);
    int clave = htonl(key);    
    err = sendMessage(sd, (char *) &op, sizeof(op)); // envía op
	if (err == -1){
		printf("Error en envio de op\n");
        close(sd);
		return -1;
	}
    err = sendMessage(sd, (char *) &clave, sizeof(clave)); // envía op
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
    close(sd);
    res = ntohl(res);
    return res;
}

int exist(int key){
    int sd;
    struct sockaddr_in server_addr;
    struct hostent *hp;       
	int32_t op;
	int err;
    uint32_t res;
    err = obtenerVariablesEntorno();
    if (err == -1){
        printf("Error en las variables de entorno");
        return -1;
    }

    sd = socket(AF_INET, SOCK_STREAM, 0);
	if (sd == 1) {
		printf("Error en socket\n");
		return -1;
	}
    hp = gethostbyname (obtenerIP());
	if (hp == NULL) {
		printf("Error en gethostbyname\n");
		return -1;
	}

    bzero((char *)&server_addr, sizeof(server_addr));
    memcpy (&(server_addr.sin_addr), hp->h_addr, hp->h_length);
   	server_addr.sin_family  = AF_INET;
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
    op = htonl(5);
    int clave = htonl(key);    
    err = sendMessage(sd, (char *) &op, sizeof(op)); // envía op
	if (err == -1){
		printf("Error en envio de op\n");
        close(sd);
		return -1;
	}
    err = sendMessage(sd, (char *) &clave, sizeof(clave)); // envía op
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
    close(sd);
    res = ntohl(res);
    return res;
}