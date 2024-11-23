#pragma once

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
#include "poly.hpp"
#include "functional"

struct lex_err : public std::exception {
  int problem_;
  lex_err(int ind) : problem_(ind){};
  char const* what() const noexcept override { return "unexpected lexeme"; }
};

struct pars_err : std::exception {
  int problem_;
  pars_err(int ind) : problem_(ind){};
  char const* what() const noexcept override { return "bad syntaxis"; }
};
struct brack_err : std::exception {
  int problem_;
  brack_err(int ind) : problem_(ind){};
  char const* what() const noexcept override { return "brackets err"; }
};
struct math_err : std::exception {
  std::string problem;
  math_err(std::string& op) : problem(op){};
  char const* what() const noexcept override { return "math_er"; };
};
struct division_by_zero_err : math_err {
  division_by_zero_err(std::string& op) : math_err(op){};

  char const* what() const noexcept override { return "division by zero is UNDEFINED"; };
};
struct persent_op_for_floating : math_err {
  persent_op_for_floating(std::string& op) : math_err(op){};

  char const* what() const noexcept override { return "using % operation on float is restricted"; };
};

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
    using type = std::shared_ptr<polynome>;
};


template <typeLex T>
using mapped_type_t = typename type_map<T>::type;

struct lexeme_proxy {
  typeLex type_;
  int prior_;
  std::string inner_;

  lexeme_proxy(typeLex t, int prior, std::string inner) : type_(t), prior_(prior), inner_(std::move(inner)) {}


};

template <typename type>
struct Lexeme : public lexeme_proxy {};

template <>
struct Lexeme<long long int> :lexeme_proxy
{
  long long int value;
  long long int get_val() { return value; }
  Lexeme(typeLex t, long long int val, int prior_, std::string inner_) : lexeme_proxy{t, prior_, inner_}, value(val){};


};

template <>
struct Lexeme<double> :lexeme_proxy
{
  double value;
  double get_val() { return value; }
  Lexeme(typeLex t, double val, int prior_, std::string inner_) : lexeme_proxy{t, prior_, inner_}, value(val){};

};

template <>
struct Lexeme<polynome> : lexeme_proxy {
  std::shared_ptr<polynome> value;
  auto get_val() { return value; }
  Lexeme(typeLex t, polynome* val, int prior_, std::string inner_) : lexeme_proxy{t, prior_, inner_}, value(val){};

};