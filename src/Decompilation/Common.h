#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED

// https://net.cs.uni-bonn.de/fileadmin/user_upload/plohmann/x86_opcode_structure_and_instruction_overview.pdf

/// This file has common data which is used in multiple commands.
/// Dear lord this shit is confusing.

typedef struct Bit {
    unsigned x:1;
} bit;
//typedef struct MOD { unsigned x:2; } mod;
//typedef struct REG { unsigned x:3; } reg;

enum MOD {
    ZERO_BYTE_DISPLACEMENT,
    ONE_BYTE_DISPLACEMENT,
    FOUR_BYTE_DISPLACEMENT,
    FIELD_IS_REGISTER
};

char getMod(unsigned char * opCodes) { printf("MOD: %d\n", opCodes[1] >> 6); return opCodes[1] >> 6; }

/// Prefixes
unsigned char PREFIXES_LIST[11] = { 0xF0, 0x66, 0x67, 0xF3, 0xF2, 0x2E, 0x36, 0x3E, 0x26, 0x64, 0x65 };
enum PREFIXES {
    LOCK = 0xF0,

    OPERAND_OVERRIDE = 0x66,
    ADDRESS_OVERRIDE = 0x67,

    // Repeat
    REPE = 0xF3,
    REPNE = 0xF2,

    // Segment Override
    CS = 0x2E,
    SS = 0x36,
    DS = 0x3E,
    ES = 0x26,
    FS = 0x64,
    GS = 0x65
};


/// Register values
// http://www.cs.loyola.edu/~binkley/371/Encoding_Real_x86_Instructions.html
char * REG_VALUE_8b = {"ALCLDLBLAHCHDHBH"};
char * REG_VALUE_16b = {"AXCXDXBXSPBPSIDI"};
char * REG_VALUE_32b = {"EAXECXEDXEBXESPEBPESIEDI"};
char * getRegisterName_8b(char regValue) { // 2.330sec for 10,000,000 runs
    printf("Register given is: %s\n", REG_VALUE_8b + (regValue*2));
    return REG_VALUE_8b + (regValue*2);
}
char * getRegisterName_16b(char regValue) {
    printf("Register given is: %s\n", REG_VALUE_16b + (regValue*2));
    return REG_VALUE_16b + (regValue*2);
}
char * getRegisterName_32b(char regValue) {
    printf("Register given is: %s\n", REG_VALUE_32b + (regValue*3));
    return REG_VALUE_32b + (regValue*3);
}

int getRegisterName(char regValue, bool s, bool SizeOverride, char mod, char * destination) { // Sets destination to the register name and returns the length of the register name
    if (s) {
        if (!SizeOverride) {
            memcpy(destination, getRegisterName_32b(regValue), 3);
            return 3;

        } else {
            memcpy(destination, getRegisterName_16b(regValue), 2);
            return 2;
        }
    } else {
        memcpy(destination, getRegisterName_8b(regValue), 2);
        return 2;
    }
}
int getRMName(char regValue, bool s, bool SizeOverride, char mod, char * destination) {
    if (mod ==0b11){
        if (s) {
            if (!SizeOverride) {
                memcpy(destination, getRegisterName_32b(regValue), 3);
                return 3;

            } else {
                memcpy(destination, getRegisterName_16b(regValue), 2);
                return 2;
            }
        } else {
            memcpy(destination, getRegisterName_8b(regValue), 2);
            return 2;
        }
    } else {
        if (SizeOverride) {
            memcpy(destination, getRegisterName_16b(regValue), 2);
            return 2;
        } else {
            memcpy(destination, getRegisterName_32b(regValue), 3);
            return 3;
        }
    }
}

char * getRegisterName_8b_ALT(char regValue) { // 2.200sec for 10,000,000 runs ---0.00000004 seconds faster per execution
    if (regValue == 0) { return "AL"; }
    else if (regValue == 1) { return "CL"; }
    else if (regValue == 2) { return "DL"; }
    else if (regValue == 3) { return "BL"; }
    else if (regValue == 4) { return "AH"; }
    else if (regValue == 5) { return "CH"; }
    else if (regValue == 6) { return "DH"; }
    else if (regValue == 7) { return "BH"; }

    return "XX";
}
char * getRegisterName_8b_ALT2(char regValue) { // 2.600sec for 10,000,000 runs
    char * REG_VALUE_8b_ptr = REG_VALUE_8b;
    for(char i = 0; i < regValue; i++) {REG_VALUE_8b_ptr++;REG_VALUE_8b_ptr++;}
    return REG_VALUE_8b_ptr;
}

