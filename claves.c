#include "mensajes.h"
#include "claves.h"
#include <mqueue.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int init(){
    mqd_t q_servidor;       /* cola de mensajes del proceso servidor */
	mqd_t q_cliente;        /* cola de mensajes para el proceso cliente */
    char queuename[MAXSIZE];

	//struct peticion pet = (struct peticion *)malloc(sizeof(struct peticion));
	struct peticion pet;
	int res;        struct mq_attr attr;

	attr.mq_maxmsg = 1;     attr.mq_msgsize = sizeof(int);
    sprintf(queuename,  "/Cola-%d", getpid());
	q_cliente = mq_open(queuename, O_CREAT|O_RDONLY, 0700, &attr);
	if (q_cliente == -1) {
		perror("mq_open");
		return -1;
	}

    q_servidor = mq_open("/100472037", O_WRONLY);
	if (q_servidor == -1){
		mq_close(q_cliente);
		perror("mq_open 1");
		return -1;
	}
	pet.op = 0;
	strcpy(pet.q_name, queuename);

    printf("Tamaño de la estructura peticion: %zu bytes\n", sizeof(&pet));
    printf("Tamaño de la estructura peticion: %zu bytes\n", sizeof(struct peticion));

	if (mq_send(q_servidor, (const char *)&pet, sizeof(struct peticion), 0) < 0) {
		perror("mq_send 1");
        mq_close(q_servidor);
        mq_close(q_cliente);
        mq_unlink(queuename);
		return -1;
	}
	
	if (mq_receive(q_cliente, (char *)&res, sizeof(int), 0) < 0) {
        mq_close(q_servidor);
        mq_close(q_cliente);
        mq_unlink(queuename);
		perror("mq_receive");
		return -1;
	}
	printf("recibir");
	mq_close(q_servidor);
	mq_close(q_cliente);
	mq_unlink(queuename);
	return res;

}

int set_value(int key,char *value1, int N_value, double *V_value2){

	mqd_t q_servidor;       
	mqd_t q_cliente;
    char queuename[MAXSIZE];
	struct peticion pet;
	int res;        struct mq_attr attr;

	attr.mq_maxmsg = 1;     attr.mq_msgsize = sizeof(int);
    sprintf(queuename,  "/Cola-%d", getpid());

	q_cliente = mq_open(queuename, O_CREAT|O_RDONLY, 0700, &attr);
	if (q_cliente == -1) {
		perror("mq_open");
		return -1;
	}

	struct mq_attr attr_s;
	attr_s.mq_maxmsg = 10;
	attr_s.mq_msgsize = sizeof(struct peticion);

    q_servidor = mq_open("/100472037", O_WRONLY);
	if (q_servidor == -1){
		mq_close(q_cliente);
		perror("mq_open");
		return -1;
	}
	pet.op = 1;
	pet.clave = key;
    strcpy(pet.valor1, value1);
    pet.N = N_value;

    for (int i = 0; i< N_value; i++){
        pet.valor2[i] = V_value2[i];
        printf("%f", pet.valor2[i]);
    }
	strcpy(pet.q_name, queuename);

    printf("Tamaño de la estructura peticion: %zu bytes\n", sizeof(pet));
    printf("Tamaño de la estructura peticion: %zu bytes\n", sizeof(struct peticion));
	if (mq_send(q_servidor, (const char *)&pet, sizeof(struct peticion), 0) < 0) {
		perror("mq_send 3");
		mq_close(q_servidor);
		mq_close(q_cliente);
        mq_unlink(queuename);
		return -1;
	}
	if (mq_receive(q_cliente, (char *)&res, sizeof(int), 0) < 0) {
		perror("mq_receive");
		mq_close(q_servidor);
		mq_close(q_cliente);
        mq_unlink(queuename);
		return -1;
	}
	mq_close(q_servidor);
	mq_close(q_cliente);
	mq_unlink(queuename);
	return res;
}

