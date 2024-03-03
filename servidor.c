#include "claves.h"
#include <mqueue.h>
#include <stdio.h>
#include <pthread.h>

pthread_mutex_t mutex;
pthread_cond_t cond;
mqd_t q_servidor;     	   
mqd_t q_cliente;  

typedef struct Tupla {
    int clave;
    char valor1[256]; // Máximo de 255 caracteres + 1 para el carácter nulo de terminación de cadena
    int N;
    double *valor2;
    struct Tupla *siguiente;
} Tupla;

Tupla *lista = NULL; // Lista enlazada para almacenar las tuplas

int init() {
    pthread_mutex_lock(&mutex);
    // Eliminar todas las tuplas almacenadas previamente
    Tupla *actual = lista;
    while (actual != NULL) {
        Tupla *temp = actual;
        actual = actual->siguiente;
        free(temp->valor2); // Liberar memoria del vector de valores (valor2)
        free(temp);
    }
    lista = NULL; // Reiniciar la lista

    pthread_mutex_unlock(&mutex);
    return 0; // Éxito
}

typedef struct Node * List;

  

void atender_peticion(struct peticion *pet){
    int res;
    if (pet->op ==0)   
        res = init();
    else if (pet->op == 1){
        set_value(pet->clave, pet->valor1, pet->N, pet->valor2);
    }
    else if (pet->op == 2){
        get_value(pet->clave, pet->valor1, pet->N, pet->valor2);
    }
    else if (pet->op == 3){
        modify_value(pet->clave, pet->valor1, pet->N, pet->valor2);
    }
    else if (pet->op == 4){
        delete_key(pet->clave);
    }
    else if (pet->op == 5){
        exist(pet->clave);
    }   
    else{
        perror("Operacion no valida");
        return -1;
    } 

    /* se responde al cliente abriendo reviamente su cola */
    q_cliente = mq_open(pet->q_name, O_WRONLY);
    if (q_cliente < 0) {
        perror("mq_open");
        mq_close(q_servidor);
        mq_unlink("/SERVIDOR_SUMA");
        return -1;
    }
    
    if (mq_send(q_cliente, (const char *)&res, sizeof(int), 0) < 0) {
        perror("mq_send");
        mq_close(q_servidor);
        mq_unlink("/SERVIDOR_SUMA");
        mq_close(q_cliente);
        return -1;
        
    }

    mq_close(q_cliente);
}

int main(){
    pthread_t hilo;
    pthread_attr_t t_attr;		// atributos de los threads

    struct peticion pet;      int res;
    struct mq_attr attr;

    attr.mq_maxmsg = 10;                
	attr.mq_msgsize = sizeof(struct peticion);

    q_servidor = mq_open("/SERVIDOR", O_CREAT|O_RDONLY, 0700, &attr);
    if (q_servidor == -1) {
		perror("mq_open");
		return -1;
	}

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);
    pthread_attr_init(&t_attr);

	// atributos de los threads, threads independientes
	pthread_attr_setdetachstate(&t_attr, PTHREAD_CREATE_DETACHED);
    
    while(1) {
        if (mq_receive(q_servidor, (char *) &pet, sizeof(pet), 0) < 0){
            perror("mq_recev");
            return -1;
        }

        if(pthread_create(&hilo, &t_attr, (void *) &atender_peticion, (void *) &pet) == -1){
            perror("pthread_create");
            return -1;
        }


       
    }
}