#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED

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

int getRegisterName(char regValue, bool s, bool OperandOverride66h, char * destination) { // Sets destination to the register name and returns the length of the register name
    if (s) {
        if (!OperandOverride66h) {
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

int getDisplacement(unsigned char * dispStart, bool s, bool OperandOverride66h, char * destination) {
    int len = 1; if (s) {len = 4; } if (OperandOverride66h) { len = 2; }
    for (int i = 0; i < len; i++) {
        sprintf(destination + (2 * i), "%.2X", dispStart[len -1 - i]);
    }
    return 2 * len;
}

#endif // COMMON_H_INCLUDED
