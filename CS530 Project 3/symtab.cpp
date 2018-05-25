/*  symtab.cc
Taylor Ewertz, Daniel Hernandez, Ian Mckay, Kai O'Neil
Team OHIO
cssc0995
CS530, Fall 2017
*/

#include <iostream>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <cstdio>
#include "symtab.h"
#include "symtab_exception.h"
#include "opcodetab.h"
#include "opcode_error_exception.h"
#include "file_parser.h"
#include "file_parse_exception.h"

#define SET_3E 0x1000
#define SET_3P 0x2000
#define SET_3B 0x4000
#define SET_3X 0x8000
#define SET_3I 0x10000
#define SET_3N 0x20000
#define SET_4E 0x100000
#define SET_4P 0x200000
#define SET_4B 0x400000
#define SET_4X 0x800000
#define SET_4I 0x1000000
#define SET_4N 0x2000000

using namespace std;

symtab::symtab() {
	fileName = "";
	base = "";
	loCounter = 0;
}

symtab::symtab(string f) {
	fileName = f;
	loCounter = 0;
}

void symtab::set_struct() {
	try {
		file_parser fp(fileName);	//Get file name
		fp.read_file();			//Set Token structure to be used
		numLines = fp.size();

		//Loading new structure with file_parse structure
		for (int i = 0; i < fp.size(); i++) {
			ListLines tok;
			for (int j = 0; j < 3; j++) {
				if (j == 0)
					tok.label = to_uppercase(fp.get_token(i, j));
				if (j == 1)
					tok.opcode = to_uppercase(fp.get_token(i, j));
				if (j == 2) {
					if (toupper(fp.get_token(i, j)[0]) == 'C' && fp.get_token(i, j)[1] == '\'') {
						tok.operand = fp.get_token(i, j);
					}
					else if (toupper(fp.get_token(i, j)[0]) == 'X' && fp.get_token(i, j)[1] == '\'') {
						tok.operand = fp.get_token(i, j);
					}
					else
						tok.operand = to_uppercase(fp.get_token(i, j));
				}
			}
			lines.push_back(tok);
		}
	}
	catch (file_parse_exception excpt) {
		cout << "**Sorry, error " << excpt.getMessage() << endl;
	}
}

