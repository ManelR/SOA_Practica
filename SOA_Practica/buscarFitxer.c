//
//  buscarFitxer.c
//  SOA_Practica
//
//  Created by MRR on 30/04/15.
//  Copyright (c) 2015 MRR. All rights reserved.
//

#include "buscarFitxer.h"

void Buscar_PrepararNomFitxerFAT(char * sNomFitxerBuscar, char * nouNomFitxer){
    //El nom del fitxer té max 8 lletres sino -> ~1.
    char sAux[9];
    int nContador = 0;
    int i = 0;
    while (sNomFitxerBuscar[i] != '\0' && sNomFitxerBuscar[i] != '.') {
        if (nContador < 8) {
            sAux[i] = sNomFitxerBuscar[i];
        }
        i++;
        nContador++;
    }
    if (nContador > 8) {
        sAux[6] = '~';
        sAux[7] = '1';
        sAux[8] = '\0';
    }else{
        sAux[i] = '\0';
    }
    
    strcpy(nouNomFitxer, sAux);
    
}

void Buscar_BuscarFitxerFat(char * nomVolumFAT, char * nomFitxerBuscar){
    
}