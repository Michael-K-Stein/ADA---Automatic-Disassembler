#ifndef DISASSEMBLEMASTER_H_INCLUDED
#define DISASSEMBLEMASTER_H_INCLUDED

#include "Common.h"
#include "ControlFlow_Conditional/JMP_Call.h"

typedef struct PrefixOut {
    int prefixOffset; /// The offset from which to start writing the opcode names, not the offset of how many bytes are used for prefixes.

    bool Operand_Size_Override;
    bool Address_Size_Override;
};

typedef struct EXTRA { /// Extra information which might need to be passed between functions
    bool DEBUG_MODE; // Prints extra information during runtime.
    bool FORCE; // Forcefully continue if you encounter an unknown/ invalid opcode. This is UNRELIABLE!
};

PrefixOut WritePrefixes(unsigned char * opCodes, char * output, int *OpCodeOffset) {
    unsigned char * prefixes = (unsigned char *)calloc(sizeof(unsigned char), 4);

    int offset = 0; /// The offset from which to start writing the opcode names, not the offset of how many bytes are used for prefixes.

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
        } else { break; }
    }

    PrefixOut prefOut;
    prefOut.prefixOffset = offset;
    prefOut.Address_Size_Override = Address_Size_Override;
    prefOut.Operand_Size_Override = Operand_Size_Override;

    return prefOut;
}

