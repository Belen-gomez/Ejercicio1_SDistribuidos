#include "mensajes.h"
#include "servidor.h"
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#define MAXSIZE 256

pthread_mutex_t mutex;
pthread_mutex_t funciones;
pthread_cond_t cond;
int mensaje_no_copiado = 1;
int lista_inicializada = 0;
int res;
int err;


List lista; // Lista enlazada para almacenar las tuplas

int init(List *l) {
    pthread_mutex_lock(&funciones);
    while (*l != NULL) {
        List temp = *l;
        *l = (*l)->siguiente;
        free(temp->valor2);
        free(temp->valor1);
        free(temp);
    }
    *l = NULL;
    pthread_mutex_unlock(&funciones);
    return 0;
}


int set_value(List *l, int clave, char *valor1, int N, double *valor2){
    pthread_mutex_lock(&funciones);
    // Verificar si la clave ya existe en la lista
    List aux = *l;
    while (aux != NULL) {
        if (aux->clave == clave) {
            printf("Error: Ya existe un elemento con la clave %d\n", clave);
            return -1; // Clave duplicada, retorna error
        }
        aux = aux->siguiente;
    }
    struct Tupla *ptr;

    ptr = (struct Tupla *) malloc(sizeof(struct Tupla));
    if (ptr == NULL) 
        return -1;

    ptr->clave = clave;
    ptr->valor1 = malloc(strlen(valor1) + 1);
    strcpy(ptr->valor1, valor1);
    ptr->N = N;
    ptr->valor2 = malloc(N * sizeof(double));
    for(int i = 0; i < N; i++){
        ptr->valor2[i] = valor2[i];
    }
    ptr->siguiente = *l;
    *l = ptr;

	return 0;
}

int printList(List l) {
    List aux = l;
    printf("Imprimir\n");
    while(aux != NULL){
        printf("Nueva tupla\n");
        printf("Clave=%d    value1=%s   N=%d\n", aux->clave, aux->valor1, aux->N);
        printf("Valor2:");
        for(int i = 0; i < aux->N; i++){
            printf(" %.6f", aux->valor2[i]); // Imprimir valor2[i] con 6 decimales de precisión
        }
        printf("\n\n"); // Agregar una línea en blanco después de cada tupla
        aux = aux->siguiente;
    }
    return 0;
}

 struct respuesta get_value(List *l, int clave){
    pthread_mutex_lock(&funciones);
    struct respuesta res;
    if (*l == NULL) {
        perror("La lista está vacía");
        res.status = -1;
        return res;
    }
    // Buscar la tupla con la clave especificada
    List aux = *l;
    while (aux != NULL) {
        if (aux->clave == clave) {
            
            // Se encontró la clave, copiar los valores a la estructura de respuesta
            res.clave = aux->clave;
            strcpy(res.valor1, aux->valor1);
            res.N = aux->N;
            for (int i = 0; i < res.N; i++) {
                res.valor2[i] = aux->valor2[i];
            }
            res.status = 0;
            return res;
        }
        aux = aux->siguiente;
    }
    perror("Se ha intentado acceder a una clave que no existe");
    res.status = -1;
    return res;

}


int modify_value(List *l, int clave, char *valor1, int N, double *valor2){
    pthread_mutex_lock(&funciones);
    
    if (*l == NULL) {
        perror("La lista está vacía");
        return -1;
    }
    // Buscar la tupla con la clave especificada
    List aux = *l;
    while (aux != NULL) {
        if (aux->clave == clave) {
            // Se encontró la clave, copiar los valores a la estructura de respuesta
            aux->clave = clave;
            free(aux->valor1);
            aux->valor1 = malloc(strlen(valor1) + 1);
            strcpy(aux->valor1, valor1);
            aux->N = N;
            free(aux->valor2);
            aux->valor2 = malloc(N * sizeof(double));
            for(int i = 0; i < N; i++){
                aux->valor2[i] = valor2[i];
            }
            return 0;
        }
        aux = aux->siguiente;
    }
    perror("Se ha intentado modificar a una clave que no existe");
    return -1;
}

