#include "C:\Users\allba\c++ progs\Calculator\include\header.h"
int main()
{
	string input;
	getline(cin, input);
	
	try {
		Calculator calculator;
		cout<<calculator.Calculate(input)<<std::endl;
		
#ifdef ENABLEFUNC
#ifndef ENABLEDOUBLE
		cout << "You are using functions, but without ENABLEDOUBLE answer gonna be rounded, please define it then"<<std::endl;
#endif
#endif // ENABLEFUNC

		
		system("pause");
	}
	catch (const lex_err& e)  {
		cout << e.what();
	}
	catch (const brack_err& e) {
		cout << e.what();
	}
	catch (const pars_err& e)  {
		cout << e.what();
	}
	catch (const math_err& e)
	{
		cout << e.what();
	}
	
	
}