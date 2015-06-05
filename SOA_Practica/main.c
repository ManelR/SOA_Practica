//
//  main.c
//  SOA_Practica
//
//  Created by MRR on 13/04/15.
//  Copyright (c) 2015 MRR. All rights reserved.
//

#include "comprovacio.h"
#include "mostrarPantalla.h"
#include "buscarFitxer.h"

int main(int argc, char * argv[]) {
    int nOpcio;
    int nTipus;
    char * contingutFitxer = NULL;
    char nomFitxer[10];
    unsigned int numNode = 0;
    unsigned int fitxerSize = 0;
    char textAux[100];
    char extFitxer[5];
    DadesFAT dadesFat;
    DadesEXT2 dadesExt;
    int nComptadorNom = 0;
    
    
    nOpcio = Comprovacio_ParametresEntrada(argc, argv);
    
    switch (nOpcio) {
        case OPERACIO_INFO:
            nTipus = Comprovacio_TipusFitxer(argv[2]);
            if(nTipus){
                if (nTipus == TIPUS_FAT) {
                    if(Comprovacio_OmpleFitxerFAT(&dadesFat, argv[2])){
                        Mostrar_mostrarInfoFAT(dadesFat);
                    }else{
                        write(1, "File System not recognized\n", strlen("File System not recognized\n"));
                    }
                }else if (nTipus == TIPUS_EXT2){
                    if (Comprovacio_OmpleFitxerEXT(&dadesExt, argv[2])) {
                        Mostrar_mostrarInfoEXT(dadesExt);
                    }
                }else{
                    write(1, "File System not recognized\n", strlen("File System not recognized\n"));
                }
            }
            break;
        case OPERACIO_FIND:
            nComptadorNom = 1;
            nTipus = Comprovacio_TipusFitxer(argv[2]);
            if(nTipus){
                if (nTipus == TIPUS_FAT) {
                    if(Comprovacio_OmpleFitxerFAT(&dadesFat, argv[2])){
                        //Mètode buscar fitxer a FAT16
                        Buscar_PrepararNomFitxerFAT(argv[3], nomFitxer, extFitxer, nComptadorNom);
                        if(!Buscar_BuscarFitxerFat(argv[2], nomFitxer, dadesFat, extFitxer)){
                            write(1, "Error. File not found.\n", strlen("Error. File not found.\n"));
                        }
                    }else{
                        write(1, "File System not recognized\n", strlen("File System not recognized\n"));
                    }
                }else if (nTipus == TIPUS_EXT2){
                    if (Comprovacio_OmpleFitxerEXT(&dadesExt, argv[2])) {
                        //Mètode buscar fitxer a EXT2
                        numNode = Buscar_BuscarFitxerExt(argv[2], argv[3], dadesExt);
                        if (numNode != 0) {
                            Buscar_recuperarMidaFitxer(argv[2], dadesExt, numNode, &fitxerSize);
                            bzero(textAux, sizeof(textAux));
                            sprintf(textAux, "File found! Size: %u bytes.\n", fitxerSize);
                            write(1, textAux , strlen(textAux));
                        }
                    }
                }else{
                    write(1, "Error. The volumen is neither FAT16 or EXT2.\n", strlen("Error. The volumen is neither FAT16 or EXT2.\n"));
                }
            }
            break;
        case OPERACIO_CAT:
            nComptadorNom = 1;
            nTipus = Comprovacio_TipusFitxer(argv[2]);
            if(nTipus){
                if (nTipus == TIPUS_FAT) {
                    if(Comprovacio_OmpleFitxerFAT(&dadesFat, argv[2])){
                        //Mètode buscar fitxer a FAT16
                        Buscar_PrepararNomFitxerFAT(argv[3], nomFitxer, extFitxer, nComptadorNom);
                        Buscar_BuscarFitxerFat(argv[2], nomFitxer, dadesFat, extFitxer);
                    }else{
                        write(1, "File System not recognized\n", strlen("File System not recognized\n"));
                    }
                }else if (nTipus == TIPUS_EXT2){
                    if (Comprovacio_OmpleFitxerEXT(&dadesExt, argv[2])) {
                        //Mètode buscar fitxer a EXT2
                        numNode = Buscar_BuscarFitxerExt(argv[2], argv[3], dadesExt);
                        if (numNode != 0) {
                            Buscar_recuperarMidaFitxer(argv[2], dadesExt, numNode, &fitxerSize);
                            Buscar_recuperarContingutFitxer(argv[2], numNode, dadesExt, &contingutFitxer);
                            write(1, contingutFitxer, fitxerSize);
                        }
                    }
                }else{
                    write(1, "Error. The volumen is neither FAT16 or EXT2.\n", strlen("Error. The volumen is neither FAT16 or EXT2.\n"));
                }
            }

            break;
        case OPERACIO_COPY:
            nComptadorNom = 1;
            //Primer es comprova que el volum Ext sigui realment del format ext
            nTipus = Comprovacio_TipusFitxer(argv[2]);
            if (nTipus == TIPUS_EXT2) {
                if (Comprovacio_OmpleFitxerEXT(&dadesExt, argv[2])) {
                    //Comprovar el segon volum que sigui FAT16
                    nTipus = Comprovacio_TipusFitxer(argv[3]);
                    if (nTipus == TIPUS_FAT) {
                        if (Comprovacio_OmpleFitxerFAT(&dadesFat, argv[3])) {
                            //En aquest punt ja s'ha comprovat els dos tipus de volums i s'ha carregat la informació
                            //Ara toca buscar el fitxer que s'ha introduït
                            numNode = Buscar_BuscarFitxerExt(argv[2], argv[4], dadesExt);
                            if (numNode != 0) {
                                Buscar_recuperarMidaFitxer(argv[2], dadesExt, numNode, &fitxerSize);
                                Buscar_recuperarContingutFitxer(argv[2], numNode, dadesExt, &contingutFitxer);
                                //En primer lloc preparar el nom del fitxer
                                Buscar_PrepararNomFitxerFAT(argv[4], nomFitxer, extFitxer, nComptadorNom);
                                
                                if (!Buscar_BuscarFitxerFat(argv[3], nomFitxer, dadesFat, extFitxer)) {
                                    Buscar_CopiarFitxerAFAT(argv[3], dadesFat, contingutFitxer, fitxerSize, nomFitxer, extFitxer);
                                }else{
                                    write(1, "El fitxer ja existeix", strlen("El fitxer ja existeix"));
                                }
                            }
                        }else{
                            write(1, "Error. EL volum 2 no és FAT16\n", strlen("Error. EL volum 2 no és FAT16\n"));
                        }
                    }else{
                        write(1, "Error. EL volum 2 no és FAT16\n", strlen("Error. EL volum 2 no és FAT16\n"));
                    }
                }else{
                    write(1, "Error. EL volum 1 no és EXT2\n", strlen("Error. EL volum 1 no és EXT2\n"));
                }
            }else{
                write(1, "Error. EL volum 1 no és EXT2\n", strlen("Error. EL volum 1 no és EXT2\n"));
            }
            break;
        default:
            write(1, "Error amb els parametres d'entrada\n", strlen("Error amb els parametres d'entrada\n"));
            break;
    }
    
    return 0;
}
