#include<iostream>
#include<string>
#include<vector>
#include<map>
#include<set>
#include<stack>
#include<exception>
#include<cmath>

using std::vector;
using std::map;
using std::string;
using std::pair;
using std::make_pair;
using std::set;
using std::stack;
using std::to_string;
using std::getline;
using std::cin;
using std::cout;


struct lex_err :public std::exception {
	lex_err() = default;
	char const* what() const noexcept override { return "bad lex"; }
};
struct pars_err : std::exception
{
	char const* what() const noexcept override { return "bad syntaxis"; }
};
struct brack_err : std::exception
{
	char const* what() const noexcept override { return "brackets err"; }
};
struct math_err :std::exception { char const* what() const noexcept override { return "math_er"; }; };
#define ENABLEFUNC
class Calculator
{
	

	enum typeLex { // types of lex
		blanc,
		bin,
		una,
		num_double,
		num_int,
		skl,
		skr,
		phonk
	};
	enum STATUS { // status of parsing
		start,
		stop,
		error,
		number,
		skoba_l,
		skoba_r,
		unar,
		binar,
		phonking
	};
	struct Lexeme 
	{

		typeLex type;
		string inner;
		double value = 0;
		int prior;
		static inline int count_skr =0;
		static inline int count_skl =0;
		Lexeme(typeLex t, double val, int prior, string inner) :type(t), value(val), prior(prior), inner(inner) {};

	};

	map<string, set<pair<typeLex, STATUS>>> dictionary;
	vector<Lexeme> vct_lex;
	string s;
	
	

	

	Lexeme next_one(int& mark, STATUS curr_stat, string& input)
	{
		while (input[mark] == ' ')
		{
			mark++;
		}
		char symbol = input[mark];

		#ifdef ENABLEDOUBLE
		if (isdigit(symbol))
		{
			double value = 0;
			string ans;
			bool flag_dot = false;
			while (isdigit(symbol) || symbol == '.')
			{
				if (symbol == '.')
				{
					flag_dot = true;
				}
					
				ans += symbol;
				mark++;
				symbol = input[mark];
			}

			value = stod(ans);
			auto t = Lexeme(typeLex::num_int, value, 0, ans);
			if (flag_dot)
				t.type = num_double;
			mark--;
			return t;
		}
		#endif // ENABLEDOUBLE

		
		#ifndef ENABLEDOUBLE
		if (isdigit(symbol))
		{
			int value = 0;
			string ans;
			while (isdigit(symbol))
			{
				ans += symbol;
				mark++;
				symbol = input[mark];
			}

			value = stoi(ans);
			auto t = Lexeme(typeLex::num_int, value, 0, ans);
			mark--;
			return t;
		}
		#endif // !ENABLEDOUBLE

		
		else if (symbol == '*' || symbol == '/' || symbol=='%')
			return Lexeme(typeLex::bin, 0, 2, string(1, symbol));
		else if (symbol == '-' && (curr_stat == skoba_l || curr_stat == start))
			return Lexeme(typeLex::una, 0, 3, string(1, symbol));
		else if (symbol == '-' || symbol == '+')
			return Lexeme(typeLex::bin, 0, 1, string(1, symbol));
		else if (symbol == '(')
			return Lexeme(typeLex::skl, 0, 4, string(1, symbol));
		else if (symbol == ')')
			return Lexeme(typeLex::skr, 0, 4, string(1, symbol));
		#ifdef ENABLEFUNC
		else if (symbol < 'z' && symbol>'a')
		{
			string ans;
			while (symbol < 'z' && symbol>'a')
			{
				ans += symbol;
				mark++;
				symbol = input[mark];
			
			}
			mark--;
			return Lexeme(typeLex::phonk, 0, 3, ans);
		}
		#endif
	
	
	

		throw lex_err();
	}

	void parse()
	{
		STATUS curr_stat = STATUS::start;
		int mark = 0;

		while (curr_stat != stop && mark < s.size())
		{
			if (curr_stat != error)
			{
				if (Lexeme::count_skl >= Lexeme::count_skr)
				{
					Lexeme sled = next_one(mark, curr_stat, s);
					typeLex tip_next = sled.type;
					mark++;
					if (curr_stat == start || curr_stat == skoba_l)
					{
						bool flag = 0;
						for (auto&& iter : dictionary["start or skl"])
						{
							if (tip_next == iter.first)
							{
								if (iter.first == skl)
									Lexeme::count_skl++;
								curr_stat = iter.second;
								flag = true;
							}

						}
						if (mark == s.size() - 1 && Lexeme::count_skl == Lexeme::count_skr)
							curr_stat = stop;
						else if (flag == 0)
							curr_stat = error;

					}
					else if (curr_stat == number || curr_stat == skoba_r)
					{
						bool flag = 0;
						for (auto&& iter : dictionary["num or skr"])
						{
							auto&& t = iter.first;
							if (tip_next == iter.first)
							{
								if (iter.first == skr)
									Lexeme::count_skr++;
								curr_stat = iter.second;
								flag = true;
							}

						}
						if (flag == 0)
							curr_stat = error;
					}
					else if (curr_stat == unar || curr_stat == binar)
					{
						bool flag = 0;
						for (auto&& iter : dictionary["bin or un"])
						{
							if (tip_next == iter.first)
							{
								if (iter.first == skl)
								{
									Lexeme::count_skl++;
								}
								curr_stat = iter.second;
								flag = true;
							}

						}
						if (flag == 0)
							curr_stat = error;
					}
					#ifdef ENABLEFUNC
					else if (curr_stat == phonking) {
						bool flag = 0;
						for (auto&& iter : dictionary["phonking"])
						{
							if (tip_next == iter.first)
							{
								if (iter.first == skr)
									Lexeme::count_skr++;
								curr_stat = iter.second;
								flag = true;
							}
							if (flag == 0)
								curr_stat = error;
						}
					}
					#endif

					vct_lex.push_back(sled);




				}
				else
					curr_stat = error;
			}
			else
			{
				throw pars_err();
	;		}
			
		}
		if (Lexeme::count_skl != Lexeme::count_skr)
			throw brack_err();
	}



