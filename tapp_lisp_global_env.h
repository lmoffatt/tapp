#ifndef TAPP_LISP_GLOBAL_ENV_H
#define TAPP_LISP_GLOBAL_ENV_H

#include "tapp_lisp.h"
#include<string>

struct _ {};


struct any_type{};

struct primitive_type{};

template <class type>
constexpr auto predicate(type t){
  return fun(t, boolean_type{});
}





struct is_integer_type{
  constexpr auto operator()(int)const {return true;}
  template<class some_type>
  constexpr auto operator()(some_type )const
  {
    return false;
  }
};


template<class set, class superset>
struct promotes{
    constexpr promotes(set,superset){}
    constexpr promotes(){}
};

struct superset{};

struct hehe2: public Identifier<hehe2>{};

struct Product
{   template<class type, class value>
    auto operator()(vtv<type,value> x, vtv<type,value> y)
    {
        return vtv(type{},x()*y());
    }
};




constexpr auto g=seq(
    Def(prod{},vtv(fun(any_type{},cp(any_type{},any_type{})),Product{}))
          );

struct Global_env: public decltype(g){
  constexpr Global_env(): decltype(g)(g){}
};

constexpr auto global_env=Global_env{};


#endif // TAPP_LISP_GLOBAL_ENV_H
