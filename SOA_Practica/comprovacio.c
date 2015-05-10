//
//  comprovacio.c
//  SOA_Practica
//
//  Created by MRR on 14/04/15.
//  Copyright (c) 2015 MRR. All rights reserved.
//

#include "comprovacio.h"

//Funció que comprova que els parametres siguin correctes i retorna un valor enter que representa l'opció escollida.
int Comprovacio_ParametresEntrada(int argc, char * argv[]){
    if (argc < 2) {
        write(1, "Error amb els parametres d'entrada\n", strlen("Error amb els parametres d'entrada\n"));
        return OPERACIO_ERROR;
    }else{
        if (strcmp(argv[1], "/info") == 0 && argc == 3) {
            return OPERACIO_INFO;
        }else if (strcmp(argv[1], "/find") == 0 && argc == 4){
            return OPERACIO_FIND;
        }else{
            write(1, "Error amb els parametres d'entrada\n", strlen("Error amb els parametres d'entrada\n"));
            return OPERACIO_ERROR;
        }
    }
    return OPERACIO_ERROR;
}

//Funció que comprova que el fitxer sigui del tipus correcte i retorna un valor que identifica si és FAT16, EXT2 o Error.
int Comprovacio_TipusFitxer(char * sNomFitxer){
    int fdFile = -1;
    unsigned short int snAux; //2 Bytes
    char sAuxText[20];
    
    //Obrir el fitxer per comprovar el seu tipus.
    fdFile = open(sNomFitxer, O_RDONLY);
    if (fdFile < 0) {
        write(1, "Error al obrir el fitxer\n", strlen("Error al obrir el fitxer\n"));
        return TIPUS_ERROR_FILE;
    }else{
        //Comprovar el byte 510 i 511 per saber si és FAT els valors han de ser 0x55 i 0xAA respectivament.
        lseek(fdFile, 510, SEEK_SET);
        read(fdFile, &snAux, sizeof(unsigned short int));
        //Valor girat AA55 és 43605 en decimal, per tant, comparem
        if (snAux == 43605) {
            //És FAT
            //Llegir la cadena del tipus i printar-la
            lseek(fdFile, 54, SEEK_SET);
            read(fdFile, sAuxText, 8);
            sAuxText[5] = '\0';
            
            //Tancar el fitxer
            close(fdFile);
            //Comprovació FAT16
            if (strcmp(sAuxText, "FAT16") == 0) {
                return TIPUS_FAT;
            }else{
                return TIPUS_ERROR_FILE_SYS;
            }
        }else{
            //Comprovar si és EXT2
            lseek(fdFile, 1024, SEEK_SET);
            lseek(fdFile, 56, SEEK_CUR);
            read(fdFile, &snAux, sizeof(unsigned short int));
            if (snAux == 61267) {
                return TIPUS_EXT2;
            }else{
                return TIPUS_ERROR_FILE_SYS;
            }
        }
    }
}

//Funció que ompla les dades necessaries del format FAT16
int Comprovacio_OmpleFitxerFAT(DadesFAT* dadesFat, char * sNomFitxer){
    int fdFile = -1;
    char sAuxText[20];
    unsigned short int snAux = 0;
    char cAux;
    
    //En primer lloc omplim el FileSystem amb FAT16, ja que quan es cridi segur que és FAT16
    strcpy(dadesFat->sFileSystem, "FAT16");
    
    //obrim el fitxer
    fdFile = open(sNomFitxer, O_RDONLY);
    if (fdFile < 0) {
        write(1, "Error al obrir el fitxer\n", strlen("Error al obrir el fitxer\n"));
        return OMPLE_ERROR;
    }else{
        //System name
        lseek(fdFile, 3, SEEK_SET);
        read(fdFile, sAuxText, 8);
        sAuxText[8] = '\0';
        strcpy(dadesFat->sSystemName, sAuxText);
        //Sector size
        lseek(fdFile, 11, SEEK_SET);
        read(fdFile, &snAux, sizeof(unsigned short int));
        dadesFat->snSectorSize = snAux;
        //Sector per Cluster
        lseek(fdFile, 13, SEEK_SET);
        read(fdFile, &cAux, 1);
        dadesFat->snSectorCluster = cAux;
        //Reserved Sectors
        lseek(fdFile, 14, SEEK_SET);
        read(fdFile, &snAux, sizeof(unsigned short int));
        dadesFat->snReservedSectors = snAux;
        //Number FAT's
        lseek(fdFile, 16, SEEK_SET);
        read(fdFile, &cAux, 1);
        dadesFat->snNumFats = cAux;
        //Root Entries
        lseek(fdFile, 17, SEEK_SET);
        read(fdFile, &snAux, sizeof(unsigned short int));
        dadesFat->snMaxRootEntries = snAux;
        //Sector per FAT
        lseek(fdFile, 22, SEEK_SET);
        read(fdFile, &snAux, sizeof(unsigned short int));
        dadesFat->snSectorsPerFat = snAux;
        //Label
        lseek(fdFile, 43, SEEK_SET);
        read(fdFile, &sAuxText, 11);
        sAuxText[11] = '\0';
        strcpy(dadesFat->sLabel, sAuxText);
        
        //Tancar el fitxer
        close(fdFile);
        return OMPLE_OK;
    }
}

