#pragma once

#include "GetDisassembly.h"


/*int UnloadIdataFunctionsInfo() {
	int count = external_functions_info.count;
	for (int function_index = 0; function_index < count; function_index++) {
		free(external_functions_info.externalFunctionNames[function_index].functionName);
	}

	return count;
}*/

int Load_idata_image_import_descriptors(unsigned char * idata_section_start_ptr, external_functions_info_t * external_functions_info) {
	idata_DLL_info_t * tmp_idata_dll_info = (idata_DLL_info_t*)calloc(1, sizeof(idata_DLL_info_t));

	idata_DLL_info_t * null_idata_dll_info = (idata_DLL_info_t*)calloc(1, sizeof(idata_DLL_info_t));

	external_functions_info->dll_infos = InitializeList(sizeof(idata_DLL_info_t));

	uint32_t offset = 0;
	do {
		memcpy(tmp_idata_dll_info, idata_section_start_ptr + offset, sizeof(idata_DLL_info_t)-(sizeof(List*)+sizeof(char*)));
		if ((memcmp(tmp_idata_dll_info, null_idata_dll_info, sizeof(idata_DLL_info_t) - (sizeof(List*) + sizeof(char*))))) {
			AddItemToList(external_functions_info->dll_infos, tmp_idata_dll_info);
		}
		offset += sizeof(idata_DLL_info_t) - (sizeof(List*) + sizeof(char*));
	} while (memcmp(tmp_idata_dll_info, null_idata_dll_info, sizeof(idata_DLL_info_t) - (sizeof(List*) + sizeof(char*))));

	return external_functions_info->dll_infos->length;
}

int Print_idata_functions(Portable_Executable * PE, external_functions_info_t * external_functions_info, Section_Table * idata_section) {
	uint32_t total_offset = 0;

	for (int dll_index = 0; dll_index < external_functions_info->dll_infos->length; dll_index++) {
		idata_DLL_info_t * dll_info = (idata_DLL_info_t*)GetElementAtIndex(external_functions_info->dll_infos, dll_index)->ptr_to_value;
	
		dll_info->dll_name = (char*)idata_section->section_data->data + Address_virtual_to_real(dll_info->address_dll_name, idata_section);

		//printf("DLL name: %s\n", idata_section->section_data->data + Address_virtual_to_real(dll_info->address_dll_name, idata_section));
	
		dll_info->functions = InitializeList(sizeof(idata_function_info_t));

		uint32_t func_name_addr = 0;
		uint32_t func_name_addr_ptr = 0;
		uint32_t offset = 0;
		do {
			func_name_addr_ptr = Address_virtual_to_real(dll_info->address_functions_address_list, idata_section) + offset;
			func_name_addr = b16to10_4Bytes(idata_section->section_data->data + func_name_addr_ptr) ;
			
			idata_function_info_t * idata_function_info = (idata_function_info_t*)malloc(sizeof(idata_function_info_t));
			idata_function_info->function_name = (char*)idata_section->section_data->data + Address_virtual_to_real(func_name_addr, idata_section) + 2;

			if (func_name_addr) {
				uint32_t tmp_offset = func_name_addr_ptr + 4;
				idata_function_info->address_call = 0;
				while (idata_function_info->address_call == 0)
				{
					uint32_t tmp_loaded_mem = b16to10_4Bytes(idata_section->section_data->data + tmp_offset);

					if (tmp_loaded_mem == func_name_addr) {
						idata_function_info->address_call = Address_relative_to_absolute(tmp_offset, PE) + b16to10_4Bytes(idata_section->Virtual_Address);
					}

					tmp_offset += 4;
				}

				AddItemToList(dll_info->functions, idata_function_info);

				//printf("\t0x%.8X | 0x%.8X | %s\n", Address_relative_to_absolute(func_name_addr_ptr, PE), Address_relative_to_absolute(func_name_addr, PE), idata_section->section_data->data + Address_virtual_to_real(func_name_addr, idata_section) + 2);
			}
			free(idata_function_info);
			offset += 4;
		} while (func_name_addr != 0);
		total_offset = offset + Address_virtual_to_real(dll_info->address_functions_address_list, idata_section);
	}
	/*
	uint32_t name_address = 0;
	while (total_offset < b16to10_4Bytes(idata_section->Size_Of_Raw_Data)) {
		
		name_address = b16to10_4Bytes(idata_section->section_data->data + total_offset);

		for (int dll_index = 0; dll_index < external_functions_info->dll_infos->length; dll_index++) {
			idata_DLL_info_t * dll_info = (idata_DLL_info_t*)GetElementAtIndex(external_functions_info->dll_infos, dll_index)->ptr_to_value;

			int amount_of_functions = dll_info->functions->length;

			for (int func_index = 0; func_index < amount_of_functions; func_index++) {
				idata_function_info_t * func_info = (idata_function_info_t*)GetElementAtIndex(dll_info->functions, func_index)->ptr_to_value;

				uint32_t tmp_addr = Address_relative_to_absolute(total_offset, PE);
				uint32_t tmp = b16to10_4Bytes(idata_section->section_data->data + func_info->address_call - Address_relative_to_absolute(0, PE));

				if (tmp == name_address) {
					func_info->address_call = tmp_addr + b16to10_4Bytes(idata_section->Virtual_Address);
				}
			}
		}
		total_offset += 4;
	}*/

	return 0;
}

