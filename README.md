# x64dbg Anti-Debug POC

## Summary

This program sets a page containing code which does not execute to PAGE_NOACCESS.
Viewing the disassembly of addresses near the page boundary for the memory region
immediately before the noaccess page causes x64dbg to alter the noaccess page
protection. 

## Steps to recreate:

1. Start the .exe then attach x64dbg.
2. In the "Memory Map" tab, find the .text memory region preceding the page
with PAGE_NOACCESS memory protection.  This is 000000013F421000 in the table below:
<pre>
    000000013F420000  0000000000001000  x64dbg anti-debug poc.exe                                       IMG    -R---        ERWC-
->  000000013F421000  0000000000009000   ".text"                          Executable code               IMG    ER---        ERWC-
    000000013F42A000  0000000000001000   ".text"                          Executable code               IMG    -----        ERWC-
    000000013F42B000  0000000000007000   ".text"                          Executable code               IMG    ER---        ERWC-
    000000013F432000  0000000000002000   ".rdata"                         Read-only initialized data    IMG    -R---        ERWC-
    000000013F434000  0000000000001000   ".data"                          Initialized data              IMG    -RW--        ERWC-
    000000013F435000  0000000000003000   ".pdata", ".rsrc", ".reloc"      Exception information         IMG    -R---        ERWC-
</pre>
3. Right click this region (000000013F421000), select "Follow in Disassembler".
4. Scroll to the bottom of the view.  Press F9 (Run/Go).
5. Wait for the "Caught" message box.

## notes:

1. Compiler optimization is disabled to force generated code to be included.