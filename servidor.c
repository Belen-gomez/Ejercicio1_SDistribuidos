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
    printf("---------------------Lista inicializada--------------------\n");
    printf("\n\n"); // Agregar una línea en blanco
    pthread_mutex_unlock(&funciones);
    return 0;
}


int set_value(List *l, int clave, char *valor1, int N, double *valor2){
    pthread_mutex_lock(&funciones);
    // Verificar si la clave ya existe en la lista
    List aux = *l;
    while (aux != NULL) {
        if (aux->clave == clave) {
            printf("----Error: Ya existe un elemento con la clave %d. No se puede insertar----\n", clave);
            printf("\n\n"); // Agregar una línea en blanco
            pthread_mutex_unlock(&funciones);
            return -1; // Clave duplicada, retorna error
        }
        aux = aux->siguiente;
    }
    pthread_mutex_unlock(&funciones);
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
    pthread_mutex_lock(&funciones);
    ptr->siguiente = *l;
    *l = ptr;
    printf("------------Tupla de clave %d insertada------------\n", clave);
    printList(*l);
    pthread_mutex_unlock(&funciones);

	return 0;
}

int printList(List l) {
    List aux = l;
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
    struct respuesta res;
    pthread_mutex_lock(&funciones);
    if (*l == NULL) {
        perror("--------------Get_value. La lista está vacía----------------\n");
        printf("\n\n"); // Agregar una línea en blanco
        res.status = -1;
        pthread_mutex_unlock(&funciones);
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
            printf("------------Tupla de clave %d encontrada------------\n", clave);
            printf("\n\n"); // Agregar una línea en blanco
            pthread_mutex_unlock(&funciones);
            return res;
        }
        aux = aux->siguiente;
    }
    perror("------------Se ha intentado acceder a una clave que no existe (get_value)--------\n");
    printf("\n\n"); // Agregar una línea en blanco
    pthread_mutex_unlock(&funciones);
    res.status = -1;
    return res;

}


int modify_value(List *l, int clave, char *valor1, int N, double *valor2){
    pthread_mutex_lock(&funciones);
    
    if (*l == NULL) {
        perror("------------La lista está vacía. Modify_value--------------\n");
        printf("\n\n"); // Agregar una línea en blanco
        pthread_mutex_unlock(&funciones);
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
            printf("------------Tupla de clave %d modificada------------\n", clave);
            printList(*l);
            pthread_mutex_unlock(&funciones);
            return 0;
        }
        aux = aux->siguiente;
    }
    perror("--------------Se ha intentado modificar a una clave que no existe (modify_value)----------------\n");
    printf("\n\n"); // Agregar una línea en blanco
    pthread_mutex_unlock(&funciones);
    return -1;
}

int delete_key(List *l, int key) {
    pthread_mutex_lock(&funciones);
    if (*l == NULL) {
        perror("-------------La lista está vacía (delete_key)--------------------\n");
        printf("\n\n"); // Agregar una línea en blanco
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
        perror("--------No se ha encontrado la clave para eliminar---------\n");
        printf("\n\n"); // Agregar una línea en blanco
        pthread_mutex_unlock(&funciones);
        return -1;
    }

    // Si previous es NULL, el nodo a eliminar es el primero de la lista
    if (previous == NULL) {
        *l = current->siguiente;
    } else {
        // El nodo a eliminar está en el medio o al final de la lista
        previous->siguiente = current->siguiente;
    }
    printf("------------Tupla de clave %d eliminada------------\n", key);
    printList(*l);
    pthread_mutex_unlock(&funciones);
    // Liberar la memoria del nodo eliminado
    free(current->valor2); // Liberar la memoria del arreglo valor2
    free(current->valor1); // Liberar la memoria de la cadena valor1
    free(current); // Liberar la memoria del nodo
    return 0; // Operación exitosa
}

