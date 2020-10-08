/*int AppendToDOS_Format_Old(Portable_Executable * PE, uint64_t byteInd, unsigned char c) {
    if (byteInd < 64) { // First 64 bytes are DOS_Header
        if (byteInd < 2) { PE->dos_header.DOS_Signature[byteInd] = c; } // First 2 bytes of header are Signature
        else if (byteInd >= 60) { PE->dos_header.DOS_Pointer_to_PE_Header[byteInd - 60] = c; } // Last 4 bytes are Pointer to PE Header
        else { PE->dos_header.Header_Data[byteInd - 2] = c; } // Middle bytes are header data

    } else if (byteInd < 64 + 64) {
        PE->dos_header.DOS_STUB[byteInd - 64] = c; // 64 bytes DOS_STUB
    } else if (byteInd < 128 + 24) { // 24 bytes COFF Header
        if (byteInd < 128 + 4) { PE->coff_header.Signature[byteInd-128] = c; } // COFF Signature - 4 bytes
        else if (byteInd < 128 + 6) { PE->coff_header.Machine[byteInd-(128 + 4)] = c; } // COFF Machine - 2 bytes
        else if (byteInd < 128 + 8) { PE->coff_header.Number_Of_Sections[byteInd-(128 + 6)] = c; } // COFF Number of Sections - 2 bytes
        else if (byteInd < 128 + 12) { PE->coff_header.Time_Date_Stamp[byteInd-(128 + 8)] = c; } // COFF TimeDateStamp - 4 bytes
        else if (byteInd < 128 + 16) { PE->coff_header.Pointer_To_Symbol_Table[byteInd-(128 + 12)] = c; } // COFF Pointer to Symbol Table - 4 bytes
        else if (byteInd < 128 + 20) { PE->coff_header.Number_Of_Symbol_Table[byteInd-(128 + 16)] = c; } // COFF Number of Symbol Table - 4 bytes
        else if (byteInd < 128 + 22) { PE->coff_header.Size_Of_Optional_Header[byteInd-(128 + 20)] = c; } // COFF Size of Optional Header - 2 bytes
        else if (byteInd < 128 + 24) { PE->coff_header.Characteristics[byteInd-(128 + 22)] = c; } // COFF Characteristics - 2 bytes
    } else if (byteInd < 152 + 28) { // 28 bytes Standard COFF Fields
        if (byteInd < 152 + 2) { PE->standard_coff_fields.Magic[byteInd - 152] = c; } // Magic - 2 bytes
        else if (byteInd < 152 + 3) { PE->standard_coff_fields.Major_Linker_Version[byteInd - (152 + 2)] = c; } // Major Link Version - 1 byte
        else if (byteInd < 152 + 4) { PE->standard_coff_fields.Minor_Linker_Version[byteInd - (152 + 3)] = c; } // Minor Link Version - 1 byte
        else if (byteInd < 152 + 8) { PE->standard_coff_fields.Size_Of_Code[byteInd - (152 + 4)] = c; } // Size of Code - 4 bytes
        else if (byteInd < 152 + 12) { PE->standard_coff_fields.Size_Of_Initialized_Data[byteInd - (152 + 8)] = c; } // Size of Initialized Data - 4 bytes
        else if (byteInd < 152 + 16) { PE->standard_coff_fields.Size_Of_Uninitialized_Data[byteInd - (152 + 12)] = c; } // Size of Uninitialized Data - 4 bytes
        else if (byteInd < 152 + 20) { PE->standard_coff_fields.Address_Of_Entry_Point[byteInd - (152 + 16)] = c; } // Address of Entry Point - 4 bytes
        else if (byteInd < 152 + 24) { PE->standard_coff_fields.Base_Of_Code[byteInd - (152 + 20)] = c; } // Base of Code - 4 bytes
        else if (byteInd < 152 + 28) { PE->standard_coff_fields.Base_Of_Data[byteInd - (152 + 24)] = c; } // Base of Data - 4 bytes
    } else if (byteInd < 180 + 68) { // 68 bytes Windows Specific Fields
        if (byteInd < 180 + 4) { PE->windows_specific_fields.Image_Base[byteInd - 180] = c; } // Image Base - 4 bytes
        else if (byteInd < 180 + 8) { PE->windows_specific_fields.Section_Alignment[byteInd - (180 + 4)] = c; } // Section Alignment - 4 bytes
        else if (byteInd < 180 + 12) { PE->windows_specific_fields.File_Alignment[byteInd - (180 + 8)] = c; } // File Alignment - 4 bytes
        else if (byteInd < 180 + 14) { PE->windows_specific_fields.Major_Operating_System_Version[byteInd - (180 + 12)] = c; } // Major Operating System Version - 2 bytes
        else if (byteInd < 180 + 16) { PE->windows_specific_fields.Minor_Operating_System_Version[byteInd - (180 + 14)] = c; } // Minor Operating System Version - 2 bytes
        else if (byteInd < 180 + 18) { PE->windows_specific_fields.Major_Image_Version[byteInd - (180 + 16)] = c; } // Major Image Version - 2 bytes
        else if (byteInd < 180 + 20) { PE->windows_specific_fields.Minor_Image_Version[byteInd - (180 + 18)] = c; } // Minor Image Version - 2 bytes
        else if (byteInd < 180 + 22) { PE->windows_specific_fields.Major_Subsystem_Version[byteInd - (180 + 20)] = c; } // Major Subsystem Version - 2 bytes
        else if (byteInd < 180 + 24) { PE->windows_specific_fields.Minor_Subsystem_Version[byteInd - (180 + 22)] = c; } // Minor Subsystem Version - 2 bytes
        else if (byteInd < 180 + 28) { PE->windows_specific_fields.Win_32_Version_Value[byteInd - (180 + 24)] = c; } // Win 32 Version Value - 4 bytes
        else if (byteInd < 180 + 32) { PE->windows_specific_fields.Size_Of_Image[byteInd - (180 + 28)] = c; } // Size of Image - 4 bytes
        else if (byteInd < 180 + 36) { PE->windows_specific_fields.Size_Of_Headers[byteInd - (180 + 32)] = c; } // Size of Headers - 4 bytes
        else if (byteInd < 180 + 40) { PE->windows_specific_fields.Check_Sum[byteInd - (180 + 36)] = c; } // Check Sum - 4 bytes
        else if (byteInd < 180 + 42) { PE->windows_specific_fields.Subsystem[byteInd - (180 + 40)] = c; } // Subsystem - 2 bytes
        else if (byteInd < 180 + 44) { PE->windows_specific_fields.DLL_Characteristics[byteInd - (180 + 42)] = c; } // DLL Characteristics - 2 bytes
        else if (byteInd < 180 + 48) { PE->windows_specific_fields.Size_Of_Stack_Reserve[byteInd - (180 + 44)] = c; } // Size of Stack Reserve - 4 bytes
        else if (byteInd < 180 + 52) { PE->windows_specific_fields.Size_Of_Stack_Commit[byteInd - (180 + 48)] = c; } // Size of Stack Commit - 4 bytes
        else if (byteInd < 180 + 56) { PE->windows_specific_fields.Size_Of_Heap_Reserve[byteInd - (180 + 52)] = c; } // Size of Heap Reserve - 4 bytes
        else if (byteInd < 180 + 60) { PE->windows_specific_fields.Size_Of_Heap_Commit[byteInd - (180 + 56)] = c; } // Size of Heap Commit - 4 bytes
        else if (byteInd < 180 + 64) { PE->windows_specific_fields.Loader_Flags[byteInd - (180 + 60)] = c; } // Loader Flags - 4 bytes
        else if (byteInd < 180 + 68) { PE->windows_specific_fields.Number_Of_RVA_And_Sizes[byteInd - (180 + 64)] = c; } // Number of RVA and Sizes - 4 bytes
    } else if (byteInd < 248 + 128) { // 128 bytes Data Directories


    }
    else {
        //hexprintf(dosHeader->DOS_Signature, 2);
        //hexprintf(dosHeader->DOS_Pointer_to_PE_Header, 4);
        return 1;
    }
    return 0;
}*/

int LoadFile_Old(char * fPath, Portable_Executable * PE) {
    unsigned char buffer[8];

    FILE * pFile = fopen(fPath, "rb");

    uint64_t byteInd = 0;

    while(fread(buffer, 1, sizeof(buffer), pFile) && (byteInd < 180) ){
        for (int i = 0; i < sizeof(buffer) / sizeof(buffer[0]); i++) {
            hexprintf(buffer[i]);
            if (1) {
                cout << (_OP_CODES[buffer[i]]) << ", ";
            }

            //AppendToDOS_Format_Old(PE, byteInd, buffer[i]);

            byteInd++;
        }
    }

}
