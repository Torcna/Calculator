#include<iostream>
#include<string>
#include<vector>
#include<map>
#include<set>
#include<stack>
#include<exception>
#include<cmath>
#include <iomanip>

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
	int problem_;
	lex_err(int ind) :problem_(ind) {};
	char const* what() const noexcept override { return "unexpected lexeme"; }
};
struct pars_err : std::exception
{
	int problem_;
	pars_err(int ind) :problem_(ind) {};
	char const* what() const noexcept override { return "bad syntaxis"; }
};
struct brack_err : std::exception {
	int problem_;
	brack_err(int ind) :problem_(ind) {};
	char const* what() const noexcept override { return "brackets err"; }
};
struct math_err :std::exception {

	string problem;
	math_err(string& op):problem(op){};
	char const* what() const noexcept override { return "math_er"; };
};
struct division_by_zero_err : math_err {
	division_by_zero_err(string& op) : math_err(op) {};

	char const* what() const noexcept override { return "division by zero is UNDEFINED"; };
};

struct persent_op_for_floating : math_err { 
	persent_op_for_floating(string& op) : math_err(op) {};

	char const* what() const noexcept override { return "using % operation on float is restricted"; };
};

class Calculator
{
	int count_skr_ = 0;
	int count_skl_ = 0;

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
		typeLex type_;
		string inner_;
		long long int value_;
		double d_value;
		int prior_;
		Lexeme(typeLex t, double& val, int prior_, string inner_) :type_(t), d_value(val), prior_(prior_), inner_(inner_) {};
		Lexeme(typeLex t, long long val, int prior_, string inner_) :type_(t), value_(val), prior_(prior_), inner_(inner_) {};
	};

	map<string, set<pair<typeLex, STATUS>>> dictionary_;
	vector<Lexeme> vct_lex_;
	string str_;

	Lexeme next_one(int& mark, STATUS curr_stat, const string& input)
	{
		
		while (input[mark] == ' ') { ++mark; };
		char symbol = input[mark];

		if (isdigit(symbol))
		{
			
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
			mark--;
			
			if (flag_dot)
			{

				double value = stod(ans);
				auto t = Lexeme(typeLex::num_double, value, 0, ans);
				return t;
			}
			else
			{
				int value = stoi(ans);
				auto t = Lexeme(typeLex::num_int, value, 0, ans);
				return t;
			}
				
			
			
		}
		else if (symbol == '*' || symbol == '/' || symbol == '%')
			return Lexeme(typeLex::bin, 0, 2, string(1, symbol));
		else if (symbol == '-' && (curr_stat == skoba_l || curr_stat == start))
			return Lexeme(typeLex::una, 0, 3, string(1, symbol));
		else if (symbol == '-' || symbol == '+')
			return Lexeme(typeLex::bin, 0, 1, string(1, symbol));
		else if (symbol == '(')
			return Lexeme(typeLex::skl, 0, 4, string(1, symbol));
		else if (symbol == ')')
			return Lexeme(typeLex::skr, 0, 4, string(1, symbol));
		throw lex_err(mark);
	}

	void parse()
	{
		STATUS curr_stat = STATUS::start;
		int mark = 0;
		int str_size = str_.size();
		while (curr_stat != stop && mark < str_size)
		{
			if (curr_stat != error)
			{
				if (count_skl_ >= count_skr_)
				{
					Lexeme sled = next_one(mark, curr_stat, str_);
					typeLex tip_next = sled.type_;
					mark++;
					if (curr_stat == start || curr_stat == skoba_l)
					{
						bool flag = 0;
						for (auto&& iter : dictionary_["start or skl"])
						{
							if (tip_next == iter.first)
							{
								if (iter.first == skl)
									count_skl_++;
								curr_stat = iter.second;
								flag = true;
							}

						}
						if (mark == str_.size() && count_skl_ == count_skr_)
							curr_stat = stop;
						else if (flag == 0)
							curr_stat = error;

					}
					else if (curr_stat == number || curr_stat == skoba_r)
					{
						bool flag = 0;
						for (auto&& iter : dictionary_["num or skr"])
						{
							auto&& t = iter.first;
							if (tip_next == iter.first)
							{
								if (iter.first == skr)
									count_skr_++;
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
						for (auto&& iter : dictionary_["bin or un"])
						{
							if (tip_next == iter.first)
							{
								if (iter.first == skl)
								{
									count_skl_++;
								}
								curr_stat = iter.second;
								flag = true;
							}

						}
						if (flag == 0)
							curr_stat = error;
					}

					vct_lex_.push_back(sled);




				}
				else
					curr_stat = error;
			}
			else
			{
				throw pars_err{ mark };
			}

		}
		if (count_skl_ != count_skr_)
			throw brack_err(mark);
		if (vct_lex_.back().type_ == bin)
			throw pars_err(mark);
	}

	void to_post()// creates a reversed polish notation
	{
		vector<Lexeme> ans;
		stack<Lexeme> temp;
		temp.push(Lexeme(blanc, 0, -1, "blanc"));
		for (auto&& iter : vct_lex_)
		{
			if (iter.type_ == num_int || iter.type_ == num_double)
			{
				ans.push_back(iter);
			}
			else if (iter.type_ != skr)
			{
				if (iter.prior_ <= temp.top().prior_)
				{
					while (iter.prior_ <= temp.top().prior_ && temp.top().type_ != skl && temp.top().type_ != blanc)
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
				while (temp.top().type_ != skl) {
					ans.push_back(temp.top());
					temp.pop();
				}
			}
		}
		while (temp.top().type_ != blanc) {
			ans.push_back(temp.top());
			temp.pop();
		}
		std::swap(ans, vct_lex_);
	}

	void calculate_inner(string& ans)
	{
		stack<Lexeme> main;
		for (auto&& iter : vct_lex_)
		{

			if (iter.type_ == num_int || iter.type_ == num_double)
				main.push(iter);
			else if (iter.type_ == bin)
			{
				
				bool is_double_first = false;
				bool is_double_second = false;
				auto temp = main.top();
				auto temp1_type = main.top().type_;
				if (temp1_type == num_double)
					is_double_first = true;
				main.pop();
				auto temp2 = main.top();
				auto temp2_type = main.top().type_;
				main.pop();
				if (temp2_type == num_double)
					is_double_second = true;
				main.push(calculator_inner_hiden(is_double_first, is_double_second, temp, temp2,iter));
				
			}
		}
		
		if (main.top().type_ == num_double)
		{
			ans = to_string(main.top().d_value);
		}
		else
			ans= to_string(main.top().value_);
		
	}

	Lexeme calculator_inner_hiden(bool is_double_first,bool is_double_second, Lexeme& first,Lexeme& second,Lexeme& iter)
	{
		if (is_double_first && is_double_second)
		{
			double res = 0.0;

			auto temp = first.d_value;
			auto temp2 = second.d_value;
			auto temp1_type = first.type_;
			auto temp2_type = second.type_;
			if (iter.inner_ == "+")
			{
				res = temp + temp2;
			}
			else if (iter.inner_ == "-")
			{
				res = temp2 - temp;
			}
			else if (iter.inner_ == "*")
			{
				res = temp * temp2;
			}
			else if (iter.inner_ == "/")
			{
				if (temp == 0)
				{
					string tmp = to_string(temp2) + " / " + to_string(temp);
					throw division_by_zero_err(tmp);
				}
				else
					res = temp2 / temp;
			}
			else if (iter.inner_ == "%")
			{
				if (!(temp1_type == num_int && temp2_type == num_int))
				{
					string tmp = to_string(temp2) + " % " + to_string(temp);
					throw persent_op_for_floating(tmp);
				}
				res = (long long)temp2 % (long long)temp;
			}

			return (Lexeme(num_double, res, 0, to_string(res)));

		}
		else if (is_double_first == false && is_double_second == false)
		{
			long long res = 0;

			auto temp = first.value_;
			auto temp2 = second.value_;
			auto temp1_type = first.type_;
			auto temp2_type = second.type_;
			if (iter.inner_ == "+")
			{
				res = temp + temp2;
			}
			else if (iter.inner_ == "-")
			{
				res = temp2 - temp;
			}
			else if (iter.inner_ == "*")
			{
				res = temp * temp2;
			}
			else if (iter.inner_ == "/")
			{
				if (temp == 0)
				{
					string tmp = to_string(temp2) + " / " + to_string(temp);
					throw division_by_zero_err(tmp);
				}
					
				else
					res = temp2 / temp;
			}
			else if (iter.inner_ == "%")
			{
				if (!(temp1_type == num_int && temp2_type == num_int))
				{
					string tmp = to_string(temp2) + " % " + to_string(temp);
					throw persent_op_for_floating(tmp);
				}
				if (temp ==0)
				{
					string tmp = to_string(temp2) + " % " + to_string(temp);
					throw division_by_zero_err(tmp);
				}
				res = temp2 % temp;
				
			}

			return (Lexeme(num_int, res, 0, to_string(res)));
		}
		else if (is_double_first == false && is_double_second)
		{
			double res = 0.0;

			auto temp = first.value_;
			auto temp2 = second.d_value;
			auto temp1_type = first.type_;
			auto temp2_type = second.type_;

			if (iter.inner_ == "+")
			{
				res = temp + temp2;
			}
			else if (iter.inner_ == "-")
			{
				res = temp2 - temp;
			}
			else if (iter.inner_ == "*")
			{
				res = temp * temp2;
			}
			else if (iter.inner_ == "/")
			{
				if (temp == 0)
				{
					string tmp = to_string(temp2) + " / " + to_string(temp);
					throw division_by_zero_err(tmp);
				}
				else
					res = temp2 / temp;
			}
			else if (iter.inner_ == "%")
			{
				if (!(temp1_type == num_int && temp2_type == num_int))
				{
					string tmp = to_string(temp2) + " % " + to_string(temp);
					throw persent_op_for_floating(tmp);
				}
			}
			auto l = Lexeme(num_double, res, 0, to_string(res));
			return l;
		}
		else
		{
			double res = 0.0;

			auto temp = first.d_value;
			auto temp2 = second.value_;
			auto temp1_type = first.type_;
			auto temp2_type = second.type_;

			if (iter.inner_ == "+")
			{
				res = temp2 + temp;
			}
			else if (iter.inner_ == "-")
			{
				res = temp - temp2;
			}
			else if (iter.inner_ == "*")
			{
				res = temp2 * temp;
			}
			else if (iter.inner_ == "/")
			{
				if (temp == 0)
				{
					string tmp = to_string(temp2) + " / " + to_string(temp);
					throw division_by_zero_err(tmp);
				}
				else
					res = temp2 / temp;
			}
			else if (iter.inner_ == "%")
			{
				if (!(temp1_type == num_int && temp2_type == num_int))
				{
					string tmp = to_string(temp2) + " % " + to_string(temp);
					throw persent_op_for_floating(tmp);
				}
			}

			auto l = Lexeme(num_double, res, 0, to_string(res));
			return l;
		}
	}
public:
	Calculator()
	{
		dictionary_["start or skl"] = { make_pair(num_int,number),make_pair(num_double,number),make_pair(skl,skoba_l) ,make_pair(una,unar) };
		dictionary_["bin or un"] = { make_pair(num_int,number),make_pair(skl,skoba_l),make_pair(num_double,number) };
		dictionary_["num or skr"] = { make_pair(skr,skoba_r), make_pair(bin,binar),make_pair(blanc,stop) };
	}
	string Calculate(string& str)
	{
		str_ = str;
		parse();
		to_post();
		string temp;
		calculate_inner(temp);
		return temp;
	}
};