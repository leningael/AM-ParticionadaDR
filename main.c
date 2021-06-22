#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct particion{
    int num;
    int tam;
    int loc;
    char *estado;
    char *proceso;
}part;

typedef struct libre{
    int num;
    int tam;
    int loc;
    char *estado;
}libre;

void asignacionRAM(char *proceso, int tamanio, libre** TAL, int *numAL, part** TP, int *nPart,int modalidad);
int solicitaRAM(int tamanio, libre** TAL, int numAL);
void recuperaRAM(char *proceso, libre** TAL, int *numAL, part** TP, int nPart);
void compactacion(libre** TAL, int numAL);
int reubicacion(libre** TAL, int *numAL, part** TP, int *nPart, int modalidad);
void determinarOrden(int tipo, libre** TAL, int numAL);
void crearAL(int *numAL, int tamAL, int locAL, libre** TAL);
void imprimirTAL(libre** TAL, int numAL);
void crearPart(int *nPart, int tamP, int locP, char *proceso,part** TP);
void imprimirTP(part** TP, int nPart);
int sumaTAL(libre** TAL, int numAL);
int primerLocAL(libre** TAL, int numAL);
int main() {
    int tamMemoria, tamSO, nPart = 0, numAL = 0,op, tamanioP, modalidad;
    char buffer[20];
    char *nombreP;
    printf("Ingrese el tamanio total de la memoria\n");
    scanf("%d", &tamMemoria);
    printf("Ingrese el tamanio ocupado por el SO\n");
    scanf("%d", &tamSO);
    printf("Ingrese la modalidad de la AM:\n1.Mejor 2.Peor 3.Primer Ajuste\n");
    scanf("%d", &modalidad);
    part* TP[tamMemoria-tamSO];
    libre* TAL[tamMemoria-tamSO];
    crearAL(&numAL, tamMemoria-tamSO, tamSO, TAL);
    printf("1. LLega 2. Termina 3. Salir\n");
    scanf("%d", &op);
    while(op<3){
        printf("Introduce el proceso:\n");
        fflush(stdin);
        gets(buffer);
        nombreP = strdup(buffer);
        switch(op){
            case 1:
                printf("Introduce el tamanio del proceso:\n");
                scanf("%d", &tamanioP);
                printf("LLega %s con tamanio %d\n", nombreP, tamanioP);
                asignacionRAM(nombreP, tamanioP, TAL, &numAL, TP, &nPart, modalidad);
                break;
            case 2:
                printf("Termina %s\n", nombreP);
                recuperaRAM(nombreP, TAL, &numAL, TP, nPart);
                break;
            default:
                break;
        }
        determinarOrden(modalidad, TAL, numAL);
        imprimirTAL(TAL, numAL);
        imprimirTP(TP, nPart);
        printf("1. LLega 2. Termina 3. Salir\n");
        scanf("%d", &op);
    }
    return 0;
}

void asignacionRAM(char *proceso, int tamanio, libre** TAL, int *numAL, part** TP, int *nPart, int modalidad){
    int pos = solicitaRAM(tamanio, TAL, *numAL);
    if(pos > 0){
        crearPart(nPart, tamanio, TAL[pos]->loc, proceso,TP);
        TAL[pos]->tam = TAL[pos]->tam - tamanio;
        TAL[pos]->loc = TAL[pos]->loc + tamanio;
    }else{
        if(sumaTAL(TAL, *numAL)>=tamanio){
            int nuevoTam;
            do{
                nuevoTam = reubicacion(TAL, numAL, TP, nPart, modalidad);
                determinarOrden(modalidad, TAL, *numAL);
            }while(nuevoTam<tamanio);
            asignacionRAM(proceso, tamanio, TAL, numAL,TP, nPart, modalidad);
        }else
            printf("No hay espacio disponible.\n");
    }
}

int solicitaRAM(int tamanio, libre** TAL, int numAL){
    int i = 1;
    int pos = 0;
    int enc = 0;
    while(i<=numAL&&enc==0){
        if(!strcmp(TAL[i]->estado, "DISPONIBLE")&&TAL[i]->tam>=tamanio){
            pos = i;
            enc = 1;
        }
        i++;
    }
    return pos;
}

void recuperaRAM(char *proceso, libre** TAL, int *numAL, part** TP, int nPart){
    int i = 1;
    int enc = 0;
    while (i<=nPart&&enc==0){
        if(!strcmp(TP[i]->proceso, proceso)){
            TP[i]->estado="VACIA";
            crearAL(numAL, TP[i]->tam, TP[i]->loc, TAL);
            compactacion(TAL, *numAL);
            //determinarOrden(modalidad, TAL, numAL);
            enc = 1;
        }else{
            i++;
        }
    }
}

void compactacion(libre** TAL, int numAL){
    int flag = 0;
    //Compactacion hacia abajo
    for(int i = 1; i<numAL&&flag==0;i++){
        if(!strcmp(TAL[i]->estado, "DISPONIBLE")&&(TAL[numAL]->loc+TAL[numAL]->tam)==TAL[i]->loc){
            TAL[numAL]->tam = TAL[numAL]->tam + TAL[i]->tam;
            TAL[i]->estado = "VACIO";
            flag = 1;
        }
    }
    //Compactacion hacia arriba
    flag = 0;
    for(int i = 1; i<numAL&&flag==0;i++){
        if(!strcmp(TAL[i]->estado, "DISPONIBLE")&&(TAL[i]->loc+TAL[i]->tam)==TAL[numAL]->loc){
            TAL[i]->tam = TAL[i]->tam + TAL[numAL]->tam;
            TAL[numAL]->estado = "VACIO";
            flag = 1;
        }
    }
}

