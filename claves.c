#include "mensajes.h"
#include "claves.h"
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdint.h>

#define UINT32_MAX ((uint32_t)-1)


int init(){
    int sd;
    struct sockaddr_in server_addr;
    struct hostent *hp;       
	int32_t op;
	int err;
    uint32_t res;
    err = obtenerVariablesEntorno();
    if (err == -1){
        printf("Error en las variables de entorno");
        return -1;
    }

    sd = socket(AF_INET, SOCK_STREAM, 0);
	if (sd == 1) {
		printf("Error en socket\n");
		return -1;
	}
    hp = gethostbyname (obtenerIP());
	if (hp == NULL) {
		printf("Error en gethostbyname\n");
		return -1;
	}

    bzero((char *)&server_addr, sizeof(server_addr));
    memcpy (&(server_addr.sin_addr), hp->h_addr, hp->h_length);
   	server_addr.sin_family  = AF_INET;
   	char *endptr;
    long int puerto = strtol(obtenerPuerto(), &endptr, 10);
    if (*endptr != '\0') {
        // Error: La cadena no es un número válido
        printf("Error: La cadena no es un número válido\n");
    } else if (puerto < 0 || puerto > UINT32_MAX) {
        // Error: Puerto fuera de rango
        printf("Error: Puerto fuera de rango\n");
    } else {
        // Conversión exitosa, puerto contiene el valor numérico
        server_addr.sin_port = htons((int32_t)puerto);
    }

    // se establece la conexión
   	err = connect(sd, (struct sockaddr *) &server_addr,  sizeof(server_addr));
	if (err == -1) {
		printf("Error en connect\n");
		return -1;
	}
    op = htonl(0);

    err = sendMessage(sd, (char *) &op, sizeof(op)); // envía op
	if (err == -1){
		printf("Error en envio\n");
		return -1;
	}
    err = recvMessage(sd, (char *) &res, sizeof(uint32_t));     // recibe la respuesta
	if (err == -1){
		printf("Error en recepcion\n");
		return -1;
	}
    close(sd);
    return res;

}

