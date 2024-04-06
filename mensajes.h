#include <stddef.h>
#include <unistd.h>
#define MAXSIZE	256
#define MAX_SIZE 32
#define INIT 0
#define SET 1
#define GET 2
#define MODIFY 3
#define DELETE 4
#define EXIST 5

char * obtenerIP();
char * obtenerPuerto();
int obtenerVariablesEntorno();
int sendMessage(int socket, char * buffer, int len);
int recvMessage(int socket, char *buffer, int len);



struct respuesta{
    int status;
    int clave;
    char valor1[256];
    int N;
    double valor2[32];     
};