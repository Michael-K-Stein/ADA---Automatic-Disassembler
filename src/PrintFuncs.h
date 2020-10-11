#ifndef __PrintFuncs
#define __PrintFuncs

//#include "Parse/LoadFile.c"
#include <math.h>
#include <stdio.h>
#include <stdint.h>



// --- GENERAL FUNCS --- //
int hexprintf_Byte(unsigned char c) {
    if (c < 16) {
        printf("0x0%X ", c);
    } else {
        printf("0x%X ", c);
    }
}
int hexprintf(unsigned char * c, int length) {
    printf("0x");
    for (int i = 0; i < length; i++) {
        if (c[i] < 16) {
            printf("0%X", c[i]);
        } else {
            printf("%X", c[i]);
        }
    }
    printf(" ");
}
int hexprintf_Rev(unsigned char * c, int length) {
    printf("0x");
    for (int i = length-1; i >= 0; i--) {
        if (c[i] < 16) {
            printf("0%X", c[i]);
        } else {
            printf("%X", c[i]);
        }
    }
    printf(" ");
}
int asciiprintf(unsigned char * c, int length) {
    for (int i = 0; i < length; i++) {
        if (c[i] < 127 && c[i] > 31) {
        printf("%c", c[i]); printf(""); }
        else { printf("."); }
    }
}
int asciiprintf_Signed(char * c, int length) {
    for (int i = 0; i < length; i++) {
        if (c[i] < 127 && c[i] > 31) {
        printf("%c", c[i]); printf(""); }
        else { printf("."); }
    }
}

// --- DEBUG FUNCS --- //
void PrintSectionHeaderInfo(Section_Table * pSectionHeader) {
    printf("=== === === ===\n");

    printf("Section: \n");
    asciiprintf(pSectionHeader->Name, 8); printf("\n");

    printf("Virual Size: %d\n", b16to10_4Bytes(pSectionHeader->Virtual_Size));
    printf("Virual Address: %d\n", b16to10_4Bytes(pSectionHeader->Virtual_Address));

    printf("Size of Raw Data: %d\n", b16to10_4Bytes(pSectionHeader->Size_Of_Raw_Data));
    //hexprintf(pSectionHeader->Size_Of_Raw_Data, 4); std::cout << std::endl;

    printf("Pointer to Raw Data: ");
    hexprintf(pSectionHeader->Pointer_To_Raw_Data, 4); printf("\n");

    printf("Pointer to Relocations: ");
    hexprintf(pSectionHeader->Pointer_To_Relocations, 4); printf("\n");

    printf("Pointer to Line Numbers: ");
    hexprintf(pSectionHeader->Pointer_To_Line_Numbers, 4); printf("\n");

    printf("Number of Relocations: %d\n", b16to10_2Bytes(pSectionHeader->Number_Of_Relocations));
    printf("Number of Line Numbers: %d\n", b16to10_2Bytes(pSectionHeader->Number_Of_Line_Numbers));

    printf("Characteristics (flags): ");
    hexprintf_Rev(pSectionHeader->Characteristics, 4); printf("\n");

    printf("The data for this section is at: 0x%X\n", pSectionHeader->section_data);

    printf("=== === === ===\n");
    printf("\n");
}

#endif