int delete_key(List *l, int key) {
    pthread_mutex_lock(&funciones);
    
    if (*l == NULL) {
        perror("La lista está vacía");
        return -1;
    }

    List current = *l;
    List previous = NULL;

    // Buscar el nodo con la clave key
    while (current != NULL && current->clave != key) {
        previous = current;
        current = current->siguiente;
    }

    // Si current es NULL, significa que no se encontró la clave
    if (current == NULL) {
        perror("No se ha encontrado la clave");
        return -1;
    }

    // Si previous es NULL, el nodo a eliminar es el primero de la lista
    if (previous == NULL) {
        *l = current->siguiente;
    } else {
        // El nodo a eliminar está en el medio o al final de la lista
        previous->siguiente = current->siguiente;
    }

    // Liberar la memoria del nodo eliminado
    free(current->valor2); // Liberar la memoria del arreglo valor2
    free(current->valor1); // Liberar la memoria de la cadena valor1
    free(current); // Liberar la memoria del nodo
    return 0; // Operación exitosa
}

int exist(List *l, int clave){
    pthread_mutex_lock(&funciones);
    int encontrado = 0;
    
    if (*l == NULL) {
        perror("La lista está vacía");
        return -1;
    }
    List aux = *l;
    while (aux != NULL) {
        if (aux->clave == clave) {
            // Se encontró la clave, copiar los valores a la estructura de respuesta
            encontrado = 1;
        }
        aux = aux->siguiente;
        
    }
    if(encontrado == 0){
        perror("No se encuentra la clave");
        return 0;
    }
    else{
        return 1;
    }
}

