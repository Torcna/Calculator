
#include <cmath>
#include <exception>
#include <iomanip>
#include <iostream>
#include <map>
#include <set>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>

#include "functional"
#include "poly.hpp"

enum typeLex {  // types of lex
  blanc,
  bin,
  una,
  num_double,
  num_int,
  skl,
  skr,
  phonk,
  variable,
  polynom
};

enum STATUS {  // status of parsing
  start,
  stop,
  error,
  number,
  skoba_l,
  skoba_r,
  unar,
  binar,
  phonking,
  var,
  poly
};

template <typeLex T>
struct type_map {};

template <>
struct type_map<num_int> {
  using type = long long;
};

template <>
struct type_map<num_double> {
  using type = double;
};

template <>
struct type_map<polynom> {
  using type = polynome;
};

template <typeLex T>
using mapped_type_t = typename type_map<T>::type;

struct lexeme_proxy {
  typeLex type_;
  int prior_;
  std::string inner_;

  lexeme_proxy(typeLex t, int prior, std::string inner) : type_(t), prior_(prior), inner_(std::move(inner)) {}

  virtual ~lexeme_proxy() = default;
};

template <typename type>
struct Lexeme : public lexeme_proxy {};

template <>
struct Lexeme<long long int> : lexeme_proxy {
  long long int value;
  long long int get_val() { return value; }
  Lexeme(typeLex t, long long int val, int prior_, std::string inner_) : lexeme_proxy{t, prior_, inner_}, value(val){};
};

template <>
struct Lexeme<double> : lexeme_proxy {
  double value;
  double get_val() { return value; }
  Lexeme(typeLex t, double val, int prior_, std::string inner_) : lexeme_proxy{t, prior_, inner_}, value(val){};
};

template <>
struct Lexeme<polynome> : lexeme_proxy {
  std::shared_ptr<polynome> value;
  polynome get_val() { return *value.get(); }
  Lexeme(typeLex t, polynome* val, int prior_, std::string inner_) : lexeme_proxy{t, prior_, inner_}, value(val){};
};

inline std::string to_string_poly(const Lexeme<polynome>* lex) {
  std::string result;
  bool first = true;

  const auto& poly = lex->value->poly;
  for (auto&& term : poly) {
    std::string term_str;
    if (auto node_int = std::dynamic_pointer_cast<polynome::node<long long>>(term)) {
      if (node_int->coef == 0) {
        continue;
      }
      term_str = std::to_string(node_int->coef);
      if (!node_int->variable.empty()) {
        term_str += node_int->variable;
        if (node_int->exponent > 1) {
          term_str += "^" + std::to_string(node_int->exponent);
        }
      }
    } else if (auto node_double = std::dynamic_pointer_cast<polynome::node<double>>(term)) {
      if (std::abs(node_double->coef) < 0.000001) {
        continue;
      }
      term_str = std::to_string(node_double->coef);
      if (!node_double->variable.empty()) {
        term_str += node_double->variable;
        if (node_double->exponent > 1) {
          term_str += "^" + std::to_string(node_double->exponent);
        }
      }
    }

    if (first) {
      result += term_str;
      first = false;
    } else {
      if (term_str[0] == '-') {
        result += " - " + term_str.substr(1);
      } else {
        result += " + " + term_str;
      }
    }
  }

  return result.empty() ? "0" : result;
}
