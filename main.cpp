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
#include <map>
#include <iostream>
#include <string>
#include "src/source.h"
#include "src/Disassembly/DisassembleMaster.h"
#include "stdio.h"

using namespace std;


std::map<char, string> _OP_CODES = {
    { 0x90, "NOP" },
    { 0xAB, "STOSW" },
    { 0xF0, "Jcc" }
};


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


//extern "C" void foo(); // one way
int main()
{
    FILE * fPtr =fopen("disassTest", "rb");
    uint64_t fsize = getFileSize("disassTest");
    unsigned char * buf = (unsigned char *)calloc(fsize, sizeof(unsigned char));

    printf("File size is: %d", fsize);

    if (fread(buf, fsize, sizeof(unsigned char), fPtr)) {
        int codeOffset = 0;
        while (codeOffset < fsize) {
            char * dec = (char *)calloc(64, sizeof(char));
            int opCodeOffset = Disassemble(buf + codeOffset, dec, false, codeOffset);
            printf("\n%s : %d", dec, opCodeOffset);
            codeOffset+= opCodeOffset;
        }
    }

    while (1) {
        unsigned char * input = (unsigned char *)calloc(16, sizeof(unsigned char));
        int len = getASMInput(input);
        int codeOffset = 0;
        while (codeOffset < len) {
            char * dec = (char *)calloc(64, sizeof(char));
            int opCodeOffset = Disassemble(input + codeOffset, dec, true, codeOffset);
            printf("\n\n\n%s : %d", dec, opCodeOffset); printf("\n\n\n");
            codeOffset+= opCodeOffset;
        }
    }
    //return StartUp();

    return 0;
}