int getDisplacement(unsigned char * dispStart, char MOD, bool SizeOverride, char * destination) {
    int lenOffset = 0;
    switch (MOD) {
        case ZERO_BYTE_DISPLACEMENT:
            /// Should not get here
            break;
        case ONE_BYTE_DISPLACEMENT:
            sprintf(destination, " + 0x", 5);
            for (int i = 0; i < 1; i++) {
                sprintf(destination + 5 + (2 * i), "%.2X", dispStart[1 -1 - i]); }
            lenOffset = 3 + 2 + (1 * 2);
            break;
        case FOUR_BYTE_DISPLACEMENT: {
            sprintf(destination, " + 0x", 5);

            int leng = 4;
            if (SizeOverride) { leng = 2; }

            for (int i = 0; i < leng; i++) {
                sprintf(destination + 5 + (2 * i), "%.2X", dispStart[leng -1 - i]); }
            lenOffset = 3 + 2 + (leng * 2);

            break; }
        case FIELD_IS_REGISTER:
            /// Should not get here
            break;
    }


    /*int len = 1; if (s) {len = 4; } if (OperandOverride66h) { len = 2; }
    for (int i = 0; i < len; i++) {
        sprintf(destination + (2 * i), "%.2X", dispStart[len -1 - i]);
    }*/
    return lenOffset;
}

int getSIB(unsigned char * SIBByte, bool s, bool SizeOverride, unsigned char mod, bool withDisp, char * destination) { /// SSIIIBBB
    unsigned char scale;
    unsigned char index;
    unsigned char base;

    scale = SIBByte[0] >> 6;
    index = (SIBByte[0] % 64) >> 3;
    base = (SIBByte[0] % 8);

    printf("\nSIB Scale: %d | Mult: %d\n", scale, (int)pow(2.0, (double)scale));

    if (base == 0b101) { /// Include disp, so request one more byte
        if (index == 0b100) { /// disregard index
            char * baseChar = (char *)malloc(4 * sizeof(char)); int base_len = getRMName(base, true, SizeOverride, mod, baseChar);
            char * disp = (char *)malloc(32 * sizeof(char)); int disp_len = getDisplacement(SIBByte + 1, mod, SizeOverride, disp);
            if (withDisp) {sprintf(destination, "[%s%s]", baseChar, disp);
            } else { sprintf(destination, "[%s]", baseChar); }

            return 2 + base_len + disp_len;
        } else {
            char * indexChar = (char *)malloc(4 * sizeof(char)); int index_len = getRMName(index, true, SizeOverride, mod, indexChar);
            char * baseChar = (char *)malloc(4 * sizeof(char)); int base_len = getRMName(base, true, SizeOverride, mod, baseChar);
            char * disp = (char *)malloc(32 * sizeof(char)); int disp_len = getDisplacement(SIBByte + 1, mod, SizeOverride, disp);

            sprintf(destination, "[%s + %s*%d%s]", baseChar, indexChar, (int)pow(2.0, (double)scale), disp);

            return 7 + base_len + index_len + disp_len;
        }
    } else {
        if (index == 0b100) { /// disregard index
            char * baseChar = (char *)malloc(4 * sizeof(char)); int base_len = getRMName(base, true, SizeOverride, mod, baseChar);
            if (withDisp) { char * disp = (char *)malloc(32 * sizeof(char)); int disp_len = getDisplacement(SIBByte + 1, mod, SizeOverride, disp);
                sprintf(destination, "[%s%s]", baseChar, disp); base_len+= disp_len;
                printf("T DEST: %s\n",destination);
                printf("T ORIG: [%s%s]\n",baseChar, disp);
            } else { sprintf(destination, "[%s]", baseChar); }

            return 2 + base_len;
        } else {
            char * indexChar = (char *)malloc(4 * sizeof(char)); int index_len = getRMName(index, true, SizeOverride, mod, indexChar);
            char * baseChar = (char *)malloc(4 * sizeof(char)); int base_len = getRMName(base, true, SizeOverride, mod, baseChar);

            // Resize to proper size
            //indexChar = (char *)malloc(index_len * sizeof(char)); getRegisterName(index, s, SizeOverride, indexChar);
            //baseChar =(char *)malloc(base_len * sizeof(char)); getRegisterName(base, s, SizeOverride, baseChar);

            if (withDisp) {
                char * disp = (char *)malloc(32 * sizeof(char)); int disp_len = getDisplacement(SIBByte + 1, mod, SizeOverride, disp);
                sprintf(destination, "[%s + %s*%d%s]", baseChar, indexChar, (int)pow(2.0, (double)scale), disp); base_len+= disp_len;
            } else { sprintf(destination, "[%s + %s*%d]", baseChar, indexChar, (int)pow(2.0, (double)scale)); }

            printf("Index offset: %d", index);
            //sprintf(destination, "[%s + %s*%d]", baseChar, indexChar, (int)pow(2.0, (double)scale));

            return 7 + base_len + index_len;
        }
    }
}

#endif // COMMON_H_INCLUDED
