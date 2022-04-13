#ifndef TAPP_BASICS_H
#define TAPP_BASICS_H

#include <type_traits>
#include <utility>
#include <limits>
#include <string>
template<template<class...>class, class>
struct is_this_template_class
{
  static constexpr bool value=false;
};



template<template<class ...>class V, class...Ts>
struct is_this_template_class<V,V<Ts...>>
{
  static constexpr bool value=true;
};






namespace tapp{


template <class... statements>
struct sequential_block: public statements...
{
  constexpr sequential_block(statements...s):statements{s}...{}
};


struct any_type{};


template<class Type,typename Identifier>
struct decl{
  constexpr decl(Type,Identifier){}
  constexpr decl(){}
  constexpr Type type()const {return {};}
};


template<class... declarations> struct args{
  args(declarations...){}
};

template<class... declarations>
args(declarations...)->args<declarations...>;



template <typename Identifier, class Type, class Value, typename=void>
class def{
public:
  static_assert(std::is_empty_v<Value>);
  constexpr def(Identifier,Type,Value){}
  constexpr def(){}
  constexpr Value operator()()const {return {};}

  constexpr auto operator[](Identifier)const {return *this;}
  //constexpr Type type()const {return {};}
};


template <typename Identifier, class Type, class Value>
class def<Identifier,Type,Value,
           std::enable_if_t<!std::is_empty_v<Value>>>
{
private:
  Value v_;
public:
  static_assert(!std::is_empty_v<Value>);
  constexpr def(Identifier,Type,Value v):v_{v}{}
  constexpr def():v_{}{}
  constexpr Value operator()()const {return v_;}

  constexpr auto operator[](Identifier)const {return *this;}
 // constexpr Type type()const {return {};}
};

template <class idName>
struct Identifier{};

template<class simple_type>
class simple_literal
{
  const simple_type v_;
public:
  constexpr simple_literal(simple_type v):v_{v}{}
  constexpr simple_literal():v_{}{}
  constexpr auto operator()()const {return v_;}
};

class  boolean_literal: public simple_literal<bool>{};
class  real_literal: public simple_literal<double>{};

class  string_literal: public simple_literal<char*>{};
class  integer_literal: public simple_literal<int>{};

template<class To, class From>
class convertible_type
{
  From data_;
public:

  constexpr To operator()() const{ return data_;}
};

class  boolean_type: public convertible_type<bool>{};
class  real_type: public convertible_type<double>{};

class  string_type: public convertible_type<char*>{};
class  integer_type: public convertible_type<int>{};




constexpr auto make_it_struct(int i)
{
  return integer_literal{i};
}

constexpr auto make_it_struct(double r)
{
  return real_literal{r};
}

constexpr auto make_it_struct(char* s)
{
  return string_literal{s};
}

constexpr auto make_it_struct(boolean_literal b)
{
  return boolean_literal{b};
}

template <class T>
constexpr T make_it_struct(T&& t)
{
  return t;
}




template<class T>
using type_t=decltype(T{}.type());


template<class Output, class Input>
class fun
{
  fun(Output, Input){}
};


template <class >
struct domain;
template <class >
struct codomain;

template<class... Kinds>
struct prod: public Kinds...
{
  prod(Kinds...k):Kinds{k}...{}
};


template <typename idName, class Something>
constexpr auto operator* (Identifier<idName>, Something i)
{
  return prod(idName{}, make_it_struct(i));
}

template <typename idName, class Something>
constexpr auto operator* (Something i, Identifier<idName>)
{
  return prod( make_it_struct(i),idName{});
}





template<class Output, class Input>
struct domain<fun<Output,Input>>: public def<domain<fun<Output,Input>>,fun<any_type,fun<Output,Input>>,Input>
{
  template <class Id, class Value>
  domain(def<Id,fun<Output,Input>,Value>){}

};

template <class Id, class Output, class Input, class Value>
domain(def<Id,fun<Output,Input>,Value>)->domain<fun<Output,Input>>;



template<class Output, class Input>
struct codomain<fun<Output,Input>>: public def<codomain<fun<Output,Input>>,fun<any_type,fun<Output,Input>>,Output>
{
  template <class Id, class Value>
  codomain(def<Id,fun<Output,Input>,Value>){}

};
template <class Id, class Output, class Input, class Value>
codomain(def<Id,fun<Output,Input>,Value>)->codomain<fun<Output,Input>>;

template<class f>
using domain_t=decltype(domain(f{}));
template<class f>
using codomain_t=decltype(codomain(f{}));


template<class f, class g>
struct compose: public def<compose<f,g>,
                            fun<fun<codomain_t<g>,domain_t<f>>,
                                prod<type_t<f>,type_t<g>>
                                >,
                            compose<f,g>>{};


template<class environment, class expression>
class lambda;

template<class expression, class...Ids, class... Types>
class lambda<args<decl<Types,Ids>...>,expression>
{
  expression e_;
public:
  lambda( args<decl<Types,Ids>...> , expression e):e_{e}{}
};

template<class expression, class...Ids, class... Types>
lambda( args<decl<Types,Ids>...>, expression)->lambda<args<decl<Types,Ids>...>,expression>;




struct boolean_type;

template<class predicate>
struct type_predicate: public def<type_predicate<predicate>,fun<boolean_type,any_type>,type_predicate<predicate>>{};



template<class target_type>
struct converts_to
{
  template <class test_type>
  constexpr bool operator()(test_type)const {return std::is_convertible_v<test_type,target_type>; }
};



struct boolean_type{};
    //: public def<boolean_type,fun<bool, any_type>,type_predicate<converts_to<bool>>>{};



template<class Type, class Predicate>
struct value_predicate: public def<value_predicate<Type,Predicate>,fun<boolean_type,Type>,Predicate>{};



template<class Type, class Predicate>
struct sub
{
  sub(Type, Predicate){}
};


template<class... Kinds>
struct sum{
  sum(Kinds...){}
};




































} // namespace tapp






#endif // TAPP_BASICS_H
