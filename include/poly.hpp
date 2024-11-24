#ifndef POLY_HPP
#define POLY_HPP
#pragma once
#include <list>
#include <regex>

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

struct poly_parse_err : std::logic_error {
  poly_parse_err(std::string op) : std::logic_error(op){};

  char const* what() const noexcept override { return "polynome parsing err "; };
};

struct polynome {
  struct node_base {
    std::string variable;
    size_t exponent;
    virtual ~node_base() = default;

    explicit node_base(std::string var, size_t exp) : variable(var), exponent(exp) {}
  };

  template <typename T>
  struct node : public node_base {
    T coef;

    node(T coef_, std::string var, size_t exp) : node_base(std::move(var), exp), coef(coef_) {}
  };

  std::list<std::shared_ptr<node_base>> poly;

  void add_term(std::shared_ptr<node_base> new_term) {
    for (auto& term : poly) {
      if (term->variable == new_term->variable && term->exponent == new_term->exponent) {
        if (auto* node_int = dynamic_cast<node<long long>*>(term.get())) {
          if (auto* new_node_int = dynamic_cast<node<long long>*>(new_term.get())) {
            node_int->coef += new_node_int->coef;
            return;
          }
        } else if (auto* node_double = dynamic_cast<node<double>*>(term.get())) {
          if (auto* new_node_double = dynamic_cast<node<double>*>(new_term.get())) {
            node_double->coef += new_node_double->coef;
            return;
          }
        }
      }
    }
    poly.push_back(std::move(new_term));
  }
  std::string poly_face;

  polynome(const std::string& str, std::string variable = "x") {
    std::string cleaned_str;
    for (char ch : str) {
      if (!std::isspace(ch)) {
        cleaned_str += ch;
      }
    }

    size_t i = 0;
    while (i < cleaned_str.size()) {
      int sign = 1;
      if (cleaned_str[i] == '+' || cleaned_str[i] == '-') {
        sign = (cleaned_str[i] == '-') ? -1 : 1;
        ++i;
      }

      double d_coeff = 1.0;
      long long i_coeff = 1;
      bool is_integer = true;
      bool has_coeff = false;

      size_t coeff_start = i;
      while (i < cleaned_str.size() && (std::isdigit(cleaned_str[i]) || cleaned_str[i] == '.')) {
        if (cleaned_str[i] == '.') {
          is_integer = false;
        }
        ++i;
      }
      if (i > coeff_start) {
        std::string coeff_str = cleaned_str.substr(coeff_start, i - coeff_start);
        try {
          if (is_integer) {
            i_coeff = std::stoll(coeff_str);
          } else {
            d_coeff = std::stod(coeff_str);
            is_integer = false;
          }
        } catch (...) {
          throw poly_parse_err("Failed to parse coefficient: " + coeff_str);
        }
        has_coeff = true;
      }

      size_t exponent = 0;
      if (i < cleaned_str.size() && cleaned_str[i] == variable[0]) {
        ++i;

        if (i < cleaned_str.size() && cleaned_str[i] == '^') {
          ++i;
          size_t exp_start = i;
          while (i < cleaned_str.size() && std::isdigit(cleaned_str[i])) {
            ++i;
          }
          if (exp_start < i) {
            try {
              exponent = std::stoul(cleaned_str.substr(exp_start, i - exp_start));
            } catch (...) {
              throw poly_parse_err("Failed to parse exponent");
            }
          } else {
            throw poly_parse_err("Exponent expected after '^'");
          }
        } else {
          exponent = 1;
        }
      } else if (has_coeff) {
        exponent = 0;
      }
      std::string temp = exponent == 0 ? "" : variable;
      std::shared_ptr<node_base> coeff_node;
      if (is_integer) {
        coeff_node = std::make_shared<node<long long>>(sign * i_coeff, temp, exponent);
      } else {
        coeff_node = std::make_shared<node<double>>(sign * d_coeff, temp, exponent);
      }
      add_term(coeff_node);
    }
  }

  polynome() = default;

  ~polynome() = default;

  polynome& operator=(polynome&& other) noexcept {
    if (this != &other) {
      poly = std::move(other.poly);
    }
    return *this;
  }

  polynome(const polynome& other) {
    for (auto&& term : other.poly) {
      if (auto node_int = std::dynamic_pointer_cast<node<long long>>(term)) {
        poly.push_back(std::make_shared<node<long long>>(node_int->coef, node_int->variable, node_int->exponent));
      } else if (auto node_double = std::dynamic_pointer_cast<node<double>>(term)) {
        poly.push_back(std::make_shared<node<double>>(node_double->coef, node_double->variable, node_double->exponent));
      }
    }
  }
};

inline polynome operator+(const polynome& poly, auto value) {
  polynome result = poly;
  if constexpr (std::is_arithmetic_v<decltype(value)>) {
    result.add_term(std::make_shared<typename polynome::node<decltype(value)>>(value, "", 0));
  }
  return result;
}

inline polynome operator+(auto value, const polynome& poly) { return poly + value; }

inline polynome operator+(const polynome& p1, const polynome& p2) {
  polynome result = p1;

  for (const auto& term : p2.poly) {
    result.add_term(term);
  }

  return result;
}

