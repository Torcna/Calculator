#include "header.hpp"
int main() {
  string input;
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