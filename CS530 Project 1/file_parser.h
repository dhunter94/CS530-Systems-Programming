/*  file_parser.h
	Daniel Hernandez, Kai O'Neill, Ian Mckay, Taylor Ewertz
	cssc1012
	Team OHIO
	CS530, Fall 2017
*/

#ifndef FILE_PARSER_H
#define FILE_PARSER_H

#include <string>

using namespace std;

class file_parser {
public:
	// ctor, filename is the parameter.  A driver program will read
	// the filename from the command line, and pass the filename to
	// the file_parser constructor.  Filenames must not be hard-coded.
	file_parser(string);

	// dtor
	~file_parser();

	// reads the source file, storing the information is some
	// auxiliary data structure you define in the private section. 
	// Throws a file_parse_exception if an error occurs.
	// if the source code file fails to conform to the above
	// specification, this is an error condition.     
	void read_file();

	// returns the token found at (row, column).  Rows and columns
	// are zero based.  Returns the empty string "" if there is no 
	// token at that location. column refers to the four fields
	// identified above.
	string get_token(unsigned int, unsigned int) const;

	// prints the source code file to stdout.  Should not repeat 
	// the exact formatting of the original, but uses tabs to align
	// similar tokens in a column. The fields should match the 
	// order of token fields given above (label/opcode/operands/comments)
	void print_file();

	// returns the number of lines in the source code file
	int size();

private:
	void parse_line(string);
	string label_validation(string);

	struct Tokens {
		string label;
		string opcode;
		string operand;
		string comment;

		Tokens() {
			label = "";
			opcode = "";
			operand = "";
			comment = "";
		}
	};
	friend ostream &operator << (ostream &, const file_parser::Tokens &);
	int num_lines;
	string f_name;
	vector<Tokens>::iterator lines_iter;
	vector<Tokens> lines;
};

ostream &operator << (ostream &, const file_parser::Tokens &);

#endif 