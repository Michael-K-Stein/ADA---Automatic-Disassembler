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
#include "src/Decompilation/DisassembleMaster.h"
#include "stdio.h"

using namespace std;

std::map<char, string> _OP_CODES = {
    { 0x90, "NOP" },
    { 0xAB, "STOSW" },
    { 0xF0, "Jcc" }
};


unsigned char * getASMInput() {
    unsigned char * inp = (unsigned char *)malloc(17 * sizeof(unsigned char));
    int * inp2 = (int *)malloc(17 * sizeof(int));
    bool finished = false;
    cout << endl << "Input: ";
    for (int i = 0; i < 16; i++) {
        if (!finished) {
            cin >> std::hex >> inp2[i];
            finished = inp2[i] >= 256;
            inp[i] = (inp2[i]% 256);
        } else { inp[i] = 0; }
    }
    inp[16] = '\0';

    cout << endl<<endl;
    return inp;
}







//extern "C" void foo(); // one way
int main()
{
    while (1) {
        char * dec = (char *)calloc(64, sizeof(char));
        Disassemble(getASMInput(),dec);
        //Decomp_0x0X(getASMInput(), dec);
        printf(dec); printf("\n\n\n");
    }
    //return StartUp();

    return 0;
}
