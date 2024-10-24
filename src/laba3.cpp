#include "C:\Users\allba\c++ progs\Calculator\include\header.h"
int main()
{
	string input;
	getline(cin, input);

	try {
		Calculator calculator;
		cout << calculator.Calculate(input) << std::endl;

	}
	catch (const lex_err& e) {
		cout << std::endl;
		cout << std::setfill(' ') << std::setw(e.problem_) << '^' << std::endl;
		cout << e.what();
	}
	catch (const brack_err& e) {
		cout << std::endl;
		cout << std::setfill(' ') << std::setw(e.problem_ ) << '^' << std::endl;
		cout << e.what();
	}
	catch (const pars_err& e) {
		cout << std::endl;
		cout << std::setfill(' ') << std::setw(e.problem_ ) << '^' << std::endl;
		cout << e.what();
	}
	catch (const math_err& e)
	{
		cout << std::endl << "problem is " << e.lhs_ << e.op_ << e.rhs_ << std::endl;
		cout << e.what();
	}
	//catching by ref

}