int exist(List *l, int clave){
    int encontrado = 0;

    pthread_mutex_lock(&funciones);
    if (*l == NULL) {
        perror("--------------La lista está vacía. Exixts----------------\n");
        printf("\n\n"); // Agregar una línea en blanco
        pthread_mutex_unlock(&funciones);
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
        printf("----------------La clave %d no existe----------------\n", clave);
        printf("\n\n"); // Agregar una línea en blanco
        pthread_mutex_unlock(&funciones);
        return 0;
    }
    else{
        printf("----------------La clave %d existe----------------\n", clave);
        printf("\n\n"); // Agregar una línea en blanco
        pthread_mutex_unlock(&funciones);
        return 1;
    }
}

void atender_peticion(int * s){
    //Variables para recibir los mensajes
    int32_t  op;
    int32_t clave;
    int32_t  N;
    char valor1[MAXSIZE];
    uint32_t respuesta;
    int sc;
    struct respuesta respuesta_strc;

    //Copia del socket
    pthread_mutex_lock(&mutex);
    sc = (* (int *)s);
    mensaje_no_copiado = 0;

	pthread_cond_signal(&cond);

	pthread_mutex_unlock(&mutex);


    err = recvMessage ( sc, (char *) &op, sizeof(op));   // recibe la operación
    if (err == -1) {
        printf("Error en recepcion\n");
        close(sc);
        res = -1;
    }
    else{
        //Si no hay error al recibir la operación, se procede a recibir los demás datos
        op = ntohl(op);
        if (err!=-1){
            if (op == 0){
                res = init(&lista);
            }else if (op == 1){
                err = recvMessage ( sc, (char *) &clave, sizeof(int));   // recibe la clave
                if (err == -1) {
                    printf("Error en recepcion\n");
                    close(sc);
                    res = -1;
                }
                //Se convierte la clave a host byte order
                clave = ntohl(clave);

                err = recvMessage ( sc, (char *) &valor1, sizeof(valor1));   // recibe la cadena
                if (err == -1) {
                    printf("Error en recepcion\n");
                    close(sc);
                    res = -1;
                }

                err = recvMessage ( sc, (char *) &N, sizeof(N));   // recibe N
                if (err == -1) {
                    printf("Error en recepcion\n");
                    close(sc);
                    res = -1;
                }
                //Se convierte N a host byte order
                N = ntohl(N);

                //Los elementos del vector se reciben uno a uno
                double *valor2 = malloc(N * sizeof(double));
                for (int i = 0; i< N; i++){
                    err = recvMessage ( sc, (char *) &valor2[i], sizeof(double));   // recibe valor2[i]
                    if (err == -1) {
                        printf("Error en recepcion\n");
                        close(sc);
                        res = -1;
                    }
                }
                //Se llama a la función set_value
                res = set_value(&lista, clave, valor1, N, valor2);
                free(valor2);
            }
            else if (op == 2){
                err = recvMessage ( sc, (char *) &clave, sizeof(int));   // recibe la clave
                if (err == -1) {
                    printf("Error en recepcion\n");
                    close(sc);
                    res = -1;
                }
                //Se convierte la clave a host byte order
                clave = ntohl(clave);
                
                //Esta función tiene que devolver los elementos asociados a una clave. Para ello se guardaran en una estructura de respuesta.
                //Si la función no da ningún error se enviarán todos los datos
                struct respuesta respuesta_strc = get_value(&lista, clave);
                res = respuesta_strc.status;
            }
            else if (op == 3){
                err = recvMessage ( sc, (char *) &clave, sizeof(int));   // recibe la clave
                if (err == -1) {
                    printf("Error en recepcion\n");
                    close(sc);
                   res = -1;
                }
                //Se convierte la clave a host byte order
                clave = ntohl(clave);

                err = recvMessage ( sc, (char *) &valor1, sizeof(valor1));   // recibe la cadena
                if (err == -1) {
                    printf("Error en recepcion\n");
                    close(sc);
                    res = -1;
                }

                err = recvMessage ( sc, (char *) &N, sizeof(N));   // recibe N
                if (err == -1) {
                    printf("Error en recepcion\n");
                    close(sc);
                    res = -1;
                }
                //Se convierte la clave a host byte order
                N = ntohl(N);

                //Se recibe cada elemento del vector uno a uno
                double *valor2 = malloc(N * sizeof(double));
                for (int i = 0; i< N; i++){
                    err = recvMessage ( sc, (char *) &valor2[i], sizeof(double));   // recibe valor2[i]
                    if (err == -1) {
                        printf("Error en recepcion\n");
                        close(sc);
                        res = -1;
                    }
                }
                //Llama a la función modify_value
                res = modify_value(&lista, clave, valor1, N, valor2);
                free(valor2);
            }
            else if (op == 4){
                err = recvMessage ( sc, (char *) &clave, sizeof(int));   // recibe la clave
                if (err == -1) {
                    printf("Error en recepcion\n");
                    close(sc);
                    res = -1;
                }
                //Se convierte la clave a host byte order
                clave = ntohl(clave);

                //Llama a la función delete_key
                res = delete_key(&lista, clave);
                pthread_mutex_unlock(&funciones);
            }
            else if (op == 5){
                err = recvMessage ( sc, (char *) &clave, sizeof(int));   // recibe la clave
                if (err == -1) {
                    printf("Error en recepcion\n");
                    close(sc);
                    res = -1;
                }
                //Se convierte la clave a host byte order
                clave = ntohl(clave);

                //Llama a la función exist
                res = exist(&lista, clave);
            }
            else{
                perror("Operacion no valida");
                res = -1;
            }
        }
    }
    respuesta = htonl(res);
    err = sendMessage(sc, (char *)&respuesta, sizeof(uint32_t));  // envía el resultado
    if (err == -1) {
        printf("Error en envio\n");
        close(sc);
    }
    
    if(res==0 && op ==2){
        //Si la función de get_value no ha dado error, es decir, se ha encontrado el elemento asociado a la clave se tienen que enviar el resto de valores.
        //Sino, no se enviarán más valores y el cliente no deberá esperar más valores.
        err = sendMessage(sc, (char *) &respuesta_strc.valor1, sizeof(respuesta_strc.valor1)); // envía la cadena
        if (err == -1){
            printf("Error en envio de valor1\n");
        }
        
        int32_t N = htonl(respuesta_strc.N);
        err = sendMessage(sc, (char *) &N, sizeof(N)); // envía N
        if (err == -1){
            printf("Error en envio de N\n");
        }
        //Se envía el vector valor por valor
        for (int i = 0; i< respuesta_strc.N; i++){
            err = sendMessage(sc, (char *) &respuesta_strc.valor2[i], sizeof(respuesta_strc.valor2[i])); // envía valor2[i]
            if (err == -1){
                printf("Error en envio de valor2 %d\n", i);
            }
        }   
    
    pthread_exit(NULL);
    }
}

