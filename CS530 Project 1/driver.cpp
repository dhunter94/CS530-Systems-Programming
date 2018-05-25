#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include "file_parser.h"
#include "file_parse_exception.h"

using namespace std;

int main(void) {
	/*
	if (argc != 2) {
		cout << "Error, you must supply the name of the file " <<
			"to process at the command line." << endl;
		exit(1);
	}
	*/
	string filename = "test.txt";
	try {
		file_parser parser(filename);
		parser.read_file();
		parser.print_file();
		//cout << "Token at 20,1 is: " << parser.get_token(20, 1) << endl;
		//cout << "Token at 18,1 is: " << parser.get_token(18, 1) << endl;
		//cout << "Token at 2,2 is: " << parser.get_token(2, 2) << endl;
		//cout << "Token at 30,1 is: " << parser.get_token(30, 1) << endl;
		cout << "Number of lines: " << parser.size() << endl;
	}

	catch (file_parse_exception excpt) {
		cout << "**Sorry, error " << excpt.getMessage() << endl;
	}
	return 0;

}