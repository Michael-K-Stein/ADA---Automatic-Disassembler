#ifndef _00_H_INCLUDED
#define _00_H_INCLUDED

#include "Common.h"

#define H 0
/// 0x00 -> 0x0F : ADD
 // http://ref.x86asm.net/coder32.html
// http://www.c-jump.com/CIS77/CPU/x86/lecture.html#X77_0120_encoding_add

/**
    *   d == 0 --> add REG to R/M;      d == 1 --> add R/M to REG;
    *   s == 0 --> 8 bit values;        s == 1 --> 16/32 bit values;
    *
    *   000000 d s  MM REG R/M
    * (MM is MOD)
*/

char getMod(unsigned char * opCodes) { printf("MOD: %d\n", opCodes[1] >> 6); return opCodes[1] >> 6; }
const char *byte_to_binary(int x)
{
    static char b[9];
    b[0] = '\0';

    int z;
    for (z = 128; z > 0; z >>= 1)
    {
        strcat(b, ((x & z) == z) ? "1" : "0");
    }

    return b;
}

int Decomp_0x00(unsigned char * opCodes, char * output) { /// ADD r/m8, r8
    // 00000000 -> d = 0; s = 0
    if (opCodes[0] == (H * 16) + 0x00) { // Safety check 0x00
        char * reg = "XX";
        char * rm = "XX";
        switch (getMod(opCodes)) {
            case FIELD_IS_REGISTER: // MOD == 11
                        printf("%s\n", byte_to_binary(opCodes[1]));
                        printf("%s\n", byte_to_binary(( opCodes[1] % 64 ) >> 3));
                    printf("REG value: %d\n", ( opCodes[1] % 64 ) >> 3 );
                    printf("R/M value: %d\n", ( opCodes[1] % 8 ));
                reg = getRegisterName_8b( ( opCodes[1] % 64 ) >> 3 );
                rm = getRegisterName_8b( opCodes[1] % 8 );

                memcpy(output + 4, rm, 2);
                memcpy(output + 6, ", ", 2);
                memcpy(output + 8, reg, 2);

                break;

            case ZERO_BYTE_DISPLACEMENT: // MOD == 00
                if (opCodes[1] % 8 == 0b101) { // Displacement only - if R/M == 101 && MOD == 00
                    reg = getRegisterName_8b( ( opCodes[1] % 64 ) >> 3 );
                    rm = getRegisterName_8b( opCodes[1] % 8 );
                    char * disp = "";
                    memcpy(output + 4, "[0x", 3);
                    for (int i = 2; i < 4; i++) {
                        char * buf = (char *)malloc(1 * sizeof(char));
                        sprintf(buf, "%X0", opCodes[i]);
                        memcpy(output + 7 + ((3 - i) * 2), buf, 2);
                    }
                    memcpy(output + 11, "]", 1);
                    memcpy(output + 12, ", ", 2);
                    memcpy(output + 14, reg, 2);
                } else {
                    reg = getRegisterName_8b( ( opCodes[1] % 64 ) >> 3 );
                    rm = getRegisterName_8b( opCodes[1] % 8 );
                    char * disp = "";
                    memcpy(output + 4, rm, 2);
                    memcpy(output + 6, ", ", 2);
                    memcpy(output + 8, reg, 2);
                }
                break;
        }

    }
}

int Decomp_0x01(unsigned char * opCodes, char * output) {
    if (opCodes[0] == (H * 16) + 0x01) { // Safety check 0x01

    }
}

