#ifndef TAPP_RUN_H
#define TAPP_RUN_H

#include "tapp_basics.h"
#include <utility>

namespace tapp
{



template<class Identifier, class Type, typename=void>
class x
{
public:
  using id=Identifier;
  x(Identifier, Type){}
  auto operator[](Identifier){return *this;}
  auto operator()(){return Type{};}
};

template<class Identifier, class Type>
class x<Identifier,Type,std::enable_if_t<!std::is_empty_v<Type>>>
{
private:
  Type value_;
public:
  using id=Identifier;
  x(Identifier, Type x): value_{std::move(x)}{}
  auto& operator[](Identifier){return *this;}
  auto& operator[](Identifier)const{return *this;}
  auto& operator()(){return value_;}
  auto& operator()()const {return value_;}
};

template<class ...xs>
class env: public xs...{
public:
  using xs::operator[]...;
  env(xs&&...x): xs{std::move(x)}...{}
  env(){}
};

template<>
class env<>{
public:
  env(){}
};





template<class... Ts,class ...xs>
auto define(env<xs...>&& e, x<Ts...>&& new_var)
{
  return env<x<Ts...>,xs...> (std::move(new_var),std::move(e[typename xs::id{}])...);
}

template<class Identifier,class... Ts,class ...xs>
auto& assign(env<xs...>& e, x<Identifier,Ts...>&& new_var)
{
  e[Identifier{}]=std::move(new_var);
  return e;
}



template <class environment>
auto run(environment&& e, sequential_block<>)
{
  return e;
}

template <class... statements>
auto run(env<statements...>&& e)
{
  return e;
}

template <class environment,class statement,class... statements>
auto run(environment&& e, const statement s,const statements&... ss)
{
  return run(run(std::move(e),s),ss...);
}



template <class environment,class... statements>
auto run(environment&& e, const sequential_block<statements...>& s)
{
  return run(std::move(e),static_cast<statements const&>(s)...);
}


template <class environment,typename Identifier, class Type, class ...Value>
auto run(environment&& e, def<Identifier,Type,Value...> d)
{
  return define(std::move(e),execute_definition(e,d));
}


struct type_convert:
                      public env<x<integer_literal,int,int>,
     x<real_literal,double,int>,
     x<string_literal,char*,int>,
     x<boolean_literal,bool,int>>
{};

template<typename tapp_type>
using type_convert_t=decltype(type_convert{}[tapp_type{}]());


using hh=  type_convert_t<integer_literal>;


template<class environment, class tapp_type, typename type_candidate,
          typename= std::enable_if_t<std::is_convertible_v<type_candidate,type_convert_t<tapp_type>>>>
type_convert_t<tapp_type> build(const environment& , tapp_type, type_candidate x)
{
  return x;
}

template <class environment,typename Identifier, class Type, class... Value>
auto execute_definition(const environment& e, def<Identifier,Type,Value...> d)
{
  auto data=get_value(e,d());
  auto ddata=build(e,get_Type(e,Type{}), data);
  return x(get_Identifier(e,Identifier{}),ddata);
}

template<class environment, class simple_type>
decltype(auto) get_value(const environment& , simple_value<simple_type> x ){return x();}

template<class environment, class Value,
          typename=std::void_t<decltype(environment{}[Value{}])>>
decltype(auto) get_value(const environment& e, Value){return e[Value{}]();}


template<class environment, class...terms>
decltype(auto) get_value(const environment& e,const  prod<terms...>& p){
  return (get_value(e,static_cast<terms const&>(p))*...);
}



template<class environment, class Type,
          typename=std::void_t<type_convert_t<Type>>>
Type get_Type(const environment&, Type){return {};}


template<class environment, class Identifier_candidate>
Identifier_candidate get_Identifier(const environment&, Identifier<Identifier_candidate>){return {};}



};



#endif // TAPP_RUN_H
