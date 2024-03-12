#include "mensajes.h"
#include "claves.h"
#include <mqueue.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int init(){
	
    mqd_t q_servidor;       /* cola de mensajes del proceso servidor */
	mqd_t q_cliente;        /* cola de mensajes para el proceso cliente */

	//struct peticion pet = (struct peticion *)malloc(sizeof(struct peticion));
	struct peticion pet;
	int res;        struct mq_attr attr;

	attr.mq_maxmsg = 1;     attr.mq_msgsize = sizeof(int);
	q_cliente = mq_open("/100472037", O_CREAT|O_RDONLY, 0700, &attr);
	if (q_cliente == -1) {
		perror("mq_open");
		return -1;
	}
    struct mq_attr attr_s;
    attr_s.mq_msgsize = sizeof(struct peticion) + 10;
    q_servidor = mq_open("/SERVIDOR", O_WRONLY, &attr_s);
	if (q_servidor == -1){
		mq_close(q_cliente);
		perror("mq_open");
		return -1;
	}
	pet.op = 0;

	strcpy(pet.q_name, "/100472037");

	if (mq_send(q_servidor, (char *)&pet, sizeof(struct peticion), 0) < 0) {
		perror("mq_send 1");
		return -1;
	}
	
	if (mq_receive(q_cliente, (char *)&res, sizeof(int), 0) < 0) {
		perror("mq_receive");
		return -1;
	}
	printf("recibir");
	mq_close(q_servidor);
	mq_close(q_cliente);
	mq_unlink("/100472037");
	return res;

}

int set_value(int key, char *value1){
	mqd_t q_servidor;       
	mqd_t q_cliente;        

	struct peticion pet;
	int res;        struct mq_attr attr;

	attr.mq_maxmsg = 1;     attr.mq_msgsize = sizeof(int);
	q_cliente = mq_open("/100472037", O_CREAT|O_RDONLY, 0700, &attr);
	if (q_cliente == -1) {
		perror("mq_open");
		return -1;
	}
	struct mq_attr attr_s;
	attr_s.mq_maxmsg = 10;                
	attr_s.mq_msgsize = sizeof(struct peticion);

    q_servidor = mq_open("/SERVIDOR", O_WRONLY, 0700, &attr_s);
	if (q_servidor == -1){
		mq_close(q_cliente);
		perror("mq_open");
		return -1;
	}
	pet.op = 1;
	pet.clave = key;
    strcpy(pet.valor1, value1);

	//pet.N = N_value2;
	//pet.valor2 = V_value2;
	//strcpy(pet.valor2, V_value2);
	strcpy(pet.q_name, "/100472037");

	if (mq_send(q_servidor, (char *)&pet, sizeof(pet), 0) < 0) {
		perror("mq_send");
		mq_close(q_servidor);
		mq_close(q_cliente);
		return -1;
	}
	if (mq_receive(q_cliente, (char *)&res, sizeof(int), 0) < 0) {
		perror("mq_receive");
		mq_close(q_servidor);
		mq_close(q_cliente);
		return -1;
	}
	mq_close(q_servidor);
	mq_close(q_cliente);
	mq_unlink("/100472037");
	return 0;
}

