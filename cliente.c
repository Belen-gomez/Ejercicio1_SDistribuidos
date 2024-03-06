#include "claves.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(){
    printf("hola");
    if (init() ==0){
        printf("hola");
    }
    char *cadena = "Hola mundo";

    // Supongamos que tienes un vector de dobles con 3 elementos
    int N_value2 = 1;
    double vector[] = {1.5};
    set_value(1, cadena,  N_value2, vector);
    return 0;
}