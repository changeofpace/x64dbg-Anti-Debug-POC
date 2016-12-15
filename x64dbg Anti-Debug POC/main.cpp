/*
    x64dbg Anti-Debug POC.

    This program sets a page containing code which does not execute to PAGE_NOACCESS.
    Viewing the disassembly of addresses near the page boundary for the memory region
    immediately before the noaccess page causes x64dbg to alter the noaccess page
    protection. 

    Steps to recreate:

    1. Start the .exe then attach x64dbg.
    2. In the "Memory Map" tab, find the .text memory region preceding the page
    with PAGE_NOACCESS memory protection.  This is 000000013F421000 in the table below:
        000000013F420000  0000000000001000  x64dbg anti-debug poc.exe                                       IMG    -R---        ERWC-
    ->  000000013F421000  0000000000009000   ".text"                          Executable code               IMG    ER---        ERWC-
        000000013F42A000  0000000000001000   ".text"                          Executable code               IMG    -----        ERWC-
        000000013F42B000  0000000000007000   ".text"                          Executable code               IMG    ER---        ERWC-
        000000013F432000  0000000000002000   ".rdata"                         Read-only initialized data    IMG    -R---        ERWC-
        000000013F434000  0000000000001000   ".data"                          Initialized data              IMG    -RW--        ERWC-
        000000013F435000  0000000000003000   ".pdata", ".rsrc", ".reloc"      Exception information         IMG    -R---        ERWC-
    3. Right click this region (000000013F421000), select "Follow in Disassembler".
    4. Scroll to the bottom of the view.  Press F9 (Run/Go).
    5. Wait for the "Caught" message box.

    notes:
    
    1. Compiler optimization is disabled to force generated code to be included.
*/

#include <Windows.h>
#include <cstdio>
#include <vector>
#include "pe_header.h"
#include "generated_code.h"

int main(int argc, char* argv[])
{
    // force .text section to be arbitrarily large so we have access to pages
    // which do not execute.
    force_include_generated_code();

    // get .text section range
    PE_HEADER_DATA header;
    const ULONG_PTR imagebase = ULONG_PTR(GetModuleHandle(NULL));
    if (!FillPEHeaderData(imagebase, header))
    {
        printf("failed to read pe header at %p.\n", imagebase);
        return 0;
    }
    const PIMAGE_SECTION_HEADER textSection = GetSectionByName(header, ".text");
    if (textSection == nullptr)
    {
        printf("failed to get .text section.\n");
        return 0;
    }
    const ULONG_PTR textSectionBase = imagebase + textSection->VirtualAddress;
    const ULONG_PTR textSectionEnd = imagebase + textSection->VirtualAddress + textSection->Misc.VirtualSize;

    // choose a page in .text which won't throw an exception if we set it to PAGE_NOACCESS
    const ULONG_PTR noaccessPage = ULONG_PTR(generated_function_7) - ULONG_PTR(generated_function_7) % 0x1000;

    printf("text section:      %p - %p  (%llX)\n", textSectionBase, textSectionEnd, textSection->Misc.VirtualSize);
    printf("no access page:    %p\n", noaccessPage);
    printf("section base rva:  %llX\n", noaccessPage - textSectionBase);
    
    // set PAGE_NOACCESS
    DWORD oldprot;
    if (!VirtualProtect(LPVOID(noaccessPage), 0x1000, PAGE_NOACCESS, &oldprot))
    {
        printf("VirtualProtect failed at %p.\n", noaccessPage);
        return 0;
    }

    // simulate a thread which constantly checks the state of noaccessPage
    for (;;)
    {
        MEMORY_BASIC_INFORMATION mbi;
        if (!VirtualQuery(LPVOID(noaccessPage), &mbi, sizeof(mbi)))
        {
            printf("VirtualQuery failed at %p.\n", noaccessPage);
            return 0;
        }

        if (mbi.Protect != PAGE_NOACCESS)
        {
            MessageBoxA(0, "Debugger detected.", "Caught", 0);
            return 1;
        }

        Sleep(1000);
    }

    getchar();
    return 0;
}