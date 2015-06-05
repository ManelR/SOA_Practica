//
//  buscarFitxer.c
//  SOA_Practica
//
//  Created by MRR on 30/04/15.
//  Copyright (c) 2015 MRR. All rights reserved.
//

#include "buscarFitxer.h"

void Buscar_PrepararNomFitxerFAT(char * sNomFitxerBuscar, char * nouNomFitxer,  char * extFitxerBuscar, int nComptadorNom){
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
        sAux[7] = nComptadorNom + '0';
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

int Buscar_BuscarFitxerFat(char * nomVolumFAT, char * nomFitxerBuscar, DadesFAT dadesFat, char * extFitxerBuscar){
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
                if (archive != 0 && subdirectory == 0 && hidden == 0 && unused1 == 0 && unused2 == 0 && system == 0 && ((unsigned char)aux.sName[0]) != 0xE5) {

                    if (strcasecmp(aux.sName, nomFitxerBuscar) == 0 && strcasecmp(aux.sExt, extFitxerBuscar) == 0) {
                        nTrobat = 1;
                        bzero(sAux, sizeof(sAux));
                        sprintf(sAux, "File found! Size: %u bytes\n", aux.nFileSize);
                        write(1, sAux, strlen(sAux));
                    }
                }else if (archive == 0 && subdirectory != 0 && hidden == 0 && unused1 == 0 && unused2 == 0 && system == 0 && ((unsigned char)aux.sName[0]) != 0xE5){
                    if (strcasecmp(aux.sName, nomFitxerBuscar) == 0 && strcasecmp(aux.sExt, extFitxerBuscar) == 0) {
                        write(1, "El fitxer que busques és un directori\n", strlen("El fitxer que busques és un directori\n"));
                    }
                }
            }
            nContador++;
        }
        close(fdFAT);
        
        if (nTrobat) {
            return 1;
        }else{
            return 0;
        }
    }
    return 0;
    
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

void Buscar_recuperarMidaFitxer(char * nomVolumEXT, DadesEXT2 dadesExt, unsigned int numInode, unsigned int* fitxerSize){
    int fdExt = 0;
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
        read(fdExt, fitxerSize, sizeof(unsigned int));
        close(fdExt);
    }
}

void analitzarBlockFitxer(unsigned int numBlockMostrar, int * fdExt, DadesEXT2 dadesExt, int * nFinalDelFitxer, unsigned int * fitxerSize, char** contingutFitxer, int* nIndexContingutFitxer){
    unsigned char * informacioFitxer;
    unsigned int i = 0;
    
    if (*fitxerSize < dadesExt.nBlockSize) {
        informacioFitxer = (unsigned char*)malloc(sizeof(unsigned char) * (*fitxerSize));
        *contingutFitxer = (char*)realloc(*contingutFitxer, (*nIndexContingutFitxer) + (*fitxerSize));
    }else{
        informacioFitxer = (unsigned char*)malloc(sizeof(unsigned char) * dadesExt.nBlockSize);
        *contingutFitxer = (char*)realloc(*contingutFitxer, (*nIndexContingutFitxer) + dadesExt.nBlockSize);
    }
    if (informacioFitxer == NULL) {
        write(1, "Error al demanar memòria!\n", strlen("Error al demanar memòria!\n"));
    }else{
        //Anar al block a llegir
        lseek(*fdExt, dadesExt.nBlockSize * numBlockMostrar, SEEK_SET);
        if (*fitxerSize < dadesExt.nBlockSize) {
            read(*fdExt, informacioFitxer, (*fitxerSize));
            for (i = 0; i < (*fitxerSize); i++) {
                (*contingutFitxer)[(*nIndexContingutFitxer)++] = informacioFitxer[i];
            }
            //write(1, informacioFitxer, (*fitxerSize));
            *nFinalDelFitxer = 1;
        }else{
            read(*fdExt, informacioFitxer, dadesExt.nBlockSize);
            for (i = 0; i < dadesExt.nBlockSize; i++) {
                (*contingutFitxer)[(*nIndexContingutFitxer)++] = informacioFitxer[i];
            }
            //write(1, informacioFitxer, dadesExt.nBlockSize);
            (*fitxerSize) -= dadesExt.nBlockSize;
        }
    }
}

