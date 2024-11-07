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
  variable
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
  var
};

struct lexeme_proxy {
  typeLex type_;
  int prior_;
  std::string inner_;
};

template <typename type>
struct Lexeme : public lexeme_proxy {
  type value;
  type get_val() { return value; }
  Lexeme(typeLex t, type val, int prior_, std::string inner_) : lexeme_proxy{t, prior_, inner_}, value(val){};
};