int Load_idata_functions_info(Portable_Executable * PE, external_functions_info_t * external_functions_info) {



	external_functions_info->external_function_names = InitializeList(sizeof(idata_function_info_t));

	int idata_section_index = getSectionIndexFromSectionName(PE, idataName);

	uint32_t vptr = b16to10_4Bytes(PE->section_tables[idata_section_index].Virtual_Address);
	uint32_t idata_sec_size = b16to10_4Bytes(PE->section_tables[idata_section_index].Size_Of_Raw_Data);

	unsigned char * start_ptr = PE->section_tables[idata_section_index].section_data->data;




	uint32_t function_name_address = -1;
	int offset = 0;
	do {
		function_name_address = b16to10_4Bytes(start_ptr + offset);

		uint32_t addr = function_name_address - vptr;

		if (function_name_address != 0x00 && (addr + 2) < idata_sec_size) {

			idata_function_info_t * func_info = (idata_function_info_t*)malloc(sizeof(idata_function_info_t));

			func_info->function_name = (char*)start_ptr + addr + 2;
			func_info->size = b16to10_2Bytes(start_ptr + addr);
			//strcpy(func_info->function_name, (char*)start_ptr + addr + 2);

			AddItemToList(external_functions_info->external_function_names, func_info);
			

			printf("Address: 0x%.8X | Size: 0x%.4X | Name: %s\n", func_info->address_call, func_info->size, func_info->function_name);

			free(func_info);
		}
		offset += 4;
	} while (offset < idata_sec_size);

	for (int i = 0; i < external_functions_info->external_function_names->length; i++) {
		idata_function_info_t * func_info = (idata_function_info_t*)GetElementAtIndex(external_functions_info->external_function_names, i)->ptr_to_value;
		printf("Address: 0x%.8X | Size: 0x%.4X | Name: %s\n", func_info->address_call, func_info->size, func_info->function_name);
	}

	return 0;
}