void analitzarBlockPuntersFitxer(int numBlock, int* fdExt, DadesEXT2 dadesExt, int * nFinalDelFitxer, unsigned int * fitxerSize, int nivell, char** contingutFitxer, int* nIndexContingutFitxer){
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
                if (nBlockABuscar != 0) {
                    analitzarBlockFitxer(nBlockABuscar, fdExt, dadesExt, nFinalDelFitxer, fitxerSize, contingutFitxer, nIndexContingutFitxer);
                }
                numPuntersLlegits++;
                lseek(*fdExt, numBlock * dadesExt.nBlockSize, SEEK_SET);
            }
        }else if (nivell == 2){
            //Block de segon nivell
            while (numBytesLlegits < dadesExt.nBlockSize && *nFinalDelFitxer == 0) {
                lseek(*fdExt, 4 * numPuntersLlegits, SEEK_CUR);
                numBytesLlegits += read(*fdExt, &nBlockABuscar, sizeof(unsigned int));

                if (nBlockABuscar != 0) {
                    analitzarBlockPuntersFitxer(nBlockABuscar, fdExt, dadesExt, nFinalDelFitxer, fitxerSize, 1, contingutFitxer, nIndexContingutFitxer);
                }
                numPuntersLlegits++;
                lseek(*fdExt, numBlock * dadesExt.nBlockSize, SEEK_SET);
            }
        }else if (nivell == 3){
            //Block de tercer nivell
            while (numBytesLlegits < dadesExt.nBlockSize && *nFinalDelFitxer == 0) {
                lseek(*fdExt, 4 * numPuntersLlegits, SEEK_CUR);
                numBytesLlegits += read(*fdExt, &nBlockABuscar, sizeof(unsigned int));
                if (nBlockABuscar != 0) {
                    analitzarBlockPuntersFitxer(nBlockABuscar, fdExt, dadesExt, nFinalDelFitxer, fitxerSize, 2, contingutFitxer, nIndexContingutFitxer);
                }
                numPuntersLlegits++;
                lseek(*fdExt, numBlock * dadesExt.nBlockSize, SEEK_SET);
            }
        }
    }
}

void Buscar_recuperarContingutFitxer(char * nomVolumEXT, unsigned int numInode, DadesEXT2 dadesExt, char ** contingutFitxer){
    int fdExt = 0;
    unsigned int fitxerSize = 0;
    off_t posicioInicialInodeF;
    unsigned int inodeTableID = 0;
    int numPunterInode = 0;
    int nFinalDelFitxer = 0;
    unsigned int numBlockMostrar = 0;
    int nIndexContingutFitxer = 0;
    
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
        //Arribem al Inode Table
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
                analitzarBlockFitxer(numBlockMostrar, &fdExt, dadesExt, &nFinalDelFitxer, &fitxerSize, contingutFitxer, &nIndexContingutFitxer);
            }
            lseek(fdExt, posicioInicialInodeF, SEEK_SET);
            numPunterInode++;
        }
        lseek(fdExt, posicioInicialInodeF, SEEK_SET);
        lseek(fdExt, 40 + (4*12), SEEK_CUR);
        read(fdExt, &numBlockMostrar, sizeof(unsigned int));
        if (numBlockMostrar != 0 && !nFinalDelFitxer) {
            //Anar a buscat el block de dades amb punters
            analitzarBlockPuntersFitxer(numBlockMostrar, &fdExt, dadesExt, &nFinalDelFitxer, &fitxerSize, 1, contingutFitxer, &nIndexContingutFitxer);
        }
        
        lseek(fdExt, posicioInicialInodeF, SEEK_SET);
        lseek(fdExt, 40 + (4*13), SEEK_CUR);
        read(fdExt, &numBlockMostrar, sizeof(unsigned int));
        if (numBlockMostrar != 0 && !nFinalDelFitxer) {
            //Anar a buscat el block de dades amb punters
            analitzarBlockPuntersFitxer(numBlockMostrar, &fdExt, dadesExt, &nFinalDelFitxer, &fitxerSize, 2, contingutFitxer, &nIndexContingutFitxer);
        }
        
        lseek(fdExt, posicioInicialInodeF, SEEK_SET);
        lseek(fdExt, 40 + (4*14), SEEK_CUR);
        read(fdExt, &numBlockMostrar, sizeof(unsigned int));
        if (numBlockMostrar != 0 && !nFinalDelFitxer) {
            //Anar a buscat el block de dades amb punters
            analitzarBlockPuntersFitxer(numBlockMostrar, &fdExt, dadesExt, &nFinalDelFitxer, &fitxerSize, 3, contingutFitxer, &nIndexContingutFitxer);
        }
        
        close(fdExt);
    }
}

