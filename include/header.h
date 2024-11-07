#pragma once
#include "source_lexeme.cpp"

using std::cin;
using std::cout;
using std::getline;
using std::make_pair;
using std::map;
using std::pair;
using std::set;
using std::stack;
using std::string;
using std::to_string;
using std::unordered_map;
using std::vector;


class Calculator {
  int count_skr_ = 0;
  int count_skl_ = 0;
  string str_;
  vector<string> func_list;
  map<string, string> defined_vars;
  map<string, set<pair<typeLex, STATUS>>> dictionary_;
  vector<lexeme_proxy*> vct_lex_;


  lexeme_proxy* next_one(int& mark, STATUS curr_stat, const string& input) {
    while (input[mark] == ' ') {
      ++mark;
    };
    char symbol = input[mark];

    if (isdigit(symbol)) {
      std::string ans;
      bool flag_dot = false;
      while (isdigit(symbol) || symbol == '.') {
        if (symbol == '.') {
          flag_dot = true;
        }

        ans += symbol;
        mark++;
        symbol = input[mark];
      }
      mark--;

      if (flag_dot) {
        double value = stod(ans);
        Lexeme<double>* t = new Lexeme<double>(typeLex::num_double, value, 0, ans);
        return t;
      } else {
        int value = stoi(ans);
        auto* t = new Lexeme<long long>(typeLex::num_int, value, 0, ans);
        return t;
      }

    } else if (symbol == '*' || symbol == '/' || symbol == '%')
      return new Lexeme<double>(typeLex::bin, 0, 2, std::string(1, symbol));
    else if (symbol == '-' && (curr_stat == skoba_l || curr_stat == start))
      return new Lexeme<double>(typeLex::una, 0, 3, std::string(1, symbol));
    else if (symbol == '-' || symbol == '+')
      return new Lexeme<double>(typeLex::bin, 0, 1, std::string(1, symbol));
    else if (symbol == '(')
      return new Lexeme<double>(typeLex::skl, 0, 4, std::string(1, symbol));
    else if (symbol == ')')
      return new Lexeme<double>(typeLex::skr, 0, 4, std::string(1, symbol));
    else if (symbol < 'z' && symbol > 'a') {
      std::string ans;
      while (symbol < 'z' && symbol > 'a') {
        ans += symbol;
        mark++;
        symbol = input[mark];
      }
      mark--;
      for (auto&& it : func_list) {
        if (it == ans) {
          return new Lexeme<double>(typeLex::phonk, 0, 3, ans);
        }
      }

      return new Lexeme<long long>(typeLex::variable, 0, 0, ans);
    }
    throw lex_err(mark);
  }

