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

void analitzarBlockDirectori(int numBlock, DirectoryEXT2* stAux, int* fdExt, DadesEXT2 dadesExt, int * nTrobat, char * nomFitxerBuscar){
    int nSortir = 0;
    unsigned int numBytesLlegits = 0;
    char sAux[256];
    
    if (numBlock != 0) {
        //Moure al block de dades de la root
        lseek(*fdExt, dadesExt.nBlockSize * numBlock, SEEK_SET);
        //Llegir les dades
        while (!nSortir && numBytesLlegits < dadesExt.nBlockSize && *nTrobat == 0) {
            //Llegir la primera dada
            numBytesLlegits += read(*fdExt, &(stAux->numInode), sizeof(unsigned int));
            numBytesLlegits += read(*fdExt, &(stAux->registerLength), sizeof(unsigned short int));
            numBytesLlegits += read(*fdExt, &(stAux->nameLength), 1);
            numBytesLlegits += read(*fdExt, &(stAux->fileType), 1);
            numBytesLlegits += read(*fdExt, sAux, stAux->nameLength);
            //Avançar fins al final del block
            lseek(*fdExt, stAux->registerLength - BYTE_DIRECTORY - stAux->nameLength, SEEK_CUR);
            numBytesLlegits += (stAux->registerLength - BYTE_DIRECTORY - stAux->nameLength);
            sAux[stAux->nameLength] = '\0';
            if (stAux->numInode == 0) {
                nSortir = 1;
            }else if(stAux->fileType == 1){
                //FILE
                //printf("%s - %u - %u\n", sAux, stAux->numInode, (unsigned int)stAux->fileType);
                if (strcmp(nomFitxerBuscar, sAux) == 0) {
                    *nTrobat = 1;
                }
            }else if(stAux->fileType == 2){
                //DIR
                //printf("DIR: %s - %u - %u\n", sAux, stAux->numInode, (unsigned int)stAux->fileType);
            }
        }
    }

}

void analitzarBlockPuntersDirectori(int numBlock, DirectoryEXT2 * stAux, int* fdExt, DadesEXT2 dadesExt, int* nTrobat, char* nomFitxerBuscar, int nivell){
    unsigned int nBlockABuscar = 0;
    unsigned int numBytesLlegits = 0;
    int numPuntersLlegits = 0;
    
    if (numBlock != 0) {
        //Moure al block que toca
        lseek(*fdExt, numBlock * dadesExt.nBlockSize, SEEK_SET);
        
        if (nivell == 1) {
            //Block de primer nivell
            while (numBytesLlegits < dadesExt.nBlockSize && *nTrobat == 0) {
                lseek(*fdExt, 4 * numPuntersLlegits, SEEK_CUR);
                numBytesLlegits += read(*fdExt, &nBlockABuscar, sizeof(unsigned int));
                //printf("%d\n", nBlockABuscar);
                if (nBlockABuscar != 0) {
                    analitzarBlockDirectori(nBlockABuscar, stAux, fdExt, dadesExt, nTrobat, nomFitxerBuscar);
                }
                numPuntersLlegits++;
                lseek(*fdExt, numBlock * dadesExt.nBlockSize, SEEK_SET);
            }
        }else if (nivell == 2){
            //Block de segon nivell
            while (numBytesLlegits < dadesExt.nBlockSize && *nTrobat == 0) {
                lseek(*fdExt, 4 * numPuntersLlegits, SEEK_CUR);
                numBytesLlegits += read(*fdExt, &nBlockABuscar, sizeof(unsigned int));
                //printf("%d\n", nBlockABuscar);
                if (nBlockABuscar != 0) {
                    analitzarBlockPuntersDirectori(nBlockABuscar, stAux, fdExt, dadesExt, nTrobat, nomFitxerBuscar, 1);
                }
                numPuntersLlegits++;
                lseek(*fdExt, numBlock * dadesExt.nBlockSize, SEEK_SET);
            }
        }else if (nivell == 3){
            //Block de tercer nivell
            while (numBytesLlegits < dadesExt.nBlockSize && *nTrobat == 0) {
                lseek(*fdExt, 4 * numPuntersLlegits, SEEK_CUR);
                numBytesLlegits += read(*fdExt, &nBlockABuscar, sizeof(unsigned int));
                //printf("%d\n", nBlockABuscar);
                if (nBlockABuscar != 0) {
                    analitzarBlockPuntersDirectori(nBlockABuscar, stAux, fdExt, dadesExt, nTrobat, nomFitxerBuscar, 2);
                }
                numPuntersLlegits++;
                lseek(*fdExt, numBlock * dadesExt.nBlockSize, SEEK_SET);
            }
        }
    }
}

