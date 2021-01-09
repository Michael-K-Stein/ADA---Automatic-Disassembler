#ifndef GETDISASSEMBLY_H_INCLUDED
#define GETDISASSEMBLY_H_INCLUDED

#define sprintf sprintf_s

#include "CommonAnalysis.h"
#include "../../src/Disassembly/DisassembleMaster.h"

typedef struct Func {
	uint16_t start;
	uint32_t entryP;
	char entry[64];
	uint16_t length;
} Func;

char * getOriginalOpCodes(unsigned char * buf, int length) {
    char * origOpCodes = (char *)calloc(length * 6, sizeof(char));
    for (int ind = 0; ind < length; ind++) {
        sprintf(origOpCodes + (ind * 3), 4, "%.2X ", buf[ind]);
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

        int sectionOffset = b16to10_4Bytes(PE->section_tables[textIndex].Virtual_Address) + b16to10_4Bytes(PE->portable_executable_header.windows_specific_fields.Image_Base);

        printf("\n\nSection size to analyze: %d\n\n", bufSize);

		Func * funcs = (Func *)calloc(100, sizeof(Func)); int amtFuncs = 0;

        EXTRA extra; extra.DEBUG_MODE = false; extra.FORCE = true;

        double ratT = 0; double ratC = 0; double ratF = 0;

        int codeOffset = 0;
        printf("\n\n\n");
        while (codeOffset < bufSize) {
            char * dec = (char *)calloc(256, sizeof(char));
            int opCodeOffset = Disassemble(buf + codeOffset, dec, true, codeOffset, extra);
            if (opCodeOffset > 0 && opCodeOffset != 0xFFFF) {
                    ratC++;
                if (true) {

					if (isCall) {
						isCall = false;
						Func f;
						f.start = codeOffset;
						f.length = opCodeOffset;
						memcpy(f.entry, callAddr, 64);
						funcs[amtFuncs++] = f;
					}

                    char * origOpCodes = (char *)calloc(opCodeOffset * 6, sizeof(char)); 
					memcpy(origOpCodes, getOriginalOpCodes(buf + codeOffset, opCodeOffset), (opCodeOffset * 5) - 1);
                    char * tableDelim = (char *)calloc(16, sizeof(char)); 
					for (int i = 0; i < 8 - floor((opCodeOffset*3) /8.0); i++) { tableDelim[i] = '\t'; }
					char * allOut = (char *)calloc(256, sizeof(char));
                    sprintf(allOut, 256, "\n0x%.8X\t|\t%s\t%s%s", codeOffset + sectionOffset, origOpCodes, tableDelim, dec);
					printf(allOut);
					free(allOut);
					free(origOpCodes);
					free(tableDelim);
                } else {
                    printf("\n%s : %d", dec, opCodeOffset);
                }
                codeOffset+= opCodeOffset;
            } else if (opCodeOffset == 0xFFFF) {
                char * tableDelim = (char *)calloc(6, sizeof(char)); for (int i = 0; i < 4 - ceil(2 /2.0); i++) { tableDelim[i] = '\t'; }
                printf("\n0x%.8X\t|\tFF FF\t%s%s", codeOffset + sectionOffset, tableDelim, dec);
                codeOffset = bufSize;
            } else {
                ratF++;
                if (extra.FORCE) { codeOffset++; }
                else {
                printf("Something is broken!!!");}
            }
            free(dec);
        }
        ratT = ratC + ratF;
        printf("\n\n\n");
        printf("Correct: %f | Incorrect: %f | Ratio (C:F): %.2f:1 | Ratio of know (C/T): %f", ratC, ratF, (double)(ratC/ratF), 100*(double)(ratC/ratT) );
        printf("\n\n\n");

		printf("Functions found: %d\n", amtFuncs);
		for (int i = 0; i < amtFuncs; i++) {
			char * dec = (char *)calloc(256, sizeof(char));
			int opCodeOffset = Disassemble(buf + funcs[i].start, dec, true, funcs[i].start, extra);

			funcs[i].entryP = funcs[i].start + opCodeOffset + (int)strtol(funcs[i].entry, NULL, 16) + sectionOffset;

			printf("Func call #%d, at 0x%.8X, calls 0x%.8X with the command: %s\n", i, funcs[i].start, funcs[i].entryP, dec);
			free(dec);
		}
		free(funcs);

    } else { return 1;}
    return 0;
}

#endif // GETDISASSEMBLY_H_INCLUDED
