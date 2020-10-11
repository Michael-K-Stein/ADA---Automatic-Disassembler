#ifndef __sourceC
#define __sourceC

#include "Parse/LoadFile.h"
#include "AnalyzingFuncs.h"
#include "PrintFuncs.h"

#include <string.h>
#include <stdio.h>
#include <memory.h>


int ValidatePathString(char * dest, char * inp, int len) {
    char * tmp = dest;
    for (int i = 0; i < len; i++) { tmp[i] = inp[i]; }

    int endInd = 255;

    if (tmp[0] == '"') {
        for (int i = 0; i < len-1; i++) {
            if (tmp[i+1] == '"') { tmp[i+1] = '\0'; }
            tmp[i] = tmp[i+1];
            if (tmp[i] == '\0') { endInd = i+1; break; }
        }
        for(int i = endInd; i < len; i++) {
            tmp[i] = '\0';
        }
    } else {

    }
    dest = tmp;

    return 0;
}




int StartUp() {
    char filePath[256];
    printf("Please enter file path: ");
    if (gets(filePath)){
        char * newPath = (char *)malloc(256);
        ValidatePathString(newPath, filePath, 256);
        printf("Chosen file path: %s\n\n", newPath);

        Portable_Executable PE = LoadFile(filePath);

        printf("Is valid PE format: %d\n", VerifyPEFormat(&PE));

        printf("\n\n\n");

        printf("Please enter an analysis command or leave blank to continue: ");
        char funcCMD[64];
        scanf("%s", &funcCMD);

        Analyze(funcCMD, &PE);

        PrintSectionHeaderInfo(&PE.section_tables[0]);

        hexprintf(PE.portable_executable_header.dos_header.DOS_Signature, 2);              asciiprintf(PE.portable_executable_header.dos_header.DOS_Signature, 2);              printf("\n");
        asciiprintf(PE.portable_executable_header.dos_header.Header_Data, 58);                printf("\n");
        hexprintf(PE.portable_executable_header.dos_header.DOS_Pointer_to_PE_Header, 4);   asciiprintf(PE.portable_executable_header.dos_header.DOS_Pointer_to_PE_Header, 4);   printf("\n");
        asciiprintf(PE.portable_executable_header.dos_header.DOS_STUB, 64);                    printf("\n");

        hexprintf(PE.portable_executable_header.data_directories.Zero_Check_Valid_1, 4);   printf("\n");
        hexprintf(PE.portable_executable_header.data_directories.Zero_Check_Valid_2, 8);   printf("\n");

        printf("\n\n\n");

        hexprintf(PE.portable_executable_header.coff_header.Number_Of_Sections, 2);           printf("\n");
        printf("Number of Sections: %d\n", getNumberOfSections(&PE));



        //if (GetStringsAboveLength(&PE, 30) == -1) {printf("FAIL");}else{printf("%d", GetStringsAboveLength(&PE, 14));}

        return 0;
    } else {
        return 1;
    }
}



#endif // __sourceC
