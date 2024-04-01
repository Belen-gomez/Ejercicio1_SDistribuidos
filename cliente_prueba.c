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
    return 0;
}