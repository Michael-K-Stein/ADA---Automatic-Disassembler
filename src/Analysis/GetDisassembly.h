#ifndef GETDISASSEMBLY_H_INCLUDED
#define GETDISASSEMBLY_H_INCLUDED

#include "CommonAnalysis.h"
#include "../../src/Disassembly/DisassembleMaster.h"

char * getOriginalOpCodes(unsigned char * buf, int length) {
    char * origOpCodes = (char *)calloc(length, 3 * sizeof(char));
    for (int ind = 0; ind < length; ind++) {
        sprintf(origOpCodes + (ind * 3), "%.2X ", buf[ind]);
    }

    return origOpCodes;
}

int PrintDisassembly(Portable_Executable * PE) {

    int textIndex = -1;
    for (int i = 0; i < getNumberOfSections(PE); i++) {
        if (memcmp(PE->section_tables[i].Name, textName, sizeof(textName)) == 0) {
            textIndex = i;
        }
    }

    if (textIndex != -1) {

        unsigned char * buf = PE->section_tables[textIndex].section_data->data;
        int bufSize = b16to10_4Bytes(PE->section_tables[textIndex].Size_Of_Raw_Data);

        printf("\n\nSection size to analyze: %d\n\n", bufSize);

        EXTRA extra; extra.DEBUG_MODE = false; extra.FORCE = true;

        int codeOffset = 0;
        printf("\n\n\n");
        while (codeOffset < bufSize) {
            char * dec = (char *)calloc(64, sizeof(char));
            int opCodeOffset = Disassemble(buf + codeOffset, dec, true, codeOffset, extra);
            if (opCodeOffset > 0 && opCodeOffset != 0xFFFF) {
                if (true) {
                    char * origOpCodes = (char *)calloc(opCodeOffset, 3 * sizeof(char)); memcpy(origOpCodes, getOriginalOpCodes(buf + codeOffset, opCodeOffset), (opCodeOffset * 3) - 1);
                    char * tableDelim = (char *)calloc(6, sizeof(char)); for (int i = 0; i < 4 - ceil(opCodeOffset /2.0); i++) { tableDelim[i] = '\t'; }
                    printf("\n0x%.5X\t|\t%s\t%s%s", codeOffset, origOpCodes, tableDelim, dec);
                    free(origOpCodes);
                    free(tableDelim);
                } else {
                    printf("\n%s : %d", dec, opCodeOffset);
                }
                codeOffset+= opCodeOffset;
            } else if (opCodeOffset == 0xFFFF) {
                char * tableDelim = (char *)calloc(6, sizeof(char)); for (int i = 0; i < 4 - ceil(2 /2.0); i++) { tableDelim[i] = '\t'; }
                printf("\n0x%.5X\t|\tFF FF\t%s%s", codeOffset, tableDelim, dec);
                codeOffset = bufSize;
            } else {
                if (extra.FORCE) { codeOffset++; }
                else {
                printf("Something is broken!!!");}
            }
            free(dec);
        }
        printf("\n\n\n");

    } else { return 1;}
}

#endif // GETDISASSEMBLY_H_INCLUDED
