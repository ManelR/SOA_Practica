
//
//  tipus.h
//  SOA_Practica
//
//  Created by MRR on 13/04/15.
//  Copyright (c) 2015 MRR. All rights reserved.
//

#ifndef SOA_Practica_tipus_h
#define SOA_Practica_tipus_h

#include <time.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>




//Constants 

//Tipus per emmagazemar les dades
typedef struct{
    char sFileSystem[6];
    char sSystemName[20];
    unsigned short int snSectorSize;
    unsigned short int snSectorCluster;
    unsigned short int snReservedSectors;
    unsigned short int snNumFats;
    unsigned short int snMaxRootEntries;
    unsigned short int snSectorsPerFat;
    char sLabel[30];
}DadesFAT;

typedef struct{
    char sFileSystem[5];
    //Inode info
    unsigned short int snInodeSize;
    unsigned int nNumberInodes;
    unsigned int nFirstInode;
    unsigned int nInodeGroup;
    unsigned int nFreeInode;
    //Block info
    unsigned int nBlockSize;
    unsigned int nReservedBlocks;
    unsigned int nFreeBlocks;
    unsigned int nTotalBlocks;
    unsigned int nFirstBlocks;
    unsigned int nBlockGroup;
    unsigned int nFlagGroup;
    //Volume info
    char sVolumeName[20];
    unsigned int nLastCheck;
    unsigned int nLastMount;
    unsigned int nLastWritten;
}DadesEXT2;

typedef struct{
    char sName[9];
    char sExt[4];
    unsigned char cAttr;
    unsigned short int snTime;
    unsigned short int snDate;
    unsigned short int snStartCluster;
    unsigned int nFileSize;
}FATRootDirectory;

typedef struct{
    unsigned int numInode;
    unsigned short int registerLength;
    unsigned char nameLength;
    unsigned char fileType;
}DirectoryEXT2;

#endif
