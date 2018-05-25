/*  sicxe_asm.h
Taylor Ewertz, Daniel Hernandez, Ian Mckay, Kai O'Neil
Team OHIO
cssc0995
CS530, Fall 2017
*/

#ifndef SICXE_ASM_H
#define	SICXE_ASM_H

#include <string>
#include "symtab.h"

using namespace std;

class sicxe_asm
{
public:
	sicxe_asm(string);
	~sicxe_asm();
	void assembly_pass_one();
	void assembly_pass_two();
	void print_listing_file();
	void make_listing_file();

private:
	symtab *sym;
	string fileName;
};
#endif