	void to_post()// creates a reversed polish notation
	{
		vector<Lexeme> ans;
		stack<Lexeme> temp;
		temp.push(Lexeme(blanc, 0, -1, "blanc"));
		for (auto&& iter : vct_lex)
		{
			if (iter.type == num_int || iter.type ==num_double)
			{
				ans.push_back(iter);
			}
			else if (iter.type != skr)
			{
				if (iter.prior <= temp.top().prior)
				{
					while (iter.prior <= temp.top().prior && temp.top().type != skl && temp.top().type != blanc)
					{
						ans.push_back(temp.top());
						temp.pop();
					}
					temp.push(iter);
				}
				else
					temp.push(iter);

			}
			else
			{
				while (temp.top().type != skl) {
					ans.push_back(temp.top());
					temp.pop();
				}
			}
		}
		while (temp.top().type != blanc) {
			ans.push_back(temp.top());
			temp.pop();
		}
		std::swap(ans, vct_lex);
	}

	double calculate()
	{
		stack<Lexeme> main;
		for (auto&& iter : vct_lex)
		{

			if (iter.type == num_int || iter.type == num_double)
				main.push(iter);
			else if (iter.type == una)
			{
				auto val = main.top().value;
				auto val_type = main.top().type;
				main.pop();
				Lexeme t = Lexeme(val_type, -val, 0, to_string(-val));
				main.push(t);
			}
			else if (iter.type == bin)
			{
				auto temp = main.top().value;
				auto temp1_type = main.top().type;
				main.pop();
				auto temp2 = main.top().value;
				auto temp2_type = main.top().type;
				main.pop();
				double res = 0;
				if (iter.inner == "+")
				{
					res = temp + temp2;
				}
				else if (iter.inner == "-")
				{
					res = temp2 - temp;
				}
				else if (iter.inner == "*")
				{
					res = temp * temp2;
				}
				else if (iter.inner == "/")
				{
					if (temp == 0)
						throw math_err();
					else
						res = temp2 / temp;
				}
				else if (iter.inner == "%")
				{
					if (!(temp1_type == num_int && temp2_type == num_int))
						throw pars_err();
					res = (int)temp2 % (int)temp;
				}
				
				if (temp1_type==num_int && temp2_type==num_int)
					main.push(Lexeme(num_int, res, 0, to_string(res)));
				else
					main.push(Lexeme(num_double, res, 0, to_string(res)));
			}
#ifdef ENABLEFUNC
			else if (iter.type == phonk)
			{
				auto a = main.top().value;
				double res = 0.;
				if (iter.inner == "sin")
					res = sin(a);
				else if (iter.inner == "cos")
					res = cos(a);
				else if (iter.inner == "tg")
					res = tan(a);

				else if (iter.inner == "ctg")
				{
					if (tan(a) != 0)
						res = 1 / std::tan(a);
					else
						throw math_err();
				}
				else if (iter.inner == "exp")
					res = exp(a);
				else if (iter.inner == "ln")
					res = log(a);
				else if (iter.inner == "abs")
					res = abs(a);

				main.push(Lexeme(num_int, res, 0, to_string(res)));
			}
#endif // ENABLEFUNC
		}
		return main.top().value;
	}


public:
	Calculator()
	{

		dictionary["start or skl"] = { make_pair(num_int,number),make_pair(num_double,number),make_pair(skl,skoba_l) ,make_pair(una,unar)};
		dictionary["bin or un"] = { make_pair(num_int,number),make_pair(num_double,number)  ,make_pair(skl,skoba_l)};
		dictionary["num or skr"] = { make_pair(skr,skoba_r), make_pair(bin,binar),make_pair(blanc,stop) };

		#ifdef ENABLEFUNC
		dictionary["start or skl"].insert(make_pair(phonk, phonking));
		dictionary["bin or un"].insert(make_pair(phonk, phonking));
		dictionary["phonk"] = { make_pair(skl,skoba_l) };
		#endif
		
	}
	#ifndef ENABLEDOUBLE
	long long Calculate(string& str)
	{
		s = str;
		parse();
		to_post();


		return (long long int)calculate();
	}
	#endif

	#ifdef ENABLEDOUBLE
	double Calculate(string& str)
	{
		s = str;
		parse();
		to_post();


		return calculate();
	}
	#endif
};
