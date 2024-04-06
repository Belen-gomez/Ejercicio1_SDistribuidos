
typedef struct Tupla {
    int clave;
    char *valor1; // Máximo de 255 caracteres + 1 para el carácter nulo de terminación de cadena
    int N;
    double *valor2;
    struct Tupla *siguiente;
} Tupla;

typedef struct Tupla * List;
int printList(List l);

int init(List *l);
int set_value(List *l, int clave, char *valor1, int N, double *valor2);
struct respuesta get_value(List *l, int clave);
int modify_value(List *l, int clave, char *valor1, int N, double *valor2);
int delete_key(List *l, int key);
int exist(List *l, int clave);
void atender_peticion(int * s);