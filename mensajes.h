#define MAXSIZE	256
#define INIT 0
#define SET 1
#define GET 2
#define MODIFY 3
#define DELETE 4
#define EXIST 5

struct peticion{
    int op;
    int clave;
    char valor1[256];
    int N;
    double *valor2;      /* Esto hay que hacerlo con memoria dinamica */
    char q_name[MAXSIZE]; 	/* nombre de la cola cliente donde debe enviar la respuesta 	*/
};