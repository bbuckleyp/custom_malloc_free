# custom_malloc_free
Implemented custom Malloc() and Free() functions

Open report.docx for more detailed information on design choices.

Instructions:
1. Modify the makefile to compile the workload you want to run.
2. The smallest memory that can be malloced is 16 bytes. Please modify "small" test files accordingly.  
   The free node is 16 bytes so 16 + sizeof(malloc_header) = 24 bytes. This is my OS malloc specification.
3. Type "make" at terminal prompt
4. Type "./project2" at terminal prompt to run program.