unsigned int Buscar_BuscarFitxerExt(char * nomVolumEXT, char * nomFitxerBuscar, DadesEXT2 dadesExt){
    int fdExt = 0;
    unsigned int inodeTableID = 0;
    unsigned int numBlock = 0;
    off_t posicioInicialTI;
    int numPunterInode = 0;
    int nTrobat = 0;
    DirectoryEXT2 stAux;
    
    fdExt = open(nomVolumEXT, O_RDONLY);
    if (fdExt <= 0) {
        write(1, "Error al obrir el volum EXT\n", strlen("Error al obrir el volum EXT\n"));
    }else{
        //Arribem al superblock
        if (dadesExt.nBlockSize <= 1024) {
            lseek(fdExt, dadesExt.nBlockSize * 2, SEEK_SET);
        }else if (dadesExt.nBlockSize >= 2048){
            lseek(fdExt, dadesExt.nBlockSize, SEEK_SET);
        }
        lseek(fdExt, 8, SEEK_CUR);
        read(fdExt, &inodeTableID, sizeof(unsigned int));
        posicioInicialTI = lseek(fdExt, inodeTableID * dadesExt.nBlockSize, SEEK_SET);
        
        
        while (numPunterInode < 12 && !nTrobat) {
            lseek(fdExt, dadesExt.snInodeSize, SEEK_CUR);
            //Ara estem al root directory
            
            lseek(fdExt, 40 + (numPunterInode*4), SEEK_CUR);
            read(fdExt, &numBlock, sizeof(unsigned int));
            if (numBlock != 0) {
                analitzarBlockDirectori(numBlock, &stAux, &fdExt, dadesExt, &nTrobat, nomFitxerBuscar);
            }
            lseek(fdExt, posicioInicialTI, SEEK_SET);
            numPunterInode++;
        }
        lseek(fdExt, posicioInicialTI + dadesExt.snInodeSize, SEEK_SET);
        //Analitzar el punter de primer nivell
        lseek(fdExt, 40 + (4*12), SEEK_CUR);
        read(fdExt, &numBlock, sizeof(unsigned int));
        if (numBlock != 0 && !nTrobat) {
            //Anar a buscat el block de dades amb punters
            analitzarBlockPuntersDirectori(numBlock, &stAux, &fdExt, dadesExt, &nTrobat, nomFitxerBuscar, 1);
        }
        lseek(fdExt, posicioInicialTI + dadesExt.snInodeSize, SEEK_SET);
        //Analitzar el punter de primer nivell
        lseek(fdExt, 40 + (4*13), SEEK_CUR);
        read(fdExt, &numBlock, sizeof(unsigned int));
        if (numBlock != 0 && !nTrobat) {
            //Anar a buscat el block de dades amb punters
            analitzarBlockPuntersDirectori(numBlock, &stAux, &fdExt, dadesExt, &nTrobat, nomFitxerBuscar, 2);
        }
        lseek(fdExt, posicioInicialTI + dadesExt.snInodeSize, SEEK_SET);
        //Analitzar el punter de primer nivell
        lseek(fdExt, 40 + (4*14), SEEK_CUR);
        read(fdExt, &numBlock, sizeof(unsigned int));
        if (numBlock != 0 && !nTrobat) {
            //Anar a buscat el block de dades amb punters
            analitzarBlockPuntersDirectori(numBlock, &stAux, &fdExt, dadesExt, &nTrobat, nomFitxerBuscar, 3);
        }
        if (nTrobat != 0){
            //Anar a buscar l'inode i mostrar la mida del fitxer
            close(fdExt);
            return stAux.numInode;
        }else{
            close(fdExt);
            write(1, "Error. File not found.\n", strlen("Error. File not found.\n"));
            return 0;
        }
    }
    return 0;
}

