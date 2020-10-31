#include <map>
#include <iostream>
#include <string>
#include "src/source.h"
#include "src/Disassembly/DisassembleMaster.h"
#include "stdio.h"

using namespace std;


int getASMInput(unsigned char * dest) {
    unsigned char * inp = (unsigned char *)malloc(17 * sizeof(unsigned char));
    int * inp2 = (int *)malloc(17 * sizeof(int));
    bool finished = false;
    int len = 0;
    cout << endl << "Input: ";
    for (int i = 0; i < 16; i++) {
        if (!finished) {
            cin >> std::hex >> inp2[i];
            finished = inp2[i] >= 256;
            inp[i] = (inp2[i]% 256);
            len++;
        } else { inp[i] = 0; }
    }
    inp[16] = '\0';
    len--;
    cout << endl<<endl;
    memcpy(dest, inp,  16 * sizeof(char));
    return len;
}

char * getOriginalOpCodes(unsigned char * buf, int length) {
    char * origOpCodes = (char *)calloc(length, 3 * sizeof(char));
    for (int ind = 0; ind < length; ind++) {
        sprintf(origOpCodes + (ind * 3), "%.2X ", buf[ind]);
    }

    return origOpCodes;
}

#define PRETTY_PRINTING true

int main()
{
    EXTRA extra; extra.DEBUG_MODE = false;

    FILE * fPtr =fopen("disassTest", "rb");
    uint64_t fsize = getFileSize("disassTest");
    unsigned char * buf = (unsigned char *)calloc(fsize, sizeof(unsigned char));

    printf("File size is: %d", fsize);

    if (fread(buf, fsize, sizeof(unsigned char), fPtr)) {
        int codeOffset = 0;
        while (codeOffset < fsize) {
            char * dec = (char *)calloc(64, sizeof(char));
            int opCodeOffset = Disassemble(buf + codeOffset, dec, false, codeOffset, extra);
            if (PRETTY_PRINTING) {
                char * origOpCodes = (char *)calloc(opCodeOffset, 3 * sizeof(char)); memcpy(origOpCodes, getOriginalOpCodes(buf + codeOffset, opCodeOffset), (opCodeOffset * 3) - 1);
                char * tableDelim = (char *)calloc(5, sizeof(char)); for (int i = 0; i < 3 - ceil(opCodeOffset/2.0); i++) { tableDelim[i] = '\t'; }
                printf("\n0x%.2X\t|\t%s\t%s%s", codeOffset, origOpCodes, tableDelim, dec);
            } else {
                printf("\n%s : %d", dec, opCodeOffset);
            }
            codeOffset+= opCodeOffset;
        }
    }

    while (1) {
        unsigned char * input = (unsigned char *)calloc(16, sizeof(unsigned char));
        int len = getASMInput(input);
        int codeOffset = 0;
        while (codeOffset < len) {
            char * dec = (char *)calloc(64, sizeof(char));
            int opCodeOffset = Disassemble(input + codeOffset, dec, true, codeOffset, extra);
            printf("\n\n\n%s : %d", dec, opCodeOffset); printf("\n\n\n");
            codeOffset+= opCodeOffset;
        }
    }
    //return StartUp();

    return 0;
}
