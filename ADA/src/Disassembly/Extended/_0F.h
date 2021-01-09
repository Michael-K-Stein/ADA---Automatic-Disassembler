#ifndef __0F_Extended
#define __0F_Extended

#include "../Common.h"

int _0F_Singular(unsigned char * opCodes, char * output, bool addr_ovrd, bool size_ovrd) {
	switch (opCodes[0]) {
	case 0x06: { sprintf(output, 128, "CLTS"); break; }
	case 0x08: { sprintf(output, 128, "INVD"); break; }
	case 0x09: { sprintf(output, 128, "WBINVD"); break; }

	case 0xFF: { sprintf(output, 128, "UD0"); break; }
	case 0xB9: { sprintf(output, 128, "UD1"); break; }
	case 0x0B: { sprintf(output, 128, "UD2"); break; }

	case 0x30: { sprintf(output, 128, "WRMSR"); break; }
	case 0x31: { sprintf(output, 128, "RDTSC"); break; }
	case 0x32: { sprintf(output, 128, "RDMSR"); break; }
	case 0x33: { sprintf(output, 128, "RDPMC"); break; }
	case 0x34: { sprintf(output, 128, "SYSENTER"); break; }
	case 0x35: { sprintf(output, 128, "SYSEXIT"); break; }

	case 0x37: { sprintf(output, 128, "GETSEC"); break; }

	case 0xA0: { sprintf(output, 128, "PUSH FS"); break; }
	case 0xA1: { sprintf(output, 128, "POP FS"); break; }

	case 0xA2: { sprintf(output, 128, "CPUID"); break; }

	case 0xA8: { sprintf(output, 128, "PUSH GS"); break; }
	case 0xA9: { sprintf(output, 128, "POP GS"); break; }

	case 0xAA: { sprintf(output, 128, "RSM"); break; }
	default: { return 0; break; }
	}
	return 1;
}

int _0F_00(unsigned char * opCodes, char * output, bool addr_ovrd, bool size_ovrd) {
	char regVal = (opCodes[1] >> 3) % 8;

	char * rm2 = (char*)calloc(64, sizeof(char));  int rm_len_off = generateRM_fullReg(opCodes, rm2, addr_ovrd, size_ovrd || regVal >= 2);

	switch (regVal)
	{
	case 0: { sprintf(output, 128, "SLDT %s", rm2); break; }
	case 1: { sprintf(output, 128, "STR %s", rm2); break; }
	case 2: { sprintf(output, 128, "LLDT %s", rm2); break; }
	case 3: { sprintf(output, 128, "LTR %s", rm2); break; }
	case 4: { sprintf(output, 128, "VERR %s", rm2); break; }
	case 5: { sprintf(output, 128, "VERW %s", rm2); break; }
	default:
		break;
	}

	free(rm2);

	return rm_len_off;
}

int _0F_01(unsigned char * opCodes, char * output, bool addr_ovrd, bool size_ovrd) {
	int rm_len_off = 1;

	switch (opCodes[1]) {
		case 0xC1: { sprintf(output, 128, "VMCALL"); break; }
		case 0xC2:{ sprintf(output, 128, "VMLAUNCH"); break; }
		case 0xC3:{ sprintf(output, 128, "VMRESUME"); break; }
		case 0xC4:{ sprintf(output, 128, "VMXOFF"); break; }
		case 0xC8:{ sprintf(output, 128, "MONITOR"); break; }
		case 0xC9:{ sprintf(output, 128, "MWAIT"); break; }
		case 0xD0:{ sprintf(output, 128, "XGETBV"); break; }
		case 0xD1:{ sprintf(output, 128, "XSETBV"); break; }
		case 0xF9:{ sprintf(output, 128, "RDTSCP"); break; }
	default:
		char regVal = (opCodes[1] >> 3) % 8;

		char * rm = (char*)calloc(64, sizeof(char));  rm_len_off = generateRM_fullReg(opCodes, rm, addr_ovrd, size_ovrd || regVal == 6);

		switch (regVal)
		{
		case 0: { sprintf(output, 128, "SGDT %s", rm); break; }
		case 1: { sprintf(output, 128, "SIDT %s", rm); break; }
		case 2: { sprintf(output, 128, "LGDT %s", rm); break; }
		case 3: { sprintf(output, 128, "LIDT %s", rm); break; }
		case 4: { sprintf(output, 128, "SMSW %s", rm); break; }
		case 6: { sprintf(output, 128, "LMSW %s", rm); break; }
		case 7: { sprintf(output, 128, "INVLPG %s", rm); break; }
		default:
			break;
		}

		free(rm);
	}


	return rm_len_off;
}

int _0F_0203(unsigned char * opCodes, char * output, bool addr_ovrd, bool size_ovrd) {
	char * opCMD = (char *)calloc(32, sizeof(char));
	if (opCodes[0] == 0x02) { memcpy(opCMD, &"LAR", 3); }
	else if (opCodes[0] == 0x03) { memcpy(opCMD, &"LSL", 3); }
	
	char * reg = (char *)calloc(4, sizeof(char));  int reg_len_off = getRegisterName((opCodes[1] % 64) >> 3, true, size_ovrd, reg);
	char * rm = (char *)calloc(64, sizeof(char)); int rm_len_off = generateRM_fullSize(opCodes, rm, addr_ovrd, true);

	sprintf(output, 128, "%s %s, %s", opCMD, reg, rm);

	free(opCMD);
	free(reg);
	free(rm);

	return rm_len_off;
}

int Disassemble_0F(unsigned char * opCodes, char * output, int TOTAL_FILE_OFFSET, bool addr_ovrd, bool size_ovrd) {

	if (_0F_Singular(opCodes, output, addr_ovrd, size_ovrd)) {
		return 1;
	}
	else if (opCodes[0] == 0x00) {
		return 1 + _0F_00(opCodes, output, addr_ovrd, size_ovrd);
	}
	else if (opCodes[0] == 0x01) {
		return 1 + _0F_01(opCodes, output, addr_ovrd, size_ovrd);
	}
	else if (opCodes[0] == 0x02 || opCodes[0] == 0x03) {
		return 1 + _0F_0203(opCodes, output, addr_ovrd, size_ovrd);
	}
	return 0;
}

#endif // !__0F_Extended