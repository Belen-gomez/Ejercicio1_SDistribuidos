#include "claves.h"
#include <mqueue.h>
#include <stdio.h>

int init(){

}
int set_value(int key, char *value1, int N_value2, double *V_value_2){

}

int get_value(int key, char *value1, int *N_value2, double *V_value2){

}

int modify_value(int key, char *value1, int N_value2, double V_value2){

}

int delete_key(int key){

}

int exist(int key){

}

int main(){
    mqd_t q_servidor;     	    /* cola de mensajes del servidor */
    mqd_t q_cliente;                /* cola de mensajes del cliente */
    struct peticion pet;      int res;
    struct mq_attr attr;

    attr.mq_maxmsg = 10;                
	attr.mq_msgsize = sizeof(struct peticion);

    q_servidor = mq_open("/SERVIDOR", O_CREAT|O_RDONLY, 0700, &attr);
    if (q_servidor == -1) {
		perror("mq_open");
		return -1;
	}
    
        while(1) {
            if (mq_receive(q_servidor, (char *) &pet, sizeof(pet), 0) < 0){
                perror("mq_recev");
                return -1;
            }
            if (pet.op ==0)   
                init();
            else if (pet.op == 1){
                set_value(pet.clave, pet.valor1, pet.N, pet.valor2);
            }
            else if (pet.op == 2){
                get_value(pet.clave, pet.valor1, pet.N, pet.valor2);
            }
            else if (pet.op == 3){
                modify_value(pet.clave, pet.valor1, pet.N, *pet.valor2);
            }
            else if (pet.op == 4){
                delete_key(pet.clave);
            }
            else if (pet.op == 5){
                exist(pet.clave);
            }   
            else{
                perror("Operacion no valida");
                return -1;
            }         
    
            /* se responde al cliente abriendo reviamente su cola */
            q_cliente = mq_open(pet.q_name, O_WRONLY);
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
                return -1;
            }
    
            mq_close(q_cliente);
        }
}