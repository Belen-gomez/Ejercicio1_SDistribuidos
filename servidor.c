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

int set_value(int clave, char *valor1, int N, double *valor2){

    FILE *archivo_leer = fopen("base.txt", "r"); // Abrir el archivo en modo de lectura
    if (archivo_leer == NULL) {
        printf("No se pudo abrir el archivo.\n");
        return -1;
    }

    char *linea = NULL;
    size_t longitud = 0;

    while (getline(&linea, &longitud, archivo_leer) != -1) {
        int clave_actual;
        // Intentar extraer la clave de la línea
        if (sscanf(linea, "%d,", &clave_actual) == 1) {
            // Verificar si la clave actual es igual a la clave nueva
            if (clave_actual == clave) {
                // Cerrar el archivo y liberar la memoria de la línea
                fclose(archivo_leer);
                perror("Se ha intentado insertar una tupla con una clave repetido");
                free(linea);
                return -1; // Clave encontrada
            }
        }
    }
    fclose(archivo_leer);
    free(linea); // Liberar la memoria asignada por getline

    FILE *archivo = fopen("base.txt", "a"); // Abrir el archivo en modo de escritura "a" (append)
    if (archivo == NULL) {
        printf("No se pudo abrir el archivo.\n");
        return -1;
    }
    fprintf(archivo, "%d,%s,", clave, valor1);

    for (int i = 0; i < N; i++) {
        fprintf(archivo, "%f", valor2[i]);
        if (i < N - 1) {
            fprintf(archivo, ",");
        }
    }
    fprintf(archivo, "\n");

    fclose(archivo);
    return 0;
}

 int get_value(int clave){
    FILE *archivo = fopen("base.txt", "r"); // Abrir el archivo en modo de lectura
    if (archivo == NULL) {
        printf("No se pudo abrir el archivo.\n");
        return -1;
    }

    char *linea = NULL;
    size_t longitud = 0;
    
    while (getline(&linea, &longitud, archivo) != -1) {
        int clave_actual;
        
        // Intentar extraer la clave de la línea
        if (sscanf(linea, "%d,", &clave_actual) == 1) {
            
            // Verificar si la clave actual es igual a la clave nueva
            if (clave_actual == clave) {
                
                // Extraer el valor 1
                char valor1[MAXSIZE];
                sscanf(linea, "%d,%[^,],", &clave_actual, valor1);
                res.clave = clave_actual;
                strcpy(res.valor1, valor1);

                // Extraer el valor 2
                int N = 0;
                double valor2[32];
                char *token = strtok(linea, ",");
                while (token != NULL) {
                    if (N > 1) {
                        sscanf(token, "%lf", &valor2[N - 2]);
                    }
                    token = strtok(NULL, ",");
                    N++;
                }
                res.N = N - 2;
                for (int i = 0; i < N - 2; i++) {
                    res.valor2[i] = valor2[i];
                }
                fclose(archivo);
                free(linea);
                return 0; // Clave encontrada
            }
        }
    }
    fclose(archivo);
    free(linea); // Liberar la memoria asignada por getline
    perror("No se ha encontrado la clave");
    return -1; // Clave no encontrada
}

int modify_value(int clave, char *valor1, int N, double *valor2){
    FILE *archivo = fopen("base.txt", "r+"); // Abrir el archivo en modo de lectura y escritura
    if (archivo == NULL) {
        printf("No se pudo abrir el archivo.\n");
        return -1;
    }

    char *linea = NULL;
    size_t longitud = 0;
    int clave_actual;
    long int posicion = -1;

    while (getline(&linea, &longitud, archivo) != -1) {
        // Intentar extraer la clave de la línea
        if (sscanf(linea, "%d,", &clave_actual) == 1) {
            // Verificar si la clave actual es igual a la clave buscada
            if (clave_actual == clave) {
                posicion = ftell(archivo) - strlen(linea); // Guardar la posición de la línea encontrada
                break;
            }
        }
    }

    if (posicion == -1) {
        fclose(archivo);
        free(linea);
        perror("No se ha encontrado la clave");
        return -1; // Clave no encontrada
    }

    // Mover el puntero de lectura/escritura a la posición de la línea encontrada
    fseek(archivo, posicion, SEEK_SET);

    // Modificar el valor 1
    fprintf(archivo, "%d,%s,", clave, valor1);

    // Modificar el valor 2
    for (int i = 0; i < N; i++) {
        fprintf(archivo, "%f", valor2[i]);
        if (i < N - 1) {
            fprintf(archivo, ",");
        }
    }
    fprintf(archivo, "\n");

    fclose(archivo);
    free(linea);
    return 0;
}

void atender_peticion(struct peticion *pet){

    struct peticion peticion;
    pthread_mutex_lock(&mutex);
    peticion = *pet;
    mensaje_no_copiado = 0;

	pthread_cond_signal(&cond);

	pthread_mutex_unlock(&mutex);

    if (peticion.op == INIT)
        res.status = init();
    else if (peticion.op == 1){
        res.status = set_value(peticion.clave, peticion.valor1, peticion.N, peticion.valor2);
    }
    else if (peticion.op == 2){
        res.status =get_value(pet->clave);
    }
    else if (peticion.op == 3){
        res.status = modify_value(pet->clave, pet->valor1, pet->N, pet->valor2);
    }
    else if (peticion.op == 4){
        //delete_key(pet->clave);
    }
    else if (peticion.op == 5){
        //exist(pet->clave);
    }
    else{
        perror("Operacion no valida");
        res.status = -1;
    }

    //se responde al cliente abriendo reviamente su cola

    q_cliente = mq_open(peticion.q_name, O_WRONLY);
    if (q_cliente < 0) {
        perror("mq_open 2");
        mq_close(q_servidor);
        mq_unlink("/100472037");
        res.status = -1;
    }
    else{
        if (mq_send(q_cliente, (const char *)&res, sizeof(struct respuesta), 0) < 0) {
            perror("mq_send");
            mq_close(q_servidor);
            mq_unlink("/100472037");
            mq_close(q_cliente);
            res.status = -1;
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

    q_servidor = mq_open("/100472037", O_CREAT|O_RDONLY, 0700, &attr);
    if (q_servidor == -1) {
		perror("mq_open 1");
        res.status = -1;
		return res.status;
	}

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);
    pthread_attr_init(&t_attr);

	//atributos de los threads, threads independientes
	pthread_attr_setdetachstate(&t_attr, PTHREAD_CREATE_DETACHED);

    while(1) {

        if (mq_receive(q_servidor, (char *) &pet, sizeof(pet), 0) < 0){
            mq_close(q_servidor);
            mq_unlink("/100472037");
            perror("mq_recev");
            res.status = -1;
            return res.status;
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