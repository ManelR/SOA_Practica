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
    char nomFitxer[10];
    char extFitxer[5];
    DadesFAT dadesFat;
    DadesEXT2 dadesExt;
    
    
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
            nTipus = Comprovacio_TipusFitxer(argv[2]);
            if(nTipus){
                if (nTipus == TIPUS_FAT) {
                    if(Comprovacio_OmpleFitxerFAT(&dadesFat, argv[2])){
                        //Mètode buscar fitxer a FAT16
                        Buscar_PrepararNomFitxerFAT(argv[3], nomFitxer, extFitxer);
                        Buscar_BuscarFitxerFat(argv[2], nomFitxer, dadesFat, extFitxer);
                    }else{
                        write(1, "File System not recognized\n", strlen("File System not recognized\n"));
                    }
                }else if (nTipus == TIPUS_EXT2){
                    if (Comprovacio_OmpleFitxerEXT(&dadesExt, argv[2])) {
                        //Mètode buscar fitxer a EXT2
                        Buscar_BuscarFitxerExt(argv[2], argv[3], dadesExt);
                    }
                }else{
                    write(1, "Error. The volumen is neither FAT16 or EXT2.\n", strlen("Error. The volumen is neither FAT16 or EXT2.\n"));
                }
            }
            break;
        default:
            break;
    }
    
    return 0;
}