  void parse() {
    STATUS curr_stat = STATUS::start;
    int mark = 0;
    int str_size = str_.size();
    while (curr_stat != stop && mark < str_size) {
      if (curr_stat != error) {
        if (count_skl_ >= count_skr_) {
          auto sled = next_one(mark, curr_stat, str_);
          typeLex tip_next = sled->type_;
          mark++;
          if (curr_stat == start || curr_stat == skoba_l) {
            bool flag = 0;
            for (auto&& iter : dictionary_["start or skl"]) {
              if (tip_next == iter.first) {
                if (iter.first == skl) count_skl_++;
                curr_stat = iter.second;
                flag = true;
              }
            }
            if (mark == str_.size() && count_skl_ == count_skr_)
              curr_stat = stop;
            else if (flag == 0)
              curr_stat = error;

          } else if (curr_stat == number || curr_stat == skoba_r) {
            bool flag = 0;
            for (auto&& iter : dictionary_["num or skr"]) {
              auto&& t = iter.first;
              if (tip_next == iter.first) {
                if (iter.first == skr) count_skr_++;
                curr_stat = iter.second;
                flag = true;
              }
            }
            if (flag == 0) curr_stat = error;
          } else if (curr_stat == unar || curr_stat == binar) {
            bool flag = 0;
            for (auto&& iter : dictionary_["bin or un"]) {
              if (tip_next == iter.first) {
                if (iter.first == skl) {
                  count_skl_++;
                }
                curr_stat = iter.second;
                flag = true;
              }
            }
            if (flag == 0) curr_stat = error;
          } else if (curr_stat == phonking) {
            bool flag = 0;
            for (auto iter : dictionary_["phonking"]) {
              if (tip_next == iter.first) {
                if (iter.first == skr) count_skl_++;
                curr_stat = iter.second;
                flag = true;
              }
              if (flag == 0) curr_stat = error;
            }
          }
          vct_lex_.push_back(sled);
        } else
          curr_stat = error;
      } else {
        throw pars_err{mark};
      }
    }
    if (curr_stat == error) throw pars_err{mark};
    if (count_skl_ != count_skr_) throw brack_err(mark);
    if (vct_lex_.back()->type_ == bin) throw pars_err(mark);
    // initialize vars
    for (auto&& temp : vct_lex_) {
      if (temp->type_ == typeLex::variable) {
        auto&& finded_val = defined_vars.find(temp->inner_);
        if (finded_val != defined_vars.end())  // check if we already know value of the variable
        {
          int mark = 0;
          auto sled = next_one(mark, curr_stat, finded_val->second);
          temp = sled;
        } else {
          cout << std::endl;
          cout << "you have entered this variable:  " << '"' << temp->inner_ << '"' << " define it." << std::endl;

          int mark = 0;
          string str_temp;
          getline(cin, str_temp);

          auto sled = next_one(mark, curr_stat, str_temp);

          if (sled->type_ != typeLex::num_double && sled->type_ != typeLex::num_int) throw pars_err{mark};
          mark++;
          if (mark != str_temp.size() && mark != str_temp.size() - 1) {
            sled = next_one(mark, curr_stat, str_temp);
            throw lex_err{mark + 1};
          }
          string r = temp->inner_;
          string l = sled->inner_;
          defined_vars[r] = l;
          temp = sled;
        }
      }
    }
  }

  void to_post()  // creates a reversed polish notation
  {
    vector<lexeme_proxy*> ans;
    stack<lexeme_proxy*> temp;
    temp.push(new Lexeme<long long>(blanc, 0, -1, "blanc"));
    for (auto&& iter : vct_lex_) {
      if (iter->type_ == num_int || iter->type_ == num_double) {
        ans.push_back(iter);
      } else if (iter->type_ != skr) {
        if (iter->prior_ <= temp.top()->prior_) {
          while (iter->prior_ <= temp.top()->prior_ && temp.top()->type_ != skl && temp.top()->type_ != blanc) {
            ans.push_back(temp.top());
            temp.pop();
          }
          temp.push(iter);
        } else
          temp.push(iter);
      } else {
        while (temp.top()->type_ != skl) {
          ans.push_back(temp.top());
          temp.pop();
        }
      }
    }
    while (temp.top()->type_ != blanc) {
      ans.push_back(temp.top());
      temp.pop();
    }
    std::swap(ans, vct_lex_);
  }

  void calculate_inner(string& ans) {
    stack<lexeme_proxy*> main;
    for (auto&& iter : vct_lex_) {
      if (iter->type_ == num_int || iter->type_ == num_double)
        main.push(iter);
      else if (iter->type_ == bin) {
        bool there_is_double = false;
        auto&& temp = main.top();
        auto temp1_type = main.top()->type_;
        if (temp1_type == num_double) there_is_double = true;
        main.pop();
        auto&& temp2 = main.top();
        auto temp2_type = main.top()->type_;
        main.pop();
        if (temp2_type == num_double) there_is_double = true;
        if (temp1_type == num_double && temp2_type == num_double)
          main.push(
              calculator_inner_hiden(static_cast<Lexeme<double>*>(temp), static_cast<Lexeme<double>*>(temp2), iter));
        else if (temp1_type == num_double && temp2_type == num_int)
          main.push(
              calculator_inner_hiden(static_cast<Lexeme<double>*>(temp), static_cast<Lexeme<long long>*>(temp2), iter));
        else if (temp1_type == num_int && temp2_type == num_double)
          main.push(
              calculator_inner_hiden(static_cast<Lexeme<long long>*>(temp), static_cast<Lexeme<double>*>(temp2), iter));
        else
          main.push(calculator_inner_hiden(static_cast<Lexeme<long long>*>(temp),
                                           static_cast<Lexeme<long long>*>(temp2), iter));
      } else if (iter->type_ == phonk) {
        auto&& value_in_func = main.top();
        auto temp1_type = main.top()->type_;
        main.pop();
        if (temp1_type == num_double)
          main.push(calculator_inner_hiden(static_cast<Lexeme<double>*>(value_in_func),
                                           static_cast<Lexeme<long long>*>(nullptr), iter));
        else
          main.push(calculator_inner_hiden(static_cast<Lexeme<long long>*>(value_in_func),
                                           static_cast<Lexeme<long long>*>(nullptr), iter));
      }
    }

    if (main.top()->type_ == num_double) {
      auto* t = static_cast<Lexeme<double>*>(main.top());
      ans = to_string(static_cast<Lexeme<double>*>(main.top())->get_val());
    } else
      ans = to_string(static_cast<Lexeme<long long>*>(main.top())->get_val());
  }