int main(int argc, char *argv[]){
    //Verificar los argumentos
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

    //Variables para los sockets
    struct sockaddr_in server_addr, client_addr;
    socklen_t size;
    int sd, sc; //El socket principal es sd y el socket de conexión es sc

    //Creación del socket TCP
    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Error en el socket");
        return 0;
    }
    //COnfiguración del socket
    int val = 1;
    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *)&val, sizeof(int));
    bzero((char *)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;

    //Obtener el puerto. Se utiliza la función strtol para convertir el argumento de puerto a un número y manejar errores
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

    //Asocia la dirección al socket
    err = bind(sd, (const struct sockaddr *)&server_addr,sizeof(server_addr));
	if (err == -1) {
		printf("Error en bind\n");
		return -1;
	}

    //Escucha en el socket
    err = listen(sd, SOMAXCONN);
	if (err == -1) {
		printf("Error en listen\n");
		return -1;
	}

    size = sizeof(client_addr);

    while(1) {
        //Cada vez que llega una nueva petición y se acepta, se crea un nuevo socket
    	sc = accept(sd, (struct sockaddr *)&client_addr, (socklen_t *)&size);
        if (sc == -1) {
			printf("Error en accept\n");
			return -1;
		} 

        //Servidor bajo demanda. Cada petición se atiende en un hilo diferente
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