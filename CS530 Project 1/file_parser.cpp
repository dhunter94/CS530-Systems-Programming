/*  file_parser.h
Daniel Hernandez, Kai O'Neill, Ian Mckay, Taylor Ewertz
cssc1012
Team OHIO
CS530, Fall 2017
*/

#include <iostream>
#include <iomanip>
#include <vector>
#include <fstream>
#include "file_parser.h"
#include "file_parse_exception.h"

using namespace std;

file_parser::file_parser(string n) {
	f_name = n;
	num_lines = 0;
}

file_parser::~file_parser() {
}

void file_parser::read_file() {
	ifstream infile;
	string line;
	infile.open(f_name.c_str(), ios::in);

	if (!infile)
		throw file_parse_exception("file not found");

	while (!infile.eof()) {
		getline(infile, line);
		parse_line(line);
		num_lines++;
	}
}

string file_parser::get_token(unsigned int r, unsigned int c) const {
	if (r > num_lines)
		throw file_parse_exception("Invalid line");
	switch (c)
	{
	case 0:	return lines[r].label;
	case 1:	return lines[r].opcode;
	case 2: return lines[r].operand;
	case 3: return lines[r].comment;
	default:
		return "";
	}
}

void file_parser::print_file()
{
	for (lines_iter = lines.begin(); lines_iter != lines.end(); lines_iter++) {
		cout << *lines_iter;
	}
}

int file_parser::size() {
	return num_lines;
}

void file_parser::parse_line(string l) {
	Tokens tok;

	//i = 0 : Label, i = 1 : Opcode, i = 2 : Operand
	int i = 0;
	string delimiters = " \t\n";
	int last = l.find_first_not_of(delimiters, 0);
	int first = l.find_first_of(delimiters, last);

	//Empty line case
	if (last == -1 && first == -1) {
		lines.push_back(tok);
		return;
	}

	string token = l.substr(last, first - last);
	if (isspace(l.at(0))) {
		i++;
	}

	while (first != -1 || last != -1) {
		token = l.substr(last, first - last);

		//Comment test
		if (token.at(0) == '.') {
			first = l.find_first_of("\n", last);
			tok.comment = l.substr(last, first - last);
			lines.push_back(tok);
			return;
		}

		//Single-quote test
		if (token.length() != 1 && token.at(1) == '\'') {
			first = l.rfind("\'") + 1;
			token = l.substr(last, first - last);
		}

		switch (i)
		{
		case 0:	tok.label = label_validation(token);
			break;
		case 1:	tok.opcode = token;
			break;
		case 2: tok.operand = token;
			break;
		}

		i++;
		last = l.find_first_not_of(delimiters, first);
		first = l.find_first_of(delimiters, last);

		//Checks to see for tokens
		if (i >= 4) {
			throw file_parse_exception("too many tokens. Line: " + to_string(num_lines + 1));
		}
	}
	lines.push_back(tok);
}

string file_parser::label_validation(string t) {
	for (int i = 0; i < t.length(); i++) {
		if (!isalnum(t.at(i)))
			throw file_parse_exception("label is not alphanumeric. Line: " + to_string(num_lines + 1));
	}

	if (t.length() > 8)
		t = t.substr(0, 8);
	return t;
}

ostream &operator<<(ostream &out, const file_parser::Tokens &value) {
	out << value.label << "\t"
		<< value.opcode << "\t"
		<< value.operand << "\t"
		<< value.comment << endl;
	return out;
}