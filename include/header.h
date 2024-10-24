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
	long long lhs_;
	long long rhs_;
	string op_;
	math_err(long long f, long long s, string& op) :lhs_(f), rhs_(s), op_(op) {};
	char const* what() const noexcept override { return "math_er"; };
};

struct double_usage_err : lex_err {
	double_usage_err(int problem) :lex_err(problem) {};
	char const* what() const noexcept override { return "using floating is restricted"; };
};
struct division_by_zero_err : math_err {
	division_by_zero_err(long long f, long long s, string op) : math_err(f, s, op) {};

	char const* what() const noexcept override { return "division by zero is UNDEFINED"; };
};
//struct persent_op_for_floating : lex_err { char const* what() const noexcept override { return "using % operation on float is restricted"; }; };

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

		typeLex type_;
		string inner_;
		long long int value_ = 0;
		int prior_;
		static inline int count_skr_ = 0;
		static inline int count_skl_ = 0;
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
			if (curr_stat == number)
				throw pars_err(mark);
			int value_ = 0;
			string ans;
			while (isdigit(symbol))
			{
				ans += symbol;
				mark++;
				symbol = input[mark];
			}
			if (symbol == ',' || symbol == '.')
				throw double_usage_err(mark);
			value_ = stoi(ans);
			auto t = Lexeme(typeLex::num_int, value_, 0, ans);
			mark--;
			return t;
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
				if (Lexeme::count_skl_ >= Lexeme::count_skr_)
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
									Lexeme::count_skl_++;
								curr_stat = iter.second;
								flag = true;
							}

						}
						if (mark == str_.size() && Lexeme::count_skl_ == Lexeme::count_skr_)
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
									Lexeme::count_skr_++;
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
									Lexeme::count_skl_++;
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
		if (Lexeme::count_skl_ != Lexeme::count_skr_)
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

	double calculate_inner()
	{
		stack<Lexeme> main;
		for (auto&& iter : vct_lex_)
		{

			if (iter.type_ == num_int || iter.type_ == num_double)
				main.push(iter);
			else if (iter.type_ == una)
			{
				auto val = main.top().value_;
				auto val_type = main.top().type_;
				main.pop();
				Lexeme t = Lexeme(val_type, -val, 0, to_string(-val));
				main.push(t);
			}
			else if (iter.type_ == bin)
			{
				auto temp = main.top().value_;
				auto temp1_type = main.top().type_;
				main.pop();
				auto temp2 = main.top().value_;
				auto temp2_type = main.top().type_;
				main.pop();
				long long res = 0;
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
						throw division_by_zero_err(temp2, temp, iter.inner_);
					else
						res = temp2 / temp;
				}
				else if (iter.inner_ == "%")
				{
					if (temp == 0)
						throw division_by_zero_err(temp2, temp, iter.inner_);
					res = temp2 % temp;
				}

				if (temp1_type == num_int && temp2_type == num_int)
					main.push(Lexeme(num_int, res, 0, to_string(res)));
			}
		}
		return main.top().value_;
	}


public:
	Calculator()
	{
		dictionary_["start or skl"] = { make_pair(num_int,number),make_pair(skl,skoba_l) ,make_pair(una,unar) };
		dictionary_["bin or un"] = { make_pair(num_int,number),make_pair(skl,skoba_l) };
		dictionary_["num or skr"] = { make_pair(skr,skoba_r), make_pair(bin,binar),make_pair(blanc,stop) };
	}
	long long Calculate(string& str)
	{
		str_ = str;
		parse();
		to_post();
		return calculate_inner();
	}
};