void comprovacioFATRoot(){
    //TODO implementar la comprovacio del root directory (espai)
}

void escriureFitxerFATRoot(int * fdFAT, DadesFAT dadesFat, unsigned short int primerCluster, char * nomFitxer, char * extFitxer, unsigned int fitxerSize){
    int nTrobat = 0;
    int i = 0;
    char sNomFitxer[8], sExtFitxer[3];
    unsigned short int nTime, nDate;
    time_t epoch_time;
    struct tm *tm_p;
    int bytesRootsLlegits = 0;
    FATRootDirectory auxRoot;
    //moure fins els clusters
    lseek(*fdFAT, (dadesFat.snReservedSectors * dadesFat.snSectorSize), SEEK_SET);
    lseek(*fdFAT, (dadesFat.snNumFats * dadesFat.snSectorsPerFat * dadesFat.snSectorSize), SEEK_CUR);
    //Buscar el primer root lliure
    while (!nTrobat && bytesRootsLlegits < (dadesFat.snMaxRootEntries * 32)) {
        read(*fdFAT, auxRoot.sName, 8);
        //Col·locar el \0
        i = 0;
        while (auxRoot.sName[i] != ' ' && i < 8) {
            i++;
        }
        auxRoot.sName[i] = '\0';
        read(*fdFAT, auxRoot.sExt, 3);
        i = 0;
        while (auxRoot.sExt[i] != ' ' && i < 3) {
            i++;
        }
        auxRoot.sExt[i] = '\0';
        read(*fdFAT, &auxRoot.cAttr, 1);
        //Reserved
        lseek(*fdFAT, 10, SEEK_CUR);
        read(*fdFAT, &auxRoot.snTime, sizeof(unsigned short int));
        read(*fdFAT, &auxRoot.snDate, sizeof(unsigned short int));
        read(*fdFAT, &auxRoot.snStartCluster, sizeof(unsigned short int));
        read(*fdFAT, &auxRoot.nFileSize, sizeof(unsigned int));
        bytesRootsLlegits += 32;
        if ((auxRoot.cAttr  & 0x40) != 0 || (auxRoot.cAttr & 0x80) != 0 || ((unsigned char)auxRoot.sName[0]) != 0xE5 || ((unsigned char)auxRoot.sName[0]) != 0x00) {
            nTrobat = 1;
        }
    }
    //Comprovar si s'ha trobat
    if (nTrobat) {
        lseek(*fdFAT, -32, SEEK_CUR);
        strcpy(auxRoot.sName, nomFitxer);

        for (i = 0; i < (int)strlen(auxRoot.sName); i++) {
            sNomFitxer[i] = auxRoot.sName[i];
        }
        for (i = (int)strlen(auxRoot.sName); i < 8; i++) {
            sNomFitxer[i] = ' ';
        }
        strcpy(auxRoot.sExt, extFitxer);
        for (i = 0; i < (int)strlen(auxRoot.sExt); i++) {
            sExtFitxer[i] = auxRoot.sExt[i];
        }
        for (i = (int)strlen(auxRoot.sExt); i < 3; i++) {
            sExtFitxer[i] = ' ';
        }
        auxRoot.cAttr = 0x20;
        auxRoot.nFileSize = fitxerSize;
        auxRoot.snStartCluster = primerCluster;
        epoch_time = time(0);
        tm_p = localtime( &epoch_time );
        nTime = (tm_p->tm_hour * 2048) + (tm_p->tm_min * 32) + (tm_p->tm_sec/2);
        auxRoot.snTime = nTime;
        nDate = ((tm_p->tm_year - 80)*512) + ((tm_p->tm_mon + 1)*32) + tm_p->tm_mday;
        auxRoot.snDate = nDate;
        //Escriure a la root
        write(*fdFAT, sNomFitxer, 8);
        write(*fdFAT, sExtFitxer, 3);
        write(*fdFAT, &auxRoot.cAttr, 1);
        lseek(*fdFAT, 10, SEEK_CUR);
        write(*fdFAT, &nTime, sizeof(unsigned short int));
        write(*fdFAT, &nDate, sizeof(unsigned short int));
        write(*fdFAT, &auxRoot.snStartCluster, sizeof(unsigned short int));
        write(*fdFAT, &auxRoot.nFileSize, sizeof(unsigned int));
    }else{
        write(1, "No hi ha espai al root directory", strlen("No hi ha espai al root directory"));
    }
}

