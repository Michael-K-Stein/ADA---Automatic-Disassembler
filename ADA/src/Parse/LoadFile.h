/**
    Loads a Portable Executable file (.acm, .ax, .cpl, .dll, .drv, .efi, .exe, .mui, .ocx, .scr, .sys, .tsp)
    Into the given pointer (PE), which must have already been initialized!
*/

/*
Reference:

.text: Code
.data: Initialized data
.bss: Uninitialized data
.rdata: Const/read-only (and initialized) data
.edata: Export descriptors
.idata: Import descriptors
.reloc: Relocation table (for code instructions with absolute addressing when
          the module could not be loaded at its preferred base address)
.rsrc: Resources (icon, bitmap, dialog, ...)
.tls: __declspec(thread) data (Fails with dynamically loaded DLLs -> hard to find bugs)

*/

#ifndef __ParseLoadFile
#define __ParseLoadFile

#include "DOS_Format.h"
#include <string.h>
#include <stdio.h>
#include <memory.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

uint16_t getNumberOfSections(Portable_Executable * PE) {
    return PE->portable_executable_header.coff_header.Number_Of_Sections[0] + (PE->portable_executable_header.coff_header.Number_Of_Sections[1] * 256);
}

uint8_t b16to10_1Byte(unsigned char * hex) {
    return (uint8_t)hex[0];
}
uint16_t b16to10_2Bytes(unsigned char * hex) {
    return (uint16_t)hex[0] + ((uint16_t)hex[1] * 256);
}
uint32_t b16to10_4Bytes(unsigned char * hex) {
    uint32_t total = 0;
    for (int i = 0; i < 4; i++) {
        total += (uint32_t)hex[i] * (pow(256, i));
    }
    return total;
}
uint64_t b16to10_8Bytes(unsigned char * hex) {
    uint64_t total = 0;
    for (int i = 0; i < 8; i++) {
        total += (uint64_t)hex[i] * (pow(256, i));
    }
    return total;
}

int LoadFile_PE(FILE * pFile, Portable_Executable * PE) {
    unsigned char buffer[sizeof(PE->portable_executable_header)];

    uint64_t byteInd = 0;

    // Load Portable Executable Header
    printf("Load DOS Header.\n");
    while(byteInd < sizeof(PE->portable_executable_header) && fread(buffer, 1, sizeof(buffer), pFile)){
        memcpy(PE, &buffer, sizeof(PE->portable_executable_header));
        byteInd+=sizeof(PE->portable_executable_header);
    }
    printf("Successfully loaded DOS header.\n\n");

    // Initialize Section_Tables memory
    PE->section_tables = (Section_Table *)malloc(getNumberOfSections(PE) * sizeof(Section_Table));

    // Load Section Headers
    printf("Loading section headers.\n");
    for (int secInd = 0; secInd < getNumberOfSections(PE); secInd++) {

        // Initialize Section
        Section_Table sectionHeader;

        // Load Section Header
        uint64_t lastByteInd = byteInd;
        unsigned char sectionHeaderBuffer[sizeof(Section_Table)];
        uint32_t ActualSectionTableSize = sizeof(Section_Table) - 4;
        while(byteInd - lastByteInd < ActualSectionTableSize && fread(sectionHeaderBuffer, 1, ActualSectionTableSize, pFile)){
            memcpy(&sectionHeader, &sectionHeaderBuffer, ActualSectionTableSize);
            byteInd+=ActualSectionTableSize; // The Section_Table is 4 bytes larger than the file section header.
        }

        // Copy Section Header to PE data
        memcpy(&PE->section_tables[secInd], &sectionHeader, sizeof(sectionHeader));

        //PrintSectionHeaderInfo(&sectionHeader);
        //PrintSectionHeaderInfo(&PE->section_tables[secInd]);
    }
    printf("Successfully loaded section headers.\n\n");

    // Load Section Data
    printf("Load section data.\n");
    for (int secInd = 0; secInd < getNumberOfSections(PE); secInd++) {

        // Find Section: (According to header)
        unsigned char * ptrData = (unsigned char *)PE->section_tables[secInd].Pointer_To_Raw_Data; // Surprisingly, this works
        uint32_t SizeOfRawData = b16to10_4Bytes((unsigned char *)PE->section_tables[secInd].Size_Of_Raw_Data);
        uint32_t AddressToRawData = b16to10_4Bytes(ptrData);

         if (b16to10_4Bytes(ptrData) == 0) {
            /// This section is empty.
            /// No need to load it.
            /// Probably a good idea to inform the user of this. Though it's not uncommon.
            if (SizeOfRawData != 0) {
                /// This shouldn't happen. Probably means the file is either not valid PE, or it has been tampered with.
            }
        } else {

            // Initialize the raw data section to the necessary size.
            PE->section_tables[secInd].section_data = (Section_Data *)malloc(SizeOfRawData);
            PE->section_tables[secInd].section_data->data = (unsigned char *)malloc(SizeOfRawData);
            unsigned char * sectionDataBuffer = (unsigned char *)calloc(SizeOfRawData, sizeof(unsigned char));

            fseek(pFile, AddressToRawData, SEEK_SET);

            if (fread(sectionDataBuffer, 1, SizeOfRawData, pFile)) {
                memcpy(PE->section_tables[secInd].section_data->data, sectionDataBuffer, SizeOfRawData);
				free(sectionDataBuffer);
            }

        }
    }
    printf("Successfully loaded section data.\n\n");

	return 0;
}

// Get the size of a file in bytes.
uint64_t getFileSize(char * fPath) {
    FILE * pFile = fopen(fPath, "rb"); // Get file stream.
    fseek(pFile, 0L, SEEK_END); // Find end of file.
    return ftell(pFile); // Get index of file stream pointer.
}

Portable_Executable LoadFile(char * fPath) {
    FILE * pFile = fopen(fPath, "rb"); // Get file stream.

    Portable_Executable PE;
    Portable_Executable * ptrPE = &PE;
    ptrPE = (Portable_Executable *)malloc(getFileSize(fPath) * sizeof(unsigned char));

    LoadFile_PE(pFile, &PE);

	fclose(pFile);

    return PE;
}


bool VerifyPEFormat(Portable_Executable * PE) {
    unsigned char propperPESignature[2] = { 'M', 'Z' };
    unsigned char propperPE[4] = {0x50, 0x45, 0, 0};
    bool flagValid = true;
    for (int i = 0; i < 4; i++) { flagValid = flagValid && propperPE[i] == PE->portable_executable_header.coff_header.Signature[i]; }

    for (int i = 0; i < 2; i++) { flagValid = flagValid && propperPESignature[i] == PE->portable_executable_header.dos_header.DOS_Signature[i]; }

    for (int i = 0; i < 4; i++) { flagValid = flagValid && PE->portable_executable_header.data_directories.Zero_Check_Valid_1[i] == 0; }
    for (int i = 0; i < 8; i++) { flagValid = flagValid && PE->portable_executable_header.data_directories.Zero_Check_Valid_2[i] == 0; }

    return flagValid;
}

#endif //__ParseLoadFile
