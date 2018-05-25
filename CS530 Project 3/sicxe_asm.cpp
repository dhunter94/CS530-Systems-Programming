/*  sicxe_asm.cpp
Taylor Ewertz, Daniel Hernandez, Ian Mckay, Kai O'Neil
Team OHIO
cssc0995
CS530, Fall 2017
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib> 
#include "sicxe_asm.h"
#include "symtab.h"
#include "symtab_exception.h"

using namespace std;

sicxe_asm::sicxe_asm(string f) {
	fileName = f;
	try {
		sym = new symtab(f);
		assembly_pass_one();
		assembly_pass_two();
		print_listing_file();
		make_listing_file();
		sym->print_symbol_table();
	}
	catch (symtab_exception excpt) {
		cout << "**Sorry, error " << excpt.getMessage() << endl;
	}
}

sicxe_asm::~sicxe_asm() {
	delete sym;
}

void sicxe_asm::assembly_pass_one() {
	sym->set_struct();
	sym->set_listing_file();
}

void sicxe_asm::assembly_pass_two() {
	sym->set_machine_code();
}

void sicxe_asm::print_listing_file() {
	sym->print_listing_file();
}
void sicxe_asm::make_listing_file() {
	string newExt = "lis";
	string::size_type i = fileName.rfind('.', fileName.length());
	if (i != string::npos) {
		fileName.replace(i + 1, newExt.length(), newExt);
	}

	stringstream ss;
	auto old_buf = cout.rdbuf(ss.rdbuf());
	sym->print_listing_file();
	cout.rdbuf(old_buf);

	ofstream myFile;
	myFile.open(fileName.c_str());
	myFile << ss.str();
	myFile.close();
}

int main(int argc, char *argv[]) {
	/*if (argc != 2) {
		cout << "Error, you must supply the name of the file " <<
			"to process at the command line." << endl;
		exit(1);
	}*/
	string file = "test.txt";
	//sicxe_asm as(argv[1]);
	sicxe_asm as(file);

	return 0;
}
