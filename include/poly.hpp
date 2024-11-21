#pragma once
#include <list>
struct polynome {

  struct node_base {
    size_t exponent;
  };

  template <typename coeff_type>
  struct node : node_base {};
  template <>
  struct node<long long int>
  {
    long long int coef;
  };
  template <>
  struct node<double>
  {
    double coef;
  };

  std::string poly_face;

  std::list<node_base*> poly;

  polynome (std::string str)
  { 
      if (str.length() == 1) {
        poly_face = str;
        poly.push_back(new node_base{1});
      }
    
  }

};