void escriureFitxerFATCluster(int * fdFAT, DadesFAT dadesFat, unsigned int* bytesCopiats, char * contingutFitxer, unsigned int fitxerSize, unsigned short int numCluster){
    char * dadesACopiar;
    int i;
    unsigned int clusterSize = 0;
    int bytesACopiar = 0;
    //moure fins els clusters
    lseek(*fdFAT, (dadesFat.snReservedSectors * dadesFat.snSectorSize), SEEK_SET);
    lseek(*fdFAT, (dadesFat.snNumFats * dadesFat.snSectorsPerFat * dadesFat.snSectorSize), SEEK_CUR);
    //Saltar la root
    lseek(*fdFAT, (dadesFat.snMaxRootEntries * 32), SEEK_CUR);
    //Anar al cluster que toca
    lseek(*fdFAT, (dadesFat.snSectorCluster * dadesFat.snSectorSize) * (numCluster - 2), SEEK_CUR);
    //Escriure les dades
    clusterSize = (dadesFat.snSectorCluster * dadesFat.snSectorSize);
    if ((fitxerSize - (*bytesCopiats)) > clusterSize) {
        bytesACopiar = (dadesFat.snSectorCluster * dadesFat.snSectorSize);
    }else{
        bytesACopiar = (fitxerSize - (*bytesCopiats));
    }
    dadesACopiar = (char*)malloc(sizeof(char) * bytesACopiar);
    if (dadesACopiar != NULL) {
        for (i = 0; i < bytesACopiar; i++) {
            dadesACopiar[i] = contingutFitxer[(*bytesCopiats)++];
            
        }
    }
    write(*fdFAT, dadesACopiar, bytesACopiar);
}

