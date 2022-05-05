#ifndef TAPP_BASE_H
#define TAPP_BASE_H

#include "tapp_utility.h"

#include <limits>
#include <string>
#include <type_traits>
#include <utility>


template<class...types>
struct type_sequence{};


template<std::size_t I, class type, typename=void>
class v
{
  type x_;
public:
  v(type x): x_{x}{}
  v(): x_{}{}
  auto& operator()()const {return x_;}
  auto& operator()() {return x_;}
  template<std::size_t J, typename= std::enable_if_t<I==J>>
  auto& get()const {return x_;}

  template<std::size_t J, typename= std::enable_if_t<I==J>>
  auto& get() {return x_;}
};

template<std::size_t I, class type>
class v<I,type,std::enable_if_t<std::is_empty_v<type>>>
{
public:
  constexpr v(type ) {}
  constexpr v() {}
  constexpr auto operator()()const {return type{};}
  template<std::size_t J, typename= std::enable_if_t<I==J>>
  constexpr auto get()const {return type{};}
};


template<template<class...>class, class...> struct sequence;

template<template<class...>class prod,class... types, std::size_t...Is>
struct sequence<prod,std::index_sequence<Is...>,types...>:public v<Is,types>...
{
  using v<Is,types>::get...;


  constexpr sequence(types...ts):v<Is,types>{ts}...{}
  constexpr sequence(){}
};



template<template<class...>class Op,class... types>
using make_sequence_t=sequence<Op,std::index_sequence_for<types...>,types...>;



template<class F,template<class...>class Op,class... types, std::size_t...Is>
auto map( F f,sequence<Op,std::index_sequence<Is...>,types...> list)
{
  return Op(f(list.template get<Is>())...);
}

template<template<class...>class Op1,class F,template<class...>class Op,class... types, std::size_t...Is>
auto map( F f,sequence<Op,std::index_sequence<Is...>,types...> list)
{
  return Op1(f(list.template get<Is>())...);
}




template<class Start,class Op,template<class...>class prod,class... types, std::size_t...Is>
auto reduce(Start&& s, Op ,sequence<prod,std::index_sequence<Is...>,types...> ,std::index_sequence<> )
{
  return s;
}


template<class Start,class Op,template<class...>class prod,class... types, std::size_t...Is0,std::size_t I, std::size_t...Is>
auto reduce(Start&& s, Op&& op,const sequence<prod,std::index_sequence<Is0...>,types...>& list,std::index_sequence<I,Is...> )
{
  return reduce(std::forward<Op>(op)(std::forward<Start>(s),list.template get<I>()),op,list,std::index_sequence<Is...>{});
}

template<class Start,class Op,template<class...>class prod,class... types, std::size_t...Is>
auto reduce(Start&& s, Op&& op,const sequence<prod,std::index_sequence<Is...>,types...>& list)
{
  return reduce(std::forward<Start>(s),std::forward<Op>(op),list,std::index_sequence<Is...>{});
}



template<template<class...>class Op,class... types, std::size_t...Is, class new_type>
auto push_back( sequence<Op,std::index_sequence<Is...>,types...> list, new_type new_data)
{
  return Op(list.template get<Is>()..., new_data);
}




template <template<class...>class Op,class... xs>
class dict : public xs... {
public:
  using xs::operator[]...;
  constexpr dict(xs &&...x) : xs{std::move(x)}... {}
  constexpr dict(): xs{}... {}
};




template<class F,template<class...>class Op,class... types>
auto map( F f,const dict<Op,types...>& d)
{
  return Op(f(static_cast<types const&>(d)...));
}

template<template<class...>class Op1,class F,template<class...>class Op,class... types>
auto map( F f,const dict<Op,types...>& d)
{
  return Op1(f(static_cast<types const&>(d)...));
}


template<class Start,class Op,template<class...>class C,class... types>
auto reduce(Start&& s, Op ,dict<C,types...> ,type_sequence<> )
{
  return s;
}


template<class Start,class Op,template<class...>class prod,class... types, class rtype, class...rtypes>
auto reduce(Start&& s, Op&& op,const dict<prod,types...>& list,type_sequence<rtype,rtypes...> )
{
  return reduce(std::forward<Op>(op)(std::forward<Start>(s),static_cast<rtype const&>(list)),op,list,type_sequence<rtypes...>{});
}

template<class Start,class Op,template<class...>class prod,class... types>
auto reduce(Start&& s, Op&& op,const dict<prod,types...>& list)
{
  return reduce(std::forward<Start>(s),std::forward<Op>(op),list,type_sequence<types...>{});
}



template<template<class...>class Op,class... types, class new_type>
auto push_back( dict<Op,types...>const & list, new_type new_data)
{
  return Op(static_cast<types const&>(list)..., new_data);
}


template<template<class...>class Op,class... types, class new_type>
auto push_back( dict<Op,types...>&& list, new_type new_data)
{
  return Op(std::move(static_cast<types&>(list))..., new_data);
}


