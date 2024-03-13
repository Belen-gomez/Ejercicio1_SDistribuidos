
typedef struct Tupla {
    int clave;
    char valor1[256]; // Máximo de 255 caracteres + 1 para el carácter nulo de terminación de cadena
    int N;
    double *valor2;
    struct Tupla *siguiente;
} Tupla;

typedef struct Tupla * List;