void atender_peticion(int * s){
    int32_t  op;
    int32_t clave;
    int32_t  N;
    char valor1[256];
    double valor2[32];
    uint32_t respuesta;
    int sc;
    pthread_mutex_lock(&mutex);
    sc = (* (int *)s);
    mensaje_no_copiado = 0;

	pthread_cond_signal(&cond);

	pthread_mutex_unlock(&mutex);


    err = recvMessage ( sc, (char *) &op, sizeof(op));   // recibe la operación
    if (err == -1) {
        printf("Error en recepcion\n");
        close(sc);
        //res = -1;
    }
    op = ntohl(op);
    if (err!=-1){
        if (op == 0){
        res = init(&lista);
        }else if (op == 1){
            err = recvMessage ( sc, (char *) &clave, sizeof(int));   // recibe la operación
            if (err == -1) {
                printf("Error en recepcion\n");
                close(sc);
                res = -1;
            }
            clave = ntohl(clave);
            err = recvMessage ( sc, (char *) &valor1, sizeof(valor1));   // recibe la operación
            if (err == -1) {
                printf("Error en recepcion\n");
                close(sc);
                res = -1;
            }
            err = recvMessage ( sc, (char *) &N, sizeof(N));   // recibe la operación
            if (err == -1) {
                printf("Error en recepcion\n");
                close(sc);
                res = -1;
            }
            N = ntohl(N);
            for (int i = 0; i< N; i++){
                err = recvMessage ( sc, (char *) &valor2[i], sizeof(double));   // recibe la operación
                if (err == -1) {
                    printf("Error en recepcion\n");
                    close(sc);
                    res = -1;
                }
            }
            res = set_value(&lista, clave, valor1, N, valor2);
            printList(lista);
            pthread_mutex_unlock(&funciones);
        }
        else if (op == 2){
            err = recvMessage ( sc, (char *) &clave, sizeof(int));   // recibe la operación
            if (err == -1) {
                printf("Error en recepcion\n");
                close(sc);
                res = -1;
            }
            clave = ntohl(clave);
            printf("CLAVE %d", clave);
            struct respuesta res=get_value(&lista, clave);
            respuesta = htonl(res.status);
            err = sendMessage(sc, (char *)&respuesta, sizeof(uint32_t));  // envía el resultado
            if (err == -1) {
                printf("Error en envio\n");
            }
            if(respuesta==0){
                err = sendMessage(sc, (char *) &res.valor1, sizeof(res.valor1)); // envía op
                if (err == -1){
                    printf("Error en envio de valor1\n");
                }
                int32_t N = htonl(res.N);
                err = sendMessage(sc, (char *) &N, sizeof(N)); // envía op
                if (err == -1){
                    printf("Error en envio de N\n");
                }
                for (int i = 0; i< res.N; i++){
                        valor2[i] = res.valor2[i];
                        err = sendMessage(sc, (char *) &valor2[i], sizeof(valor2[i])); // envía op
                        if (err == -1){
                            printf("Error en envio de valor2 %d\n", i);

                }
                }
            }
            pthread_mutex_unlock(&funciones);
        }
        else if (op == 3){
            err = recvMessage ( sc, (char *) &clave, sizeof(int));   // recibe la operación
            if (err == -1) {
                printf("Error en recepcion\n");
                close(sc);
                res = -1;
            }
            clave = ntohl(clave);
            err = recvMessage ( sc, (char *) &valor1, sizeof(valor1));   // recibe la operación
            if (err == -1) {
                printf("Error en recepcion\n");
                close(sc);
                res = -1;
            }
            err = recvMessage ( sc, (char *) &N, sizeof(N));   // recibe la operación
            if (err == -1) {
                printf("Error en recepcion\n");
                close(sc);
                res = -1;
            }
            N = ntohl(N);
            for (int i = 0; i< N; i++){
                err = recvMessage ( sc, (char *) &valor2[i], sizeof(double));   // recibe la operación
                if (err == -1) {
                    printf("Error en recepcion\n");
                    close(sc);
                    res = -1;
                }
            }
            res = modify_value(&lista, clave, valor1, N, valor2);
            printList(lista);
            pthread_mutex_unlock(&funciones);
        }
        else if (op == 4){
            err = recvMessage ( sc, (char *) &clave, sizeof(int));   // recibe la operación
            if (err == -1) {
                printf("Error en recepcion\n");
                close(sc);
                res = -1;
            }
            clave = ntohl(clave);
            res = delete_key(&lista, clave);
            printList(lista);
            pthread_mutex_unlock(&funciones);
        }
        else if (op == 5){
            err = recvMessage ( sc, (char *) &clave, sizeof(int));   // recibe la operación
            if (err == -1) {
                printf("Error en recepcion\n");
                close(sc);
                res = -1;
            }
            clave = ntohl(clave);
            res = exist(&lista, clave);
            pthread_mutex_unlock(&funciones);
        }
        else{
            perror("Operacion no valida");
            res = -1;
        }
    }
    if (op!=2){
    respuesta = htonl(res);
    printf("%d",res);
    err = sendMessage(sc, (char *)&respuesta, sizeof(uint32_t));  // envía el resultado
    if (err == -1) {
        printf("Error en envio\n");
        close(sc);
    }
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[]){

    if (argc!=2){
        perror("Error en los argumentos");
        return -1;
    }

    pthread_t hilo;
    pthread_attr_t t_attr;		// atributos de los threads

    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&funciones, NULL);
    pthread_cond_init(&cond, NULL);
    pthread_attr_init(&t_attr);

	//atributos de los threads, threads independientes
	pthread_attr_setdetachstate(&t_attr, PTHREAD_CREATE_DETACHED);

    struct sockaddr_in server_addr, client_addr;
    socklen_t size;
    int sd, sc;
    int val;

    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Error en el socket");
        return 0;
    }

    val = 1;
    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *)&val, sizeof(int));

    bzero((char *)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;

    char *endptr;
    uint32_t puerto;
    long port_arg = strtol(argv[1], &endptr, 10);
    if (endptr != argv[1] && *endptr == '\0') {
        puerto = (int)port_arg;
    } else {
        fprintf(stderr, "El argumento de puerto no es un número válido.\n");
        return -1;
    }

    server_addr.sin_port = htons(puerto);

    err = bind(sd, (const struct sockaddr *)&server_addr,
			sizeof(server_addr));
	if (err == -1) {
		printf("Error en bind\n");
		return -1;
	}

    err = listen(sd, SOMAXCONN);
	if (err == -1) {
		printf("Error en listen\n");
		return -1;
	}

    size = sizeof(client_addr);

    while(1) {

        printf("esperando conexion\n");
    	sc = accept(sd, (struct sockaddr *)&client_addr, (socklen_t *)&size);
        printf("conexión aceptada de IP: %s   Puerto: %d\n",
		inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

		if (sc == -1) {
			printf("Error en accept\n");
			return -1;
		}        

        if(pthread_create(&hilo, &t_attr, (void *)atender_peticion, (int *) &sc) == 0){
            pthread_mutex_lock(&mutex);
			while (mensaje_no_copiado)
				pthread_cond_wait(&cond, &mutex);
			mensaje_no_copiado = 1;
			pthread_mutex_unlock(&mutex);

        }
        else{
            perror("error creación de hilo");
            return -1;
        }
    }
    close (sd);

    return(0);
}