int reubicacion(libre** TAL, int *numAL, part** TP, int *nPart, int modalidad){
    int enc = 0;
    int primerAL = primerLocAL(TAL, *numAL);
    for(int i = 1; i<=*nPart && enc == 0; i++){
        if(TP[i]->loc==(TAL[primerAL]->loc+TAL[primerAL]->tam)){
            recuperaRAM(TP[i]->proceso, TAL, numAL, TP, *nPart);
            asignacionRAM(TP[i]->proceso, TP[i]->tam, TAL, numAL,TP, nPart, modalidad);
            enc = 1;
        }
    }
    return TAL[primerAL]->tam;
}

void determinarOrden(int tipo, libre** TAL, int numAL){
    libre* auxLibre;
    switch (tipo) {
        case 1://Mejor
            for(int i = 1; i <= numAL; i++){
                for(int j = i; j<=numAL; j++){
                    if(TAL[i]->tam>TAL[j]->tam){
                        auxLibre = TAL[j];
                        TAL[j] = TAL[i];
                        TAL[i] = auxLibre;
                    }
                }
            }
            break;
        case 2://Peor
            for(int i = 1; i <= numAL; i++){
                for(int j = i; j<=numAL; j++){
                    if(TAL[i]->tam<TAL[j]->tam){
                        auxLibre = TAL[j];
                        TAL[j] = TAL[i];
                        TAL[i] = auxLibre;
                    }
                }
            }
            break;
        case 3://Primer
            for(int i = 1; i <= numAL; i++){
                for(int j = i; j<=numAL; j++){
                    if(TAL[i]->loc>TAL[j]->loc){
                        auxLibre = TAL[j];
                        TAL[j] = TAL[i];
                        TAL[i] = auxLibre;
                    }
                }
            }
            break;
        default:
            break;
    }
}

void crearAL(int *numAL, int tamAL, int locAL, libre** TAL){
    *numAL = *numAL+1;
    libre* nuevaAL = malloc(sizeof (libre));
    nuevaAL->num = *numAL;
    nuevaAL->tam = tamAL;
    nuevaAL->loc = locAL;
    nuevaAL->estado="DISPONIBLE";
    TAL[*numAL] = nuevaAL;
}

void imprimirTAL(libre** TAL, int numAL){
    printf("Tabla de Areas Libres\n");
    printf("Numero:%6sTamanio:%6sLocalidad:%6sEstado:%6s\n"," "," "," "," ");
    for(int i = 1;i <= numAL; i++){
        if(!strcmp(TAL[i]->estado,"VACIO")&&TAL[i]->tam > 0)
        printf("%3d%8s%7d%11s%4d%11s%-10s\n",TAL[i]->num," " ,TAL[i]->tam," ",TAL[i]->loc," ",TAL[i]->estado);
    }
    for(int i = 1;i <= numAL; i++){
        if(!strcmp(TAL[i]->estado,"DISPONIBLE")&&TAL[i]->tam > 0)
        printf("%3d%8s%7d%11s%4d%8s%-10s\n",TAL[i]->num," " ,TAL[i]->tam," ",TAL[i]->loc," ",TAL[i]->estado);
    }
}

void crearPart(int *nPart, int tamP, int locP, char *proceso, part** TP){
    *nPart = *nPart + 1;
    part* nuevaPart = malloc(sizeof (part));
    nuevaPart->num = *nPart;
    nuevaPart->tam = tamP;
    nuevaPart->loc = locP;
    nuevaPart->estado="ASIGNADA";
    nuevaPart->proceso = proceso;
    TP[*nPart] = nuevaPart;
}

void imprimirTP(part** TP, int nPart){
    printf("Tabla de Particiones\n");
    printf("Numero:%6sTamanio:%6sLocalidad:%6sEstado:%6sProceso:\n", " ", " ", " ", " ");
    for(int i = 1;i <= nPart; i++){
        if(!strcmp(TP[i]->estado,"VACIA"))
            printf("%3d%8s%7d%11s%4d%11s%-10s%2s%-5s\n",TP[i]->num," " ,TP[i]->tam," ",TP[i]->loc," ",TP[i]->estado," ",TP[i]->proceso);
    }
    for(int i = 1;i <= nPart; i++){
        if(!strcmp(TP[i]->estado,"ASIGNADA"))
        printf("%3d%8s%7d%11s%4d%10s%-10s%3s%-5s\n",TP[i]->num," " ,TP[i]->tam," ",TP[i]->loc," ",TP[i]->estado," ",TP[i]->proceso);
    }
}

int sumaTAL(libre** TAL, int numAL){
    int suma = 0;
    for(int i = 1; i <= numAL; i++){
        if(!strcmp(TAL[i]->estado, "DISPONIBLE")){
            suma = suma + TAL[i]->tam;
        }
    }
    return suma;
}

int primerLocAL(libre** TAL, int numAL){
    int menor = 0;
    for(int i = 1; i<=numAL&&menor==0;i++){
        if(!strcmp(TAL[i]->estado,"DISPONIBLE")){
            menor = i;
        }
    }
    for(int i = 1; i<=numAL;i++){
        if(!strcmp(TAL[i]->estado,"DISPONIBLE") && TAL[i]->loc<TAL[menor]->loc){
            menor = i;
        }
    }
    return menor;
}