#include<iostream>
#include<string>
#include<vector>
#include<map>
#include<set>
#include<stack>

using namespace std;

enum typeLex {
	blanc,
	bin,
	una,
	num,
	skl,
	skr,
	phonk
};

enum STATUS {
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
	static int count_skr;
	static int count_skl;
	Lexeme();
	Lexeme(typeLex t, double val, int prior, string name);
	~Lexeme() {};
};
int Lexeme::count_skl = 0;
int Lexeme::count_skr = 0;
Lexeme::Lexeme(typeLex t, double val, int prior, string name) {
	this->type = t;
	this->value = val;
	this->prior = prior;
	this->inner = name;
}

typeLex gettype(char v, STATUS curr)
{
	if (isdigit(v))
		return typeLex::num;
	else if ((v == '-' || v == '!') && (curr == STATUS::start || curr == STATUS::skoba_l))
		return typeLex::una;
	else if (v == '+' || v == '-' || v == '*' || v == '/' || v == '^')
		return typeLex::bin;
	else if (v >= 'a' && v <= 'z')
	{
		return typeLex::phonk;
	}

	else if (v == '(')
		return typeLex::skl;
	else if (v == ')')
		return typeLex::skr;
	throw "god damn...";
}

Lexeme next_one(int& mark, STATUS curr_stat, string input)
{
	char symbol = input[mark];

	if (isdigit(symbol))
	{
		double value = 0;
		string ans;
		while (isdigit(symbol) || symbol == '.')
		{
			ans += symbol;
			mark++;
			symbol = input[mark];
		}
		value = stod(ans);
		auto t = Lexeme(typeLex::num, value, 0, ans);
		mark--;
		return t;
	}
	else if (symbol == '*' || symbol == '/')
		return Lexeme(typeLex::bin, 0, 2, string(1, symbol));
	else if (symbol == '-' && (curr_stat == skoba_l || curr_stat == start))
		return Lexeme(typeLex::una, 0, 3, string(1, symbol));
	else if (symbol == '-' || symbol == '+')
		return Lexeme(typeLex::bin, 0, 1, string(1, symbol));
	else if (symbol == '(')
		return Lexeme(typeLex::skl, 0, 4, string(1, symbol));
	else if (symbol == ')')
		return Lexeme(typeLex::skr, 0, 4, string(1, symbol));
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
	throw 1;
}

vector<Lexeme> razlom(string s)
{

	vector<Lexeme> ans;
	STATUS curr_stat = STATUS::start;
	int mark = 0;
	map<string, set<pair<typeLex, STATUS>>> slovo;
	slovo["start or skl"] = { make_pair(num,number),make_pair(skl,skoba_l) ,make_pair(una,unar), make_pair(phonk,phonking) };
	slovo["bin or un"] = { make_pair(num,number) ,make_pair(skl,skoba_l), make_pair(phonk,phonking) };
	slovo["num or skr"] = { make_pair(skr,skoba_r), make_pair(bin,binar),make_pair(blanc,stop) };
	slovo["phonk"] = { make_pair(skl,skoba_l) };

	while (curr_stat != stop && mark < s.size())
	{
		if (curr_stat != error)
		{
			if (Lexeme::count_skl >= Lexeme::count_skr)
			{
				Lexeme sled = next_one(mark, curr_stat, s);
				typeLex tip_next = sled.type;
				mark++;
				if (curr_stat == start || curr_stat == skl)
				{
					bool flag = 0;
					for (auto iter : slovo["start or skl"])
					{
						if (tip_next == iter.first)
						{
							if (iter.first == skl)
								Lexeme::count_skl++;
							curr_stat = iter.second;
							flag++;
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
					for (auto iter : slovo["num or skr"])
					{
						auto t = iter.first;
						if (tip_next == iter.first)
						{
							if (iter.first == skr)
								Lexeme::count_skr++;
							curr_stat = iter.second;
							flag++;
						}

					}
					if (flag == 0)
						curr_stat = error;
				}
				else if (curr_stat == unar || curr_stat == binar)
				{
					bool flag = 0;
					for (auto iter : slovo["bin or un"])
					{
						if (tip_next == iter.first)
						{
							if (iter.first == skl)
							{
								Lexeme::count_skl++;
							}
							curr_stat = iter.second;
							flag++;
						}

					}
					if (flag == 0)
						curr_stat = error;
				}
				else if (curr_stat == phonking) {
					bool flag = 0;
					for (auto iter : slovo["phonking"])
					{
						if (tip_next == iter.first)
						{
							if (iter.first == skr)
								Lexeme::count_skr++;
							curr_stat = iter.second;
							flag++;
						}
						if (flag == 0)
							curr_stat = error;
					}
				}

				ans.push_back(sled);




			}
			else
				curr_stat = error;
		}
		else
			throw "peredelai";
	}
	if (Lexeme::count_skl != Lexeme::count_skr)
		throw "peredelai";
	return ans;
}



vector<Lexeme> postMalone(vector<Lexeme> input)
{
	vector<Lexeme> ans;
	stack<Lexeme> temp;
	temp.push(Lexeme(blanc, 0, -1, "blanc"));
	for (auto iter : input)
	{
		if (iter.type == num)
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
	return ans;

}

double calculate(vector<Lexeme> input)
{
	stack<Lexeme> main;
	for (auto iter : input)
	{
		if (iter.type == num)
			main.push(iter);
		else if (iter.type == una)
		{
			auto temp = main.top().value;
			main.pop();
			Lexeme t = Lexeme(num, -temp, 0, to_string(-temp));
			main.push(t);
		}
		else if (iter.type == bin)
		{
			auto temp = main.top().value;
			main.pop();
			auto temp2 = main.top().value;
			main.pop();
			double res = 0;
			if (iter.inner == "+")
				res = temp + temp2;
			if (iter.inner == "-")
				res = temp2 - temp;
			if (iter.inner == "*")
				res = temp * temp2;
			if (iter.inner == "/")
			{
				if (temp == 0)
					throw "CANT DEVIDE BY 0";
				else
					res = temp2 / temp;
			}
			main.push(Lexeme(num, res, 0, to_string(res)));

		}
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
					res = 1 / tan(a);
				else
					throw "CANT DEVIDE BY 0";
			}
			else if (iter.inner == "exp")
				res = exp(a);
			else if (iter.inner == "ln")
				res = log(a);
			else if (iter.inner == "abs")
				res = abs(a);

			main.push(Lexeme(num, res, 0, to_string(res)));
		}

	}
	return main.top().value;
}