inline polynome operator-(const polynome& poly) {
  polynome result;
  for (const auto& term : poly.poly) {
    if (auto node_int = std::dynamic_pointer_cast<typename polynome::node<long long>>(term)) {
      result.add_term(std::make_shared<typename polynome::node<long long>>(-node_int->coef, node_int->variable,
                                                                           node_int->exponent));
    } else if (auto node_double = std::dynamic_pointer_cast<typename polynome::node<double>>(term)) {
      result.add_term(std::make_shared<typename polynome::node<double>>(-node_double->coef, node_double->variable,
                                                                        node_double->exponent));
    }
  }
  return result;
}
inline polynome operator-(const polynome& poly, auto value) { return poly + (-value); }

inline polynome operator-(auto value, const polynome& poly) {
  polynome result;
  result.add_term(std::make_shared<typename polynome::node<decltype(value)>>(value, "", 0));
  for (const auto& term : poly.poly) {
    if (auto node_int = std::dynamic_pointer_cast<typename polynome::node<long long>>(term)) {
      result.add_term(std::make_shared<typename polynome::node<long long>>(-node_int->coef, node_int->variable,
                                                                           node_int->exponent));
    } else if (auto node_double = std::dynamic_pointer_cast<typename polynome::node<double>>(term)) {
      result.add_term(std::make_shared<typename polynome::node<double>>(-node_double->coef, node_double->variable,
                                                                        node_double->exponent));
    }
  }
  return result;
}

inline polynome operator-(const polynome& p1, const polynome& p2) { return p1 + (-p2); }

inline polynome operator*(const polynome& poly, auto value) {
  polynome result;
  if constexpr (std::is_arithmetic_v<decltype(value)>) {
    for (const auto& term : poly.poly) {
      if (auto node_int = std::dynamic_pointer_cast<typename polynome::node<long long>>(term)) {
        result.add_term(std::make_shared<typename polynome::node<long long>>(node_int->coef * value, node_int->variable,
                                                                             node_int->exponent));
      } else if (auto node_double = std::dynamic_pointer_cast<typename polynome::node<double>>(term)) {
        result.add_term(std::make_shared<typename polynome::node<double>>(
            node_double->coef * value, node_double->variable, node_double->exponent));
      }
    }
  }
  return result;
}

inline polynome operator*(auto value, const polynome& poly) { return poly * value; }

inline polynome operator*(const polynome& p1, const polynome& p2) {
  polynome result;
  for (const auto& term1 : p1.poly) {
    for (const auto& term2 : p2.poly) {
      if (auto node_int1 = std::dynamic_pointer_cast<typename polynome::node<long long>>(term1)) {
        if (auto node_int2 = std::dynamic_pointer_cast<typename polynome::node<long long>>(term2)) {
          result.add_term(std::make_shared<typename polynome::node<long long>>(
              node_int1->coef * node_int2->coef, node_int1->variable, node_int1->exponent + node_int2->exponent));
        } else if (auto node_double2 = std::dynamic_pointer_cast<typename polynome::node<double>>(term2)) {
          result.add_term(std::make_shared<typename polynome::node<double>>(
              node_int1->coef * node_double2->coef, node_double2->variable,
              node_int1->exponent + node_double2->exponent));
        }
      } else if (auto node_double1 = std::dynamic_pointer_cast<typename polynome::node<double>>(term1)) {
        if (auto node_int2 = std::dynamic_pointer_cast<typename polynome::node<long long>>(term2)) {
          result.add_term(std::make_shared<typename polynome::node<double>>(
              node_double1->coef * node_int2->coef, node_double1->variable,
              node_double1->exponent + node_int2->exponent));
        } else if (auto node_double2 = std::dynamic_pointer_cast<typename polynome::node<double>>(term2)) {
          result.add_term(std::make_shared<typename polynome::node<double>>(
              node_double1->coef * node_double2->coef, node_double1->variable,
              node_double1->exponent + node_double2->exponent));
        }
      }
    }
  }
  return result;
}
#endif

inline polynome operator/(const polynome& poly, auto value) {
  polynome result;

  for (const auto& term : poly.poly) {
    if (auto node_int = std::dynamic_pointer_cast<polynome::node<long long>>(term)) {
      if (value == static_cast<long long>(value)) {
        long long int_value = static_cast<long long>(value);
        if (int_value != 0 && node_int->coef % int_value == 0) {
          result.add_term(std::make_shared<polynome::node<long long>>(node_int->coef / int_value, node_int->variable,
                                                                      node_int->exponent));
        } else {
          result.add_term(
              std::make_shared<polynome::node<double>>(static_cast<double>(node_int->coef) / static_cast<double>(value),
                                                       node_int->variable, node_int->exponent));
        }
      } else {
        result.add_term(std::make_shared<polynome::node<double>>(
            static_cast<double>(node_int->coef) / static_cast<double>(value), node_int->variable, node_int->exponent));
      }
    } else if (auto node_double = std::dynamic_pointer_cast<polynome::node<double>>(term)) {
      result.add_term(std::make_shared<polynome::node<double>>(node_double->coef / static_cast<double>(value),
                                                               node_double->variable, node_double->exponent));
    }
  }

  return result;
}