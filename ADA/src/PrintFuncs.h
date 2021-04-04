#ifndef __PrintFuncs
#define __PrintFuncs

#include <math.h>
#include <stdio.h>
#include <stdint.h>

#include "Analysis/CommonAnalysis.h"


// --- GENERAL FUNCS --- //
int hexprintf_Byte(unsigned char c) {
    if (c < 16) {
        printf("0x0%X ", c);
    } else {
        printf("0x%X ", c);
    }
	return 0;
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
	return 0;
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
	return 0;
}
int asciiprintf(unsigned char * c, int length) {
    for (int i = 0; i < length; i++) {
        if (c[i] < 127 && c[i] > 31) {
        printf("%c", c[i]); printf(""); }
        else { printf("."); }
    }
	return 0;
}
int asciiprintf_Signed(char * c, int length) {
    for (int i = 0; i < length; i++) {
        if (c[i] < 127 && c[i] > 31) {
        printf("%c", c[i]); printf(""); }
        else { printf("."); }
    }
	return 0;
}

// --- DEBUG FUNCS --- //
void PrintSectionHeaderInfo(Section_Table * pSectionHeader, Portable_Executable * PE) {
    printf("=== === === ===\n");

    printf("Section: \n");
    asciiprintf_Signed(pSectionHeader->Name, 8); printf("\n");

    printf("Virual Size: 0x%.8X\n", b16to10_4Bytes(pSectionHeader->Virtual_Size));
    printf("Virual Address: 0x%.8X\n", Address_relative_to_absolute(b16to10_4Bytes(pSectionHeader->Virtual_Address), PE));

    printf("Size of Raw Data: 0x%.8X\n", b16to10_4Bytes(pSectionHeader->Size_Of_Raw_Data));

    printf("Pointer to Raw Data: ");
	hexprintf_Rev(pSectionHeader->Pointer_To_Raw_Data, 4); printf("\n");

    printf("Pointer to Relocations: ");
	hexprintf_Rev(pSectionHeader->Pointer_To_Relocations, 4); printf("\n");

    printf("Pointer to Line Numbers: ");
	hexprintf_Rev(pSectionHeader->Pointer_To_Line_Numbers, 4); printf("\n");

    printf("Number of Relocations: %d\n", b16to10_2Bytes(pSectionHeader->Number_Of_Relocations));
    printf("Number of Line Numbers: %d\n", b16to10_2Bytes(pSectionHeader->Number_Of_Line_Numbers));

    printf("Characteristics (flags): ");
    hexprintf_Rev(pSectionHeader->Characteristics, 4); printf("\n");

    printf("The data for this section is at: 0x%X\n", pSectionHeader->section_data);

    printf("=== === === ===\n");
    printf("\n");
}

#endif
