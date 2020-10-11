#ifndef GETSTRINGS_H_INCLUDED
#define GETSTRINGS_H_INCLUDED

#include "../Parse/DOS_Format.h"
#include "../PrintFuncs.h"

/**
    Constant strings in Portable Executable files should be found in the section ".rdata".
    These "valid" strings will also be suffixed by '\0' (the NULL character).
*/

unsigned char rdataName[] = { 0x2E, 0x72, 0x64, 0x61, 0x74, 0x61, 0x0, 0x0 };

char * GetStringsAboveLength(Portable_Executable * PE, int minLength) {
    unsigned char rdataNameHEX[] = { 0x2E, 0x72, 0x64, 0x61, 0x74, 0x61, 0x0, 0x0 };
    // Find the index of .rdata
    int rdataIndex = -1;
    for (int i = 0; i < getNumberOfSections(PE); i++) {
        if (memcmp(PE->section_tables[i].Name, rdataNameHEX, sizeof(rdataNameHEX)) == 0) {
            rdataIndex = i;
        }
    }

    unsigned char * data = PE->section_tables[rdataIndex].section_data->data;

    unsigned char * tmpString = (unsigned char *)malloc(b16to10_4Bytes(PE->section_tables[rdataIndex].Size_Of_Raw_Data));

    char * allStrings = (char *)malloc(b16to10_4Bytes(PE->section_tables[rdataIndex].Size_Of_Raw_Data));
    int stringsInd = 0;

    int sInd = 0;
    for (int i = 0; i < b16to10_4Bytes(PE->section_tables[rdataIndex].Size_Of_Raw_Data); i++) {
        tmpString[sInd] = data[i];
        // Printable range is 32 <= x <= 126
        if (data[i] == '\0' || ( data[i] < 32 || data[i] > 126 )) {
            if (sInd >= minLength) {
                tmpString[sInd] = '\n';
                //printf("%s\n", tmpString);
                for (int ind = 0; ind <= sInd; ind++) {
                    allStrings[stringsInd] = tmpString[ind];
                    stringsInd++;
                }
            }
            sInd = 0;
        } else {sInd++; }
    }

    return allStrings;
}

void PrintStrings(Portable_Executable * PE, int minLength) {
        char * Strings;
        Strings = GetStringsAboveLength(PE,minLength);

        // Print the strings:
        while ( strchr(Strings, '\n') ) {
            asciiprintf_Signed(Strings, strchr(Strings, '\n') - Strings);
            printf("\n");
            Strings = strchr(Strings, '\n')+1;
        }
}

#endif // GETSTRINGS_H_INCLUDED
