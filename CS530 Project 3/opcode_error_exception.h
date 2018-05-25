/*  opcode_error_exception.h
Taylor Ewertz, Daniel Hernandez, Ian Mckay, Kai O'Neil
Team OHIO
cssc0995
CS530, Fall 2017
*/

#ifndef OPCODE_ERROR_EXCEPTION_H
#define OPCODE_ERROR_EXCEPTION_H
#include <string>

using namespace std;

class opcode_error_exception {

public:
	opcode_error_exception(string s) {
		message = s;
	}

	opcode_error_exception() {
		message = "An error has occurred";
	}

	string getMessage() {
		return message;
	}

private:
	string message;
};
#endif