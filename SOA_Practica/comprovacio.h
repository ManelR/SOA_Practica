//
//  comprovacio.h
//  SOA_Practica
//
//  Created by MRR on 13/04/15.
//  Copyright (c) 2015 MRR. All rights reserved.
//

#ifndef SOA_Practica_comprovacio_h
#define SOA_Practica_comprovacio_h

#include "tipus.h"

//Constants pel tipus de operació a realitzar
#define OPERACIO_ERROR 0
#define OPERACIO_INFO 1
#define OPERACIO_FIND 2

#define TIPUS_ERROR_FILE 0
#define TIPUS_ERROR_FILE_SYS 1
#define TIPUS_FAT 2
#define TIPUS_EXT2 3

#define OMPLE_ERROR 0
#define OMPLE_OK 1

//Funció que comprova que els parametres siguin correctes i retorna un valor enter que representa l'opció escollida.
int Comprovacio_ParametresEntrada(int argc, char * argv[]);

//Funció que comprova que el fitxer sigui del tipus correcte i retorna un valor que identifica si és FAT16, EXT2 o Error.
int Comprovacio_TipusFitxer(char * sNomFitxer);

//Funció que ompla les dades necessaries del format FAT16
int Comprovacio_OmpleFitxerFAT(DadesFAT* dadesFat, char * sNomFitxer);

//Funcio que ompla les dades necessaries del format EXT2
int Comprovacio_OmpleFitxerEXT(DadesEXT2* dadesExt, char * sNomFitxer);

#endif
