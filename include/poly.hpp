#pragma once
#include <list>
#include <regex>

struct poly_parse_err : std::logic_error {
  poly_parse_err(std::string op) : std::logic_error(op){};

  char const* what() const noexcept override { return "polynome parsing err "; };
};

class polynome {
  struct node_base {
    size_t exponent;
    node_base(size_t exp) : exponent(exp){};
    virtual ~node_base() = default;
  };
  std::list<node_base*> poly;
  template <typename coeff_type>
  struct node : node_base {
    coeff_type coef;

    node(coeff_type c, size_t exp) : node_base(exp), coef(c){};
  };

template <typename type_node_ptr>
  void add_term(type_node_ptr* term) {
    for (auto it = poly.begin(); it != poly.end(); ++it) {
      if ((*it)->exponent == term->exponent) {
        if constexpr (std::is_same_v<type_node_ptr, decltype(*it)>) {
          auto* existing = static_cast<node<type_node_ptr>*>(*it);
          existing->coef += term->coef;
          if constexpr (std::is_same_v<type_node_ptr, node<long long>> && existing->coef == 0) {
            delete *it;
            poly.erase(it);
          } else if constexpr (std::is_same_v<type_node_ptr, node<double>> && std::fabs(existing->coef) < 1e-9) {
            delete *it;
            poly.erase(it);
          }
        } else if constexpr (std::is_same_v<type_node_ptr, node<double>> && dynamic_cast<node<long long>*>(*it)) {
          double new_coef = static_cast<double>(static_cast<node<long long>*>(*it)->coef) + term->coef;
          delete *it;
          *it = new node<double>(new_coef, term->exponent);
        } else if constexpr (std::is_same_v<type_node_ptr, node<long long>> && dynamic_cast<node<double>*>(*it)) {

          double new_coef =static_cast<node<double>*>(*it)->coef + term->coef;
          delete *it;                                        
          *it = new node<double>(new_coef, term->exponent);  
        }
        delete term;
        return;
      }
    }
    poly.push_back(term);
  }


 public:
  std::string poly_face;

  polynome(const std::string& str, char variable = 'x') {
    std::regex term_regex(R"(([+-]?\d*\.?\d+)?\s*\*?\s*)" + std::string(1, variable) + R"((?:\^(\d+))?)");
    std::smatch match;

    auto begin = str.cbegin();
    auto end = str.cend();

    while (std::regex_search(begin, end, match, term_regex)) {
      node_base* coeff_node = nullptr;

      if (match[1].matched && !match[1].str().empty()) {
        std::string coeff_str = match[1].str();
        try {
          long long int_coeff = std::stoll(coeff_str);
          coeff_node = new node<long long>(int_coeff, 0);
        } catch (const std::invalid_argument&) {
          double double_coeff = std::stod(coeff_str);
          coeff_node = new node<double>(double_coeff, 0);
        }
      } else if (match[1].str() == "-") {
        coeff_node = new node<long long>(-1, 0);
      } else if (match[1].str() == "+") {
        coeff_node = new node<long long>(1, 0);
      } else {
        coeff_node = new node<long long>(1, 0);
      }

      size_t exponent = 1;
      if (match[2].matched) {
        exponent = std::stoul(match[2].str());
      }

      coeff_node->exponent = exponent;
      poly.push_back(coeff_node);

      begin = match.suffix().first;
    }
  }
  polynome() = default;

  ~polynome() {
    for (auto* node : poly) {
      delete node;
    }
  }

  polynome(long long value) {
    poly.push_back(new node<long long>(value, 0));
  }

  // Оператор преобразования для double
  polynome(double value) {
    poly.push_back(new node<double>(value, 0));  
  }

  polynome operator+(const polynome& other) const {
    polynome result = *this;


    for (auto* term : other.poly) {
      result.add_term(term);
    }

    return result;
  }

  polynome operator-(const polynome& other) const {
    polynome result = *this; 


    for (auto* term : other.poly) {
      if constexpr (std::is_same_v<decltype(*term), node<long long>>) {
        auto* temp = static_cast<node<long long int>*>(term); 
        auto* neg_term = new node<long long>(-temp->coef, term->exponent);
        result.add_term(neg_term);
      } else if constexpr (std::is_same_v<decltype(*term), node<double>>) {
        auto* temp = static_cast<node<double>*>(term); 
        auto* neg_term = new node<double>(-temp->coef, term->exponent);
        result.add_term(neg_term);
      }
    }

    return result;
  }


};



