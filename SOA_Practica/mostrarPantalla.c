//
//  mostrarPantalla.c
//  SOA_Practica
//
//  Created by MRR on 15/04/15.
//  Copyright (c) 2015 MRR. All rights reserved.
//

#include "mostrarPantalla.h"

void Mostrar_mostrarInfoFAT(DadesFAT dadesfat){
    char sAuxText[50];
    
    write(1, "\n\n---- Filesystem Information ----\n\n", strlen("\n\n---- Filesystem Information ----\n\n"));
    //Filesystem
    bzero(sAuxText, sizeof(sAuxText));
    sprintf(sAuxText, "Filesystem: %s\n\n", dadesfat.sFileSystem);
    write(1, sAuxText, strlen(sAuxText));
    //System Name
    bzero(sAuxText, sizeof(sAuxText));
    sprintf(sAuxText, "System Name: %s\n", dadesfat.sSystemName);
    write(1, sAuxText, strlen(sAuxText));
    //Sector Size
    bzero(sAuxText, sizeof(sAuxText));
    sprintf(sAuxText, "Sector Size: %hu\n", dadesfat.snSectorSize);
    write(1, sAuxText, strlen(sAuxText));
    //Sector Cluster
    bzero(sAuxText, sizeof(sAuxText));
    sprintf(sAuxText, "Sector per Cluster: %hu\n", dadesfat.snSectorCluster);
    write(1, sAuxText, strlen(sAuxText));
    //Reserved Sectors
    bzero(sAuxText, sizeof(sAuxText));
    sprintf(sAuxText, "Reserved Sectors: %hu\n", dadesfat.snReservedSectors);
    write(1, sAuxText, strlen(sAuxText));
    //Number Fat's
    bzero(sAuxText, sizeof(sAuxText));
    sprintf(sAuxText, "Number of FATs: %hu\n", dadesfat.snNumFats);
    write(1, sAuxText, strlen(sAuxText));
    //Maximum Root Entries
    bzero(sAuxText, sizeof(sAuxText));
    sprintf(sAuxText, "Maximum Root Entries: %hu\n", dadesfat.snMaxRootEntries);
    write(1, sAuxText, strlen(sAuxText));
    //Sectors Per FAT
    bzero(sAuxText, sizeof(sAuxText));
    sprintf(sAuxText, "Sectors per FAT: %hu\n", dadesfat.snSectorsPerFat);
    write(1, sAuxText, strlen(sAuxText));
    //Label
    bzero(sAuxText, sizeof(sAuxText));
    sprintf(sAuxText, "Label: %s\n", dadesfat.sLabel);
    write(1, sAuxText, strlen(sAuxText));
}

void Mostrar_mostrarInfoEXT(DadesEXT2 dadesext){
    char sAuxText[50];
    time_t timeAux;
    
    write(1, "\n\n---- Filesystem Information ----\n\n", strlen("\n\n---- Filesystem Information ----\n\n"));
    //Filesystem
    bzero(sAuxText, sizeof(sAuxText));
    sprintf(sAuxText, "Filesystem: %s\n\n", dadesext.sFileSystem);
    write(1, sAuxText, strlen(sAuxText));
    //INODE INFO
    write(1, "INODE INFO\n", strlen("INODE INFO\n"));
    bzero(sAuxText, sizeof(sAuxText));
    sprintf(sAuxText, "Inode Size: %hu\n", dadesext.snInodeSize);
    write(1, sAuxText, strlen(sAuxText));
    //Number of inodes
    bzero(sAuxText, sizeof(sAuxText));
    sprintf(sAuxText, "Number of inodes: %d\n", dadesext.nNumberInodes);
    write(1, sAuxText, strlen(sAuxText));
    //First Inode
    bzero(sAuxText, sizeof(sAuxText));
    sprintf(sAuxText, "First Inode: %d\n", dadesext.nFirstInode);
    write(1, sAuxText, strlen(sAuxText));
    //Inode Group
    bzero(sAuxText, sizeof(sAuxText));
    sprintf(sAuxText, "Inode Group: %d\n", dadesext.nInodeGroup);
    write(1, sAuxText, strlen(sAuxText));
    //Free inodes
    bzero(sAuxText, sizeof(sAuxText));
    sprintf(sAuxText, "Free inodes: %d\n", dadesext.nFreeInode);
    write(1, sAuxText, strlen(sAuxText));
    //BLOCK INFO
    write(1, "\nBLOCK INFO\n", strlen("\nBLOCK INFO\n"));
    bzero(sAuxText, sizeof(sAuxText));
    sprintf(sAuxText, "Block size: %d\n", dadesext.nBlockSize);
    write(1, sAuxText, strlen(sAuxText));
    //Reserved Blocks
    bzero(sAuxText, sizeof(sAuxText));
    sprintf(sAuxText, "Reserved Blocks: %d\n", dadesext.nReservedBlocks);
    write(1, sAuxText, strlen(sAuxText));
    //Free Blocks
    bzero(sAuxText, sizeof(sAuxText));
    sprintf(sAuxText, "Free Blocks: %d\n", dadesext.nFreeBlocks);
    write(1, sAuxText, strlen(sAuxText));
    //Total Blocks
    bzero(sAuxText, sizeof(sAuxText));
    sprintf(sAuxText, "Total Blocks: %d\n", dadesext.nTotalBlocks);
    write(1, sAuxText, strlen(sAuxText));
    //First Block
    bzero(sAuxText, sizeof(sAuxText));
    sprintf(sAuxText, "First Block: %d\n", dadesext.nFirstBlocks);
    write(1, sAuxText, strlen(sAuxText));
    //Block Group
    bzero(sAuxText, sizeof(sAuxText));
    sprintf(sAuxText, "Block Group: %d\n", dadesext.nBlockGroup);
    write(1, sAuxText, strlen(sAuxText));
    //Flags Group
    bzero(sAuxText, sizeof(sAuxText));
    sprintf(sAuxText, "Block Group: %d\n", dadesext.nFlagGroup);
    write(1, sAuxText, strlen(sAuxText));
    //Volume Info
    write(1, "\nVOLUME INFO\n", strlen("\nVOLUME INFO\n"));
    //volume name
    bzero(sAuxText, sizeof(sAuxText));
    sprintf(sAuxText, "Volume name: %s\n", dadesext.sVolumeName);
    write(1, sAuxText, strlen(sAuxText));
    //Dates
    timeAux = dadesext.nLastCheck;
    bzero(sAuxText, sizeof(sAuxText));
    sprintf(sAuxText, "Last check: %s", ctime(&timeAux));
    write(1, sAuxText, strlen(sAuxText));
    
    timeAux = dadesext.nLastMount;
    bzero(sAuxText, sizeof(sAuxText));
    sprintf(sAuxText, "Last mount: %s", ctime(&timeAux));
    write(1, sAuxText, strlen(sAuxText));
    
    timeAux = dadesext.nLastWritten;
    bzero(sAuxText, sizeof(sAuxText));
    sprintf(sAuxText, "Last written: %s", ctime(&timeAux));
    write(1, sAuxText, strlen(sAuxText));
}