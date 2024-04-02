#include "claves.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MAX_SIZE 32
#define MAXSIZE_STRING 256

int main(){
    //Primer cliente. EL primer cliente debe hacer la función init.
    if(init()==-1){
        printf("Error en init\n");
    }
    else{
        printf("Lista inicializada\n");
    }
    //El cliente 1 inserta el elemento <1, "Hola mundo", [1.5, 2.2]>
    char cadena[MAXSIZE_STRING] = "Hola mundo";
    int N_value2 = 2;
    double vector[] = {1.5, 2.2};
    if(set_value(1, cadena, N_value2, vector) == -1){
        printf("Error en set_value\n");
    }
    else{
        printf("Elemento insertado\n");
    }
    //El cliente 1 obtiene los valores asociados a la clave 1
    char *gvalue= (char *) malloc(sizeof(char)*MAXSIZE_STRING);
    int N3;
    double *vector3 = (double *) malloc(sizeof(double)*MAX_SIZE);
    if(get_value(1, gvalue, &N3, vector3)){
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
    //El cliente 1 verifica si existe la clave 1
    int existe = exist(1);
    printf("Existe %d",existe);
    if(existe==1){
        printf("Existe la clave 1 cliente 3\n");
    }
    else if (existe==0){
        printf("No existe la clave 1 cliente 3\n");
    }
    else{
        printf("Error en exist cliente 3\n");
    }
    return 0;
}