//
//  buscarFitxer.c
//  SOA_Practica
//
//  Created by MRR on 30/04/15.
//  Copyright (c) 2015 MRR. All rights reserved.
//

#include "buscarFitxer.h"

void Buscar_PrepararNomFitxerFAT(char * sNomFitxerBuscar, char * nouNomFitxer,  char * extFitxerBuscar){
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
    nContador = 0;
    //Extensió
    if (sNomFitxerBuscar[i] == '.') {
        i++;
        while (sNomFitxerBuscar[i] != ' ' && nContador < 3) {
            sAux[nContador] = sNomFitxerBuscar[i];
            nContador++;
            i++;
        }
        sAux[nContador] = '\0';
        strcpy(extFitxerBuscar, sAux);
    }
}

void Buscar_BuscarFitxerFat(char * nomVolumFAT, char * nomFitxerBuscar, DadesFAT dadesFat, char * extFitxerBuscar){
    //En primer lloc obrim el fitxer i arribem a la root directory region
    int fdFAT = -1;
    FATRootDirectory aux;
    int nContador = 0;
    int i = 0;
    int nTrobat = 0;
    int nSortir = 0;
    char sAux[50];
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
        
        while (nContador < dadesFat.snMaxRootEntries && !nSortir && !nTrobat) {
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
                if (archive != 0 && hidden == 0 && unused1 == 0 && unused2 == 0 && system == 0 && ((unsigned char)aux.sName[0]) != 0xE5) {
                    if (strcasecmp(aux.sName, nomFitxerBuscar) == 0 && strcasecmp(aux.sExt, extFitxerBuscar) == 0) {
                        nTrobat = 1;
                        bzero(sAux, sizeof(sAux));
                        sprintf(sAux, "File found! Size: %u bytes\n", aux.nFileSize);
                        write(1, sAux, strlen(sAux));
                    }
                }
            }
            nContador++;
        }

        close(fdFAT);
    }
    
}

void Buscar_BuscarFitxerExt(char * nomVolumEXT, char * nomFitxerBuscar, DadesEXT2 dadesExt){
    int fdExt = 0;
    unsigned int inodeTableID = 0;
    char textAux[50];
    unsigned int numBlock = 0;
    char sAux[256];
    off_t posicioInicialTI;
    int numBytesLlegits = 0;
    int numPunterInode = 0;
    int nSortir = 0;
    unsigned int fitxerSize = 0;
    int nTrobat = 0;
    DirectoryEXT2 stAux;
    
    fdExt = open(nomVolumEXT, O_RDONLY);
    if (fdExt < 0) {
        write(1, "Error al obrir el volum FAT\n", strlen("Error al obrir el volum FAT\n"));
    }else{
        //Arribem al superblock
        lseek(fdExt, 1024, SEEK_SET);
        lseek(fdExt, 1024, SEEK_CUR);
        lseek(fdExt, 8, SEEK_CUR);
        read(fdExt, &inodeTableID, sizeof(unsigned int));
        posicioInicialTI = lseek(fdExt, inodeTableID * dadesExt.nBlockSize, SEEK_SET);
        
        
        while (numPunterInode < 12 && !nTrobat) {
            lseek(fdExt, dadesExt.snInodeSize, SEEK_CUR);
            //Ara estem al root directory
            
            lseek(fdExt, 40 + (numPunterInode*4), SEEK_CUR);
            read(fdExt, &numBlock, sizeof(unsigned int));
            if (numBlock != 0) {
                //Moure al block de dades de la root
                lseek(fdExt, dadesExt.nBlockSize * numBlock, SEEK_SET);
                //Llegir les dades
                while (!nSortir && numBytesLlegits < dadesExt.nBlockSize && !nTrobat) {
                    //Llegir la primera dada
                    numBytesLlegits += read(fdExt, &stAux.numInode, sizeof(unsigned int));
                    numBytesLlegits += read(fdExt, &stAux.registerLength, sizeof(unsigned short int));
                    numBytesLlegits += read(fdExt, &stAux.nameLength, 1);
                    numBytesLlegits += read(fdExt, &stAux.fileType, 1);
                    numBytesLlegits += read(fdExt, sAux, stAux.nameLength);
                    //Avançar fins al final del block
                    lseek(fdExt, stAux.registerLength - BYTE_DIRECTORY - stAux.nameLength, SEEK_CUR);
                    numBytesLlegits += (stAux.registerLength - BYTE_DIRECTORY - stAux.nameLength);
                    sAux[stAux.nameLength] = '\0';
                    if (stAux.numInode == 0) {
                        nSortir = 1;
                    }else if(stAux.fileType == 1){
                        //FILE
                        //printf("%s - %u - %u\n", sAux, stAux.numInode, (unsigned int)stAux.fileType);
                        if (strcmp(nomFitxerBuscar, sAux) == 0) {
                            nTrobat = 1;
                        }
                    }else if(stAux.fileType == 2){
                        //DIR
                        //printf("DIR: %s - %u - %u\n", sAux, stAux.numInode, (unsigned int)stAux.fileType);
                    }
                }
            }
            lseek(fdExt, posicioInicialTI, SEEK_SET);
            numPunterInode++;
        }
        if (nTrobat != 0){
            //Anar a buscar l'inode i mostrar la mida del fitxer
            lseek(fdExt, posicioInicialTI, SEEK_SET);
            lseek(fdExt, dadesExt.snInodeSize * (stAux.numInode-1), SEEK_CUR);
            lseek(fdExt, 4, SEEK_CUR);
            read(fdExt, &fitxerSize, sizeof(unsigned int));
            bzero(textAux, sizeof(textAux));
            sprintf(textAux, "File found! Size: %u bytes.\n", fitxerSize);
            write(1, textAux , strlen(textAux));
        }else{
            write(1, "Error. File not found.\n", strlen("Error. File not found.\n"));
        }
    }
}