void symtab::set_listing_file() {
	int i = 0;
	lines_iter = lines.begin();

	//Check program for START directive
	while (lines_iter != lines.end() && lines[i].opcode != "START") {
		if (lines[i].label != "" || lines[i].opcode != "" || lines[i].operand != "")
			throw symtab_exception("START must begin the program. Line: " + int_to_string(i));
		i++;
		lines_iter++;

		if (lines_iter == lines.end())
			throw symtab_exception("START was not found. Line: " + int_to_string(i));
	}

	i = addressing_check(i);

	//Check between START and END directives
	while (lines_iter != lines.end() && lines[i].opcode != "END") {
		//Allocation directives check
		if (lines[i].opcode == "BYTE" || lines[i].opcode == "WORD" || lines[i].opcode == "RESB" ||
			lines[i].opcode == "RESW" || lines[i].opcode == "BASE" || lines[i].opcode == "NOBASE" ||
			lines[i].opcode == "EQU") {

			if (lines[i].opcode == "EQU") {
				if (lines[i].label == "")
					throw symtab_exception("EQU dir must have a label. Line: " + int_to_string(i));
				if (find_symbols(lines[i].label))
					throw symtab_exception("cannot use same label name. Line: " + int_to_string(i));
				insert_symbols(lines[i].label, int_to_hex(loCounter, 5));
				equ.insert(pair<string, string>(lines[i].label, lines[i].operand));
				lines[i].address = int_to_hex(loCounter, 5);
			}

			if (lines[i].opcode == "BASE" || lines[i].opcode == "NOBASE") {
				if (lines[i].opcode == "BASE") {
					base = lines[i].operand;
					lines[i].address = int_to_hex(loCounter, 5);
				}
				else {
					lines[i].address = int_to_hex(loCounter, 5);
				}
			}

			if (lines[i].opcode == "WORD") {
				if (find_symbols(lines[i].label))
					throw symtab_exception("cannot use same label name. Line: " + int_to_string(i));
				insert_symbols(lines[i].label, int_to_hex(loCounter, 5));
				lines[i].address = int_to_hex(loCounter, 5);
				loCounter += 3;
			}

			if (lines[i].opcode == "BYTE") {
				string temp;
				int found;
				if (find_symbols(lines[i].label))
					throw symtab_exception("cannot use same label name. Line: " + int_to_string(i));
				insert_symbols(lines[i].label, int_to_hex(loCounter, 5));
				found = lines[i].operand.find_last_of("\'");
				temp = lines[i].operand.substr(2, found - 2);

				if (toupper(lines[i].operand[0]) == 'C') {
					lines[i].address = int_to_hex(loCounter, 5);
					loCounter += temp.length();
				}

				else if (toupper(lines[i].operand[0]) == 'X') {
					if ((temp.length() & 1) != 0)
						throw symtab_exception("hex digits must be an even number of digits. Line: " + int_to_string(i));
					lines[i].address = int_to_hex(loCounter, 5);
					loCounter += temp.length() >> 1;
				}

				else
					throw symtab_exception("BYTE directive must either be in decimal (c''/C'') or hex format (x''/X'') Line: " + int_to_string(i));
			}

			if (lines[i].opcode == "RESW") {
				if (find_symbols(lines[i].label))
					throw symtab_exception("cannot use same label name. Line: " + int_to_string(i));
				insert_symbols(lines[i].label, int_to_hex(loCounter, 5));
				lines[i].address = int_to_hex(loCounter, 5);
				loCounter += 3 * string_to_int(lines[i].operand);
			}

			if (lines[i].opcode == "RESB") {
				if (find_symbols(lines[i].label))
					throw symtab_exception("cannot use same label name. Line: " + int_to_string(i));;
				insert_symbols(lines[i].label, int_to_hex(loCounter, 5));
				lines[i].address = int_to_hex(loCounter, 5);
				loCounter += string_to_int(lines[i].operand);
			}
		}

		//Other opcode check
		else if (lines[i].opcode != "") {
			if (lines[i].label != "" && find_symbols(lines[i].label))
				throw symtab_exception("cannot use same label name. Line: " + int_to_string(i));
			else if (lines[i].label != "")
				insert_symbols(lines[i].label, int_to_hex(loCounter, 5));
			lines[i].address = int_to_hex(loCounter, 5);
			try {
				opcodetab o;
				loCounter += o.get_instruction_size(lines[i].opcode);
			}
			catch (opcode_error_exception excpt) {
				cout << "**Sorry, error " << excpt.getMessage() << endl;
			}
		}

		//Just a label
		else if (lines[i].opcode == "" && lines[i].label != "") {
			insert_symbols(lines[i].label, int_to_hex(loCounter, 5));
			lines[i].address = int_to_hex(loCounter, 5);
		}

		//Check for blank lines or comments between START and END
		else {
			lines[i].address = int_to_hex(loCounter, 5);
		}

		i++;
		lines_iter++;

		//Checks if no END was found
		if (lines_iter == lines.end())
			throw symtab_exception("END was not found. Line: " + int_to_string(i));
	}

	//Anything remaining after END
	while (lines_iter != lines.end()) {
		lines[i].address = int_to_hex(loCounter, 5);
		i++;
		lines_iter++;
	}
}

