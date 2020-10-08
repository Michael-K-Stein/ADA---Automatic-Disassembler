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

#include "Parse/LoadFile.c"

#include <iostream>
#include <map>
#include <string>
#include <string.h>
#include <stdio.h>
#include <memory.h>

//#include "PrintFuncs.c"


using namespace std;

std::map<char, string> _OP_CODES = {
    { 0x90, "NOP" },
    { 0xAB, "STOSW" },
    { 0xF0, "Jcc" }
};

// --- GENERAL FUNCS --- //
int hexprintf_Byte(unsigned char c) {
    if (c < 16) {
        printf("0x0%X ", c);
    } else {
        printf("0x%X ", c);
    }
}
int hexprintf(unsigned char * c, int length) {
    printf("0x");
    for (int i = 0; i < length; i++) {
        if (c[i] < 16) {
            printf("0%X", c[i]);
        } else {
            printf("%X", c[i]);
        }
    }
    printf(" ");
}
int hexprintf_Rev(unsigned char * c, int length) {
    printf("0x");
    for (int i = length-1; i >= 0; i--) {
        if (c[i] < 16) {
            printf("0%X", c[i]);
        } else {
            printf("%X", c[i]);
        }
    }
    printf(" ");
}
int asciiprintf(unsigned char * c, int length) {
    for (int i = 0; i < length; i++) {
        if (c[i] < 127 && c[i] > 31) {
        printf("%c", c[i]); printf(" "); }
        else { printf(". "); }
    }
}

// --- DEBUG FUNCS --- //
void PrintSectionHeaderInfo(Section_Table * pSectionHeader) {
    printf("=== === === ===\n");

    printf("Section: \n");
    asciiprintf(pSectionHeader->Name, 8); printf("\n");

    printf("Virual Size: %d\n", b16to10_4Bytes(pSectionHeader->Virtual_Size));
    printf("Virual Address: %d\n", b16to10_4Bytes(pSectionHeader->Virtual_Address));

    printf("Size of Raw Data: %d\n", b16to10_4Bytes(pSectionHeader->Size_Of_Raw_Data));
    //hexprintf(pSectionHeader->Size_Of_Raw_Data, 4); std::cout << std::endl;

    printf("Pointer to Raw Data: ");
    hexprintf(pSectionHeader->Pointer_To_Raw_Data, 4); printf("\n");

    printf("Pointer to Relocations: ");
    hexprintf(pSectionHeader->Pointer_To_Relocations, 4); printf("\n");

    printf("Pointer to Line Numbers: ");
    hexprintf(pSectionHeader->Pointer_To_Line_Numbers, 4); printf("\n");

    printf("Number of Relocations: %d\n", b16to10_2Bytes(pSectionHeader->Number_Of_Relocations));
    printf("Number of Line Numbers: %d\n", b16to10_2Bytes(pSectionHeader->Number_Of_Line_Numbers));

    printf("Characteristics (flags): ");
    hexprintf_Rev(pSectionHeader->Characteristics, 4); printf("\n");

    printf("The data for this section is at: %d\n", pSectionHeader->section_data);

    printf("=== === === ===\n");
    printf("\n");
}


//extern "C" void f(); // one way
int main()
{
    Portable_Executable PE = LoadFile("2.exe");

    cout << endl;
    cout << endl;
    cout << endl;

    cout << "Is valid PE format: " << VerifyPEFormat(&PE) << endl;

    PrintSectionHeaderInfo(&PE.section_tables[0]);

    cout << endl;
    cout << endl;
    cout << endl;

    hexprintf(PE.portable_executable_header.dos_header.DOS_Signature, 2);              asciiprintf(PE.portable_executable_header.dos_header.DOS_Signature, 2);              cout << endl;
    asciiprintf(PE.portable_executable_header.dos_header.Header_Data, 58);                cout << endl;
    hexprintf(PE.portable_executable_header.dos_header.DOS_Pointer_to_PE_Header, 4);   asciiprintf(PE.portable_executable_header.dos_header.DOS_Pointer_to_PE_Header, 4);   cout << endl;
    asciiprintf(PE.portable_executable_header.dos_header.DOS_STUB, 64);                    cout << endl;

    hexprintf(PE.portable_executable_header.data_directories.Zero_Check_Valid_1, 4);   cout << endl;
    hexprintf(PE.portable_executable_header.data_directories.Zero_Check_Valid_2, 8);   cout << endl;

    cout << endl;
    cout << endl;
    cout << endl;

    hexprintf(PE.portable_executable_header.coff_header.Number_Of_Sections, 2);           cout << endl;
    cout << getNumberOfSections(&PE) << endl;

    /*for (int i = 0; i < getNumberOfSections(&PE); i++) {
        hexprintf(PE.section_table.Name, 8); cout << endl; asciiprintf(PE.section_table.Name, 8);
    }*/


    //cout << PE.section_tables[2].section_data->data << endl;

    for (int i = 0; i < b16to10_4Bytes(PE.section_tables[2].Size_Of_Raw_Data); i++) {
        cout << PE.section_tables[2].section_data->data[i] << " ";
    }

    //asciiprintf(PE.section_tables[2].section_data->data, 16); cout << endl;
    cout << endl;
    cout << endl;
    cout << endl;

    return 0;
}