bool IsBetween(unsigned char Val, int incmin, int incmax) { return (Val >= incmin && Val <= incmax); }
int IsRegular(unsigned char * opCode, char * opCMDOutput) {
    if (IsBetween(opCode[0], 0x00, 0x03)) { memcpy(opCMDOutput, &"ADD", 3); return 1; }
    if (IsBetween(opCode[0], 0x08, 0x0B)) { memcpy(opCMDOutput, &"OR", 2); return 1; }
    if (IsBetween(opCode[0], 0x10, 0x13)) { memcpy(opCMDOutput, &"ADC", 3); return 1; }
    if (IsBetween(opCode[0], 0x18, 0x1B)) { memcpy(opCMDOutput, &"SBB", 3); return 1; }
    if (IsBetween(opCode[0], 0x20, 0x23)) { memcpy(opCMDOutput, &"AND", 3); return 1; }
    if (IsBetween(opCode[0], 0x28, 0x2B)) { memcpy(opCMDOutput, &"SUB", 3); return 1; }
    if (IsBetween(opCode[0], 0x30, 0x33)) { memcpy(opCMDOutput, &"XOR", 3); return 1; }
    if (IsBetween(opCode[0], 0x38, 0x3B)) { memcpy(opCMDOutput, &"CMP", 3); return 1; }
    if (IsBetween(opCode[0], 0x84, 0x85)) { memcpy(opCMDOutput, &"TEST", 4); return 1; }
    if (IsBetween(opCode[0], 0x86, 0x87)) { memcpy(opCMDOutput, &"XCHG", 4); return 1; }
    if (IsBetween(opCode[0], 0x88, 0x8B)) { memcpy(opCMDOutput, &"MOV", 3); return 1; }
    if (IsBetween(opCode[0], 0x62, 0x62)) { memcpy(opCMDOutput, &"BOUND", 5); return 1; }
    return 0;
}
int IsMove(unsigned char * opCode, char * opCMDOutput) {
    /* if (IsBetween(opCode[0], 0x88, 0x8B)) { memcpy(opCMDOutput, &"MOV", 3); return 1; } /// MOV REG */ /** This is very well covered in IsRegular */
    if (opCode[0] == 0x8C) { memcpy(opCMDOutput, &"MOV", 3); return 1; } /// 	MOV r/m16,Sreg
    if (opCode[0] == 0x8E) { memcpy(opCMDOutput, &"MOV", 3); return 1; } ///    MOV Sreg,r/m16

    if (IsBetween(opCode[0], 0xB0, 0xB7)) { memcpy(opCMDOutput, &"MOV", 3); return 1; } /// MOV r8,imm8
    if (IsBetween(opCode[0], 0xB8, 0xBF)) { memcpy(opCMDOutput, &"MOV", 3); return 1; } /// MOV r16/32,imm16/32

    if (IsBetween(opCode[0], 0xC6, 0xC7)) { memcpy(opCMDOutput, &"MOV", 3); return 1; } /// MOV IMM
}
int IsMultiple80(unsigned char * opCode, char * opCMDOutput) {
    if (IsBetween(opCode[0], 0x80, 0x83)) { return 1; }
    return 0;
}
int IsOnlyIMM(unsigned char * opCode, char * opCMDOutput) {
    if (opCode[0] == 0x68) { memcpy(opCMDOutput, &"PUSH", 4); return 1; }
    if (opCode[0] == 0x6A) { memcpy(opCMDOutput, &"PUSH", 4); return 1; }
    if (opCode[0] == 0xCD) { memcpy(opCMDOutput, &"INT", 3); return 1; }
    if (IsBetween(opCode[0], 0xD4, 0xD5)) { memcpy(opCMDOutput, (opCode[0] % 2) ? &"AAD" : &"AAM", 3); return 1; }
    return 0;
}
int IsOneByteOpCode(unsigned char * opCode, char * opCMDOutput, bool size_override) {
    switch (opCode[0]) {
        case 0x27: { memcpy(opCMDOutput, &"DAA", 3); return 1; break; }
        case 0x2F: { memcpy(opCMDOutput, &"DAS", 3); return 1; break; }
        case 0x37: { memcpy(opCMDOutput, &"AAA", 3); return 1; break; }
        case 0x3F: { memcpy(opCMDOutput, &"AAS", 3); return 1; break; }
        case 0x60: { memcpy(opCMDOutput, size_override ? &"PUSHA\0" : &"PUSHAD", 6); return 1; break; }
        case 0x61: { memcpy(opCMDOutput, size_override ? &"POPA\0" : &"POPAD", 5); return 1; break; }
        case 0x6C: { memcpy(opCMDOutput, &"INSB", 4); return 1; break; }
        case 0x6D: { memcpy(opCMDOutput, size_override ? &"INSW" : &"INSD", 4); return 1; break; }
        case 0x6E: { memcpy(opCMDOutput, &"OUTB", 4); return 1; break; }
        case 0x6F: { memcpy(opCMDOutput, size_override ? &"OUTW" : &"OUTD", 4); return 1; break; }
        case 0x9B: { memcpy(opCMDOutput, &"WAIT", 4); return 1; break; }
        case 0xCC: { memcpy(opCMDOutput, &"INT3", 4); return 1; break; }
        case 0xCE: { memcpy(opCMDOutput, &"INTO", 4); return 1; break; }
        case 0x90: { memcpy(opCMDOutput, &"NOP", 3); return 1; break; }
        case 0x98: { memcpy(opCMDOutput, size_override ? &"CBW\0" : &"CWDE", 4); return 1; break; }
        case 0x99: { memcpy(opCMDOutput, size_override ? &"CWD" : &"CDQ", 3); return 1; break; }
        case 0x9C: { memcpy(opCMDOutput, size_override ? &"PUSHF\0" : &"PUSHFD", 6); return 1; break; }
        case 0x9D: { memcpy(opCMDOutput, size_override ? &"POPF\0" : &"POPFD", 5); return 1; break; }
        case 0x9E: { memcpy(opCMDOutput, &"SAHF", 4); return 1; break; }
        case 0x9F: { memcpy(opCMDOutput, &"LAHF", 4); return 1; break; }
        case 0xA4: { memcpy(opCMDOutput, &"MOVSB", 5); return 1; break; }
        case 0xA5: { memcpy(opCMDOutput, size_override ? &"MOVSW" : &"MOVSD", 5); return 1; break; }
        case 0xA6: { memcpy(opCMDOutput, &"CMPSB", 5); return 1; break; }
        case 0xA7: { memcpy(opCMDOutput, size_override ? &"CMPSW" : &"CMPSD", 5); return 1; break; }
        case 0xAA: { memcpy(opCMDOutput, &"STOSB", 5); return 1; break; }
        case 0xAB: { memcpy(opCMDOutput, size_override ? &"STOSW" : &"STOSD", 5); return 1; break; }
        case 0xAC: { memcpy(opCMDOutput, &"LODSB", 5); return 1; break; }
        case 0xAD: { memcpy(opCMDOutput, size_override ? &"LODSW" : &"LODSD", 5); return 1; break; }
        case 0xAE: { memcpy(opCMDOutput, &"SCASB", 5); return 1; break; }
        case 0xAF: { memcpy(opCMDOutput, size_override ? &"SCASW" : &"SCASD", 5); return 1; break; }
        case 0xC3: { memcpy(opCMDOutput, &"RETN", 4); return 1; break; }
        case 0xC9: { memcpy(opCMDOutput, &"LEAVE", 5); return 1; break; }
        case 0xCB: { memcpy(opCMDOutput, &"RETF", 4); return 1; break; }
        case 0xCF: { memcpy(opCMDOutput, size_override ? &"IRET\0" : &"IRETD", 5); return 1; break; }
        case 0xD7: { memcpy(opCMDOutput, &"SALC", 4); return 1; break; }
        case 0xD8: { memcpy(opCMDOutput, &"XLAT", 4); return 1; break; }
        case 0xEC: { memcpy(opCMDOutput, &"IN AL, DX", 9); return 1; break; }
        case 0xED: { memcpy(opCMDOutput, size_override ? &"IN AX, DX\0" : &"IN EAX, DX", 10); return 1; break; }
        case 0xEE: { memcpy(opCMDOutput, &"OUT AL, DX", 10); return 1; break; }
        case 0xEF: { memcpy(opCMDOutput, size_override ? &"OUT AX, DX\0" : &"OUT EAX, DX", 11); return 1; break; }
        case 0xF1: { memcpy(opCMDOutput, &"ICEBP", 5); return 1; break; }
        case 0xF4: { memcpy(opCMDOutput, &"HLT", 3); return 1; break; }
        case 0xF5: { memcpy(opCMDOutput, &"CMC", 3); return 1; break; }
        case 0xF8: { memcpy(opCMDOutput, &"CLC", 3); return 1; break; }
        case 0xF9: { memcpy(opCMDOutput, &"STC", 3); return 1; break; }
        case 0xFA: { memcpy(opCMDOutput, &"CLI", 3); return 1; break; }
        case 0xFB: { memcpy(opCMDOutput, &"STI", 3); return 1; break; }
        case 0xFC: { memcpy(opCMDOutput, &"CLD", 3); return 1; break; }
        case 0xFD: { memcpy(opCMDOutput, &"STD", 3); return 1; break; }
    }
    return 0;
}
int IsSingularOpCode(unsigned char * opCode, char * opCMDOutput) {
    if (IsBetween(opCode[0], 0x40, 0x47)) { memcpy(opCMDOutput, &"INC", 3); return 1; }
    if (IsBetween(opCode[0], 0x48, 0x4F)) { memcpy(opCMDOutput, &"DEC", 3); return 1; }
    if (IsBetween(opCode[0], 0x50, 0x57)) { memcpy(opCMDOutput, &"PUSH", 4); return 1; }
    if (IsBetween(opCode[0], 0x58, 0x5F)) { memcpy(opCMDOutput, &"POP", 3); return 1; }
    return 0;
}
int IsSegment(unsigned char * opCode, char * opCMDOutput) {
    int ret = 0;
    char * type = (char *)calloc(5, sizeof(char));
    char * seg = (char *)calloc(3, sizeof(char));

    if (opCode[0] == 0x0F) { return 0; } /// This is a two byte op-code

    sprintf(type, "%s", (opCode[0] % 2) ? &"POP\0" : &"PUSH" );

    if (opCode[0] >> 1 == 0b0000011) { memcpy(seg, &"ES", 2); ret = 1; }
    else if (opCode[0] >> 1 == 0b0001011) { memcpy(seg, &"SS", 2); ret = 1; }
    else if (opCode[0] >> 1 == 0b0000111) { memcpy(seg, &"CS", 2); ret = 1; }
    else if (opCode[0] >> 1 == 0b0001111) { memcpy(seg, &"DS", 2); ret = 1; }

    sprintf(opCMDOutput, "%s %s", type, seg);

    if (!ret) { memcpy(opCMDOutput, &"\0\0\0\0\0\0\0\0", 8); }
    return ret;
}
int IsSpecial(unsigned char * opCode, char * opCMDOutput, bool size_override) {
    int ret = 0;
    char * operationType = (char *)calloc(5, sizeof(char));
    if (IsBetween(opCode[0], 0x91, 0x97)) {memcpy(opCMDOutput, &"XCHG", 4); return 1; }
    else if (IsBetween(opCode[0], 0x04, 0x05)) { memcpy(operationType, &"ADD", 3); ret = 1; }
    else if (IsBetween(opCode[0], 0x0C, 0x0D)) { memcpy(operationType, &"OR\0", 3); ret = 1; }
    else if (IsBetween(opCode[0], 0x14, 0x15)) { memcpy(operationType, &"ADC", 3); ret = 1; }
    else if (IsBetween(opCode[0], 0x1C, 0x1D)) { memcpy(operationType, &"SBB", 3); ret = 1; }
    else if (IsBetween(opCode[0], 0x24, 0x25)) { memcpy(operationType, &"AND", 3); ret = 1; }
    else if (IsBetween(opCode[0], 0x2C, 0x2D)) { memcpy(operationType, &"SUB", 3); ret = 1; }
    else if (IsBetween(opCode[0], 0x34, 0x35)) { memcpy(operationType, &"XOR", 3); ret = 1; }
    else if (IsBetween(opCode[0], 0x3C, 0x3D)) { memcpy(operationType, &"CMP", 3); ret = 1; }
    else if (IsBetween(opCode[0], 0xA8, 0xA9)) { memcpy(operationType, &"TEST", 4); ret = 1; }
    bool _8bit = (opCode[0] % 2 == 0);

    if (ret) {
        if (_8bit) {
            sprintf(opCMDOutput, "%s AL", operationType);
        } else {
            sprintf(opCMDOutput, "%s %s", operationType, size_override ? &"AX\0\0" : &"EAX\0");
        }
    }

    return ret;
}
int IsSpecialA(unsigned char * opCode, char * opCMDOutput) {
    if (IsBetween(opCode[0], 0xA0, 0xA3)) { memcpy(opCMDOutput, &"MOV", 3); return 1; }
    return 0;
}
int IsIO(unsigned char * opCode, char * opCMDOutput, bool size_override) {
    if (opCode[0] == 0xE4) { memcpy(opCMDOutput, &"IN", 2); return 1; }
    if (opCode[0] == 0xE5) { memcpy(opCMDOutput, size_override ? &"IN" : &"IN", 2); return 1; }
    if (opCode[0] == 0xE6) { memcpy(opCMDOutput, &"OUT", 3); return 1; }
    if (opCode[0] == 0xE7) { memcpy(opCMDOutput, size_override ? &"OUT" : &"OUT", 3); return 1; }
    return 0;
}
int IsShift(unsigned char * opCode, char * opCMDOutput) {
    if (IsBetween(opCode[0], 0xC0, 0xC1)) { return 1; }
    if (IsBetween(opCode[0], 0xD0, 0xD4)) { return 1; }
    return 0;
}
int IsArithLogic(unsigned char * opCode, char * opCMDOutput) {
    if (IsBetween(opCode[0], 0xF6, 0xF7)) { return 1; }
    if (opCode[0] == 0xFE) { return 1; }
    return 0;
}
int IsJMPCall(unsigned char * opCode, char * opCMDOutput) {
    if (opCode[0] == 0x9A) { memcpy(opCMDOutput, &"CALL FAR", 8); return 1; }
    if (opCode[0] == 0xE8) { memcpy(opCMDOutput, &"CALL", 4); return 1; }
    if (opCode[0] == 0xE9) { memcpy(opCMDOutput, &"JMP", 3); return 1; }
    if (opCode[0] == 0xEA) { memcpy(opCMDOutput, &"JMP FAR", 7); return 1; }
    if (opCode[0] == 0xEB) { memcpy(opCMDOutput, &"JMP", 3); return 1; }
    return 0;
}

