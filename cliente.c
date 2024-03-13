#include "claves.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(){
    init();


    char cadena[256] = "Hola mundo";

    // Supongamos que tienes un vector de dobles con 3 elementos
    int N_value2 = 2;
    double vector[] = {1.5, 2.2};
    if(set_value(1, cadena, N_value2, vector) == 0){
        //printf("hola2");
    }
    int N_value3 = 3;
    double vector2[] = {3.5, 2.2, 6.3};
    if(set_value(2, cadena, N_value3, vector2) == 0){
        //printf("hola2");
    }

    char gvalue[256];
    int N3;
    double vector3[32];
    get_value(1, gvalue, &N3, vector3);

    char cadena4[256] = "Adios mundo";

    // Supongamos que tienes un vector de dobles con 3 elementos
    int N_value4 = 1;
    double vector4[] = {4.5};
    
    modify_value(1, cadena4, N_value4, vector4);
    return 0;
}