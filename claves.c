#include "claves.h"
#include <mqueue.h>
#include <stdio.h>

int init(){

}

int main(){
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

}
