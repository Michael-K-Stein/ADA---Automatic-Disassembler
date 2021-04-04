#ifndef COMMONANALYSIS_H_INCLUDED
#define COMMONANALYSIS_H_INCLUDED

#include "../General/List.h"

const  char rdataName[] = {	0x2E, 0x72, 0x64, 0x61, 0x74, 0x61, 0x00, 0x00 };
const  char rsrcName[] = {	0x2E, 0x72, 0x73, 0x72, 0x63, 0x00, 0x00, 0x00 };
const  char textName[] = {	0x2E, 0x74, 0x65, 0x78, 0x74, 0x00, 0x00, 0x00 };
const  char idataName[] = {	0x2E, 0x69, 0x64, 0x61, 0x74, 0x61, 0x00, 0x00 };

typedef struct idata_function_info_t {
	uint16_t size;
	uint32_t address_call;
	char * function_name;
};

/**
	=== IMAGE IMPORT DESCRIPTOR LAYOUT ===
	4bytes: Virtual Address of functions address' list
	4bytes: Null
	4bytes: Null
	4bytes: Address of DLL name
	4bytes: Address to first function decleration
*/
typedef struct idata_DLL_info_t {

	uint32_t address_functions_address_list;
	unsigned char null_bytes[8];
	uint32_t address_dll_name;
	uint32_t address_function_declerations;

	List * functions;
	char * dll_name;
};
//#define idata_dll_info_size 20 // Size of image import descriptor in bytes (including null bytes)

// For indirect jumps at end of .text section
typedef struct external_function_jmp_t {
	uint32_t called_address;
	uint32_t call_address;
};
typedef struct extern_functions_t {
	List * functions;
};

typedef struct external_functions_info_t {
	int count;
	List * dll_infos;
	List * external_function_names;
	extern_functions_t * extern_funcs;
};

int getSectionIndexFromSectionName(Portable_Executable * PE, const char * sectionName) {
	int secInd = -1;

	for (int i = 0; i < getNumberOfSections(PE); i++) {
		if (strcmp(PE->section_tables[i].Name, sectionName) == 0) {
			secInd = i;
		}
	}

	return secInd;
}

uint32_t Address_virtual_to_real(uint32_t address, Section_Table * section_table) {
	return address - b16to10_4Bytes(section_table->Virtual_Address);
}

uint32_t Address_relative_to_absolute(uint32_t address, Portable_Executable * PE) {
	return address + b16to10_4Bytes(PE->portable_executable_header.windows_specific_fields.Image_Base);
}

char * Get_function_name_from_call_address(Portable_Executable * PE, external_functions_info_t * external_functions_info, extern_functions_t * extrn_funcs, uint32_t call_address) {
	for (int dll_index = 0; dll_index < external_functions_info->dll_infos->length; dll_index++) {
		idata_DLL_info_t * dll_info = (idata_DLL_info_t*)(GetElementAtIndex(external_functions_info->dll_infos, dll_index)->ptr_to_value);
		int amount_of_functions = dll_info->functions->length;

		for (int func_index = 0; func_index < amount_of_functions; func_index++) {
			idata_function_info_t * func_info = (idata_function_info_t*)GetElementAtIndex(dll_info->functions, func_index)->ptr_to_value;

			if (func_info->address_call == call_address) {
				return func_info->function_name;
			}
		}

	}

	for (int jmp_index = 0; jmp_index < extrn_funcs->functions->length; jmp_index++) {
		external_function_jmp_t * ext_f = (external_function_jmp_t*)(GetElementAtIndex(extrn_funcs->functions, jmp_index)->ptr_to_value);
		if (ext_f->called_address == call_address) {
			return Get_function_name_from_call_address(PE, external_functions_info, extrn_funcs, ext_f->call_address);
		}
	}
	return NULL;
}

char * Get_function_dll_from_call_address(Portable_Executable * PE, external_functions_info_t * external_functions_info, extern_functions_t * extrn_funcs, uint32_t call_address) {
	for (int dll_index = 0; dll_index < external_functions_info->dll_infos->length; dll_index++) {
		idata_DLL_info_t * dll_info = (idata_DLL_info_t*)(GetElementAtIndex(external_functions_info->dll_infos, dll_index)->ptr_to_value);
		int amount_of_functions = dll_info->functions->length;

		for (int func_index = 0; func_index < amount_of_functions; func_index++) {
			idata_function_info_t * func_info = (idata_function_info_t*)GetElementAtIndex(dll_info->functions, func_index)->ptr_to_value;

			if (func_info->address_call == call_address) {
				return dll_info->dll_name;
			}
		}
	}

	for (int jmp_index = 0; jmp_index < extrn_funcs->functions->length; jmp_index++) {
		external_function_jmp_t * ext_f = (external_function_jmp_t*)(GetElementAtIndex(extrn_funcs->functions, jmp_index)->ptr_to_value);
		if (ext_f->called_address == call_address) {
			return Get_function_dll_from_call_address(PE, external_functions_info, extrn_funcs, ext_f->call_address);
		}
	}
	return NULL;
}


#endif // COMMONANALYSIS_H_INCLUDED
