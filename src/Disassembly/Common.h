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

char getMod(unsigned char * opCodes) { /*printf("MOD: %d\n", opCodes[1] >> 6);*/ return opCodes[1] >> 6; }

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
    //printf("Register given is: %s\n", REG_VALUE_8b + (regValue*2));
    return REG_VALUE_8b + (regValue*2);
}
char * getRegisterName_16b(char regValue) {
    //printf("Register given is: %s\n", REG_VALUE_16b + (regValue*2));
    return REG_VALUE_16b + (regValue*2);
}
char * getRegisterName_32b(char regValue) {
    //printf("Register given is: %s\n", REG_VALUE_32b + (regValue*3));
    return REG_VALUE_32b + (regValue*3);
}

int getRegisterName(char regValue, bool s, bool SizeOverride, char * destination) { // Sets destination to the register name and returns the length of the register name
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

int getDisplacement_00(unsigned char * dispStart, char * destination, bool SizeOverride) {
    sprintf(destination, "0x");

    int leng = SizeOverride ? 2 : 4;

    for (int i = 0; i < leng; i++) {
        sprintf(destination + 2 + (2 * i), "%.2X", dispStart[leng -1 - i]); }

    return leng;
}
int getDisplacement_01(unsigned char * dispStart, char * destination) {
    sprintf(destination, "0x");
    sprintf(destination + 2, "%.2X", dispStart[0]);

    return 1;
}
int getDisplacement_10(unsigned char * dispStart, char * destination, bool SizeOverride) {
    sprintf(destination, "0x");

    int leng = SizeOverride ? 2 : 4;

    for (int i = 0; i < leng; i++) {
        sprintf(destination + 2 + (2 * i), "%.2X", dispStart[leng -1 - i]); }

    return leng;
}


int getDisplacement(unsigned char * dispStart, char MOD, bool SizeOverride, char * destination) {
    switch (MOD) {
        case ZERO_BYTE_DISPLACEMENT: {
            sprintf(destination, "0x");

            int leng = 4;
            if (SizeOverride) { leng = 2; }

            for (int i = 0; i < leng; i++) {
                sprintf(destination + 2 + (2 * i), "%.2X", dispStart[leng -1 - i]); }

            return leng;
            break; }

            break;
        case ONE_BYTE_DISPLACEMENT:
            sprintf(destination, "0x");
            for (int i = 0; i < 1; i++) {
                sprintf(destination + 2 + (2 * i), "%.2X", dispStart[1 -1 - i]); }
            return 1;
            break;
        case FOUR_BYTE_DISPLACEMENT: {
            sprintf(destination, "0x");

            int leng = 4;
            if (SizeOverride) { leng = 2; }

            for (int i = 0; i < leng; i++) {
                sprintf(destination + 2 + (2 * i), "%.2X", dispStart[leng -1 - i]); }

            return leng;
            break; }
        case FIELD_IS_REGISTER:
            /// Should not get here
            return 0;
            break;
    }


    /*int len = 1; if (s) {len = 4; } if (OperandOverride66h) { len = 2; }
    for (int i = 0; i < len; i++) {
        sprintf(destination + (2 * i), "%.2X", dispStart[len -1 - i]);
    }*/
    return 0;
}
int getDisplacementJcc(unsigned char * dispStart, char * destination, int TOTAL_FILE_OFFSET, bool near, bool size_override) {
    //sprintf(destination, "0x");
    if (near) {
        char * hx = (char *)calloc(10, 1);
        int leng = size_override ? 2 : 4;

        int tot = 0;

        for (int i = 0; i < leng; i++) {
            sprintf(hx + (2 * i), "%.2X", dispStart[leng -1 - i]); }

        tot += (int)strtol(hx, NULL, 16) + TOTAL_FILE_OFFSET + leng + 2;

        sprintf(destination, "%d", tot);

        return leng;
    }
    else {
        sprintf(destination, "%d", (TOTAL_FILE_OFFSET + 2 + dispStart[0]) % 256);
        return 1;
    }
}

int getSIB(unsigned char * SIBByte, bool SizeOverride, unsigned char mod, bool withDisp, char * destination) { /// SSIIIBBB
    unsigned char scale;
    unsigned char index;
    unsigned char base;

    scale = SIBByte[0] >> 6;
    index = (SIBByte[0] % 64) >> 3;
    base = (SIBByte[0] % 8);

    //printf("\nSIB Scale: %d | Mult: %d\n", scale, (int)pow(2.0, (double)scale));

    int disp_len_off = 0;

    if (base == 0b101) { /// Include disp, so request one more byte
        if (index == 0b100) { /// disregard index
            char * baseChar = (char *)malloc(4 * sizeof(char)); int base_len = getRMName(base, true, SizeOverride, mod, baseChar);
            char * disp = (char *)malloc(32 * sizeof(char)); disp_len_off = getDisplacement(SIBByte + 1, mod, SizeOverride, disp);
            if (withDisp) {sprintf(destination, "[%s + %s]", baseChar, disp);
            } else { sprintf(destination, "[%s]", baseChar); }

            return 1 + (withDisp ? disp_len_off : 0);
        } else {
            char * indexChar = (char *)malloc(4 * sizeof(char)); int index_len = getRMName(index, true, SizeOverride, mod, indexChar);
            char * baseChar = (char *)malloc(4 * sizeof(char)); int base_len = getRMName(base, true, SizeOverride, mod, baseChar);
            char * disp = (char *)malloc(32 * sizeof(char)); disp_len_off = getDisplacement(SIBByte + 1, mod, SizeOverride, disp);

            sprintf(destination, "[%s + %s*%d + %s]", baseChar, indexChar, (int)pow(2.0, (double)scale), disp);

            return 1 + (withDisp ? disp_len_off : 0);
        }
    } else {
        if (index == 0b100) { /// disregard index
            char * baseChar = (char *)malloc(4 * sizeof(char)); int base_len = getRMName(base, true, SizeOverride, mod, baseChar);
            if (withDisp) { char * disp = (char *)malloc(32 * sizeof(char)); disp_len_off = getDisplacement(SIBByte + 1, mod, SizeOverride, disp);
                sprintf(destination, "[%s + %s]", baseChar, disp);
            } else { sprintf(destination, "[%s]", baseChar); }

            return 1 + (withDisp ? disp_len_off : 0);
        } else {
            char * indexChar = (char *)malloc(4 * sizeof(char)); int index_len = getRMName(index, true, SizeOverride, mod, indexChar);
            char * baseChar = (char *)malloc(4 * sizeof(char)); int disp_len_off = getRMName(base, true, SizeOverride, mod, baseChar);

            // Resize to proper size
            //indexChar = (char *)malloc(index_len * sizeof(char)); getRegisterName(index, s, SizeOverride, indexChar);
            //baseChar =(char *)malloc(base_len * sizeof(char)); getRegisterName(base, s, SizeOverride, baseChar);

            if (withDisp) {
                char * disp = (char *)malloc(32 * sizeof(char)); disp_len_off = getDisplacement(SIBByte + 1, mod, SizeOverride, disp);
                sprintf(destination, "[%s + %s*%d + %s]", baseChar, indexChar, (int)pow(2.0, (double)scale), disp);
            } else { sprintf(destination, "[%s + %s*%d]", baseChar, indexChar, (int)pow(2.0, (double)scale)); }

            printf("Index offset: %d", index);
            //sprintf(destination, "[%s + %s*%d]", baseChar, indexChar, (int)pow(2.0, (double)scale));

            return 1 + (withDisp ? disp_len_off : 0);
        }
    }
}

int generateRM_ZERO_BYTE_DISPLACEMENT(unsigned char * opCodes, char * output, char rmVal, bool address_override) {
    int ret = 0;
    if (address_override) { // 16 bit
         /// second bit BX|BP, third bit SI|DI
        char * reg = (char *)calloc(2, sizeof(char));
        char * regAct = {"BX\0BP"};
        reg = (rmVal >> 1) % 2 == 0 ? regAct : regAct+3;
        char * rof = (char *)calloc(2, sizeof(char));
        char * rofAct = {"SI\0DI"};
        rof = (rmVal % 2) == 0 ? rofAct : rofAct+3;

        if (rmVal < 4) {
            sprintf(output, "[%s + %s]", reg, rof);
            ret++;
        } else if (rmVal >> 2 == 1 && rmVal >> 1 == 0){
            sprintf(output, "[%s]", rof);
            ret++;
        } else if (rmVal == 0b110) {
            /** ONLY DISPLACEMENT 16bit */
            char * disp = (char *)calloc(4, sizeof(char));
            getDisplacement_00(opCodes + 2, disp, address_override);
            sprintf(output, "[%s]", disp);
            ret += 2 + 1;
        } else if (rmVal == 0b111) { sprintf(output, "[%s]", &"BX"); ret++;}
    } else { // 32 bit
        if (rmVal == 0b100) { /// SIB
            ret += 1 + getSIB(opCodes + 2, address_override, 0b00, false, output);
        } else if (rmVal == 0b101) {
            /// ONLY DISP 32bit
            char * disp = (char *)calloc(8, sizeof(char));
            getDisplacement_00(opCodes + 2, disp, address_override);
            sprintf(output, "[%s]", disp);
            ret += 4 + 1;
        } else {
            sprintf(output, "[XXX]");
            memcpy(output+1, getRegisterName_32b(rmVal), 3);
            ret++;
        }
    }
    return ret;
}
int generateRM_ONE_BYTE_DISPLACEMENT(unsigned char * opCodes, char * output, char rmVal, bool address_override) {

    char * disp = (char *)calloc(4, sizeof(char)); // disp8
    getDisplacement_01(opCodes + 2, disp);

    if (address_override) { // 16 bit
         /// second bit BX|BP, third bit SI|DI
        char * reg = (char *)calloc(2, sizeof(char));
        char * regAct = {"BX\0BP"};
        reg = (rmVal >> 1) % 2 == 0 ? regAct : regAct+3;
        char * rof = (char *)calloc(2, sizeof(char));
        char * rofAct = {"SI\0DI"};
        rof = (rmVal % 2) == 0 ? rofAct : rofAct+3;

        if (rmVal < 4) {
            sprintf(output, "[%s + %s + %s]", reg, rof, disp);
        } else if (rmVal == 0b110) {
            sprintf(output, "[%s + %s]", &"BP", disp);
        } else if (rmVal == 0b111) { sprintf(output, "[%s + %s]", &"BX", disp);
        } else if (rmVal >> 2 == 1){
            sprintf(output, "[%s + %s]", rof, disp);
        }
    } else { // 32 bit
        if (rmVal == 0b100) { /// SIB
            //char * sib = (char *)calloc(16, sizeof(char));
            return 1 + getSIB(opCodes + 2, address_override, 0b01, true, output);
        } else {
            sprintf(output, "[XXX + %s]", disp);
            memcpy(output+1, getRegisterName_32b(rmVal), 3);
        }
    }

    return 2;
}
int generateRM_FOUR_BYTE_DISPLACEMENT(unsigned char * opCodes, char * output, char rmVal, bool address_override) {

    char * disp = (char *)calloc(address_override ? 6 : 10, sizeof(char)); // disp8
    int disp_len_off = getDisplacement_10(opCodes + 2, disp, address_override);

    if (address_override) { // 16 bit
         /// second bit BX|BP, third bit SI|DI
        char * reg = (char *)calloc(2, sizeof(char));
        char * regAct = {"BX\0BP"};
        reg = (rmVal >> 1) % 2 == 0 ? regAct : regAct+3;
        char * rof = (char *)calloc(2, sizeof(char));
        char * rofAct = {"SI\0DI"};
        rof = (rmVal % 2) == 0 ? rofAct : rofAct+3;

        if (rmVal < 4) {
            sprintf(output, "[%s + %s + %s]", reg, rof, disp);
            return 1 + disp_len_off;
        } else if (rmVal == 0b110) {
            sprintf(output, "[%s + %s]", &"BP", disp);
        } else if (rmVal == 0b111) { sprintf(output, "[%s + %s]", &"BX", disp); return 1 + disp_len_off;
        } else if (rmVal >> 2 == 1){
            sprintf(output, "[%s + %s]", rof, disp); return 1 + disp_len_off;
        }
        return 0;
    } else { // 32 bit
        if (rmVal == 0b100) { /// SIB
            //char * sib = (char *)calloc(16, sizeof(char));
            return 1 + getSIB(opCodes + 2, address_override, 0b10, true, output);
        } else {
            sprintf(output, "[XXX + %s]", disp);
            memcpy(output+1, getRegisterName_32b(rmVal), 3);
            return 4;
        }
    }

    return 0;
}
int generateRM(unsigned char * opCodes, char * output, bool address_override, bool size_override) { // opCodes should be given starting right after the prefixes. Including the action op code.
    char mod = getMod(opCodes);
    char rmVal = opCodes[1] % 8;

    switch (mod) {
        case ZERO_BYTE_DISPLACEMENT: {
            return generateRM_ZERO_BYTE_DISPLACEMENT(opCodes, output, rmVal, address_override);
            break; }
        case ONE_BYTE_DISPLACEMENT: {
            return generateRM_ONE_BYTE_DISPLACEMENT(opCodes, output, rmVal, address_override);
            break; }
        case FOUR_BYTE_DISPLACEMENT: {
            return generateRM_FOUR_BYTE_DISPLACEMENT(opCodes, output, rmVal, address_override);
            break; }
        case FIELD_IS_REGISTER: {
            getRegisterName(opCodes[1] % 8, opCodes[0] % 2, size_override, output);
            return 1;
            break;}
    }
}

int generateIMM(unsigned char * opCodes, char * output, bool address_override, bool size_override, char type) { // opCodes should be given starting after RM.
    switch (type) {
        case 0b00: { ///    ADD r/m8, imm8
            sprintf(output, "0x%.2X", opCodes[0]);
            return 1;
        break; }
        case 0b01: { ///	ADD r/m32, imm32 or imm16
            sprintf(output, "0x");

            int leng = size_override ? 2 : 4;

            for (int i = 0; i < leng; i++) {
                sprintf(output + 2 + (2 * i), "%.2X", opCodes[leng -1 - i]); }
            return leng;
        break; }
        case 0b11: {///     ADD r/m32, imm8
            sprintf(output, "0x%.2X", opCodes[0]);
            return 1;
        break; }
    }
    return 0;
}

char * SingleOpCMD(unsigned char * opCodes, bool size_override) {
    char reg_val = opCodes[0] % 8;
    char * reg = (char *)calloc(4, sizeof(char)); getRegisterName(reg_val, true, size_override, reg);
    //sprintf(destination, "%s", reg);
    return reg;
}

#endif // COMMON_H_INCLUDED
