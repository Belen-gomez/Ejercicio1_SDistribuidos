#include "mensajes.h"
#include "servidor.h"
#include <mqueue.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#define MAXSIZE 256

pthread_mutex_t mutex;
pthread_cond_t cond;
int mensaje_no_copiado = 1;
mqd_t q_servidor;
mqd_t q_cliente =0;
struct respuesta res;


List lista; // Lista enlazada para almacenar las tuplas

int init(List *l) {

    *l = NULL;
    return 0;
}


int set_value(List *l, int clave, char *valor1, int N, double *valor2){
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

 int get_value(List *l, int clave){
    if (*l == NULL) {
        perror("La lista está vacía");
        return -1;
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
            return 0;
        }
        aux = aux->siguiente;
    }
    perror("Se ha intentado acceder a una clave que no existe");
    return -1;

}


int modify_value(List *l, int clave, char *valor1, int N, double *valor2){
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
            strcpy(aux->valor1, valor1);
            aux->N = N;
            aux->valor2 = malloc(N * sizeof(double));
            for(int i = 0; i < N; i++){
                aux->valor2[i] = valor2[i];
            }
            return 0;
        }
        aux = aux->siguiente;
    }
    perror("Se ha intentado acceder a una clave que no existe");
    return -1;
}

int delete_key(List *l, int key) {
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
    free(current); // Liberar la memoria del nodo
    return 0; // Operación exitosa
}

int exist(List *l, int clave){
    if (*l == NULL) {
        perror("La lista está vacía");
        return -1;
    }
    List aux = *l;
    while (aux != NULL) {
        if (aux->clave == clave) {
            // Se encontró la clave, copiar los valores a la estructura de respuesta
            return 1;
        }
        aux = aux->siguiente;
    }
    perror("No se encuentra la clave");
    return 0;
}

void atender_peticion(struct peticion *pet){
    
    struct peticion peticion;
    pthread_mutex_lock(&mutex);
    peticion = *pet;
    mensaje_no_copiado = 0;

	pthread_cond_signal(&cond);

	pthread_mutex_unlock(&mutex);

    if (peticion.op == 0){
        res.status = init(&lista);
    }else if (peticion.op == 1){
        res.status = set_value(&lista, peticion.clave, peticion.valor1, peticion.N, peticion.valor2);
    }
    else if (peticion.op == 2){
        res.status =get_value(&lista, pet->clave);
    }
    else if (peticion.op == 3){
        res.status = modify_value(&lista, pet->clave, pet->valor1, pet->N, pet->valor2);
        
    }
    else if (peticion.op == 4){
        res.status = delete_key(&lista, pet->clave);
    }
    else if (peticion.op == 5){
        res.status = exist(&lista, pet->clave);
    }
    else{
        perror("Operacion no valida");
        res.status = -1;
    }

    //se responde al cliente abriendo reviamente su cola

    q_cliente = mq_open(peticion.q_name, O_WRONLY);
    if (q_cliente < 0) {
        perror("mq_open 2");
        if(mq_close(q_servidor)==-1){
            perror("mq_close servidor");
        }
        if(mq_unlink("/100472037")==-1){
            perror("mq_unlink servidor");
        }
        res.status = -1;
    }
    else{
        if (mq_send(q_cliente, (const char *)&res, sizeof(struct respuesta), 0) < 0) {
            perror("mq_send");
            if(mq_close(q_servidor)==-1){
                perror("mq_close servidor");
            }
            if(mq_unlink("/100472037")==-1){
                perror("mq_unlink servidor");
            }
            if(mq_close(q_cliente)==-1){
                perror("mq_close cliente");
            }
            res.status = -1;
        }
    }
    if(mq_close(q_cliente)==-1){
        perror("mq_close cliente");
    }

    pthread_exit(0);
}

int main(){
    pthread_t hilo;
    pthread_attr_t t_attr;		// atributos de los threads

    struct peticion pet;
    struct mq_attr attr;

    attr.mq_maxmsg = 10;
	attr.mq_msgsize = sizeof(struct peticion);

    q_servidor = mq_open("/100472037", O_CREAT|O_RDONLY, 0700, &attr);
    if (q_servidor == -1) {
		perror("servidor mq_open");
		return -1;
	}

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);
    pthread_attr_init(&t_attr);

	//atributos de los threads, threads independientes
	pthread_attr_setdetachstate(&t_attr, PTHREAD_CREATE_DETACHED);

    while(1) {

        if (mq_receive(q_servidor, (char *) &pet, sizeof(pet), 0) < 0){
            if(mq_close(q_servidor)==-1){
                perror("servidor mq_close");
            }
            if(mq_unlink("/100472037")==1){
                perror("servidor mq_unlink");
            }
            perror("servidor: mq_recev");
            return -1;
        }

        if(pthread_create(&hilo, &t_attr, (void *)atender_peticion, (void *) &pet) == 0){
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
    return 0;
}