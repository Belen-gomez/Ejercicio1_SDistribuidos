#include "claves.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MAX_SIZE 32
#define MAXSIZE_STRING 256

int main(){
    //Primer cliente. EL primer cliente debe hacer la función init.
    if(init()==-1){
        printf("---------------Cliente 1 error init-----------------\n");
        printf("Error en init\n");
    }
    else{
        printf("---------------Cliente 1 init-----------------\n");
        printf("Lista inicializada\n");
    }

    //El cliente 1 inserta el elemento <1, "Hola mundo", [1.5, 2.2]>
    char cadena[MAXSIZE_STRING] = "Hola mundo";
    int N_value2 = 2;
    double vector[] = {1.5, 2.2};
    if(set_value(1, cadena, N_value2, vector) == -1){
        printf("---------------Cliente 1 error set_value-----------------\n");
        printf("Error en set_value en cliente 1\n");
    }
    else{
        printf("---------------Cliente 1 set_value-----------------\n");
        printf("Clave 1\n");
        printf("Cadena: Hola mundo\n");
        printf("Vector: [1.5, 2.2]\n");
        printf("Elemento insertado\n");
    }

    //El cliente 1 inserta el elemento <2, "Adios mundo", [3.5, 2.2, 6.3]>
    char cadena2[MAXSIZE_STRING] = "Hola mundo 2";
    int N_value3 = 3;
    double vector2[] = {3.5, 2.2, 6.3};
    if(set_value(2, cadena2, N_value3, vector2) == -1){
        printf("---------------Cliente 1 error set_value-----------------\n");
        printf("Error en set_value  en cliente 1\n");
    }
    else{
        printf("---------------Cliente 1 set_value-----------------\n");
        printf("Clave 2\n");
        printf("Cadena: Hola mundo 2\n");
        printf("Vector: [3.5, 2.2, 6.3]\n");
        printf("Elemento insertado \n");
    }
    
    
    //El cliente 1 obtiene los valores asociados a la clave 1
    char *gvalue= (char *) malloc(sizeof(char)*MAXSIZE_STRING);
    int N3;
    double *vector3 = (double *) malloc(sizeof(double)*MAX_SIZE);
    if(get_value(2, gvalue, &N3, vector3)){
        printf("---------------Cliente 1 error get_value-----------------\n");
        printf("Error en get_value en cliente 1 \n");
    }
    else{
        printf("---------------Cliente 1 get_value-----------------\n");
        printf("Tupla de clave %d\n", 2);
        printf("Cadena de caracteres: %s\n", gvalue);
        printf("Tamaño del vector: %d\n", N3);
        printf("vector: [");
        for(int i=0; i<N3; i++){
            printf("%f, ", vector3[i]);
        }    
        printf("]\n");    
    }
    
    //El cliente 1 modifica los valores asociados a la clave 1
    char cadena4[MAXSIZE_STRING] = "Adios mundo";
    int N_value4 = 4;
    double vector4[] = {4.5, 7.4, 8.9, 17.8};
    if(modify_value(1, cadena4, N_value4, vector4)==-1){
        printf("---------------Cliente 1 error modify_value-----------------\n");
        printf("Error en modify_value en cliente 1\n");
    }
    else{
        printf("---------------Cliente 1 modify_value-----------------\n");
        printf("Clave 1\n");
        printf("Cadena: Adios mundo\n");
        printf("Vector: [4.5, 7.4, 8.9, 17.8]\n");
        printf("Elemento modificado\n");
    }

    //El cliente 1 verifica si existe la clave 1
    int existe = exist(1);
    if(existe==1){
        printf("---------------Cliente 1 exist-----------------\n");
        printf("Existe la clave 1\n");
    }
    else if (existe==0){
        printf("---------------Cliente 1 exist-----------------\n");
        printf("No existe la clave 1\n");
    }
    else{
        printf("---------------Cliente 1 error exist-----------------\n");
        printf("Error en exist en cliente 1\n");
    }

    //El cliente 1 elimina los valores asociados a la clave 1
    if(delete_key(1) == -1){
        printf("---------------Cliente 1 error delete-----------------\n");
        printf("Error en delete_key en clave 1 en cliente 1\n");
    }
    else{
        printf("---------------Cliente 1 delete-----------------\n");
        printf("Elemento de clave 1 eliminado \n");
    }

    //El cliente 1 verifica si existe la clave 1. Como se ha eliminado, la función debe devolver 0
    int existe2 = exist(1);
    if(existe2==1){
        printf("---------------Cliente 1 exist-----------------\n");
        printf("Existe la clave 1 cliente 1\n");
    }
    else if (existe2==0){
        printf("---------------Cliente 1 exist-----------------\n");
        printf("No existe la clave 1 cliente 1\n");
    }
    else{
        printf("---------------Cliente 1 error exist-----------------\n");
        printf("Error en exist cliente 1\n");
    }


    
    return 0;
}