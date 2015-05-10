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

//Funció que serveix per convertir el nom introduït, en el nom que surtirà en el volum FAT.
void Buscar_PrepararNomFitxerFAT(char * sNomFitxerBuscar, char * nouNomFitxer);

void Buscar_BuscarFitxerFat(char * nomVolumFAT, char * nomFitxerBuscar, DadesFAT dadesFat);

#endif /* defined(__SOA_Practica__buscarFitxer__) */