void Buscar_CopiarFitxerAFAT(char * nomVolumFAT, DadesFAT dadesFat, char * contingutFitxer, unsigned int fitxerSize, char * nomFitxer, char * extFitxer){
    int fdFAT;
    int numClustersTotals = 0;
    int bytesLLegits = 0;
    int numClustersComprovacio = 0;
    int nTrobat = 0;
    int nFinal = 0;
    int nContador = 0;
    unsigned int bytesCopiats = 0;
    unsigned short int primerCluster = 0;
    unsigned short int numCluster = 2;
    off_t posicio_cluster_anterior = 0;
    off_t posicio_cluster_actual = 0;
    unsigned short int auxCluster = 0;
    
    fdFAT = open(nomVolumFAT, O_RDWR);
    if (fdFAT <= 0) {
        write(1, "Error al obrir el volum FAT\n", strlen("Error al obrir el volum FAT\n"));
    }else{
        //En primer lloc s'ha de comprovar si hi ha suficient espai per allotjar el fitxer.
        //Primer ens saltem la reserved region
        lseek(fdFAT, (dadesFat.snReservedSectors * dadesFat.snSectorSize), SEEK_SET);
        //Calcular quants clusters necessitem
        numClustersTotals = fitxerSize/(dadesFat.snSectorCluster*dadesFat.snSectorSize);
        if(fitxerSize%(dadesFat.snSectorCluster*dadesFat.snSectorSize) != 0)numClustersTotals++;
        //Recorrer la copia de la fat per saber si hi ha espai suficient.
        //Llegir els dos primers que no serveixen
        lseek(fdFAT, 6, SEEK_CUR);
        read(fdFAT, &auxCluster, sizeof(unsigned short int));
        nContador = 1;
        while (!nTrobat && !nFinal) {
            nContador++;
            bytesLLegits += sizeof(unsigned short int);
            if (auxCluster == 0) {
                numClustersComprovacio++;
                if (numClustersComprovacio == numClustersTotals) {
                    nTrobat = 1;
                }
            }
            if (bytesLLegits == (dadesFat.snSectorsPerFat * dadesFat.snSectorSize)) {
                nFinal = 1;
            }
            read(fdFAT, &auxCluster, sizeof(unsigned short int));
        }
        //Si nTrobat != 0 vol dir que hi ha espai suficient.
        if (nTrobat) {
            //tornem al inici
            lseek(fdFAT, (dadesFat.snReservedSectors * dadesFat.snSectorSize), SEEK_SET);
            //saltem els dos primers
            lseek(fdFAT, 6, SEEK_CUR);
            //-------COPIEM EL FITXER----------
            //Primer Cluster
            //Trobar el cluster lliure
            nTrobat = 0;
            //inicialitzat a 1 ja que el primer cluster és 2
            numCluster = 2;
            while (!nTrobat) {
                read(fdFAT, &auxCluster, sizeof(unsigned short int));
                numCluster++;
                if (auxCluster == 0) {
                    nTrobat = 1;
                }
            }
            primerCluster = numCluster;
            posicio_cluster_anterior = lseek(fdFAT, (-1)*sizeof(unsigned short int), SEEK_CUR);
            escriureFitxerFATCluster(&fdFAT, dadesFat, &bytesCopiats, contingutFitxer, fitxerSize, numCluster);
            nFinal = 0;
            //Tornar al cluster que estavem
            lseek(fdFAT, posicio_cluster_anterior, SEEK_SET);
            //Avançar
            lseek(fdFAT, sizeof(unsigned short int), SEEK_CUR);
            while (!nFinal) {
                //Trobar el cluster lliure
                nTrobat = 0;
                while (!nTrobat) {
                    read(fdFAT, &auxCluster, sizeof(unsigned short int));
                    numCluster++;
                    if (auxCluster == 0) {
                        nTrobat = 1;
                    }
                }

                posicio_cluster_actual = lseek(fdFAT, (-1)*sizeof(unsigned short int), SEEK_CUR);
                //Anar a escriure el cluster anterior
                lseek(fdFAT, posicio_cluster_anterior, SEEK_SET);
                write(fdFAT, &numCluster, sizeof(unsigned short int));
                //Anar al cluster que toca i escriure la informació
                escriureFitxerFATCluster(&fdFAT, dadesFat, &bytesCopiats, contingutFitxer, fitxerSize, numCluster);
                if (bytesCopiats >= fitxerSize) {
                    nFinal = 1;
                    lseek(fdFAT, posicio_cluster_actual, SEEK_SET);
                    numCluster = 0xFFF8;
                    write(fdFAT, &numCluster, sizeof(unsigned short int));
                }else{
                    posicio_cluster_anterior = posicio_cluster_actual;
                }
                //Tornar al cluster que estavem
                lseek(fdFAT, posicio_cluster_anterior, SEEK_SET);
                //Avançar
                lseek(fdFAT, sizeof(unsigned short int), SEEK_CUR);
            }
            //Escriure el fitxer al root
            escriureFitxerFATRoot(&fdFAT, dadesFat, primerCluster, nomFitxer, extFitxer, fitxerSize);
        }else{
            write(1, "El fitxer que es vol copiar no hi cap!!!\n", strlen("El fitxer que es vol copiar no hi cap!!!\n"));
        }
        close(fdFAT);
    }
    
}