void symtab::set_machine_code() {
	opcodetab o;
	int code = 0;
	int i = 0;
	lines_iter = lines.begin();

	while (lines_iter != lines.end() && lines[i].opcode != "START") {
		i++;
		lines_iter++;
	}

	while (lines_iter != lines.end() && lines[i].opcode != "END") {
		if (lines[i].opcode == "START" || lines[i].opcode == "RESB" ||
			lines[i].opcode == "RESW" || lines[i].opcode == "BASE" || lines[i].opcode == "NOBASE" ||
			lines[i].opcode == "EQU" || lines[i].opcode == "") {
			i++;
			lines_iter++;
		}

		else if (lines[i].opcode == "WORD") {
			if (lines[i].operand[0] == '$') {
				code = hex_to_int(lines[i].operand.substr(1, lines[i].operand.length()));
			}
			else {
				code = string_to_int(lines[i].operand);
			}
			lines[i].machine = int_to_hex(code, 5);
			i++;
			lines_iter++;
		}

		else if (lines[i].opcode == "BYTE") {
			if (toupper(lines[i].operand[0]) == 'C') {
				string chars = lines[i].operand.substr(2, lines[i].operand.length() - 3);
				string temp = "";
				for (int k = 0; k < chars.length(); k++) {
					int charInt = (int)chars[k];
					temp.append(int_to_hex(charInt,2));
				}
				lines[i].machine = temp;
			}
			else {
				lines[i].machine = to_uppercase(lines[i].operand.substr(2, lines[i].operand.length() - 3));
			}
			i++;
			lines_iter++;
		}

		else {

			//Format 1 Instruction
			if (o.get_instruction_size(lines[i].opcode) == 1) {
				lines[i].machine = o.get_machine_code(lines[i].opcode);
				i++;
				lines_iter++;
			}

			//Format 2 Instruction
			else if (o.get_instruction_size(lines[i].opcode) == 2) {
				if (lines[i].opcode == "SVC") {
					int temp;
					code = string_to_int(lines[i].operand);
					code <<= 4;
					temp = hex_to_int(o.get_machine_code(lines[i].opcode));
					temp <<= 8;
					code |= temp;
					lines[i].machine = int_to_hex(code, 4);
					i++;
					lines_iter++;
				}

				else if (lines[i].opcode == "TIXR" || lines[i].opcode == "CLEAR") {
					int temp;
					code = register_checker(lines[i].operand);
					code <<= 4;
					temp = hex_to_int(o.get_machine_code(lines[i].opcode));
					temp <<= 8;
					code |= temp;
					lines[i].machine = int_to_hex(code, 4);
					i++;
					lines_iter++;
				}

				else if (lines[i].opcode == "SHIFTR" || lines[i].opcode == "SHIFTL") {
					string r1;
					int comma, r2, temp;
					comma = lines[i].operand.find_first_of(",");
					r1 = lines[i].operand.substr(0, comma);
					r2 = string_to_int(lines[i].operand.substr(comma + 1, lines[i].operand.length()));
					if (r2 == 0)
						throw symtab_exception("cannot shift 0 bits...does nothing");
					r2 -= 1;
					code = register_checker(r1);
					code <<= 4;
					code |= r2;
					temp = hex_to_int(o.get_machine_code(lines[i].opcode));
					temp <<= 8;
					code |= temp;
					lines[i].machine = int_to_hex(code, 4);
					i++;
					lines_iter++;
				}

				else {
					string r1, r2;
					int comma, temp;
					comma = lines[i].operand.find_first_of(",");
					r1 = lines[i].operand.substr(0, comma);
					r2 = lines[i].operand.substr(comma + 1, lines[i].operand.length());
					code = register_checker(r1);
					code <<= 4;
					code |= register_checker(r2);
					temp = hex_to_int(o.get_machine_code(lines[i].opcode));
					temp <<= 8;
					code |= temp;
					lines[i].machine = int_to_hex(code, 4);
					i++;
					lines_iter++;
				}
			}


			if (o.get_instruction_size(lines[i].opcode) == 3) {
				string offsetStr = "";
				string address;
				int temp, offset;

				if (lines[i].opcode == "RSUB") {
					code = 0;
					code |= SET_3I;
					code |= SET_3N;
				}

				//check for $
				else if (lines[i].operand[0] == '$') {
					code = hex_to_int(lines[i].operand.substr(1, lines[i].operand.length()));
					if (code > 2047 || code < -2048) {
						throw symtab_exception("offset out-of-range.");
					}
					code |= SET_3N;
					code |= SET_3I;
				}

				//Throw exception if not a valid operand (maybe)
				else if (lines[i].operand[0] == '#') {
					if (lines[i].operand[1] == '$') {
						code = hex_to_int(lines[i].operand.substr(2, lines[i].operand.length()));
						if (code > 2047 || code < -2048) {
							throw symtab_exception("offset out-of-range.");
						}
						code |= SET_3I;
					}
					//#number
					else if (isdigit(lines[i].operand[1])) {
						code = string_to_int(lines[i].operand.substr(1, lines[i].operand.length()));
						if (code > 2047 || code < -2048) {
							throw symtab_exception("offset out-of-range.");
						}
						code |= SET_3I;
					}

					//#label
					else {
						address = find_address(lines[i].operand.substr(1, lines[i].operand.length()));
						offset = hex_to_int(address) - (hex_to_int(lines[i].address) + 3);
						if (offset > 2047 || offset < -2048) {
							if (base == "")
							{
								throw symtab_exception("no base found.");
							}
							offset = hex_to_int(address) - hex_to_int(find_address(base));
							code = 0;
							//code = offset;
							code |= SET_3B;
							code |= SET_3I;
						}
						else {
							code = 0;
							//code = offset;
							code |= SET_3P;
							code |= SET_3I;
						}
						offsetStr = int_to_hex(offset, 6);
						offsetStr = offsetStr.substr(offsetStr.length() - 3, 3);
						temp = hex_to_int(o.get_machine_code(lines[i].opcode));
						temp <<= 16;
						code |= temp;
						string reallyTemp = int_to_hex(code, 6);
						reallyTemp = reallyTemp.substr(0, 3);
						lines[i].machine = reallyTemp.append(offsetStr);
						i++;
						lines_iter++;
						continue;
					}
				}

				//Throw exception if not a valid operand (maybe)
				else if (lines[i].operand[0] == '@') {
					//@number
					if (isdigit(lines[i].operand[1])) {
					code = string_to_int(lines[i].operand.substr(1, lines[i].operand.length()));
					if (code > 2047 || code < -2048) {
						throw symtab_exception("offset out-of-range.");
					}
					code |= SET_3N;
					}

					//@$
					else if (lines[i].operand[1] == '$')
					{
						code = hex_to_int(lines[i].operand.substr(2, lines[i].operand.length()));
						if (code > 2047 || code < -2048) {
							throw symtab_exception("offset out-of-range.");
						}
						code |= SET_3N;
					}
					//@label
					else {
						address = find_address(lines[i].operand.substr(1, lines[i].operand.length()));
						offset = hex_to_int(address) - (hex_to_int(lines[i].address) + 3);
						if (offset > 2047 || offset < -2048) {
							if (base == "")
							{
								throw symtab_exception("no base found.");
							}
							offset = hex_to_int(address) - hex_to_int(find_address(base));
							code = 0;
							//code = offset;
							code |= SET_3B;
							code |= SET_3N;
						}
						else {
							code = 0;
							//code = offset;
							code |= SET_3P;
							code |= SET_3N;
						}
						offsetStr = int_to_hex(offset, 6);
						offsetStr = offsetStr.substr(offsetStr.length() - 3, 3);
						temp = hex_to_int(o.get_machine_code(lines[i].opcode));
						temp <<= 16;
						code |= temp;
						string reallyTemp = int_to_hex(code, 6);
						reallyTemp = reallyTemp.substr(0, 3);
						lines[i].machine = reallyTemp.append(offsetStr);
						i++;
						lines_iter++;
						continue;
					}

				}

				//label,register
				//Check for number,register
				else {
					int comma = lines[i].operand.find_first_of(",");
					if (comma != string::npos) {
						address = find_address(lines[i].operand.substr(0, comma));
						offset = hex_to_int(address) - (hex_to_int(lines[i].address) + 3);
						if (offset > 2047 || offset < -2048) {
							if (base == "")
							{
								throw symtab_exception("no base found.");
							}
							offset = hex_to_int(address) - hex_to_int(find_address(base));
							//code = offset;
							code = 0;
							code |= SET_3B;
							code |= SET_3X;
							code |= SET_3I;
							code |= SET_3N;
						}
						else {
							//code = offset;
							code = 0;
							code |= SET_3P;
							code |= SET_3X;
							code |= SET_3I;
							code |= SET_3N;
						}
						offsetStr = int_to_hex(offset, 6);
						offsetStr = offsetStr.substr(offsetStr.length() - 3, 3);
						temp = hex_to_int(o.get_machine_code(lines[i].opcode));
						temp <<= 16;
						code |= temp;
						string reallyTemp = int_to_hex(code, 6);
						reallyTemp = reallyTemp.substr(0, 3);
						lines[i].machine = reallyTemp.append(offsetStr);
						i++;
						lines_iter++;
						continue;
					}

					//label
					//Check for number
					else {
						if (isdigit(lines[i].operand[0])) {
							code = string_to_int(lines[i].operand);
							if (code > 2047 || code < -2048) {
								throw symtab_exception("offset out-of-range.");
							}
							code |= SET_3I;
							code |= SET_3N;
						}
						else {
							address = find_address(lines[i].operand);
							offset = hex_to_int(address) - (hex_to_int(lines[i].address) + 3);
							if (offset > 2047 || offset < -2048) {
								if (base == "")
								{
									throw symtab_exception("no base found.");
								}
								offset = hex_to_int(address) - hex_to_int(find_address(base));

								//code = offset;
								code = 0;
								code |= SET_3B;
								code |= SET_3I;
								code |= SET_3N;
							}
							else {

								//code = offset;
								code = 0;
								code |= SET_3P;
								code |= SET_3I;
								code |= SET_3N;
							}
						}
						offsetStr = int_to_hex(offset, 6);
						offsetStr = offsetStr.substr(offsetStr.length() - 3, 3);
						temp = hex_to_int(o.get_machine_code(lines[i].opcode));
						temp <<= 16;
						code |= temp;
						string reallyTemp = int_to_hex(code, 6);
						reallyTemp = reallyTemp.substr(0, 3);
						lines[i].machine = reallyTemp.append(offsetStr);
						i++;
						lines_iter++;
						continue;
					}
				}

				temp = hex_to_int(o.get_machine_code(lines[i].opcode));
				temp <<= 16;
				code |= temp;
				lines[i].machine = int_to_hex(code, 6);
				i++;
				lines_iter++;
			}
			
			//Format 4 Instruction
			else if (o.get_instruction_size(lines[i].opcode) == 4) {
				string address;
				int temp;

				//check for $
				if (lines[i].operand[0] == '$') {
					code = hex_to_int(lines[i].operand.substr(1, lines[i].operand.length()));
					code |= SET_4E;
					code |= SET_4I;
				}

				//Throw exception if not a valid operand (maybe)
				else if (lines[i].operand[0] == '#') {
					if (lines[i].operand[1] == '$') {

						code = hex_to_int(lines[i].operand.substr(2, lines[i].operand.length()));
						code |= SET_4E;
						code |= SET_4I;
					}
					//#number
					else if (isdigit(lines[i].operand[1])) {
						code = string_to_int(lines[i].operand.substr(1, lines[i].operand.length()));
						code |= SET_4E;
						code |= SET_4I;
					}

					//#label
					else {
						address = find_address(lines[i].operand.substr(1, lines[i].operand.length()));
						code = hex_to_int(address);
						code |= SET_4E;
						code |= SET_4I;
					}
				}

				//Throw exception if not a valid operand (maybe)
				else if (lines[i].operand[0] == '@') {
					//@number
					if (isdigit(lines[i].operand[1])) {
						code = hex_to_int(lines[i].operand.substr(1, lines[i].operand.length()));
						code |= SET_4E;
						code |= SET_4N;
					}

					//@$
					else if (lines[i].operand[1] == '$')
					{
						code = hex_to_int(lines[i].operand.substr(2, lines[i].operand.length()));
						code |= SET_4E;
						code |= SET_4N;
					}
					//@label
					else {
						address = find_address(lines[i].operand.substr(1, lines[i].operand.length()));
						code = hex_to_int(address);
						code |= SET_4E;
						code |= SET_4N;
					}

				}

				//label,register
				//Check for number,register
				else {
					int comma = lines[i].operand.find_first_of(",");
					if (comma != string::npos) {
						address = find_address(lines[i].operand.substr(0, comma));
						code = hex_to_int(address);
						code |= SET_4E;
						code |= SET_4X;
						code |= SET_4I;
						code |= SET_4N;
					}

					//label
					//Check for number
					else {
						if (isdigit(lines[i].operand[0])) {
							code = string_to_int(lines[i].operand);
							//code = hex_to_int(address);
							code |= SET_4E;
							code |= SET_4I;
							code |= SET_4N;
						}
						else {
							string equVal = equ_checker(lines[i].operand);
							if (equVal != "-1") {
								address = equVal;
								code = string_to_int(address);
							}
							else {
								address = find_address(lines[i].operand);
								code = hex_to_int(address);
							}
							code |= SET_4E;
							code |= SET_4I;
							code |= SET_4N;
						}
					}
				}

				temp = hex_to_int(o.get_machine_code(lines[i].opcode));
				temp <<= 24;
				code |= temp;
				lines[i].machine = int_to_hex(code, 8);
				i++;
				lines_iter++;
			}
		}

	}

}