char * JCCCMD = { "JO\0\0JNO\0JB\0\0JNB\0JE\0\0JNE\0JBE\0JA\0\0JS\0\0JNS\0JPE\0JPO\0JL\0\0JGE\0JLE\0JG\0\0" };
int IsJCC(unsigned char * opCode, char * opCMDOutput, bool size_override) {
    if (IsBetween(opCode[0], 0x70, 0x7F)) { memcpy(opCMDOutput, JCCCMD + ((int)(opCode[0] - 0x70) * 4), 3); return 1; }
    if (opCode[0] == 0x0F && IsBetween(opCode[1], 0x80, 0x8F)) { memcpy(opCMDOutput, JCCCMD + ((int)(opCode[1] - 0x80) * 4), 3); return 1; }

    if (opCode[0] == 0xE0) { memcpy(opCMDOutput, &"LOOPNE", 6); return 1; }
    if (opCode[0] == 0xE1) { memcpy(opCMDOutput, &"LOOPE", 5); return 1; }
    if (opCode[0] == 0xE2) { memcpy(opCMDOutput, &"LOOP", 4); return 1; }

    if (opCode[0] == 0xE3) { memcpy(opCMDOutput, size_override ? &"JCXZ\0" : &"JECXZ", 5); return 1; }

    return 0;
}