int Comprovacio_OmpleFitxerEXT(DadesEXT2* dadesExt, char * sNomFitxer){
    int fdFile = -1;
    unsigned int nAux;
    unsigned short int snAux;
    char sAux[17];
    
    //En primer lloc copien EXT2 a les dades, ja que si s'executa ompleFitxerExt segur que el volum és del tipus EXT2
    strcpy(dadesExt->sFileSystem, "EXT2");
    
    //Obrir el fitxer
    fdFile = open(sNomFitxer, O_RDONLY);
    if (fdFile < 0) {
        write(1, "Error al obrir el fitxer\n", strlen("Error al obrir el fitxer\n"));
        return OMPLE_ERROR;
    }else{
        //Moure al superblock
        lseek(fdFile, 1024, SEEK_SET);
        //Number of inodes
        read(fdFile, &nAux, sizeof(unsigned int));
        dadesExt->nNumberInodes = nAux;
        //Number Blocks
        read(fdFile, &nAux, sizeof(unsigned int));
        dadesExt->nTotalBlocks = nAux;
        //Reserved Blocks
        read(fdFile, &nAux, sizeof(unsigned int));
        dadesExt->nReservedBlocks = nAux;
        //Free Blocks
        read(fdFile, &nAux, sizeof(unsigned int));
        dadesExt->nFreeBlocks = nAux;
        //Free inodes
        read(fdFile, &nAux, sizeof(unsigned int));
        dadesExt->nFreeInode = nAux;
        //First block
        read(fdFile, &nAux, sizeof(unsigned int));
        dadesExt->nFirstBlocks = nAux;
        //Block size
        read(fdFile, &nAux, sizeof(unsigned int));
        dadesExt->nBlockSize = 1024 << nAux;
        //Blocks per group
        lseek(fdFile, 4, SEEK_CUR);
        read(fdFile, &nAux, sizeof(unsigned int));
        dadesExt->nBlockGroup = nAux;
        //Frags per group
        read(fdFile, &nAux, sizeof(unsigned int));
        dadesExt->nFlagGroup = nAux;
        //Inodes per group
        read(fdFile, &nAux, sizeof(unsigned int));
        dadesExt->nInodeGroup = nAux;
        //Last mount
        read(fdFile, &nAux, sizeof(unsigned int));
        dadesExt->nLastMount = nAux;
        //Last write
        read(fdFile, &nAux, sizeof(unsigned int));
        dadesExt->nLastWritten = nAux;
        //Last check
        lseek(fdFile, 1088, SEEK_SET);
        read(fdFile, &nAux, sizeof(unsigned int));
        dadesExt->nLastCheck = nAux;
        //First inode
        lseek(fdFile, 1108, SEEK_SET);
        read(fdFile, &nAux, sizeof(unsigned int));
        dadesExt->nFirstInode = nAux;
        //Inode size
        read(fdFile, &snAux, sizeof(unsigned short int));
        dadesExt->snInodeSize = snAux;
        //Volume name
        lseek(fdFile, 1144, SEEK_SET);
        read(fdFile, sAux, 16);
        sAux[16] = '\0';
        strcpy(dadesExt->sVolumeName, sAux);
        
        return OMPLE_OK;
    }
}