void symtab::print_listing_file() {
	int i = 1;
	cout << "Line#\t\t" << "Address\t\t" << "Label\t\t" << "Opcode\t\t" << "Operand\t\t" << "Machine" << endl;
	cout << "=====\t\t" << "=======\t\t" << "=====\t\t" << "======\t\t" << "=======\t\t" << "=======" << endl;
	for (lines_iter = lines.begin(); lines_iter != lines.end(); lines_iter++) {
		if (i <= numLines) {
			cout << i << "\t\t";
			cout << *lines_iter;
			i++;
		}
	}
	cout << endl;
}

void symtab::insert_symbols(string l, string a) {
	labels.insert(pair<string, string>(l, a));
}

bool symtab::find_symbols(string l) {
	m_iter = labels.find(l);
	if (m_iter != labels.end())
		return true;
	else
		return false;
}

string symtab::find_address(string o) {
	m_iter = labels.find(o);
	if (m_iter != labels.end())
		return m_iter->second;
	else
		throw symtab_exception("not a valid label.");
}

void symtab::print_symbol_table() {
	cout << "Symbol Table" << endl
		<< "=============" << endl;
	for (map<string, string>::const_iterator it = labels.begin();
		it != labels.end(); it++)
	{
		cout << it->first << "\t" << it->second << endl;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

string symtab::to_uppercase(string s) {
	transform(s.begin(), s.end(), s.begin(), ::toupper);
	return s;
}

int symtab::string_to_int(string s) {
	istringstream instr(s);
	int n;
	instr >> n;
	return n;
}

string symtab::int_to_hex(int num, int width) {
	stringstream out;
	out << std::setfill('0') << std::setw(width)
		<< std::hex << num;
	return to_uppercase(out.str());
}

int symtab::hex_to_int(string s) {
	int value;
	//sscanf(s.c_str(), "%x", &value);
	sscanf_s(s.c_str(), "%x", &value);
	return value;
}

string symtab::int_to_string(int i) {
	stringstream ss;
	ss << i + 1;
	string nl = ss.str();
	return nl;
}

int symtab::addressing_check(int index) {
	if (lines[index].operand[0] == '#') {
		//#$
		if (lines[index].operand[1] == '$') {
			insert_symbols(lines[index].label, int_to_hex(loCounter, 5));
			loCounter = hex_to_int(lines[index].operand.substr(2, lines[index].operand.length()));
			index++;
			lines_iter++;
			return index;
		}
		//#
		else {
			insert_symbols(lines[index].label, int_to_hex(loCounter, 5));
			loCounter = string_to_int(lines[index].operand.substr(1, lines[index].operand.length()));
			index++;
			lines_iter++;
			return index;
		}
	}

	//$
	if (lines[index].operand[0] == '$') {
		insert_symbols(lines[index].label, int_to_hex(loCounter, 5));
		loCounter = hex_to_int(lines[index].operand.substr(1, lines[index].operand.length()));
		lines_iter++;
		index++;
		return index;
	}
	//Decimal number
	else {
		insert_symbols(lines[index].label, int_to_hex(loCounter, 5));
		loCounter = string_to_int(lines[index].operand);
		lines_iter++;
		index++;
		return index;
	}
}

int symtab::register_checker(string reg) {
	if (reg == "A")
		return 0;
	if (reg == "X")
		return 1;
	if (reg == "L")
		return 2;
	if (reg == "B")
		return 3;
	if (reg == "S")
		return 4;
	if (reg == "T")
		return 5;
	if (reg == "F")
		return 6;
	else
		throw symtab_exception("not a valid register");
}

string symtab::equ_checker(string op) {
	map<string, string>::iterator equ_iter;
	equ_iter = equ.begin();
	while (equ_iter != equ.end()) {
		if (equ_iter->first == op)
			return equ_iter->second;
		equ_iter++;
	}
	return "-1";
}

ostream &operator<<(ostream &out, const symtab::ListLines &value) {
	out << value.address << "\t\t"
		<< value.label << "\t\t"
		<< value.opcode << "\t\t"
		<< value.operand << "\t\t"
		<< value.machine << endl;
	return out;
}