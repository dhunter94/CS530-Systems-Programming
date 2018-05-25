#include "opcode_error_exception.h"
#include "opcodetab.h"
#include <iostream>

using namespace std;

int main() {
	opcodetab test;
	try {
		cout << test.get_machine_code("+ADD") << test.get_instruction_size("+ADD") << endl;
		cout << test.get_machine_code("+ADD") << test.get_instruction_size("ADD") << endl;
	}
	catch (opcode_error_exception excpt) {
		cout << "**Sorry, error " << excpt.getMessage() << endl;
	}

	return 0;
}