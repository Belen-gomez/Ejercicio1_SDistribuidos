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
ssize_t readLine(int fd, void *buffer, size_t n);


struct peticion{
    int op;
    int clave;
    char valor1[256];
    int N;
    double valor2[32];      /* Esto hay que hacerlo con memoria dinamica */
    char q_name[MAXSIZE]; 	/* nombre de la cola cliente donde debe enviar la respuesta 	*/
};

struct respuesta{
    int status;
    int clave;
    char valor1[256];
    int N;
    double valor2[32];      /* Esto hay que hacerlo con memoria dinamica */
};