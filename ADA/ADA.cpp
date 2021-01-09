// ADA.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <map>
#include <iostream>
#include <string>
#include "src/source.h"
#include "src/Disassembly/DisassembleMaster.h"
#include "stdio.h"

//#include "leak_detector_c.h"

using namespace std;


int getASMInput(unsigned char * dest) {
	unsigned char * inp = (unsigned char *)malloc(33 * sizeof(unsigned char));
	int * inp2 = (int *)malloc(33 * sizeof(int));
	bool finished = false;
	int len = 0;
	cout << endl << "Input: ";
	for (int i = 0; i < 32; i++) {
		if (!finished) {
			cin >> std::hex >> inp2[i];
			finished = inp2[i] >= 256;
			inp[i] = (inp2[i] % 256);
			len++;
		}
		else { inp[i] = 0; }
	}
	inp[32] = '\0';
	len--;
	cout << endl << endl;
	memcpy(dest, inp, 32 * sizeof(char));
	return len;
}

#define PRETTY_PRINTING true

int main()
{
	//unsigned char dat[8] = { 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0 };
	// Stress testing!
	/*for (int i = 0; i < 100000; i++) {
		//getSegmentName(i % 8);
		//getRegisterName(i % 8, rand() % 2, rand() % 2, out);
		//getRMName(i % 8, rand() % 2, rand() % 2, rand() % 4, out);
		//getDisplacement(dat, 1, 0, out);									| 16
		//getDisplacementJcc(dat, out, 0, 1, rand() % 2);					| 16
		//getSIB(dat2, rand() % 2, rand() % 4, rand() % 2, out);			| 64
		//generateRM_ZERO_BYTE_DISPLACEMENT(dat2, out, i % 8, rand() % 2);	| 64
		//generateRM_ONE_BYTE_DISPLACEMENT(dat2, out, i % 8, rand() % 2);	| 64
		//generateRM_FOUR_BYTE_DISPLACEMENT(dat2, out, i % 8, rand() % 2);	| 64
		//generateRM(dat2, out, i % 2, rand() % 2);							| 64
		//generateIMM(dat2, out, i % 2, rand() % 2, rand() % 4);			| 16
		//generateMOV(dat2, out, i % 2, rand() % 2,rm, rmo);				| 128
		//generateShift(dat2, out, i % 2, rand() % 2);						| 128
		//generateArithLogic(dat2, out, rand() % 2, rand() % 2);			| 128
		//generateMultiple80(dat2, out, rand() % 2, rand() % 2);			| 128
		//generateMultipleFF(dat2, out, rand() % 2, rand() % 2);			| 128
		//SingleOpCMD(dat2, out, i % 2);									| 16
		//generateJMPCall(dat2, out, i % 2);								| 64
		unsigned char * dat2 = (unsigned char *)calloc(16, 1);
		unsigned char d = i % 256;
		unsigned char d2 = (i*17) % 256;
		memcpy(dat2, &d, 1);
		memcpy(dat2+1, &d2, 1);
		memcpy(dat2 + 2, dat, 8);
		//char * rm = (char *)calloc(64, sizeof(char));
		char * out = (char *)calloc(256, sizeof(char));
		//int rmo = generateRM(dat2, rm, rand() % 2, rand() % 2);
		EXTRA extra; extra.DEBUG_MODE = false; extra.FORCE = true;
		Disassemble(dat2, out, rand() % 2, 0, extra);
		
		//generateJMPCall(dat2, out, i % 2);
		printf("%s\n",out);
		free(out);
		//free(rm);
		free(dat2);

	}*/

	/*
	

	FILE * fPtr =fopen("disassTest", "rb");
	uint64_t fsize = getFileSize("disassTest");
	unsigned char * buf = (unsigned char *)calloc(fsize, sizeof(unsigned char));

	printf("File size is: %d", fsize);

	if (fread(buf, fsize, sizeof(unsigned char), fPtr)) {
		int codeOffset = 0;
		while (codeOffset < fsize) {
			char * dec = (char *)calloc(64, sizeof(char));
			int opCodeOffset = Disassemble(buf + codeOffset, dec, false, codeOffset, extra);
			if (PRETTY_PRINTING) {
				char * origOpCodes = (char *)calloc(opCodeOffset, 3 * sizeof(char)); memcpy(origOpCodes, getOriginalOpCodes(buf + codeOffset, opCodeOffset), (opCodeOffset * 3) - 1);
				char * tableDelim = (char *)calloc(6, sizeof(char)); for (int i = 0; i < 4 - ceil(opCodeOffset /2.0); i++) { tableDelim[i] = '\t'; }
				printf("\n0x%.2X\t|\t%s\t%s%s", codeOffset, origOpCodes, tableDelim, dec);
			} else {
				printf("\n%s : %d", dec, opCodeOffset);
			}
			codeOffset+= opCodeOffset;
		}
	}*/
	//EXTRA extra; extra.DEBUG_MODE = false;
	/*while (1) {
		unsigned char * input = (unsigned char *)calloc(32, sizeof(unsigned char));
		int len = getASMInput(input);
		int codeOffset = 0;
		while (codeOffset < len) {
			char * dec = (char *)calloc(256, sizeof(char));
			int opCodeOffset = Disassemble(input + codeOffset, dec, true, codeOffset, extra);
			printf("\n\n\n%s : %d", dec, opCodeOffset); printf("\n\n\n");
			codeOffset+= opCodeOffset;
			free(dec);
		}
		free(input);
	}*/
	if (StartUp()) {
		printf("\n\n\nSomething went wrong!\n\n\n");

		int a; std::cin >> a;

		return 1;
	}
	else {
		int a; std::cin >> a;
		return 0;
	}


	return 0;
}


// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
