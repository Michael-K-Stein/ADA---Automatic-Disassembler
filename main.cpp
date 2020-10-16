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
#include "src/source.h"
#include "src/Decompilation/_00.h"
#include "stdio.h"

using namespace std;

std::map<char, string> _OP_CODES = {
    { 0x90, "NOP" },
    { 0xAB, "STOSW" },
    { 0xF0, "Jcc" }
};




//extern "C" void f(); // one way
int main()
{
    unsigned char demoOpCode[8] = { 0x04, 0x50, 0x50, 0x50, 0x50, 0x00, 0x00 };


    char * dec = (char *)calloc(32, sizeof(char));
    Decomp_0x0X(demoOpCode, dec);
    printf(dec); printf("\n\n\n");

    //return StartUp();

    return 0;
}
