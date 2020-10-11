//#ifndef __DecompGetStrings
//#define __DecompGetStrings

#include "GetStrings.h"

/**
    Constant strings in Portable Executable files should be found in the section ".rdata".
    These "valid" strings will also be suffixed by '\0' (the NULL character).
*/
/*
unsigned char rdataName[] = { 0x2E, 0x72, 0x64, 0x61, 0x74, 0x61, 0x0, 0x0 };

int GetStringsAboveLength(Portable_Executable * PE, int minLength) {

    // Find the index of .rdata
    int rdataIndex = -1;
    for (int i = 0; i < getNumberOfSections(PE); i++) {
        if ( PE->section_tables[i].Name == rdataName ) {
            rdataIndex = i;
        }
    }

    //PE->portable_executable_header.coff_header.Number_Of_Sections
    //unsigned char * data = PE->section_tables

    return rdataIndex;
}*/

int foo(int a) {return 0;}

//#endif // __DecompGetStrings
