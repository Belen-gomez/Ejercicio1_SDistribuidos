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

int init() {
    // Abrir el archivo en modo de escritura, que borrará el contenido existente
    FILE *archivo = fopen("base.txt", "w");
    if (archivo == NULL) {
        printf("No se pudo abrir el archivo.\n");
        return -1;
    }

    // Cerrar el archivo
    fclose(archivo);
    return 0;
}

int set_value(int clave, char *valor1){
    FILE *archivo = fopen("base.txt", "a"); // Abrir el archivo en modo de escritura "a" (append)
    if (archivo == NULL) {
        printf("No se pudo abrir el archivo.\n");
        return -1;
    }
    // Verificar si valor1 es válido antes de usarlo
    if (valor1 == NULL) {
        printf("Error: valor1 no apunta a una cadena de caracteres válida.\n");
        fclose(archivo);
        return -1;
    }
    // Escribir la tupla en el archivo en el formato deseado
    fprintf(archivo, "<%d, [", clave);
    fprintf( archivo, valor1);
    /*for (int i = 0; i < N; i++) {
        fprintf(archivo, "%f", valor2[i]);
        if (i < N - 1) {
            fprintf(archivo, ", ");
        }
    }*/
    //fprintf(archivo, "]>\n");

    fclose(archivo);
    return 0;
}


void atender_peticion(void *pet){

    struct peticion peticion;
    pthread_mutex_lock(&mutex);
    printf("atender");
	peticion = (*(struct peticion *) pet);


	mensaje_no_copiado = 0;

	pthread_cond_signal(&cond);

	pthread_mutex_unlock(&mutex);
    int res;
    if (peticion.op == INIT)
        res = init();
    else if (peticion.op == 1){

        res = set_value(peticion.clave, peticion.valor1);
        //res = getLength(lista);
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
    }

    //se responde al cliente abriendo reviamente su cola
    q_cliente = mq_open(peticion.q_name, O_WRONLY);
    if (q_cliente < 0) {
        perror("mq_open 2");
        mq_close(q_servidor);
        mq_unlink("/SERVIDOR");
    }
    else{
        if (mq_send(q_cliente, (const char *)&res, sizeof(int), 0) < 0) {
        perror("mq_send");
        mq_close(q_servidor);
        mq_unlink("/SERVIDOR");
        mq_close(q_cliente);
        }
    }
    mq_close(q_cliente);

    //mq_close(q_cliente);
    pthread_exit(0);
}

int main(){
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

        if(pthread_create(&hilo, &t_attr, (void *)atender_peticion, (void *) &pet) == 0){
            pthread_mutex_lock(&mutex);
			while (mensaje_no_copiado)
				pthread_cond_wait(&cond, &mutex);
			mensaje_no_copiado = 1;
			pthread_mutex_unlock(&mutex);

        }
    }
    mq_close(q_servidor);
    mq_unlink("/SERVIDOR");
    return 0;
}