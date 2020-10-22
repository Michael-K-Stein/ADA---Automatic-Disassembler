#ifndef DISASSEMBLEMASTER_H_INCLUDED
#define DISASSEMBLEMASTER_H_INCLUDED

#include "Common.h"
#include "_00.h"

typedef struct PrefixOut {
    int prefixOffset;

    bool Operand_Size_Override;
    bool Address_Size_Override;
};

PrefixOut WritePrefixes(unsigned char * opCodes, char * output, int *OpCodeOffset) {
    unsigned char * prefixes = (unsigned char *)calloc(sizeof(unsigned char), 4);

    int offset = 0;

    bool Operand_Size_Override = false;
    bool Address_Size_Override = false;

    /// Try loading the maximum of 4 prefix bytes
    for (int prefixInd = 0; prefixInd < 4; prefixInd++) {
        for (int prefType = 0; prefType < 11; prefType++){
            if (PREFIXES_LIST[prefType] == opCodes[prefixInd]) {
                prefixes[prefixInd] = opCodes[prefixInd];
            }
        }
    }

    for (int i = 0; i < 4; i++) {
        if (prefixes[i] != 0x00) {
            *OpCodeOffset+=1;
            printf("0x%02X\n", prefixes[i]);
            switch (prefixes[i]) {
            case LOCK:
                memcpy(output + offset, &"LOCK ",5);
                offset += 5;
                break;
            case REPE:
                memcpy(output + offset, &"REPE ",5);
                offset += 5;
                break;
            case REPNE:
                memcpy(output + offset, &"REPNE ",6);
                offset += 6;
                break;
            case ES:
                memcpy(output + offset, &"ES ",3);
                offset += 3;
                break;
            case CS:
                memcpy(output + offset, &"CS ",3);
                offset += 3;
                break;
            case SS:
                memcpy(output + offset, &"SS ",3);
                offset += 3;
                break;
            case DS:
                memcpy(output + offset, &"DS ",3);
                offset += 3;
                break;
            case FS:
                memcpy(output + offset, &"FS ",3);
                offset += 3;
                break;
            case GS:
                memcpy(output + offset, &"GS ",3);
                offset += 3;
                break;
            case OPERAND_OVERRIDE:
                Operand_Size_Override = true;
                break;
            case ADDRESS_OVERRIDE:
                Address_Size_Override = true;
                break;
            }
        }
    }

    PrefixOut prefOut;
    prefOut.prefixOffset = offset;
    prefOut.Address_Size_Override = Address_Size_Override;
    prefOut.Operand_Size_Override = Operand_Size_Override;

    return prefOut;
}

int generateRM_ZERO_BYTE_DISPLACEMENT(unsigned char * opCodes, char * output, char rmVal, bool address_override) {
    if (address_override) { // 16 bit
         /// second bit BX|BP, third bit SI|DI
        char * reg = (char *)malloc(2);
        char * regAct = {"BXBP"};
        reg = (rmVal >> 1) == 0 ? regAct : regAct+2;
        char * rof = (char *)malloc(2);
        char * rofAct = {"SIDI"};
        rof = (rmVal >> 1) == 0 ? rofAct : rofAct+2;

        if (rmVal < 4) {
            sprintf(output, "[%s + %s]", reg, rof);
        } else if (rmVal >> 2 == 1 && rmVal >> 1 == 0){
            sprintf(output, "[%s]", rof);
        } else if (rmVal == 0b110) { /** ONLY DISPLACEMENT 16bit */}
        else if (rmVal == 0b111) { sprintf(output, "[%s]", &"BX"); }
    } else { // 32 bit
        if (rmVal == 0b100) { /// SIB

        } else if (rmVal == 0b101) { /// ONLY DISP 32bit
        } else {
            sprintf(output, "[%s]", getRegisterName_32b(rmVal));
        }
    }
}
int generateRM(unsigned char * opCodes, char * output, bool address_override, bool size_override) { // opCodes should be given starting right after the prefixes. Including the action op code.
    char mod = getMod(opCodes);
    char rmVal = opCodes[1] % 8;

    switch (mod) {
        case ZERO_BYTE_DISPLACEMENT: {
            return generateRM_ZERO_BYTE_DISPLACEMENT(opCodes, output, rmVal, address_override);
            break;
        }
    }
}

int Disassemble(unsigned char * opCodes, char * output) {
    int OpCodeOffset = 0;
    PrefixOut prefixOut = WritePrefixes(opCodes + OpCodeOffset, output, &OpCodeOffset);
    printf("OpCodeOffset: %d\n", OpCodeOffset);
    int codeOffset = Decomp_0x0X(opCodes + OpCodeOffset, output + prefixOut.prefixOffset, prefixOut.Operand_Size_Override, prefixOut.Address_Size_Override);
    //sprintf(dec, )
}

#endif // DISASSEMBLEMASTER_H_INCLUDED
