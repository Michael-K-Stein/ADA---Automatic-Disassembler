#ifndef GETSTRINGS_H_INCLUDED
#define GETSTRINGS_H_INCLUDED

#include "../Parse/DOS_Format.h"
#include "../PrintFuncs.h"

#include "CommonAnalysis.h"

/**
    Constant strings in Portable Executable files should be found in the section ".rdata".
    These "valid" strings will also be suffixed by '\0' (the NULL character).
*/

const char * sectionNames[] = { rdataName , rsrcName , textName };
/// ========= ADD IMPLEMENTATION OF .RSRC

char * GetStringsAboveLength(Portable_Executable * PE, int minLength) {
    unsigned char secName[8];

    char * allStrings = (char *)malloc(b16to10_4Bytes(PE->portable_executable_header.standard_coff_fields.Size_Of_Code));
    int stringsInd = 0;

    for (int rsecNameInd = 0; rsecNameInd < sizeof(sectionNames) / sizeof(sectionNames[0]); rsecNameInd++) {

        // Find the index of .rdata or .rsrc or other resource sections
        int rdataIndex = -1;
        for (int i = 0; i < getNumberOfSections(PE); i++) {
            if (memcmp(PE->section_tables[i].Name, sectionNames[rsecNameInd], sizeof(sectionNames[rsecNameInd])) == 0) {
                rdataIndex = i;
            }
        }
        if (rdataIndex != -1) {
            unsigned char * data = PE->section_tables[rdataIndex].section_data->data;

            unsigned char * tmpString = (unsigned char *)malloc(b16to10_4Bytes(PE->section_tables[rdataIndex].Size_Of_Raw_Data));

            int sInd = 0;
            // Check for ascii
            for (int i = 0; i < b16to10_4Bytes(PE->section_tables[rdataIndex].Size_Of_Raw_Data); i++) {
                tmpString[sInd] = data[i];
                // Printable range is 32 <= x <= 126
                if (data[i] == '\0' || ( data[i] < 32 || data[i] > 126 )) {
                    if (sInd >= minLength) {
                        tmpString[sInd] = '\n';
                        for (int ind = 0; ind <= sInd; ind++) {
                            allStrings[stringsInd] = tmpString[ind];
                            stringsInd++;
                        }
                    }
                    sInd = 0;
                } else {sInd++; }
            }

            sInd = 0;
            // Check for unicode "ascii"
            for (int i = 0; i < b16to10_4Bytes(PE->section_tables[rdataIndex].Size_Of_Raw_Data); i++) {
                tmpString[sInd] = data[i];
                // Printable range is 32 <= x <= 126
                if (data[i] == '\0' || ( data[i] < 32 || data[i] > 126 )) {
                    if (sInd >= minLength) {
                        tmpString[sInd] = '\n';
                        for (int ind = 0; ind <= sInd; ind++) {
                            allStrings[stringsInd] = tmpString[ind];
                            stringsInd++;
                        }
                    }
                    sInd = 0;
                } else {sInd++; }
                i++;
                if (data[i] == '\0') { /* Good, it's ascii, continue */ } else {
                    // It isn't ascii, reset.
                    sInd = 0;
                }
            }
        }
    }
    return allStrings;
}

bool IsBase64(unsigned char * text, int length) {
    bool flag = true;

    for (int i = 0; i < length; i++) {
        flag = flag && ( ( 48 <= text[i] && text[i] <= 57 ) || ( 65 <= text[i] && text[i] <= 90 ) || ( 97 <= text[i] && text[i] <= 122 ) );
    }

    return flag;
}

char * GetHashes(Portable_Executable * PE) {
    unsigned char secName[8];

    char * allStrings = (char *)malloc(b16to10_4Bytes(PE->portable_executable_header.standard_coff_fields.Size_Of_Code));
    int stringsInd = 0;

    for (int rsecNameInd = 0; rsecNameInd < sizeof(sectionNames) / sizeof(sectionNames[0]); rsecNameInd++) {

        // Find the index of .rdata or .rsrc or other resource sections
        int rdataIndex = -1;
        for (int i = 0; i < getNumberOfSections(PE); i++) {
            if (memcmp(PE->section_tables[i].Name, sectionNames[rsecNameInd], sizeof(sectionNames[rsecNameInd])) == 0) {
                rdataIndex = i;
            }
        }

        if (rdataIndex != -1) {
            unsigned char * data = PE->section_tables[rdataIndex].section_data->data;

            unsigned char * tmpString = (unsigned char *)malloc(b16to10_4Bytes(PE->section_tables[rdataIndex].Size_Of_Raw_Data));


            int sInd = 0;
            for (int i = 0; i < b16to10_4Bytes(PE->section_tables[rdataIndex].Size_Of_Raw_Data); i++) {
                tmpString[sInd] = data[i];
                // Printable range is 32 <= x <= 126
                if (data[i] == '\0' || ( data[i] < 32 || data[i] > 126 )) {
                        /**     md5 = 32bytes

                        */

                    if (sInd == 32) {
                        if (IsBase64(tmpString, sInd)) {
                            tmpString[sInd] = '\n';
                            for (int ind = 0; ind <= sInd; ind++) {
                                allStrings[stringsInd] = tmpString[ind];
                                stringsInd++;
                            }
                        }
                    }
                    sInd = 0;
                } else {sInd++; }
            }

            sInd=0;
            // Check for unicode "ascii"
            for (int i = 0; i < b16to10_4Bytes(PE->section_tables[rdataIndex].Size_Of_Raw_Data); i++) {
                tmpString[sInd] = data[i];
                // Printable range is 32 <= x <= 126
                if (data[i] == '\0' || ( data[i] < 32 || data[i] > 126 )) {
                    /**     md5 = 32bytes

                    */

                    if (sInd == 32) {
                        if (IsBase64(tmpString,sInd)) {
                            tmpString[sInd] = '\n';
                            for (int ind = 0; ind <= sInd; ind++) {
                                allStrings[stringsInd] = tmpString[ind];
                                stringsInd++;
                            }
                        }
                    }
                    sInd = 0;
                } else {sInd++; }
                i++;
                if (data[i] == '\0') { /* Good, it's ascii, continue */ } else {
                    // It isn't ascii, reset.
                    sInd = 0;
                }
            }

        }
    }
    return allStrings;
}

void PrintHashes(Portable_Executable * PE) {
    char * Strings;
    Strings = GetHashes(PE);

    // Print the strings:
    while ( strchr(Strings, '\n') ) {
        asciiprintf_Signed(Strings, strchr(Strings, '\n') - Strings);
        printf("\n");
        Strings = strchr(Strings, '\n')+1;
    }
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