int Decomp_0x0X(unsigned char * opCodes, char * output) {
    int OpCodesUsed = 0; // The return. The amount of bytes this command actual uses.

    unsigned char h = opCodes[0] >> 4;
    unsigned char l = opCodes[0] % 0x10;

    if (l >=0 && l <= 5) { memcpy(output, "ADD ", 4); }

    bool d = (opCodes[0] % 4) >> 1; // Add R/M to REG
    bool s = (opCodes[0] % 2); // 32bit values

    printf("d:%d \t s:%d\n",d,s);



    char * reg = (char *)malloc(32);  int reg_len = getRegisterName( ( opCodes[1] % 64 ) >> 3, s, false, reg  );
    char * rm = (char *)malloc(32);   int rm_len = getRegisterName( opCodes[1] % 8, s, false, rm  );
    char * disp;
    if (l >= 0 && l <= 3) {
        switch (getMod(opCodes)) {
        case FIELD_IS_REGISTER: // MOD == 11
                /*    printf("%s\n", byte_to_binary(opCodes[1]));
                    printf("%s\n", byte_to_binary(( opCodes[1] % 64 ) >> 3));
                printf("REG value: %d\n", ( opCodes[1] % 64 ) >> 3 );
                printf("R/M value: %d\n", ( opCodes[1] % 8 ));*/
            OpCodesUsed = 2;
            break;

        case ZERO_BYTE_DISPLACEMENT: // MOD == 00
            if (opCodes[1] % 8 == 0b101) { // Displacement only - if R/M == 101 && MOD == 00
                //char * disp = "";
                memcpy(rm, "[0x", 3);
                for (int i = 2; i < 4; i++) {
                    char * buf = (char *)malloc(1 * sizeof(char));
                    sprintf(buf, "%X0", opCodes[i]);
                    memcpy(rm + 3 + ((3 - i) * 2), buf, 2);
                }
                memcpy(rm + 7, "]", 1);
                OpCodesUsed = 6;
            } else if (opCodes[1] % 8 == 0b100) { // Multiple of register ptr - if R/M == 100 && MOD == 00
                unsigned char SIB = opCodes[2] >> 3;
                char * Base = (char *)malloc(3 * sizeof(char)); int Base_len = getRegisterName( (opCodes[2] % 8), s, false, Base );
                int SIBpow = SIB >> 3;
                char * SIBreg = (char *)malloc(3 * sizeof(char)); int SIBreg_len = getRegisterName( (SIB % 8), s, false, SIBreg );
                /// The format for such a command is "ADD REG, [BASE + SIB[2:5]*SIB[0:1]]
                if ( (opCodes[2] % 8) == 0b101) { // Only displacement, not base
                    disp = (char *)malloc(4 * sizeof(char));
                    int disp_len = getDisplacement(opCodes + 3, s, false, disp);
                    //char str_disp[disp_len + 1]; for (int i = 0; i < disp_len; i++) { memcpy(str_disp + i, disp + i, sizeof(char)); } str_disp[disp_len] = 0;
                    sprintf(rm, "[0x%s + %s*%d]", disp, SIBreg, (int)pow(2, SIBpow));
                    rm_len = 3 + disp_len + 3 + SIBreg_len + 1 + 1 + 1;

                    OpCodesUsed = 7; // Or maybe 4 if s == 0?
                } else {
                    sprintf(rm, "[%s + %s*%d]", Base, SIBreg, (int)pow(2, SIBpow));

                    rm_len = 1 + Base_len + 3 + SIBreg_len + 1 + 1 + 1;
                    OpCodesUsed = 3; // Or maybe 4 if s == 0?
                }

            } else {
                disp = (char *)malloc((rm_len + 2) * sizeof(char));
                memcpy(disp, "[", 1);
                memcpy(disp + 1, rm, rm_len);
                memcpy(disp + 1 + rm_len, "]", 1);
                rm_len = rm_len + 2;
                rm = (char *)malloc((rm_len) * sizeof(char));
                memcpy(rm, disp, rm_len);

                OpCodesUsed = 2;
            }
            break;
        case ONE_BYTE_DISPLACEMENT: // MOD == 01
            disp = (char *)malloc((9 + rm_len) * sizeof(char));
            memcpy(disp, "[", 1);
            memcpy(disp + 1, rm, rm_len);
            sprintf(disp + 1 + rm_len, " + 0x%X0", opCodes[2]);
            memcpy(disp + 8 + rm_len, "]", 1);
            rm_len = rm_len + 9;
            memcpy(rm, disp, rm_len * sizeof(char));

            OpCodesUsed = 3; // Add OP code + MOD-REG-R\M + Disp8 (8bits = 1 byte)
            break;

        case FOUR_BYTE_DISPLACEMENT: // MOD == 10
            disp = (char *)malloc((7 + 8 + rm_len) * sizeof(char));
            memcpy(disp, "[", 1);
            memcpy(disp + 1, rm, rm_len);
            sprintf(disp + 1 + rm_len, " + 0x");
            int opCodeDispStart = 2;
            for (int i = 0; i < 4; i++) {
                sprintf(disp + 6 + rm_len + (2 * i), "%X0", opCodes[opCodeDispStart + (3 - i)]);
            }
            memcpy(disp + 6 + 8 + rm_len, "]", 1);
            rm_len = rm_len + 7 + 8;
            memcpy(rm, disp, rm_len * sizeof(char));

            OpCodesUsed = 6; // Add OP code + MOD-REG-R\M + Disp32 ( 32bits = 4 bytes)

            break;
    }
    } else if (l >= 4 && l <= 5) {
        d = 1;
        if (l == 4) { // ADD AL, DISP
            memcpy(reg, "AL", sizeof("AL"));
            disp = (char *)malloc(sizeof(char));
            int disp_len = getDisplacement(opCodes + 1, s, false, disp); // should return 1
            sprintf(rm, "0x%s", disp);
            rm_len = 2 + disp_len;
        } else if (l == 5) { // ADD EAX, DISP
            memcpy(reg, "EAX", sizeof("EAX"));
            disp = (char *)malloc(4 * sizeof(char));
            int disp_len = getDisplacement(opCodes + 1, s, false, disp); // should return 4
            sprintf(rm, "0x%s", disp);
            rm_len = 2 + disp_len;
        }
    }
    printf("REG: %s, REG_LEN: %d\n", reg, reg_len);
    printf("R/M: %s, RM_LEN: %d\n", rm, rm_len);

    if (d) {
        memcpy(output + 4, reg, reg_len);
        memcpy(output + 4 + reg_len, ", ", 2);
        memcpy(output + 6 + reg_len, rm, rm_len);
    } else {
        memcpy(output + 4, rm, rm_len);
        memcpy(output + 4 + rm_len, ", ", 2);
        memcpy(output + 6 + rm_len, reg, reg_len);
    }

    printf("out: %s\n", output);

    /*if (h == 0x0) {
        memcpy(output, &"ADD ", sizeof("ADD "));
        //output += "ADD ";
        if (l == 0x00) {
            Decomp_0x00(opCodes, output);
        }
        else if (l == 0x01) {
            Decomp_0x01(opCodes, output);
        }
    }*/

    return OpCodesUsed;
}



#endif // _00_H_INCLUDED
