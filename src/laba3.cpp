#include "header.h"
int main()
{
	string input;
	cin >> input;
	try {
		auto vct = razlom(input);
		vct = postMalone(vct);
		auto ans = calculate(vct);
		cout << endl << ans;
	}
	catch (const char* e) {
		cout << e;
	}
	catch (int e) {
		cout << e;
	}
	catch (...) {
		cout << "error";
	}
}