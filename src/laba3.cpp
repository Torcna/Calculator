#include "header.hpp"
int main() {
  string input;
  std::cout << "HELLO, this calculator supports polynomes as variables. It means, that you have to input polynome "
            << std::endl;
  std::cout << "after program asks you to define a variable" << std::endl;
  std::cout << "Polynome should look like aX^n + ... + bX^l " << std::endl;
  std::cout << "Where a and b can be long long and double, X can be any char, n and l can be only integer value"
            << std::endl;
  getline(cin, input);
  try {
    Calculator calculator;
    cout << std::endl << calculator.Calculate(input) << std::endl;

  } catch (const lex_err& e) {
    cout << std::endl;
    cout << std::setfill(' ') << std::setw(e.problem_) << '^' << std::endl;
    cout << e.what();
  } catch (const brack_err& e) {
    cout << std::endl;
    cout << std::setfill(' ') << std::setw(e.problem_) << '^' << std::endl;
    cout << e.what();
  } catch (const pars_err& e) {
    cout << std::endl;
    cout << std::setfill(' ') << std::setw(e.problem_) << '^' << std::endl;
    cout << e.what();
  } catch (const math_err& e) {
    cout << std::endl << "problem is " << e.problem << std::endl;
    cout << e.what();
  }
  // catching by ref
}