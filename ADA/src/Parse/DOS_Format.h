/**
    Here are the parts of common DOS file types.
*/

#ifndef __ParseDOS_FORMAT
#define __ParseDOS_FORMAT

#include <stdbool.h>
#include <stdint.h>

typedef struct DOS_Header {
    unsigned char DOS_Signature[2]; // Should be MZ
    unsigned char Header_Data[58];
    unsigned char DOS_Pointer_to_PE_Header[4];
    unsigned char DOS_STUB[64];
} DOS_Header;

typedef struct COFF_Header {
    unsigned char Signature[4]; // Should be 0x50450000
    unsigned char Machine[2];
    unsigned char Number_Of_Sections[2];
    unsigned char Time_Date_Stamp[4];
    unsigned char Pointer_To_Symbol_Table[4];
    unsigned char Number_Of_Symbol_Table[4];
    unsigned char Size_Of_Optional_Header[2];
    unsigned char Characteristics[2];
} COFF_Header;

typedef struct Standard_COFF_Fields {
    unsigned char Magic[2];
    unsigned char Major_Linker_Version[1];
    unsigned char Minor_Linker_Version[1];
    unsigned char Size_Of_Code[4];
    unsigned char Size_Of_Initialized_Data[4];
    unsigned char Size_Of_Uninitialized_Data[4];
    unsigned char Address_Of_Entry_Point[4];
    unsigned char Base_Of_Code[4];
    unsigned char Base_Of_Data[4];
} Standard_COFF_Fields;

typedef struct Windows_Specific_Fields {
    unsigned char Image_Base[4];
    unsigned char Section_Alignment[4];
    unsigned char File_Alignment[4];
    unsigned char Major_Operating_System_Version[2];
    unsigned char Minor_Operating_System_Version[2];
    unsigned char Major_Image_Version[2];
    unsigned char Minor_Image_Version[2];
    unsigned char Major_Subsystem_Version[2];
    unsigned char Minor_Subsystem_Version[2];
    unsigned char Win_32_Version_Value[4];
    unsigned char Size_Of_Image[4];
    unsigned char Size_Of_Headers[4];
    unsigned char Check_Sum[4];
    unsigned char Subsystem[2];
    unsigned char DLL_Characteristics[2];
    unsigned char Size_Of_Stack_Reserve[4];
    unsigned char Size_Of_Stack_Commit[4];
    unsigned char Size_Of_Heap_Reserve[4];
    unsigned char Size_Of_Heap_Commit[4];
    unsigned char Loader_Flags[4];
    unsigned char Number_Of_RVA_And_Sizes[4];

} Windows_Specific_Fields;

typedef struct Data_Directories {
    unsigned char Export_Table[4];
    unsigned char Size_Of_Export_Table[4];

    unsigned char Import_Table[4];
    unsigned char Size_Of_Import_Table[4];

    unsigned char Resource_Table[4];
    unsigned char Size_Of_Resource_Table[4];

    unsigned char Exception_Table[4];
    unsigned char Size_Of_Exception_Table[4];

    unsigned char Certificate_Table[4];
    unsigned char Size_Of_Certificate_Table[4];

    unsigned char Base_Relocation_Table[4];
    unsigned char Size_Of_Base_Relocation_Table[4];

    unsigned char Debug_Table[4];
    unsigned char Size_Of_Debug_Table[4];

    unsigned char Architecture_Data[4];
    unsigned char Size_Of_Architecture_Data[4];

    unsigned char Global_Pointer[4];
    unsigned char Zero_Check_Valid_1[4]; // Should be 00 00 00 00 | 4 bytes of \0

    unsigned char TLS_Table[4];
    unsigned char Size_Of_TLS_Table[4];

    unsigned char Load_Config_Table[4];
    unsigned char Size_Of_Load_Config_Table[4];

    unsigned char Bound_Import_Table[4];
    unsigned char Size_Of_Bound_Import_Table[4];

    unsigned char Import_Address_Table[4];
    unsigned char Size_Of_Import_Address_Table[4];

    unsigned char Delay_Import_Descriptor_Table[4];
    unsigned char Size_Of_Delay_Import_Descriptor_Table[4];

    unsigned char CLR_Runtime_Header[4];
    unsigned char Size_Of_CLR_Runtime_Header[4];

    unsigned char Zero_Check_Valid_2[8]; // Should be 00 00 00 00 00 00 00 00 | 8 bytes of \0
} Data_Directories;

typedef struct Section_Data {
    unsigned char * data;
} Section_Data;

typedef struct Section_Table {
    char Name[8];
    unsigned char Virtual_Size[4];
    unsigned char Virtual_Address[4];
    unsigned char Size_Of_Raw_Data[4];
    unsigned char Pointer_To_Raw_Data[4];
    unsigned char Pointer_To_Relocations[4];
    unsigned char Pointer_To_Line_Numbers[4];
    unsigned char Number_Of_Relocations[2];
    unsigned char Number_Of_Line_Numbers[2];
    unsigned char Characteristics[4];

    Section_Data * section_data;
} Section_Table;

typedef struct Portable_Executable_Header {
    DOS_Header dos_header;
    COFF_Header coff_header;
    Standard_COFF_Fields standard_coff_fields;
    Windows_Specific_Fields windows_specific_fields;
    Data_Directories data_directories;
} Portable_Executable_Header;


// This structure includes all the data of a PE file type.
typedef struct Portable_Executable {
    Portable_Executable_Header portable_executable_header;
    Section_Table * section_tables;
} Portable_Executable;


#endif // __ParseDOS_FORMAT
