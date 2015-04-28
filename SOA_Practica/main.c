//
//  main.c
//  SOA_Practica
//
//  Created by MRR on 13/04/15.
//  Copyright (c) 2015 MRR. All rights reserved.
//

#include "comprovacio.h"
#include "mostrarPantalla.h"

int main(int argc, char * argv[]) {
    int nOpcio;
    int nTipus;
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
            
        default:
            break;
    }
    
    return 0;
}
