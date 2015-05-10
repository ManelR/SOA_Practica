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
        if (nContador < 8 && sNomFitxerBuscar[i] != ' ') {
            sAux[i] = sNomFitxerBuscar[i];
        }
        if (sNomFitxerBuscar[i] == ' ') {
            nContador--;
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

void Buscar_BuscarFitxerFat(char * nomVolumFAT, char * nomFitxerBuscar, DadesFAT dadesFat){
    //En primer lloc obrim el fitxer i arribem a la root directory region
    int fdFAT = -1;
    FATRootDirectory aux;
    int nContador = 0;
    int i = 0;
    int nSortir = 0;
    char hidden, system, subdirectory, archive, unused1, unused2;
    
    
    fdFAT = open(nomVolumFAT, O_RDONLY);
    if (fdFAT < 0) {
        write(1, "Error al obrir el volum FAT\n", strlen("Error al obrir el volum FAT\n"));
    }else{
        //Primer ens saltem la reserved region
        lseek(fdFAT, (dadesFat.snReservedSectors * dadesFat.snSectorSize), SEEK_SET);
        //Després ens saltem les copies de la fat
        lseek(fdFAT, (dadesFat.snNumFats * dadesFat.snSectorsPerFat * dadesFat.snSectorSize), SEEK_CUR);
        //En aquest moment ja estem al ROOT DIRECTORY (32B per entrada)
        
        while (nContador < dadesFat.snMaxRootEntries && !nSortir) {
            read(fdFAT, aux.sName, 8);
            //Col·locar el \0
            i = 0;
            while (aux.sName[i] != ' ' && i < 8) {
                i++;
            }
            aux.sName[i] = '\0';
            read(fdFAT, aux.sExt, 3);
            i = 0;
            while (aux.sExt[i] != ' ' && i < 3) {
                i++;
            }
            aux.sExt[i] = '\0';
            read(fdFAT, &aux.cAttr, 1);
            //Reserved
            lseek(fdFAT, 10, SEEK_CUR);
            read(fdFAT, &aux.snTime, sizeof(unsigned short int));
            read(fdFAT, &aux.snDate, sizeof(unsigned short int));
            read(fdFAT, &aux.snStartCluster, sizeof(unsigned short int));
            read(fdFAT, &aux.nFileSize, sizeof(unsigned int));
            //Attribute
            hidden = (aux.cAttr & 0x02);
            system = (aux.cAttr & 0x04);
            subdirectory = (aux.cAttr & 0x10);
            archive = (aux.cAttr & 0x20);
            unused1 = (aux.cAttr & 0x40);
            unused2 = (aux.cAttr & 0x80);
            if (aux.sName[0] == 0x00) {
                nSortir = 1;
            }else{
                if (archive != 0 && hidden == 0 && unused1 == 0 && unused2 == 0 && system == 0 && aux.sName[0] != 0xE5) {
                    printf("%s.%s\n", aux.sName, aux.sExt);
                }
            }
            nContador++;
        }

        close(fdFAT);
    }
    
}