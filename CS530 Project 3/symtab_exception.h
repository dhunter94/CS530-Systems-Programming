/*  symtab_exception.h
Taylor Ewertz, Daniel Hernandez, Ian Mckay, Kai O'Neil
Team OHIO
cssc0995
CS530, Fall 2017
*/

#ifndef SYMTAB_EXCEPTION_H
#define SYMTAB_EXCEPTION_H
#include <string>

using namespace std;

class symtab_exception {

public:
	symtab_exception(string s) {
		message = s;
	}

	symtab_exception() {
		message = "An error has occurred";
	}

	string getMessage() {
		return message;
	}

private:
	string message;
};
#endif
