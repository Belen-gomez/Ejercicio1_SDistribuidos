#include "claves.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(){
    //Primer cliente. EL primer cliente debe hacer la función init.

    if(init()==-1){
        printf("Error en init\n");
    }
    

    //El cliente 1 inserta el elemento <1, "Hola mundo", [1.5, 2.2]>
    char cadena[256] = "Hola mundo";
    int N_value2 = 2;
    double vector[] = {1.5, 2.2};
    if(set_value(1, cadena, N_value2, vector) == -1){
        printf("Error en set_value\n");
    }

    //El cliente 1 inserta el elemento <2, "Adios mundo", [3.5, 2.2, 6.3]>
    char cadena2[256] = "Hola mundo 2";
    int N_value3 = 3;
    double vector2[] = {3.5, 2.2, 6.3};
    if(set_value(2, cadena2, N_value3, vector2) == -1){
        printf("Error en set_value\n");
    }
    //El cliente 1 intenta insertar un elemento con clave 1, que ya existe. La función debe devolver -1
    if(set_value(1, cadena2, N_value3, vector2) == -1){
        printf("Error en set_value\n");
    }
    
    //El cliente 1 obtiene los valores asociados a la clave 1
    char gvalue[256];
    int N3;
    double vector3[32];
    if(get_value(2, gvalue, &N3, vector3)){
        printf("Error en get_value\n");
    }
    else{
        printf("Tupla de clave %d\n", 2);
        printf("Cadena de caracteres: %s\n", gvalue);
        printf("Tamaño del vector: %d\n", N3);
        printf("vector: ");
        for(int i=0; i<N3; i++){
            printf("%f\n", vector3[i]);
        }        
    }
    //El cliente 1 intenta obtener los valores asociados a la clave 4, que no existe
    char gvalue5[256];
    int N5;
    double vector5[32];
    if(get_value(4, gvalue5, &N5, vector5)){
        printf("Error en get_value\n");
    }
    else{
        printf("cadena de caracteres: %s\n", gvalue);
        printf("tamaño del vector: %d\n", N3);
        printf("vector: ");
        for(int i=0; i<N3; i++){
            printf("%f\n", vector3[i]);
        }        
    }
    //El cliente 1 modifica los valores asociados a la clave 1
    char cadena4[256] = "Adios mundo";
    int N_value4 = 4;
    double vector4[] = {4.5, 7.4, 8.9, 17.8};
    
    if(modify_value(1, cadena4, N_value4, vector4)==-1){
        printf("Error en modify_value\n");
    }

    //El cliente 1 elimina los valores asociados a la clave 1
    if(delete_key(2) == -1){
        printf("Error en delete_key\n");
    }
    //El cliente 1 intenta eliminar los valores asociados a la clave 10, que no existe
    if(delete_key(10) == -1){
        printf("Error en delete_key\n");
    }
    //El cliente 1 verifica si existe la clave 1
    if(exist(1)==1){
        printf("Existe la clave 1\n");
    }
    else if (exist(1)==0){
        printf("No existe la clave 1\n");
    }
    else{
        printf("Error en exist\n");
    }
    

    //El cliente 1 verifica si existe la clave 10
    if(exist(10)==1){
        printf("Existe la clave 1\n");
    }
    else if (exist(10)==0){
        printf("No existe la clave 1\n");
    }
    else{
        printf("Error en exist\n");
    }
    return 0;
}