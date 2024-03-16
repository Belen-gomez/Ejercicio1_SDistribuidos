#include "claves.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MAX_SIZE 32
#define MAXSIZE_STRING 256

int main(){
    
    //El cliente 3 inserta el elemento 3
    char cadena[MAXSIZE_STRING] = "Elemento 3";
    int N_value2 = 2;
    double vector[] = {1.5, 2.2};
    if(set_value(3, cadena, N_value2, vector) == -1){
        printf("Error en set_value cliente 3\n");
    }
    else{
        printf("Elemento insertado cliente 3\n");
    }

    //El cliente 3 intenta insertar el elemento 1. Como ya lo ha insertado el cliente 1, la función debe devolver -1
    char cadena2[MAXSIZE_STRING] = "Hola mundo 2";
    int N_value3 = 3;
    double vector2[] = {3.5, 2.2, 6.3};
    if(set_value(1, cadena2, N_value3, vector2) == -1){
        printf("Error en set_value cliente 3\n");
    }
    else{
        printf("Elemento insertado cliente 3\n");
    }
    

    //El cliente 3 obtiene los valores asociados a la clave 2, que ha insertado el cliente 1
    char *gvalue= (char *) malloc(sizeof(char)*MAXSIZE_STRING);
    int N3;
    double *vector3 = (double *) malloc(sizeof(double)*MAX_SIZE);
    if(get_value(1, gvalue, &N3, vector3)){
        printf("Error en get_value cliente 3\n");
    }
    else{
        printf("cliente 3\n");
        printf("Tupla de clave %d\n", 2);
        printf("Cadena de caracteres: %s\n", gvalue);
        printf("Tamaño del vector: %d\n", N3);
        printf("vector: ");
        for(int i=0; i<N3; i++){
            printf("%f\n", vector3[i]);
        }        
    }
    
    //El cliente 3 modifica los valores asociados a la clave 2, que ha insertado el cliente 1
    char cadena4[MAXSIZE_STRING] = "Adios mundo";
    int N_value4 = 4;
    double vector4[] = {4.5, 7.4, 8.9, 17.8};
    
    if(modify_value(2, cadena4, N_value4, vector4)==-1){
        printf("Error en modify_value cliente 3\n");
    }
    else{
        printf("Elemento modificado cliente 3\n");
    }


    //El cliente 1 verifica si existe la clave 1
   int existe = exist(1);
    if(existe==1){
        printf("Existe la clave 1 cliente 3\n");
    }
    else if (existe==0){
        printf("No existe la clave 1 cliente 3\n");
    }
    else{
        printf("Error en exist cliente 3\n");
    }



    //El cliente 1 elimina los valores asociados a la clave 1
    if(delete_key(1) == -1){
        printf("Error en delete_key cliente 3\n");
    }
    else{
        printf("Elemento eliminado cliente 3\n");
    }

    //El cliente 1 verifica si existe la clave 1. Como se ha eliminado, la función debe devolver 0
    int existe2 = exist(1);
    if(existe2==1){
        printf("Existe la clave 1 cliente 3\n");
    }
    else if (existe2==0){
        printf("No existe la clave 1 cliente 3\n");
    }
    else{
        printf("Error en exist cliente 3\n");
    }

    
    return 0;
}