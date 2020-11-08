#ifndef JMP_CALL_H_INCLUDED
#define JMP_CALL_H_INCLUDED

#include "../Common.h"

int generateJMPCall(unsigned char * opCodes, char * output, bool size_override) { // opCodes starting and including the operation op-code.
    int opsUsed = 0;
    if (opCodes[0] == 0xFF) {
        opsUsed+=2;
    } else {
        opsUsed++;
        if (opCodes[0] == 0xEA || opCodes[0] == 0x9A) { /// JMP FAR
            char * disp1 = (char*)calloc(16,sizeof(char)); int disp1_len_off = generateIMM(opCodes + opsUsed, disp1, false, size_override, true);
            char * disp2 = (char*)calloc(16,sizeof(char)); int disp2_len_off = generateIMM(opCodes + opsUsed + disp1_len_off, disp2, false, true, true);
            sprintf(output, 64, "%s:%s", disp2, disp1);
			free(disp1); free(disp2);
            opsUsed += disp1_len_off + disp2_len_off;
        }
        else { /// JMP NEAR
            char * disp = (char*)calloc(16,sizeof(char)); 
			int disp_len_off = generateIMM(opCodes + opsUsed, disp, false, size_override, !(opCodes[0] == 0xEB));
            sprintf(output, 64, "%s", disp);
			free(disp);
            opsUsed += disp_len_off;
        }
    }
    return opsUsed;
}

#endif // JMP_CALL_H_INCLUDED
