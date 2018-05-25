/*  symtab.h
Taylor Ewertz, Daniel Hernandez, Ian Mckay, Kai O'Neil
Team OHIO
cssc0995
CS530, Fall 2017
*/

#ifndef SYMTAB_H
#define SYMTAB_H

#include <string>
#include <map>
#include <vector>

using namespace std;

class symtab {
public:
	symtab();
	symtab(string);
	void set_struct();
	void set_listing_file();
	void set_machine_code();
	void print_listing_file();
	void insert_symbols(string, string);
	bool find_symbols(string);
	string find_address(string);
	void print_symbol_table();

private:
	struct ListLines {
		string address;
		string label;
		string opcode;
		string operand;
		string machine;

		ListLines() {
			address = "00000";
			label = "";
			opcode = "";
			operand = "";
			machine = "";
		}
	};

	map<string, string> labels;
	map<string, string>::iterator m_iter;
	map<string, string> equ;
	vector<ListLines> lines;
	vector<ListLines>::iterator lines_iter;
	string fileName;
	string base;
	int loCounter;
	int numLines;

	//Conversion functions
	string to_uppercase(string);
	int string_to_int(string);
	string int_to_hex(int, int);
	int hex_to_int(string);
	string int_to_string(int);
	int addressing_check(int);
	int register_checker(string);
	string equ_checker(string);
	friend ostream &operator << (ostream &, const symtab::ListLines &);
};

ostream &operator << (ostream &, const symtab::ListLines &);

#endif