#include <iostream>
#include <string>
#include <algorithm>
#include "opcodetab.h"
#include "opcode_error_exception.h"

using namespace std;

opcodetab::opcodetab() {
	m.insert(pair<string, pair<string, int> >("ADD", pair<string, int>("18", 3)));
	m.insert(pair<string, pair<string, int> >("ADDF", pair<string, int>("58", 3)));
	m.insert(pair<string, pair<string, int> >("ADDR", pair<string, int>("90", 2)));
	m.insert(pair<string, pair<string, int> >("AND", pair<string, int>("40", 3)));
	m.insert(pair<string, pair<string, int> >("CLEAR", pair<string, int>("B4", 2)));
	m.insert(pair<string, pair<string, int> >("COMP", pair<string, int>("28", 3)));
	m.insert(pair<string, pair<string, int> >("COMPF", pair<string, int>("88", 3)));
	m.insert(pair<string, pair<string, int> >("COMPR", pair<string, int>("A0", 2)));
	m.insert(pair<string, pair<string, int> >("DIV", pair<string, int>("24", 3)));
	m.insert(pair<string, pair<string, int> >("DIVF", pair<string, int>("64", 3)));
	m.insert(pair<string, pair<string, int> >("DIVR", pair<string, int>("9C", 2)));
	m.insert(pair<string, pair<string, int> >("FIX", pair<string, int>("C4", 1)));
	m.insert(pair<string, pair<string, int> >("FLOAT", pair<string, int>("C0", 1)));
	m.insert(pair<string, pair<string, int> >("HIO", pair<string, int>("F4", 1)));
	m.insert(pair<string, pair<string, int> >("J", pair<string, int>("3C", 3)));
	m.insert(pair<string, pair<string, int> >("JEQ", pair<string, int>("30", 3)));
	m.insert(pair<string, pair<string, int> >("JGT", pair<string, int>("34", 3)));
	m.insert(pair<string, pair<string, int> >("JLT", pair<string, int>("38", 3)));
	m.insert(pair<string, pair<string, int> >("JSUB", pair<string, int>("48", 3)));
	m.insert(pair<string, pair<string, int> >("LDA", pair<string, int>("00", 3)));
	m.insert(pair<string, pair<string, int> >("LDB", pair<string, int>("68", 3)));
	m.insert(pair<string, pair<string, int> >("LDCH", pair<string, int>("50", 3)));
	m.insert(pair<string, pair<string, int> >("LDF", pair<string, int>("70", 3)));
	m.insert(pair<string, pair<string, int> >("LDL", pair<string, int>("08", 3)));
	m.insert(pair<string, pair<string, int> >("LDS", pair<string, int>("6C", 3)));
	m.insert(pair<string, pair<string, int> >("LDT", pair<string, int>("74", 3)));
	m.insert(pair<string, pair<string, int> >("LDX", pair<string, int>("04", 3)));
	m.insert(pair<string, pair<string, int> >("LPS", pair<string, int>("D0", 3)));
	m.insert(pair<string, pair<string, int> >("MUL", pair<string, int>("20", 3)));
	m.insert(pair<string, pair<string, int> >("MULF", pair<string, int>("60", 3)));
	m.insert(pair<string, pair<string, int> >("MULR", pair<string, int>("98", 2)));
	m.insert(pair<string, pair<string, int> >("NORM", pair<string, int>("C8", 1)));
	m.insert(pair<string, pair<string, int> >("OR", pair<string, int>("44", 3)));
	m.insert(pair<string, pair<string, int> >("RD", pair<string, int>("D8", 3)));
	m.insert(pair<string, pair<string, int> >("RMO", pair<string, int>("AC", 2)));
	m.insert(pair<string, pair<string, int> >("RSUB", pair<string, int>("4C", 3)));
	m.insert(pair<string, pair<string, int> >("SHIFTL", pair<string, int>("A4", 2)));
	m.insert(pair<string, pair<string, int> >("SHIFTR", pair<string, int>("A8", 2)));
	m.insert(pair<string, pair<string, int> >("SIO", pair<string, int>("F0", 1)));
	m.insert(pair<string, pair<string, int> >("SSK", pair<string, int>("EC", 3)));
	m.insert(pair<string, pair<string, int> >("STA", pair<string, int>("0C", 3)));
	m.insert(pair<string, pair<string, int> >("STB", pair<string, int>("78", 3)));
	m.insert(pair<string, pair<string, int> >("STCH", pair<string, int>("54", 3)));
	m.insert(pair<string, pair<string, int> >("STF", pair<string, int>("80", 3)));
	m.insert(pair<string, pair<string, int> >("STI", pair<string, int>("D4", 3)));
	m.insert(pair<string, pair<string, int> >("STL", pair<string, int>("14", 3)));
	m.insert(pair<string, pair<string, int> >("STS", pair<string, int>("7C", 3)));
	m.insert(pair<string, pair<string, int> >("STSW", pair<string, int>("E8", 3)));
	m.insert(pair<string, pair<string, int> >("STT", pair<string, int>("84", 3)));
	m.insert(pair<string, pair<string, int> >("STX", pair<string, int>("10", 3)));
	m.insert(pair<string, pair<string, int> >("SUB", pair<string, int>("1C", 3)));
	m.insert(pair<string, pair<string, int> >("SUBF", pair<string, int>("5C", 3)));
	m.insert(pair<string, pair<string, int> >("SUBR", pair<string, int>("94", 2)));
	m.insert(pair<string, pair<string, int> >("SVC", pair<string, int>("B0", 2)));
	m.insert(pair<string, pair<string, int> >("TD", pair<string, int>("E0", 3)));
	m.insert(pair<string, pair<string, int> >("TIO", pair<string, int>("F8", 1)));
	m.insert(pair<string, pair<string, int> >("TIX", pair<string, int>("2C", 3)));
	m.insert(pair<string, pair<string, int> >("TIXR", pair<string, int>("B8", 2)));
	m.insert(pair<string, pair<string, int> >("WD", pair<string, int>("DC", 3)));
}

string opcodetab::get_machine_code(string op) {
	string temp = validation(op).first;
	if (m_iterator->second.second == 4) {
		m_iterator->second.second = 3;
	}
	return temp;
}

int opcodetab::get_instruction_size(string op) {
	int temp = validation(op).second;
	if (temp == 4) {
		m_iterator->second.second = 3;
	}
	return temp;
}

string opcodetab::to_uppercase(string s) {
	transform(s.begin(), s.end(), s.begin(), ::toupper);
	return s;
}

pair<string, int> opcodetab::validation(string op) {
	bool has_plus = false;

	if (op.at(0) == '+') {
		has_plus = true;
		op = op.substr(1, op.length() - 1);
	}

	op = to_uppercase(op);
	m_iterator = m.find(op);
	if (m_iterator == m.end()) {
		throw opcode_error_exception(" opcode not found.");
	}

	if (has_plus && m_iterator->second.second != 3)
		throw opcode_error_exception(" invalid instruction size.");

	if (has_plus && op == "RSUB")
		throw opcode_error_exception(" RSUB cannot be format 4.");

	if (has_plus)
		m_iterator->second.second = 4;

	return m_iterator->second;
}