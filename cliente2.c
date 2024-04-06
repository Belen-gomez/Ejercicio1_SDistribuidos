#include "claves.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MAX_SIZE 32
#define MAXSIZE_STRING 256

int main(){
    //Primer cliente. EL primer cliente debe hacer la función init.
    if(init()==-1){
        printf("------------Error en init------------------\n");
    }
    else{
        printf("------------Lista inicializada-------------\n");
    }
    
    //El cliente 2 inserta el elemento <1, "Hola mundo", [1.5, 2.2]>
    char cadena[MAXSIZE_STRING] = "Hola mundo";
    int N_value2 = 2;
    double vector[] = {1.5, 2.2};
    if(set_value(1, cadena, N_value2, vector) == -1){
        printf("---------------Cliente 2 error set_value-----------------\n");
        printf("Error en set_value en cliente 2\n");
    }
    else{
        printf("---------------Cliente 2 set_value-----------------\n");
        printf("Clave 1\n");
        printf("Cadena: Hola mundo\n");
        printf("Vector: [1.5, 2.2]\n");
        printf("Elemento insertado\n");
    }

    //El cliente 2 intenta inserta un elemento con N menor que 0. La función debe devolver -1
    char cadena2[MAXSIZE_STRING] = "Hola mundo";
    int N2= -1;
    double vector2[] = {1.5, 2.2};
    if(set_value(2, cadena2, N2, vector2) == -1){
        printf("---------------Cliente 2 error set_value-----------------\n");
        printf("Error en set_value en cliente 2\n");
    }
    else{
        printf("---------------Cliente 2 set_value-----------------\n");
        printf("Clave 1\n");
        printf("Cadena: Hola mundo\n");
        printf("Vector: [1.5, 2.2]\n");
        printf("Elemento insertado\n");
    }

    //El cliente intenta inserta un elemento con N mayor que 32. La función debe devolver -1
    char cadena3[MAXSIZE_STRING] = "Hola mundo";
    int N3= 33;
    double vector3[] = {1.5, 2.2};
    if(set_value(2, cadena3, N3, vector3) == -1){
        printf("---------------Cliente 2 error set_value-----------------\n");
        printf("Error en set_value en cliente 2\n");
    }
    else{
        printf("---------------Cliente 2 set_value-----------------\n");
        printf("Clave 1\n");
        printf("Cadena: Hola mundo\n");
        printf("Vector: [1.5, 2.2]\n");
        printf("Elemento insertado\n");
    }

    //El cliente 2 intenta insertar un elemento con clave 1, que ya existe. La función debe devolver -1
    char cadena4[MAXSIZE_STRING] = "Hola mundo";
    int N4 = 2;
    double vector4[] = {1.5, 2.2};
    if(set_value(1, cadena4, N4, vector4) == -1){
        printf("---------------Cliente 2 error set_value-----------------\n");
        printf("Error en set_value en cliente 2\n");
    }
    else{
        printf("---------------Cliente 2 set_value-----------------\n");
        printf("Clave 1\n");
        printf("Cadena: Hola mundo\n");
        printf("Vector: [1.5, 2.2]\n");
        printf("Elemento insertado\n");
    }
  
    //El cliente 2 intenta obtener los valores asociados a la clave 4, que no existe
    char gvalue5[MAXSIZE_STRING];
    int N5;
    double vector5[32];
    if(get_value(4, gvalue5, &N5, vector5)){
        printf("---------------Cliente 2 error get_value-----------------\n");
        printf("Error en get_value en cliente 2 \n");
    }
    else{
        printf("cadena de caracteres: %s\n", gvalue5);
        printf("tamaño del vector: %d\n", N5);
        printf("vector: ");
        for(int i=0; i<N5; i++){
            printf("%f\n", vector5[i]);
        }        
    }

    //El cliente 2 intenta modificar los valores asociados a la clave 4, que no existe
    char cadena6[MAXSIZE_STRING] = "Adios mundo";
    int N_value6 = 4;
    double vector6[] = {4.5, 7.4, 8.9, 17.8};
    
    if(modify_value(4, cadena6, N_value6, vector6)==-1){
        printf("---------------Cliente 2 error modify_value-----------------\n");
        printf("Error en set_value en cliente 2\n");
    }
    else{
        printf("---------------Cliente 2 modify_value-----------------\n");
        printf("Clave 1\n");
        printf("Cadena: Adios mundo\n");
        printf("Vector: [4.5, 7.4, 8.9, 17.8]\n");
        printf("Elemento modificado\n");
    }

    //El cliente 2 intenta borrar un elemento con clave 4, que no existe
    if(delete_key(4) == -1){
        printf("---------------Cliente 2 error delete-----------------\n");
        printf("Error en delete_key en clave 4 en cliente 2\n");
    }
    else{
        printf("---------------Cliente 2 delete-----------------\n");
        printf("Elemento eliminado\n");
    }

}