int Disassemble(unsigned char * opCodes, char * output, bool _32bit, int TOTAL_FILE_OFFSET, EXTRA extra) {
    int OpCodeOffset = 0;
    int OpCodeTotal = 0;
    PrefixOut prefixOut = WritePrefixes(opCodes + OpCodeOffset, output, &OpCodeOffset); output += prefixOut.prefixOffset;
    prefixOut.Address_Size_Override = (prefixOut.Address_Size_Override ^ !_32bit);
    prefixOut.Operand_Size_Override = (prefixOut.Operand_Size_Override ^ !_32bit);
    OpCodeTotal = OpCodeOffset;

    bool d = (opCodes[OpCodeOffset] % 4) >> 1; /// Add R/M to REG
    bool s = (opCodes[OpCodeOffset] % 2); /// 16/32bit values

    char * reg = (char *)calloc(4, sizeof(char));  int reg_len_off = getRegisterName( ( opCodes[OpCodeOffset + 1] % 64 ) >> 3, s || (opCodes[OpCodeOffset] == 0x62), prefixOut.Operand_Size_Override, reg  );
    char * rm = (char *)calloc(64, sizeof(char)); int rm_len_off = generateRM(opCodes + OpCodeOffset, rm, prefixOut.Address_Size_Override, prefixOut.Operand_Size_Override);

    char * opCMD = (char *)calloc(16, sizeof(char));
    if (IsRegular(opCodes + OpCodeOffset,opCMD)){
        sprintf(output, "%s %s, %s", opCMD, d ? reg : rm, d ? rm : reg);
        OpCodeOffset += rm_len_off + 1;
    } else if (IsMove(opCodes + OpCodeOffset, opCMD)) {
        char * movInstructions = (char *)calloc(64, sizeof(char));
        int movInstructions_len_off = generateMOV(opCodes + OpCodeOffset, movInstructions, prefixOut.Address_Size_Override, prefixOut.Operand_Size_Override, rm, rm_len_off);
        sprintf(output, "%s %s", opCMD, movInstructions);
        OpCodeOffset+= 1 + movInstructions_len_off;
    } else if (IsOnlyIMM(opCodes + OpCodeOffset, opCMD)) {
        char * imm = (char *)calloc(32, sizeof(char)); int imm_len_off = generateIMM(opCodes + OpCodeOffset + 1, imm, prefixOut.Address_Size_Override, prefixOut.Operand_Size_Override, (IsBetween(opCodes[OpCodeOffset], 0xD4, 0xD5) || opCodes[OpCodeOffset] == 0xCD) ? 0b00 : ((opCodes[OpCodeOffset] % 4) + 1));
        sprintf(output, "%s %s", opCMD, imm);
        OpCodeOffset += imm_len_off + 1;
    } else if (IsSingularOpCode(opCodes + OpCodeOffset, opCMD)) {
        sprintf(output, "%s %s", opCMD, SingleOpCMD(opCodes + OpCodeOffset, prefixOut.Operand_Size_Override));
        OpCodeOffset++;
    } else if (IsSegment(opCodes + OpCodeOffset, opCMD)) { sprintf(output, "%s", opCMD); OpCodeOffset++; }
    else if (IsSpecial(opCodes + OpCodeOffset, opCMD, prefixOut.Operand_Size_Override)) {
        if (IsBetween(opCodes[0], 0x90, 0x97)) {
            char * reg2 = (char *)calloc(4, sizeof(char));
            getRegisterName(opCodes[0] % 0x08, true, prefixOut.Operand_Size_Override, reg2);
            sprintf(output, "%s %s, %s", opCMD, prefixOut.Operand_Size_Override ? &"AX\0\0" : &"EAX\0", reg2);
            OpCodeOffset++;
        } else {
            char * imm = (char *)calloc(16, sizeof(char));
            int imm_len_off = generateIMM(opCodes + OpCodeOffset + 1, imm, false, prefixOut.Operand_Size_Override, s);
            sprintf(output, "%s, %s", opCMD, imm);
            OpCodeOffset += 1 + imm_len_off;
        }
    } else if (IsSpecialA(opCodes + OpCodeOffset, opCMD)) {
        char * imm = (char *)calloc(16, sizeof(char));
        char * immF = (char *)calloc(18, sizeof(char));
        int imm_len_off = generateIMM(opCodes + OpCodeOffset + 1, imm, false, prefixOut.Operand_Size_Override, true);
        sprintf(immF, "[%s]", imm);
        char * regs = {"AL\0AX\0EAX\0"};
        char * reg = (char*)calloc(4,sizeof(char));
        reg = &regs[s ? (prefixOut.Operand_Size_Override ? 3 : 6 ) : 0];
        sprintf(output, "%s %s, %s", opCMD, d ? immF : reg, d ? reg : immF);
        OpCodeOffset += 1 + imm_len_off;
    } else if (IsJMPCall(opCodes + OpCodeOffset, opCMD)) {
        char * disp = (char *)calloc(16, sizeof(char));
        OpCodeOffset += generateJMPCall(opCodes + OpCodeOffset, disp, prefixOut.Operand_Size_Override);
        sprintf(output, "%s %s", opCMD, disp);
    } else if (IsJCC(opCodes + OpCodeOffset, opCMD, prefixOut.Operand_Size_Override)) {
        char * disp = (char *)calloc(6, sizeof(char));
        bool near = false;
        OpCodeOffset++; if (opCodes[OpCodeOffset - 1] == 0x0F) { OpCodeOffset++; near = true; }
        OpCodeOffset += getDisplacementJcc(opCodes + OpCodeOffset, disp, TOTAL_FILE_OFFSET + OpCodeOffset - (near ? 2 : 1), near, prefixOut.Operand_Size_Override);
        sprintf(output, "%s %s", opCMD, disp);
    } else if (IsOneByteOpCode(opCodes + OpCodeOffset, opCMD,prefixOut.Operand_Size_Override)) {
        OpCodeOffset++;
        sprintf(output, "%s", opCMD);
    } else if (IsShift(opCodes + OpCodeOffset, opCMD)) {
        int len_off = generateShift(opCodes + OpCodeOffset, output, prefixOut.Address_Size_Override, prefixOut.Operand_Size_Override);
        OpCodeOffset += len_off;
    } else if (IsArithLogic(opCodes + OpCodeOffset, opCMD)) {
        int len_off = generateArithLogic(opCodes + OpCodeOffset, output, prefixOut.Address_Size_Override, prefixOut.Operand_Size_Override);
        OpCodeOffset += len_off;
    } else if (IsIO(opCodes + OpCodeOffset, opCMD, prefixOut.Operand_Size_Override)) {
        char * imm8 = (char *)calloc(7, sizeof(char)); int imm8_len_off = generateIMM(opCodes + OpCodeOffset + 1, imm8, false, false, 0b00);
        char * reg = (char *)calloc(4, sizeof(char)); if (s) { memcpy(reg, prefixOut.Operand_Size_Override ? &"AX\0" : &"EAX", 3); } else { memcpy(reg, &"AL", 2); }
        sprintf(output, "%s %s, %s", opCMD, d ? imm8 : reg, d ? reg : imm8);
        OpCodeOffset += 1 + imm8_len_off;
    } else if (IsMultiple80(opCodes + OpCodeOffset, opCMD)) {
        OpCodeOffset += generateMultiple80(opCodes + OpCodeOffset, output, prefixOut.Address_Size_Override, prefixOut.Operand_Size_Override);
    } else {
        if (opCodes[OpCodeOffset] == 0x69 || opCodes[OpCodeOffset] == 0x6B) { /// IMUL
            memcpy(opCMD, &"IMUL", 4);
            char * imm = (char *)calloc(16, sizeof(char));
            int imm_len_off = generateIMM(opCodes + OpCodeOffset + rm_len_off + 1, imm, false, prefixOut.Operand_Size_Override, !((opCodes[OpCodeOffset] >> 1) % 2));
            sprintf(output, "%s %s, %s, %s", opCMD, reg, rm, imm);
            OpCodeOffset+=imm_len_off+rm_len_off + 1;
        } else if (opCodes[OpCodeOffset] == 0x63) { /// ARPL r/m16, r16 --- Always 16bit
            reg = (char *)calloc(4, sizeof(char));      reg_len_off = getRegisterName( ( opCodes[OpCodeOffset + 1] % 64 ) >> 3, true, true, reg  );
            rm = (char *)calloc(64, sizeof(char));      rm_len_off = generateRM(opCodes + OpCodeOffset, rm, prefixOut.Address_Size_Override, true);
            sprintf(output, "ARPL %s, %s", rm, reg);
            OpCodeOffset+= 1 + rm_len_off;
        } else if (opCodes[OpCodeOffset] == 0x8D) { /// LEA r16/32, m
            reg = (char *)calloc(4, sizeof(char));      reg_len_off = getRegisterName( ( opCodes[OpCodeOffset + 1] % 64 ) >> 3, true, prefixOut.Operand_Size_Override, reg  );
            rm = (char *)calloc(64, sizeof(char));      rm_len_off = generateRM(opCodes + OpCodeOffset, rm, prefixOut.Address_Size_Override, prefixOut.Operand_Size_Override);
            sprintf(output, "LEA %s, %s", reg, rm);
            OpCodeOffset+= 1 + rm_len_off;
        } else if (opCodes[OpCodeOffset] == 0x8F) {
            sprintf(output, "POP %s", rm);
            OpCodeOffset += 1 + rm_len_off;
        } else if (opCodes[OpCodeOffset] == 0xC8) { /// ENTER
            char * imm16 = (char *)calloc(7, sizeof(char));
            char * imm8 = (char *)calloc(7, sizeof(char));
            int imm16_len_off = generateIMM(opCodes + OpCodeOffset + 1, imm16, true, true, 0b01);
            int imm8_len_off = generateIMM(opCodes + OpCodeOffset + imm16_len_off + 1, imm8, false, false, 0b00);
            sprintf(output, "ENTER %s, %s", imm16, imm8);
            OpCodeOffset+=1 + imm16_len_off + imm8_len_off;
        } else if (opCodes[OpCodeOffset] == 0xC4 || opCodes[OpCodeOffset] == 0xC5) {
            memcpy(opCMD, (opCodes[OpCodeOffset] % 2) ? &"LDS" : &"LES", 3);
            reg = (char *)calloc(4, sizeof(char));      reg_len_off = getRegisterName( ( opCodes[OpCodeOffset + 1] % 64 ) >> 3, true, prefixOut.Operand_Size_Override, reg  );
            rm = (char *)calloc(64, sizeof(char));      rm_len_off = generateRM(opCodes + OpCodeOffset, rm, prefixOut.Address_Size_Override, prefixOut.Operand_Size_Override);
            sprintf(output, "%s %s, %s", opCMD, reg, rm);
            OpCodeOffset+= 1 + rm_len_off;
        } else if (opCodes[OpCodeOffset] == 0xFF) { /// INC / DEC / CALL / CALL FAR / JMP / JMP FAR / PUSH
            if (opCodes[OpCodeOffset+1] == 0xFF) {
                /// 0xFF 0xFF is a signature that the code section is over.
                sprintf(output, "=== === ENDS === ===");
                OpCodeOffset = 0xFFFF;
            } else {
                OpCodeOffset += generateMultipleFF(opCodes + OpCodeOffset, output, prefixOut.Address_Size_Override, prefixOut.Operand_Size_Override);}
        } else if (opCodes[OpCodeOffset] == 0xC2 || opCodes[OpCodeOffset] == 0xCA) {
            if (opCodes[OpCodeOffset] == 0xC2) { memcpy(opCMD, &"RETN", 4); }
            else if (opCodes[OpCodeOffset] == 0xCA) { memcpy(opCMD, &"RETF", 4); }
            char * imm16 = (char *)calloc(7, sizeof(char));
            int imm16_len_off = generateIMM(opCodes + OpCodeOffset + 1, imm16, true, true, 0b01);
            sprintf(output, "%s %s", opCMD, imm16);
            OpCodeOffset += 1 + imm16_len_off;
        }
    }

    output -= prefixOut.prefixOffset;

    if (extra.DEBUG_MODE) { printf("\nOp codes used (in bytes): %d for operation: %s\n", OpCodeOffset, output); }

    return OpCodeOffset;

}

#endif // DISASSEMBLEMASTER_H_INCLUDED