struct Nothing{};
template <class head_type, class tail_type>
class serial : public head_type, public tail_type {
public:
  constexpr serial(head_type h, tail_type t) : head_type {h}, tail_type{t}{}
  constexpr serial(){}
//  using head_type::operator[];

  template<class Id, typename=void>
  struct has_Id: public std::false_type{};

  template<class Id>
  struct has_Id<Id, std::void_t<decltype (head_type{}[Id{}])>>: public std::true_type{};

  template<class Id>
  auto operator[](Id)const {
    if constexpr (has_Id<Id>::value)
      return head_type::operator[](Id{});
    else
      return tail_type::operator[](Id{});
  }

  template<std::size_t N>
  auto& get()const
  {
    if constexpr (N==0)
      return head(*this);
    else
      return tail_type::template get<N-1>();
  }

  friend constexpr auto& head(serial& s){return static_cast<head_type&>(s);}
  friend constexpr auto& head(serial const& s){return static_cast<head_type const&>(s);}
  friend constexpr auto& tail(serial& s){return static_cast<tail_type&>(s);}
  friend constexpr auto& tail(serial const& s){return static_cast<tail_type const&>(s);}
};

template <class head_type>
class serial<head_type,Nothing> : public head_type, Nothing{
public:
  constexpr serial(head_type h) : head_type {h}{}
  constexpr serial(){}

  template<std::size_t N, typename=std::enable_if_t<N==0>>
  auto& get()const
  {
      return head(*this);
  }

  friend constexpr auto& head(serial& s){return static_cast<head_type&>(s);}
  friend constexpr auto& head(serial const& s){return static_cast<head_type const&>(s);}
  friend constexpr auto& tail(serial& s){return static_cast<Nothing&>(s);}
  friend constexpr auto& tail(serial const& s){return static_cast<Nothing const&>(s);}
};

template <class serial_type, class new_type, typename= std::enable_if_t<is_this_template_class_v<serial,serial_type>>>
auto push_front(serial_type&& s, new_type&& o)
{
  return serial(std::forward<new_type>(o),std::forward<serial_type>(s));
}

template <class head_type>
serial(head_type)->serial<head_type,Nothing>;



template<class type>
constexpr auto make_serial_rev(type t)
{
  return t;
}


template<class type,class type2, class... types>
constexpr auto make_serial_rev(type t, type2 t2,types...ts)
{
  if constexpr (is_this_template_class_v<serial,type>)
     return make_serial_rev(serial(t2,t),ts...);
  else
    return make_serial_rev(serial(t2,serial(t)),ts...);

}

template<class type>
constexpr auto make_serial(type t)
{
  return serial(t);
}


template<class type,class type2, class... types>
constexpr auto make_serial(type t, type2 t2,types...ts)
{
  return serial(t,make_serial(t2,ts...));
}


template<class...types>
using make_serial_t=decltype(make_serial(types{}...));

template<class...types>
using make_serial_rev_t=decltype(make_serial_rev(types{}...));



template<template<class...>class C,class...types>
struct serial_environment: public make_serial_t<types...>
{
  using base_type=make_serial_t<types...>;

  base_type const& operator()()const{return *this;}

  base_type & operator()(){return *this;}

  serial_environment(base_type t):base_type{t}{}

  serial_environment(types...t):base_type{make_serial(t...)}{}
};



template<template<class...>class C,class...types>
struct serial_rev_environment: public make_serial_rev_t<types...>
{
  using base_type=make_serial_rev_t<types...>;

  base_type const& operator()()const{return *this;}

  base_type & operator()(){return *this;}
  serial_rev_environment(types...t):base_type{make_serial_rev(t...)}{}
};

template<class F,template<class...>class Op,class... types, std::size_t...Is>
auto map(std::index_sequence<Is...>, F f,const serial_environment<Op,types...>& d)
{
  return Op(f(d.template get<Is>...));
}
template<class F,template<class...>class Op1,template<class...>class Op,class... types, std::size_t...Is>
auto map(std::index_sequence<Is...>, F f,const serial_environment<Op,types...>& d)
{
  return Op1(f(d.template get<Is>...));
}


template<class F,template<class...>class Op,class... types>
auto map( F f,const serial_environment<Op,types...>& d)
{
  return map(std::index_sequence_for<types...>(),f,d);
}

template<template<class...>class Op1,class F,template<class...>class Op,class... types>
auto map( F f,const serial_environment<Op,types...>& d)
{
  return map<Op1>(std::index_sequence_for<types...>(),f,d);
}


template<class Start,class Op>
auto reduce(Start&& s, Op&& ,Nothing)
{
  return  std::forward<Start>(s);
}



template<class Start,class Op,template<class...>class prod,class... types>
auto reduce(Start&& s, Op&& op,const serial_environment<prod,types...>& list)
{
  return  reduce(std::forward<Op>(op)(std::forward<Start>(s),head(list)),op,tail(list));;
}



template<template<class...>class Op,class... types, class new_type>
auto push_back( serial_environment<Op,types...>const & list, new_type new_data)
{
  return Op(push_back(list(),new_data));
}







#endif // TAPP_BASE_H
