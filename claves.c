#include "claves.h"
#include <mqueue.h>
#include <stdio.h>


int init(){
    mqd_t q_servidor;       /* cola de mensajes del proceso servidor */
	mqd_t q_cliente;        /* cola de mensajes para el proceso cliente */

	struct peticion pet;
	int res;        struct mq_attr attr;

	attr.mq_maxmsg = 1;     attr.mq_msgsize = sizeof(int);
	q_cliente = mq_open("/100472037", O_CREAT|O_RDONLY, 0700, &attr);
	if (q_cliente == -1) {
		perror("mq_open");
		return -1;
	}
    q_servidor = mq_open("/SERVIDOR", O_WRONLY);
	if (q_servidor == -1){
		mq_close(q_cliente);
		perror("mq_open");
		return -1;
	}
	pet.op = 0;
	if (mq_send(q_servidor, (const char *)&pet, sizeof(pet), 0) < 0) {
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

}