/*int set_value(int key,char *value1, int N_value2, double *V_value2){
    struct respuesta res;
    if(N_value2>32){
        perror("Vector demasiado grande");
        res.status = -1;
        return res.status;

    }
    if(N_value2<0){
        perror("Argumento N negativo");
        res.status = -1;
        return res.status;

    }

	mqd_t q_servidor;       
	mqd_t q_cliente;
    char queuename[MAXSIZE];
	struct peticion pet;
	struct mq_attr attr;

	attr.mq_maxmsg = 1;     attr.mq_msgsize = sizeof(struct respuesta);
    sprintf(queuename,  "/Cola-%d", getpid());

	q_cliente = mq_open(queuename, O_CREAT|O_RDONLY, 0700, &attr);
	if (q_cliente == -1) {
		perror("set_value: mq_open cliente");
        res.status = -1;
        return res.status;
	}

    q_servidor = mq_open("/100472037", O_WRONLY);
	if (q_servidor == -1){
		mq_close(q_cliente);
		perror("set_value: mq_open servidor");
        res.status = -1;
        return res.status;
	}
	pet.op = 1;
	pet.clave = key;
    strcpy(pet.valor1, value1);
    pet.N = N_value2;
    for (int i = 0; i< N_value2; i++){
        pet.valor2[i] = V_value2[i];
    }


	strcpy(pet.q_name, queuename);

	if (mq_send(q_servidor, (const char *)&pet, sizeof(struct peticion), 0) < 0) {
		perror("set_value: mq_send servidor");
		mq_close(q_servidor);
		mq_close(q_cliente);
        mq_unlink(queuename);
        res.status = -1;
        return res.status;
	}
	if (mq_receive(q_cliente, (char *)&res, sizeof(struct respuesta), 0) < 0) {
		perror("set_value: mq_receive cliente");
		mq_close(q_servidor);
		mq_close(q_cliente);
        mq_unlink(queuename);
        res.status = -1;
		return res.status;
	}
	if(mq_close(q_servidor) == -1){
        perror("set_value: mq_close servidor");
        res.status = -1;
        return res.status;
    }
	if(mq_close(q_cliente)==-1){
        perror("set_value: mq_close cliente");
        res.status = -1;
        return res.status;
    }
    if(mq_unlink(queuename)==-1){
        perror("set_value: mq_unlink");
        res.status = -1;
        return res.status;
    }
	return res.status;
}

int get_value(int key,char *value1, int *N_value, double *V_value2){
    struct respuesta res;
    mqd_t q_servidor;
    mqd_t q_cliente;
    char queuename[MAXSIZE];
    struct peticion pet;
    struct mq_attr attr;

    attr.mq_maxmsg = 1;     attr.mq_msgsize = sizeof(struct respuesta);
    sprintf(queuename,  "/Cola-%d", getpid());

    q_cliente = mq_open(queuename, O_CREAT|O_RDONLY, 0700, &attr);
    if (q_cliente == -1) {
        perror("get_value: mq_open cliente");
        res.status = -1;
        return res.status;
    }

    q_servidor = mq_open("/100472037", O_WRONLY);
    if (q_servidor == -1){
        mq_close(q_cliente);
        perror("get_value: mq_open servidor");
        res.status = -1;
        return res.status;
    }
    pet.op = 2;
    pet.clave = key;

    strcpy(pet.q_name, queuename);

    if (mq_send(q_servidor, (const char *)&pet, sizeof(struct peticion), 0) < 0) {
        perror("get_value: mq_send servidor");
        mq_close(q_servidor);
        mq_close(q_cliente);
        mq_unlink(queuename);
        res.status = -1;
        return res.status;
    }
    if (mq_receive(q_cliente, (char *)&res, sizeof(struct respuesta), 0) < 0) {
        perror("get_value: mq_receive cliente");
        mq_close(q_servidor);
        mq_close(q_cliente);
        mq_unlink(queuename);
        res.status = -1;
        return res.status;
    }
    strcpy(value1, res.valor1); 
    *N_value = res.N;
   
    for (int i = 0; i< res.N; i++){
        V_value2[i] = res.valor2[i];
    }
    if(mq_close(q_servidor) == -1){
        perror("get_value: mq_close servidor");
        res.status = -1;
        return res.status;
    }
	if(mq_close(q_cliente)==-1){
        perror("get_value: mq_close cliente");
        res.status = -1;
        return res.status;
    }
    if(mq_unlink(queuename)==-1){
        perror("get_value: mq_unlink");
        res.status = -1;
        return res.status;
    }
    return res.status;
}


int modify_value(int key, char *value1, int N_value2, double *V_value2){
    struct respuesta res;
    if(N_value2>32){
        perror("Vector demasiado grande");
        res.status = -1;
        return res.status;

    }
    if(N_value2<0){
        perror("Argumento N negativo");
        res.status = -1;
        return res.status;

    }

	mqd_t q_servidor;       
	mqd_t q_cliente;
    char queuename[MAXSIZE];
	struct peticion pet;
	struct mq_attr attr;

	attr.mq_maxmsg = 1;     attr.mq_msgsize = sizeof(struct respuesta);
    sprintf(queuename,  "/Cola-%d", getpid());

	q_cliente = mq_open(queuename, O_CREAT|O_RDONLY, 0700, &attr);
	if (q_cliente == -1) {
		perror("modify: mq_open cliente");
        res.status = -1;
        return res.status;
	}

    q_servidor = mq_open("/100472037", O_WRONLY);
	if (q_servidor == -1){
		mq_close(q_cliente);
		perror("modify: mq_open servidor");
        res.status = -1;
        return res.status;
	}

	pet.op =3;
	pet.clave = key;
    strcpy(pet.valor1, value1);
    pet.N = N_value2;

    for (int i = 0; i< N_value2; i++){
        pet.valor2[i] = V_value2[i];
    }
	strcpy(pet.q_name, queuename);

	if (mq_send(q_servidor, (const char *)&pet, sizeof(struct peticion), 0) < 0) {
		perror("modify: mq_send 3 servidor");
		mq_close(q_servidor);
		mq_close(q_cliente);
        mq_unlink(queuename);
        res.status = -1;
        return res.status;
	}
	if (mq_receive(q_cliente, (char *)&res, sizeof(struct respuesta), 0) < 0) {
		perror("modify: mq_receive cliente");
		mq_close(q_servidor);
		mq_close(q_cliente);
        mq_unlink(queuename);
        res.status = -1;
		return res.status;
	}
	if(mq_close(q_servidor) == -1){
        perror("modify: mq_close servidor");
        res.status = -1;
        return res.status;
    }
	if(mq_close(q_cliente)==-1){
        perror("modify: mq_close cliente");
        res.status = -1;
        return res.status;
    }
    if(mq_unlink(queuename)==-1){
        perror("modify: mq_unlink");
        res.status = -1;
        return res.status;
    }
	return res.status;
}

int delete_key(int key){
    struct respuesta res;
    mqd_t q_servidor;
    mqd_t q_cliente;
    char queuename[MAXSIZE];
    struct peticion pet;
    struct mq_attr attr;

    attr.mq_maxmsg = 1;     attr.mq_msgsize = sizeof(struct respuesta);
    sprintf(queuename,  "/Cola-%d", getpid());

    q_cliente = mq_open(queuename, O_CREAT|O_RDONLY, 0700, &attr);
    if (q_cliente == -1) {
        perror("delete: mq_open cliente");
        res.status = -1;
        return res.status;
    }

    q_servidor = mq_open("/100472037", O_WRONLY);
    if (q_servidor == -1){
        mq_close(q_cliente);
        perror("delete: mq_open servidor");
        res.status = -1;
        return res.status;
    }
    pet.op = 4;
    pet.clave = key;

    strcpy(pet.q_name, queuename);

    if (mq_send(q_servidor, (const char *)&pet, sizeof(struct peticion), 0) < 0) {
        perror("delete: mq_send 3 servidor");
        mq_close(q_servidor);
        mq_close(q_cliente);
        mq_unlink(queuename);
        res.status = -1;
        return res.status;
    }
    if (mq_receive(q_cliente, (char *)&res, sizeof(struct respuesta), 0) < 0) {
        perror("delete: mq_receive cliente");
        mq_close(q_servidor);
        mq_close(q_cliente);
        mq_unlink(queuename);
        res.status = -1;
        return res.status;
    }

    if(mq_close(q_servidor) == -1){
        perror("delete: mq_close servidor");
        res.status = -1;
        return res.status;
    }
	if(mq_close(q_cliente)==-1){
        perror("delete: mq_close cliente");
        res.status = -1;
        return res.status;
    }
    if(mq_unlink(queuename)==-1){
        perror("delete: mq_unlink");
        res.status = -1;
        return res.status;
    }
    return res.status;
}

int exist(int key){
    struct respuesta res;
    mqd_t q_servidor;
    mqd_t q_cliente;
    char queuename[MAXSIZE];
    struct peticion pet;
    struct mq_attr attr;

    attr.mq_maxmsg = 1;     attr.mq_msgsize = sizeof(struct respuesta);
    sprintf(queuename,  "/Cola-%d", getpid());

    q_cliente = mq_open(queuename, O_CREAT|O_RDONLY, 0700, &attr);
    if (q_cliente == -1) {
        perror("exist: mq_open cliente");
        res.status = -1;
        return res.status;
    }

    q_servidor = mq_open("/100472037", O_WRONLY);
    if (q_servidor == -1){
        mq_close(q_cliente);
        perror("exist: mq_open servidor");
        res.status = -1;
        return res.status;
    }
    pet.op = 5;
    pet.clave = key;

    strcpy(pet.q_name, queuename);

    if (mq_send(q_servidor, (const char *)&pet, sizeof(struct peticion), 0) < 0) {
        perror("exist: mq_send 3 servidor");
        mq_close(q_servidor);
        mq_close(q_cliente);
        mq_unlink(queuename);
        res.status = -1;
        return res.status;
    }
    if (mq_receive(q_cliente, (char *)&res, sizeof(struct respuesta), 0) < 0) {
        perror("exist: mq_receive cliente");
        mq_close(q_servidor);
        mq_close(q_cliente);
        mq_unlink(queuename);
        res.status = -1;
        return res.status;
    }

    if(mq_close(q_servidor) == -1){
        perror("exist: mq_close servidor");
        res.status = -1;
        return res.status;
    }
	if(mq_close(q_cliente)==-1){
        perror("exist: mq_close cliente");
        res.status = -1;
        return res.status;
    }
    if(mq_unlink(queuename)==-1){
        perror("exist: mq_unlink");
        res.status = -1;
        return res.status;
    }
    return res.status;
}*/