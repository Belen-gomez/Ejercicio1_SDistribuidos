#include "claves.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(){
    if (init() ==0){
        printf("hola");
    }

    char cadena[256] = "Hola mundo";

    // Supongamos que tienes un vector de dobles con 3 elementos
    int N_value2 = 2;
    double vector[] = {1.5, 3.2};
    if(set_value(1, cadena, N_value2, vector) == 0){
        printf("hola2");
    }
    return 0;
}