#include "mensajes.h"
#include "servidor.h"
#include <mqueue.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>

pthread_mutex_t mutex;
pthread_cond_t cond;
int mensaje_no_copiado = 1;
mqd_t q_servidor;     	   
mqd_t q_cliente;  


List lista; // Lista enlazada para almacenar las tuplas

int init(List *l) {
    // Eliminar todas las tuplas almacenadas previamente

    *l = NULL;
    // Eliminar todas las tuplas almacenadas previamente
    /*Tupla *actual = lista;
    while (actual != NULL) {
        Tupla *temp = actual;
        actual = actual->siguiente;
        free(temp->valor2); // Liberar memoria del vector de valores (valor2)
        free(temp);
    }
    lista = NULL; // Reiniciar la lista*/
    return 0; // Éxito
}

int set_value(List *l, int clave, char *valor1, int N, double *valor2){
    struct Tupla *ptr;

	ptr = (struct Tupla *) malloc(sizeof(struct Tupla));
	if (ptr == NULL) 
		return -1;

	if (*l == NULL) {  // emtpy list
        ptr->clave = clave;
		ptr->valor1= valor1;
		ptr->N = N;
        ptr->valor2 = valor2; 
		ptr->siguiente = NULL;
		*l= ptr;
	}
	else {
		// insert in head
		ptr->clave = clave;
		ptr->valor1 = valor1;
		ptr->N = N;
        ptr->valor2 = valor2;
		ptr->siguiente = *l;
		*l= ptr;
	}

	return 0;
}

  

int atender_peticion(void *pet){
    struct peticion peticion;	/* mensaje local */
    pthread_mutex_lock(&mutex);

	peticion = (*(struct peticion *) pet);

	/* ya se puede despertar al servidor*/
	mensaje_no_copiado = 0;

	pthread_cond_signal(&cond);

	pthread_mutex_unlock(&mutex);
    int res;
    if (peticion.op == INIT)   
        res = init(&lista);
    else if (peticion.op == 1){
        res = set_value(&lista, peticion.clave, peticion.valor1, peticion.N, peticion.valor2);
    }
    else if (peticion.op == 2){
        //get_value(pet->clave, pet->valor1, pet->N, pet->valor2);
    }
    else if (peticion.op == 3){
        //modify_value(pet->clave, pet->valor1, pet->N, pet->valor2);
    }
    else if (peticion.op == 4){
        //delete_key(pet->clave);
    }
    else if (peticion.op == 5){
        //exist(pet->clave);
    }   
    else{
        perror("Operacion no valida");
        return -1;
    } 

    /* se responde al cliente abriendo reviamente su cola */
    q_cliente = mq_open(peticion.q_name, O_WRONLY);
    if (q_cliente < 0) {
        perror("mq_open 2");
        mq_close(q_servidor);
        mq_unlink("/SERVIDOR");
        return -1;
    }
    
    if (mq_send(q_cliente, (const char *)&res, sizeof(int), 0) < 0) {
        perror("mq_send");
        mq_close(q_servidor);
        mq_unlink("/SERVIDOR");
        mq_close(q_cliente);
        return -1;
        
    }

    //mq_close(q_cliente);
    pthread_exit(0);
    return 0;
}

int main(void){
    printf("servidor1");
    pthread_t hilo;
    pthread_attr_t t_attr;		// atributos de los threads

    struct peticion pet;     
    struct mq_attr attr;

    attr.mq_maxmsg = 10;                
	attr.mq_msgsize = sizeof(struct peticion);

    q_servidor = mq_open("/SERVIDOR", O_CREAT|O_RDONLY, 0700, &attr);
    if (q_servidor == -1) {
		perror("mq_open 1");
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

        if(pthread_create(&hilo, &t_attr, (void *) &atender_peticion, (void *) &pet) == 0){
            pthread_mutex_lock(&mutex);
			while (mensaje_no_copiado)
				pthread_cond_wait(&cond, &mutex);
			mensaje_no_copiado = 1;
			pthread_mutex_unlock(&mutex);
	 	   
        }


       
    }
    return 0;
}