extern_functions_t * AnalyzeExternalFunctions(Portable_Executable * PE, external_functions_info_t * external_function_info) {
	extern_functions_t * extrn_funcs;

	extrn_funcs = (extern_functions_t*)calloc(1, sizeof(extern_functions_t));
	extrn_funcs->functions = InitializeList(sizeof(external_function_jmp_t));

	int textIndex = -1;
	/*for (int i = 0; i < getNumberOfSections(PE); i++) {
		if (memcmp(PE->section_tables[i].Name, textName, sizeof(textName)) == 0) {
			textIndex = i;
		}
	}*/

	textIndex = getSectionIndexFromSectionName(PE, textName);


	if (textIndex != -1) {

		unsigned char * buf = PE->section_tables[textIndex].section_data->data;
		int bufSize = b16to10_4Bytes(PE->section_tables[textIndex].Size_Of_Raw_Data);

		int sectionOffset = b16to10_4Bytes(PE->section_tables[textIndex].Virtual_Address) + b16to10_4Bytes(PE->portable_executable_header.windows_specific_fields.Image_Base);

		EXTRA extra; extra.DEBUG_MODE = false; extra.FORCE = true;

		double ratT = 0; double ratC = 0; double ratF = 0;

		int codeOffset = 0;
		while (codeOffset < bufSize) {
			char * dec = (char *)calloc(256, sizeof(char));
			int opCodeOffset = Disassemble(buf + codeOffset, dec, true, codeOffset, extra);
			if (opCodeOffset > 0 && opCodeOffset != 0xFFFF) {
				ratC++;
				if (true) {

					bool known_call = false;
					char * dll_name = NULL;
					char * func_name = NULL;

					if (isCall) {
						isCall = false;
						uint32_t entyP = (int)strtol(callAddr, NULL, 16); // Function address
						dll_name = Get_function_dll_from_call_address(PE, external_function_info, extrn_funcs, sectionOffset + codeOffset + opCodeOffset + entyP);
						func_name = Get_function_name_from_call_address(PE, external_function_info, extrn_funcs, sectionOffset + codeOffset + opCodeOffset + entyP);
						if (dll_name == NULL && func_name == NULL) {
							external_function_jmp_t * ext_f = (external_function_jmp_t*)calloc(1, sizeof(external_function_jmp_t));
							ext_f->called_address = sectionOffset + codeOffset;
							if (entyP > sectionOffset) {
								// absolute
								ext_f->call_address = entyP;
							}
							else {
								// relative
								ext_f->call_address = sectionOffset + codeOffset + opCodeOffset + entyP;
							}
							AddItemToList(extrn_funcs->functions, ext_f);
						}
						else {
							known_call = true;
						}
					}
				}
				else {
				}
				codeOffset += opCodeOffset;
			}
			else if (opCodeOffset == 0xFFFF) {
				codeOffset = bufSize;
			}
			else {
				ratF++;
				if (extra.FORCE) { codeOffset++; }
				else {
					printf("Something is broken!!!");
				}
			}
			free(dec);
		}
		ratT = ratC + ratF;

	}
	else { return extrn_funcs; }
	return extrn_funcs;
}


external_functions_info_t * Load_idata_info(Portable_Executable * PE) {
	external_functions_info_t * external_functions_info = (external_functions_info_t*)calloc(1, sizeof(external_functions_info_t));

	// Get idata section info
	int idata_section_index = getSectionIndexFromSectionName(PE, idataName);
	Section_Table * idata_section = PE->section_tables + idata_section_index;

	uint32_t idata_vptr = b16to10_4Bytes(idata_section->Virtual_Address);
	uint32_t idata_sec_size = b16to10_4Bytes(idata_section->Size_Of_Raw_Data);

	unsigned char * idata_start_ptr = idata_section->section_data->data;


	int amount_of_dlls_loaded = Load_idata_image_import_descriptors(idata_start_ptr, external_functions_info);

	Print_idata_functions(PE, external_functions_info, idata_section);

	for (int dll_index = 0; dll_index < amount_of_dlls_loaded; dll_index++) {
		idata_DLL_info_t * dll_info = (idata_DLL_info_t*)(GetElementAtIndex(external_functions_info->dll_infos, dll_index)->ptr_to_value);
		int amount_of_functions = dll_info->functions->length;

		printf("%s\n", dll_info->dll_name);

		for (int func_index = 0; func_index < amount_of_functions; func_index++) {
			idata_function_info_t * func_info = (idata_function_info_t*)GetElementAtIndex(dll_info->functions, func_index)->ptr_to_value;

			printf("\t0x%.8X | %s\n", func_info->address_call, func_info->function_name);
		}

	}

	extern_functions_t * extern_functions = AnalyzeExternalFunctions(PE, external_functions_info);

	external_functions_info->extern_funcs = extern_functions;

	return external_functions_info;
}