  template <typename type_lex_1, typename type_lex_2>
  lexeme_proxy* calculator_inner_hiden(type_lex_1* first, type_lex_2* second, lexeme_proxy* iter) {
    if (iter->type_ == typeLex::bin) {
      auto val1 = first->get_val();
      auto val2 = second->get_val();
      auto res = val1 + val2;
      res = 0;

      if (iter->inner_ == "+") {
        res = val1 + val2;
      } else if (iter->inner_ == "-") {
        res = val2 - val1;
      } else if (iter->inner_ == "*") {
        res = val1 * val2;
      } else if (iter->inner_ == "/") {
        if (val1 == 0) {
          string tmp = to_string(val2) + " / " + to_string(val1);
          throw division_by_zero_err(tmp);
        } else
          res = val2 / val1;
      } else if (iter->inner_ == "%") {
        if constexpr (std::is_floating_point<decltype(val1)>::value || std::is_floating_point<decltype(val2)>::value) { //this is pretty good
          string tmp = to_string(val2) + " % " + to_string(val1);
          throw persent_op_for_floating(tmp);
        } else {
          if (val1 == 0) {
            string tmp = to_string(val2) + " % " + to_string(val1);
            throw division_by_zero_err(tmp);
          }
          res = val2 % val1;
        }
      }
      if (std::is_floating_point<decltype(res)>::value)
        return new Lexeme<double>(num_double, res, 0, to_string(res));
      else
        return new Lexeme<long long>(num_int, res, 0, to_string(res));
    } else if (iter->type_ == typeLex::phonk) {
      double res = 0;
      auto val1 = first->get_val();
      if (iter->inner_ == "sin")
        res = sin(val1);
      else if (iter->inner_ == "cos")
        res = cos(val1);
      else if (iter->inner_ == "exp")
        res = exp(val1);
      else if (iter->inner_ == "lg")
        res = log10(val1);
      else if (iter->inner_ == "sqrt")
        res = sqrt(val1);

      if (std::is_floating_point<decltype(res)>::value)
        return new Lexeme<double>(num_double, res, 0, to_string(res));
      else
        return new Lexeme<long long>(num_int, res, 0, to_string(res));
    }
  }

 public:
  Calculator() {
    dictionary_["start or skl"] = {make_pair(num_int, number), make_pair(num_double, number),
                                   make_pair(skl, skoba_l),    make_pair(una, unar),
                                   make_pair(variable, var),   make_pair(phonk, phonking)};
    dictionary_["bin or un"] = {make_pair(num_int, number), make_pair(skl, skoba_l), make_pair(num_double, number),
                                make_pair(variable, var), make_pair(phonk, phonking)};
    dictionary_["num or skr"] = {make_pair(skr, skoba_r), make_pair(bin, binar), make_pair(blanc, stop)};
    dictionary_["phonk"] = {make_pair(skl, skoba_l)};
    func_list = vector<string>{"exp", "sin", "cos", "lg", "sqrt"};
  }
  string Calculate(string& str) {
    str_ = str;
    parse();
    to_post();
    string temp;
    calculate_inner(temp);
    return temp;
  }
};