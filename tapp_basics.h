#ifndef TAPP_BASICS_H
#define TAPP_BASICS_H

#include "tapp_utility.h"
#include "tapp_base.h"

#include <limits>
#include <string>
#include <type_traits>
#include <utility>

// namespace tapp{




template <class... statements>
struct sequential_block : public serial_environment<sequential_block,statements...>
{
  using base_type=serial_environment<sequential_block,statements...>;
  constexpr sequential_block(statements&&... s) : base_type{std::move(s)...} {}
};

template <>
struct sequential_block<>
{
};


template <class... reason> struct Error
{
  // static_assert(sizeof...(reason)==0);
  Error(reason...) {}
  Error() {}
};

struct any_type {
  template <class some_type> constexpr auto operator()(some_type) const {
    return true;
  }
};


/**
 *  a type is a type predicate, it indicates if a value of a set of C++ types complies with it.
 *  T
 */
template <class type_predicate>
struct T : public type_predicate {
  using type_predicate::operator();
};


template<class type>
struct value_of:public type
{
  value_of(type t): type{t}{}
  value_of(){}
};


/**
 * @brief The decl struct conveys the information that the Identifier complies with Type
 */

template <class Type, typename Identifier>
struct decl: public Type {
  constexpr decl(Type t, Identifier): Type{t} {}
  constexpr decl() {}
  constexpr auto& type() const { return static_cast<Type const&>(*this); }


  constexpr auto& operator[](Identifier )const {return *this;}
  constexpr auto& operator[](Identifier ) {return *this;}

};



/**
 * @brief The args struct contains all the declarations of a lambda
 */
template <class... declarations>
struct   args: public dict<args,declarations...>
{
  using base_type=dict<args,declarations...>;
  using base_type::operator[];
  args(declarations&&...d): base_type{std::move(d)... }{}

};


template<class some_expression>
struct expr{

};






template <class... declarations> args(declarations...) -> args<declarations...>;




template <typename Identifier, class Type, class Value>
class def: public make_sequence_t<def,Identifier,Type,Value>
{
public:
  using base_type=make_sequence_t<def,Identifier,Type,Value>;
  using base_type::get;
  constexpr def(Identifier id, Type t, Value v): base_type{id,t,v} {}
  constexpr def() {}
  constexpr Value const & operator()() const { return this->template get<2>(); }
  constexpr Value & operator()()  { return  this->template get<2>();; }

  constexpr Type const & type() const { return  this->template get<1>();; }
  constexpr Type & type()  { return this->template get<1>();; }

  constexpr auto& operator[](Identifier) const { return *this; }
  //constexpr auto& operator[](Identifier)  { return *this; }
  // constexpr Type type()const {return {};}
};

struct integer_literal;

template <typename Identifier, class Type>
def(Identifier,Type,int)->def<Identifier,Type,integer_literal>;


template <class idName> struct Identifier {};

template <class type_name,class simple_type> class simple_literal {
  const simple_type v_;

public:
  constexpr simple_literal(simple_type v) : v_{v} {}
  constexpr simple_literal() : v_{} {}
  constexpr auto operator()(simple_type)const {return true;}
  constexpr auto operator()() const { return v_; }
};

class boolean_literal : public simple_literal<boolean_literal,bool> {
  using simple_literal::simple_literal;
};
class real_literal : public simple_literal<real_literal,double> {
  using simple_literal::simple_literal;
};

class string_literal : public simple_literal<string_literal,char *> {
  using simple_literal::simple_literal;
};
class integer_literal : public simple_literal<integer_literal,int> {
  using simple_literal::simple_literal;
};

template <class... Kinds> struct prod;

template <class type> struct self_type_predictate {
  constexpr auto operator()(type) const { return true; }
};

struct boolean_type : public self_type_predictate<boolean_type> {
  using self_type_predictate<boolean_type>::operator();
  constexpr auto operator()(boolean_literal) const { return true; }
};

constexpr auto resp = boolean_type{}(true);

struct real_type : public self_type_predictate<real_type> {
  using self_type_predictate::operator();
  constexpr auto operator()(real_literal) const { return true; }
};
struct integer_type : public self_type_predictate<integer_type> {
  using self_type_predictate::operator();
  constexpr auto operator()(integer_literal) const { return true; }
};
struct string_type : public self_type_predictate<string_type> {
  using self_type_predictate::operator();
  constexpr auto operator()(string_literal) const { return true; }
};

constexpr auto make_it_literal(int i) { return integer_literal{i}; }

constexpr auto make_it_literal(double r) { return real_literal{r}; }

constexpr auto make_it_literal(char *s) { return string_literal{s}; }

constexpr auto make_it_literal(bool b) { return boolean_literal{b}; }

