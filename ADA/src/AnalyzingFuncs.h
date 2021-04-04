#ifndef ANALYZINGFUNCS_H_INCLUDED
#define ANALYZINGFUNCS_H_INCLUDED

#include "Analysis/GetStrings.h"
#include "Analysis/GetDisassembly.h"
#include "Analysis/AnalyzeIData.h"

external_functions_info_t * external_functions_info;

int Analyze(char * funcName, Portable_Executable * PE) {

    if (!strcmp(funcName, "exit")) {
        exit(0);
    } else if (!strcmp(funcName, "disassemble")) {
        PrintDisassembly(PE, external_functions_info);
	}
	else if (!strcmp(funcName, "list_funcs")) {
		external_functions_info = Load_idata_info(PE);
    } else if (!strcmp(funcName, "strings")) {
        int minLength;
        printf("Function %s requires a minimum length: ", funcName);
        if (scanf("%d", &minLength)) {
            PrintStrings(PE, minLength);
            //GetStringsAboveLength(PE, minLength);
        }
    } else if (!strcmp(funcName, "hashes")) {
        PrintHashes(PE);
    } else {
        return 1;
    }
    return 0;
}

#endif // ANALYZINGFUNCS_H_INCLUDED