void Buscar_mostrarMidaFitxer(char * nomVolumEXT, DadesEXT2 dadesExt, unsigned int numInode){
    int fdExt = 0;
    unsigned int fitxerSize = 0;
    char textAux[50];
    off_t posicioInicialTI;
    unsigned int inodeTableID = 0;
    
    fdExt = open(nomVolumEXT, O_RDONLY);
    if (fdExt <= 0) {
        write(1, "Error al obrir el volum EXT\n", strlen("Error al obrir el volum EXT\n"));
    }else{
        //Arribem al superblock
        if (dadesExt.nBlockSize <= 1024) {
            lseek(fdExt, dadesExt.nBlockSize * 2, SEEK_SET);
        }else if (dadesExt.nBlockSize >= 2048){
            lseek(fdExt, dadesExt.nBlockSize, SEEK_SET);
        }
        lseek(fdExt, 8, SEEK_CUR);
        read(fdExt, &inodeTableID, sizeof(unsigned int));
        posicioInicialTI = lseek(fdExt, inodeTableID * dadesExt.nBlockSize, SEEK_SET);
        
        lseek(fdExt, posicioInicialTI, SEEK_SET);
        lseek(fdExt, dadesExt.snInodeSize * (numInode-1), SEEK_CUR);
        lseek(fdExt, 4, SEEK_CUR);
        read(fdExt, &fitxerSize, sizeof(unsigned int));
        bzero(textAux, sizeof(textAux));
        sprintf(textAux, "File found! Size: %u bytes.\n", fitxerSize);
        write(1, textAux , strlen(textAux));
        close(fdExt);
    }
}

void analitzarBlockFitxer(unsigned int numBlockMostrar, int * fdExt, DadesEXT2 dadesExt, int * nFinalDelFitxer, unsigned int * fitxerSize){
    unsigned char * informacioFitxer;
    if (*fitxerSize < dadesExt.nBlockSize) {
        informacioFitxer = (unsigned char*)malloc(sizeof(unsigned char) * (*fitxerSize));
    }else{
        informacioFitxer = (unsigned char*)malloc(sizeof(unsigned char) * dadesExt.nBlockSize);
    }
    if (informacioFitxer == NULL) {
        write(1, "Error al demanar memòria!\n", strlen("Error al demanar memòria!\n"));
    }else{
        //Anar al block a llegir
        lseek(*fdExt, dadesExt.nBlockSize * numBlockMostrar, SEEK_SET);
        if (*fitxerSize < dadesExt.nBlockSize) {
            informacioFitxer = (unsigned char*)malloc(sizeof(unsigned char) * (*fitxerSize));
            read(*fdExt, informacioFitxer, (*fitxerSize));
            write(1, informacioFitxer, (*fitxerSize));
            *nFinalDelFitxer = 1;
        }else{
            informacioFitxer = (unsigned char*)malloc(sizeof(unsigned char) * dadesExt.nBlockSize);
            read(*fdExt, informacioFitxer, dadesExt.nBlockSize);
            write(1, informacioFitxer, dadesExt.nBlockSize);
            *fitxerSize -= dadesExt.nBlockSize;
        }
    }
}

void analitzarBlockPuntersFitxer(int numBlock, int* fdExt, DadesEXT2 dadesExt, int * nFinalDelFitxer, unsigned int * fitxerSize, int nivell){
    unsigned int nBlockABuscar = 0;
    unsigned int numBytesLlegits = 0;
    int numPuntersLlegits = 0;
    
    if (numBlock != 0) {
        //Moure al block que toca
        lseek(*fdExt, numBlock * dadesExt.nBlockSize, SEEK_SET);
        
        if (nivell == 1) {
            //Block de primer nivell
            while (numBytesLlegits < dadesExt.nBlockSize && *nFinalDelFitxer == 0) {
                lseek(*fdExt, 4 * numPuntersLlegits, SEEK_CUR);
                numBytesLlegits += read(*fdExt, &nBlockABuscar, sizeof(unsigned int));
                //printf("%d\n", nBlockABuscar);
                if (nBlockABuscar != 0) {
                    analitzarBlockFitxer(nBlockABuscar, fdExt, dadesExt, nFinalDelFitxer, fitxerSize);
                }
                numPuntersLlegits++;
                lseek(*fdExt, numBlock * dadesExt.nBlockSize, SEEK_SET);
            }
        }else if (nivell == 2){
            //Block de segon nivell
            while (numBytesLlegits < dadesExt.nBlockSize && *nFinalDelFitxer == 0) {
                lseek(*fdExt, 4 * numPuntersLlegits, SEEK_CUR);
                numBytesLlegits += read(*fdExt, &nBlockABuscar, sizeof(unsigned int));
                //printf("%d\n", nBlockABuscar);
                if (nBlockABuscar != 0) {
                    analitzarBlockPuntersFitxer(nBlockABuscar, fdExt, dadesExt, nFinalDelFitxer, fitxerSize, 1);
                }
                numPuntersLlegits++;
                lseek(*fdExt, numBlock * dadesExt.nBlockSize, SEEK_SET);
            }
        }else if (nivell == 3){
            //Block de tercer nivell
            while (numBytesLlegits < dadesExt.nBlockSize && *nFinalDelFitxer == 0) {
                lseek(*fdExt, 4 * numPuntersLlegits, SEEK_CUR);
                numBytesLlegits += read(*fdExt, &nBlockABuscar, sizeof(unsigned int));
                //printf("%d\n", nBlockABuscar);
                if (nBlockABuscar != 0) {
                    analitzarBlockPuntersFitxer(nBlockABuscar, fdExt, dadesExt, nFinalDelFitxer, fitxerSize, 2);
                }
                numPuntersLlegits++;
                lseek(*fdExt, numBlock * dadesExt.nBlockSize, SEEK_SET);
            }
        }
    }
}