template <class T> constexpr T make_it_literal(T &&t) { return t; }

template <class T> using type_t = decltype(T{}.type());

template <class environment, class expression> class lambda;

template <class... expressions, class expression, class... Ids, class... Types>
    class lambda<args<decl<Types, Ids>...>, sequential_block<expression,expressions...>>
    : public make_sequence_t<lambda,args<decl<Types, Ids>...>, sequential_block<expression,expressions...>>{
public:
  using arguments= args<decl<Types, Ids>...>;
  using body= sequential_block<expression,expressions...>;
  using base_type=make_sequence_t<lambda,arguments,body>;
  lambda(arguments a, body e) : base_type{a, e} {}

  arguments const& get_arguments()const {return this->template get<0>();}

  body const& get_body()const {return this->template get<1>();; }

};

template <class expression, class... Ids, class... Types>
lambda(args<decl<Types, Ids>...>, expression)
    -> lambda<args<decl<Types, Ids>...>, expression>;


template<class Output, class ...inputs>
    struct fun:  public  make_sequence_t<fun,Output,inputs...>, public self_type_predictate<fun<Output,inputs...>>
{
  using base_type=make_sequence_t<fun,Output,inputs...>;
  using self_type_predictate<fun<Output,inputs...>>::operator();

//   template<class otherOutput, class otherInput>
//    constexpr auto operator()(fun<otherOutput,otherInput> f2)const {
//    return f2.get_Output()(get_Input())&&get_Output()(f2.get_Input());
//  }

//   template <class expression, class... Ids, class... Types>
//  constexpr auto operator() (lambda<args<decl<Types, Ids>...>, expression> la) {
// //   return Output{}(la())(Input{});
//         ;
//  };


  fun(Output o, inputs... i): base_type{o, i...} {}
  fun() {}

  Output const & get_Output()const {return this->template get<0>();}

};


template <class> struct domain;
template <class> struct codomain;







template <class... Kinds> struct prod : public make_sequence_t<prod,Kinds...> {
  using base_type=make_sequence_t<prod,Kinds...>;
  using base_type::base_type;
  using base_type::get;
  prod(Kinds... k): base_type(k...){}
};

template<> struct prod<>{

};


template <class F, class... args> struct apply : public make_sequence_t<apply,F,args...> {
  using base_type=make_sequence_t<apply,F,args...>;
  using base_type::base_type;
  using base_type::get;
  apply(F f, args... a): base_type(f, a...){}
};




template <typename idName, class Something>
constexpr auto operator*(Identifier<idName>, Something i) {
  return apply(prod<>{},idName{}, make_it_literal(i));
}

template <typename idName, class Something>
constexpr auto operator*(Something i, Identifier<idName>) {
  return apply(prod<>{}, make_it_literal(i),idName{});
}

template <class Output, class Input>
struct domain<fun<Output, Input>>
    : public def<domain<fun<Output, Input>>, fun<any_type, fun<Output, Input>>,
                 Input> {
  template <class Id, class Value> domain(def<Id, fun<Output, Input>, Value>) {}
};

template <class Id, class Output, class Input, class Value>
domain(def<Id, fun<Output, Input>, Value>) -> domain<fun<Output, Input>>;

template <class Output, class Input>
struct codomain<fun<Output, Input>>
    : public def<codomain<fun<Output, Input>>,
                 fun<any_type, fun<Output, Input>>, Output> {
  template <class Id, class Value>
  codomain(def<Id, fun<Output, Input>, Value>) {}
};
template <class Id, class Output, class Input, class Value>
codomain(def<Id, fun<Output, Input>, Value>) -> codomain<fun<Output, Input>>;

template <class f> using domain_t = decltype(domain(f{}));
template <class f> using codomain_t = decltype(codomain(f{}));

template <class f, class g>
struct compose
    : public def<
          compose<f, g>,
          fun<fun<codomain_t<g>, domain_t<f>>, prod<type_t<f>, type_t<g>>>,
          compose<f, g>> {};



struct boolean_type;

template <class predicate>
struct type_predicate
    : public def<type_predicate<predicate>, fun<boolean_type, any_type>,
                 type_predicate<predicate>> {};

template <class target_type> struct converts_to {
  template <class test_type> constexpr bool operator()(test_type) const {
    return std::is_convertible_v<test_type, target_type>;
  }
};

template <class Type, class Predicate>
struct value_predicate : public def<value_predicate<Type, Predicate>,
                                    fun<boolean_type, Type>, Predicate> {};

template <class Type, class Predicate> struct sub {
  sub(Type, Predicate) {}
};

template <class... Kinds> struct sum {
  sum(Kinds...) {}
};







//} // namespace tapp

#endif // TAPP_BASICS_H
