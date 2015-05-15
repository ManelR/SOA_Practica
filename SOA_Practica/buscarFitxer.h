//
//  buscarFitxer.h
//  SOA_Practica
//
//  Created by MRR on 30/04/15.
//  Copyright (c) 2015 MRR. All rights reserved.
//

#ifndef __SOA_Practica__buscarFitxer__
#define __SOA_Practica__buscarFitxer__

#include <stdio.h>
#include "tipus.h"

#define BYTE_DIRECTORY 8

//Funció que serveix per convertir el nom introduït, en el nom que surtirà en el volum FAT.
void Buscar_PrepararNomFitxerFAT(char * sNomFitxerBuscar, char * nouNomFitxer,  char * extFitxerBuscar);

void Buscar_BuscarFitxerFat(char * nomVolumFAT, char * nomFitxerBuscar, DadesFAT dadesFat,  char * extFitxerBuscar);

unsigned int Buscar_BuscarFitxerExt(char * nomVolumEXT, char * nomFitxerBuscar, DadesEXT2 dadesExt);

void Buscar_mostrarMidaFitxer(char * nomVolumEXT, DadesEXT2 dadesExt, unsigned int numInode);

void Buscar_mostrarContingutFitxer(char * nomVolumEXT, unsigned int numInode, DadesEXT2 dadesExt);

#endif /* defined(__SOA_Practica__buscarFitxer__) */