void Buscar_mostrarContingutFitxer(char * nomVolumEXT, unsigned int numInode, DadesEXT2 dadesExt){
    int fdExt = 0;
    unsigned int fitxerSize = 0;
    off_t posicioInicialInodeF;
    unsigned int inodeTableID = 0;
    int numPunterInode = 0;
    int nFinalDelFitxer = 0;
    unsigned int numBlockMostrar = 0;
    
    fdExt = open(nomVolumEXT, O_RDONLY);
    if (fdExt <= 0) {
        write(1, "Error al obrir el volum EXT\n", strlen("Error al obrir el volum EXT\n"));
    }else{
        //Arribem al superblock
        if (dadesExt.nBlockSize <= 1024) {
            lseek(fdExt, dadesExt.nBlockSize * 2, SEEK_SET);
        }else if (dadesExt.nBlockSize >= 2048){
            lseek(fdExt, dadesExt.nBlockSize, SEEK_SET);
        }
        lseek(fdExt, 8, SEEK_CUR);
        read(fdExt, &inodeTableID, sizeof(unsigned int));
        lseek(fdExt, inodeTableID * dadesExt.nBlockSize, SEEK_SET);
        posicioInicialInodeF = lseek(fdExt, dadesExt.snInodeSize * (numInode-1), SEEK_CUR);
        
        lseek(fdExt, 4, SEEK_CUR);
        read(fdExt, &fitxerSize, sizeof(unsigned int));
        
        lseek(fdExt, posicioInicialInodeF, SEEK_SET);
        //Començar a mostrar contingut dels punters de l'inode
        while (numPunterInode < 12 && !nFinalDelFitxer) {
            lseek(fdExt, 40 + (numPunterInode*4), SEEK_CUR);
            read(fdExt, &numBlockMostrar, sizeof(unsigned int));
            if (numBlockMostrar != 0) {
                analitzarBlockFitxer(numBlockMostrar, &fdExt, dadesExt, &nFinalDelFitxer, &fitxerSize);
            }
            lseek(fdExt, posicioInicialInodeF, SEEK_SET);
            numPunterInode++;
        }
        lseek(fdExt, posicioInicialInodeF, SEEK_SET);
        lseek(fdExt, 40 + (4*12), SEEK_CUR);
        read(fdExt, &numBlockMostrar, sizeof(unsigned int));
        if (numBlockMostrar != 0 && !nFinalDelFitxer) {
            //Anar a buscat el block de dades amb punters
            analitzarBlockPuntersFitxer(numBlockMostrar, &fdExt, dadesExt, &nFinalDelFitxer, &fitxerSize, 1);
        }
        
        lseek(fdExt, posicioInicialInodeF, SEEK_SET);
        lseek(fdExt, 40 + (4*13), SEEK_CUR);
        read(fdExt, &numBlockMostrar, sizeof(unsigned int));
        if (numBlockMostrar != 0 && !nFinalDelFitxer) {
            //Anar a buscat el block de dades amb punters
            analitzarBlockPuntersFitxer(numBlockMostrar, &fdExt, dadesExt, &nFinalDelFitxer, &fitxerSize, 2);
        }
        
        lseek(fdExt, posicioInicialInodeF, SEEK_SET);
        lseek(fdExt, 40 + (4*14), SEEK_CUR);
        read(fdExt, &numBlockMostrar, sizeof(unsigned int));
        if (numBlockMostrar != 0 && !nFinalDelFitxer) {
            //Anar a buscat el block de dades amb punters
            analitzarBlockPuntersFitxer(numBlockMostrar, &fdExt, dadesExt, &nFinalDelFitxer, &fitxerSize, 3);
        }
        
        close(fdExt);
    }
}

