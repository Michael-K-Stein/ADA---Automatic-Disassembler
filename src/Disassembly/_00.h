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

int Decomp_0x0X(unsigned char * opCodes, char * output, bool Operand_Size_Override, bool Address_Size_Override) {
    int OpCodesUsed = 0; // The return. The amount of bytes this command actual uses.

    unsigned char h = opCodes[0] >> 4;
    unsigned char l = opCodes[0] % 0x10;

    char * opCMD = (char *)calloc(8, sizeof(char));

    int CMDoffset = 0;
    if (l >=0 && l <= 5) { memcpy(output, "ADD ", 4); CMDoffset += 4; sprintf(opCMD, "%s", &"ADD");  }
    else if (l >= 8 && l <= 0x0D) { memcpy(output, "OR ", 3); CMDoffset+=3; sprintf(opCMD, "%s", &"OR");}

    bool d = (opCodes[0] % 4) >> 1; // Add R/M to REG
    bool s = (opCodes[0] % 2); // 32bit values

    printf("d:%d \t s:%d\n",d,s);

    char mod = getMod(opCodes);

    char * reg = (char *)malloc(32);  int reg_len = getRegisterName( ( opCodes[1] % 64 ) >> 3, s, Operand_Size_Override, reg  );
    char * rm = (char *)malloc(64); int rm_len = generateRM(opCodes, rm, Address_Size_Override, Operand_Size_Override); //getRMName( opCodes[1] % 8, s, Address_Size_Override, mod, rm  );
    char * disp;
    if ((l >= 0 && l <= 3) || (l >= 8 && l <= 0x0D)) {
        /*switch (mod) {
        case FIELD_IS_REGISTER: // MOD == 11
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
                rm_len = getSIB(opCodes + 2, Address_Size_Override, mod, false, rm);
                OpCodesUsed = 3;

            } else {
                disp = (char *)malloc((rm_len + 2) * sizeof(char));
                rm_len = rm_len + 2;
                sprintf(disp, "[%s]", rm);
                realloc(rm, (rm_len) * sizeof(char));
                rm = disp;

                OpCodesUsed = 2;
            }
            break;
        case ONE_BYTE_DISPLACEMENT: // MOD == 01
            if (opCodes[1] % 8 == 0b100) { // Multiple of register ptr - if R/M == 100 && MOD == 01
                rm_len = getSIB(opCodes + 2, Address_Size_Override, mod, true, rm);
                OpCodesUsed = 3;
            } else {
                disp = (char *)malloc((9 + rm_len) * sizeof(char));
                memcpy(disp, "[", 1);
                memcpy(disp + 1, rm, rm_len);
                int disp_len = getDisplacement(opCodes + 2, getMod(opCodes), Address_Size_Override, disp + 1 + rm_len);
                memcpy(disp + 1 + disp_len + rm_len, "]", 1);
                rm_len = rm_len + 1 + disp_len;
                memcpy(rm, disp, rm_len * sizeof(char));
            }
            OpCodesUsed = 3; // Add OP code + MOD-REG-R\M + Disp8 (8bits = 1 byte)
            break;

        case FOUR_BYTE_DISPLACEMENT: // MOD == 10
            if (opCodes[1] % 8 == 0b100) { // Multiple of register ptr - if R/M == 100 && MOD == 10
                rm_len = getSIB(opCodes + 2, Address_Size_Override, getMod(opCodes), true, rm);
                OpCodesUsed = 7;
            } else {
                disp = (char *)malloc((7 + 8 + rm_len) * sizeof(char));
                memcpy(disp, "[", 1);
                memcpy(disp + 1, rm, rm_len);
                int disp_len = getDisplacement(opCodes + 2, mod, Address_Size_Override, disp + 1 + rm_len);
                memcpy(disp + 1 + disp_len + rm_len, "]", 1);
                rm_len = rm_len + 2 + disp_len;
                memcpy(rm, disp, rm_len * sizeof(char));

                OpCodesUsed = 6; // Add OP code + MOD-REG-R\M + Disp32 ( 32bits = 4 bytes)
            }
            break;
        }*/
    } else if (l >= 4 && l <= 5) {
        d = 1;
        if (l == 4) { // ADD AL, DISP
            memcpy(reg, "AL", sizeof("AL"));
            disp = (char *)malloc(16 * sizeof(char));
            int disp_len = getDisplacement(opCodes + 1, s, Operand_Size_Override, disp); // should return 1
            sprintf(rm, "%s", disp);
            rm_len = 2 + disp_len;
        } else if (l == 5) { // ADD EAX, DISP
            Operand_Size_Override ? memcpy(reg, "AX", sizeof("AX")) : memcpy(reg, "EAX", sizeof("EAX"));
            disp = (char *)malloc(16 * sizeof(char));
            int disp_len = getDisplacement(opCodes + 1, mod, Operand_Size_Override, disp); // should return 4
            sprintf(rm, "%s", disp);
            rm_len = 2 + disp_len;
        }
    }/* else if (l >= 8 && l <= 0x0D) { // OR

    }*/
    printf("REG: %s, REG_LEN: %d\n", reg, reg_len);
    printf("R/M: %s, RM_LEN: %d\n", rm, rm_len);

    sprintf(output, "%s %s, %s", opCMD, d ? reg : rm, d ? rm : reg);

    /*if (d) {
        memcpy(output + CMDoffset, reg, reg_len);
        memcpy(output + CMDoffset + reg_len, ", ", 2);
        memcpy(output + CMDoffset + 2 + reg_len, rm, rm_len);
        sprintf(output, "%s %s, %s", opCMD, reg, rm);
    } else {
        memcpy(output + CMDoffset, rm, rm_len);
        memcpy(output + CMDoffset + rm_len, ", ", 2);
        memcpy(output + CMDoffset + 2 + rm_len, reg, reg_len);
        sprintf(output, "%s %s, %s", opCMD, rm, reg);
    }*/

    printf("out: %s\n", output);

    return OpCodesUsed;
}



#endif // _00_H_INCLUDED
