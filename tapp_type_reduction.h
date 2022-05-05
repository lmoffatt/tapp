#ifndef TAPP_TYPE_REDUCTION_H
#define TAPP_TYPE_REDUCTION_H

#include "tapp_basics.h"


template <class Type, typename expression>
struct typed_expression: public make_sequence_t<typed_expression,Type,expression>
{
  using base_type=make_sequence_t<typed_expression,Type,expression>;
  typed_expression(Type&& t, expression&& e): base_type{std::move(t),std::move(e)}{}

  typed_expression(){}
  auto get_type()const {return this->template get<0>();}
  auto get_body()const {return this->template get<1>();}
};




template <class Type, typename Identifier>
auto type_deduction(decl<Type,Identifier> const& d){return typed_expression(d.type(),Identifier{});}

template<class environment, typename Id, typename=void>
auto type_reduction(const environment& ,Identifier<Id>)
{
  return Id{};
}

template<class environment, typename Id, typename=std::void_t<>>
auto type_reduction(const environment& e,Identifier<Id>)->decltype(type_deduction(e[Id{}]))
{
  return type_deduction(e[Id{}]);
}






template<class environment, typename typeId, typename type>
auto type_reduction(const environment& ,const simple_literal<typeId,type>& x)
{
  return typed_expression(x);
}

template<class environment, typename typeId>
auto type_reduction(const environment& ,const self_type_predictate<typeId>& x)
{
  return typed_expression(x);
}




template<class typed_expression, class...typed_expressions>
struct typed_sequential_block: public dict<typed_sequential_block,typed_expression, typed_expressions...>
{
  using base_type=dict<typed_sequential_block,typed_expression, typed_expressions...>;

  using base_type::operator[];
  typed_sequential_block(typed_expression&& e,typed_expressions&&...es): base_type{e,es...}{}
};





template <typename Identifier, class Type, class Value>
class typed_def: public make_sequence_t<typed_def,Type, Value>
{
public:
  using base_type=make_sequence_t<typed_def,Type, Value>;
  constexpr typed_def(Identifier, Type t, Value v): base_type{t,v} {}
  constexpr typed_def() {}
  constexpr Value const & operator()() const { return this->template get<2>(); }
  constexpr Value & operator()()  { return this->template get<2>(); }

  constexpr Type const & type() const { return this->template get<1>(); }
  constexpr Type & type()  { return this->template get<1>(); }

  constexpr auto& operator[](Identifier) const { return *this; }
  constexpr auto& operator[](Identifier)  { return *this; }
  // constexpr Type type()const {return {};}
};

template<class environment, typename Identifier, class Type, class Value>
auto type_reduction(const environment& e, const def<Identifier,Type,Value>& d)
{
  return map<typed_def>([&e](auto&& x){return type_reduction(e,std::forward<decltype(x)>(x));},d);
}




template <class environment, class expression> class typed_lambda;

template <class... expressions, class expression, class... Ids, class... Types>
class typed_lambda<args<decl<Types, Ids>...>, typed_sequential_block<expression,expressions...>>: public dict<typed_lambda,args<decl<Types, Ids>...>,
                                                                                                                typed_sequential_block<expression,expressions...>>{
public:
  using arguments= args<decl<Types, Ids>...>;
  using body= typed_sequential_block<expression,expressions...>;
  using base_type=dict<typed_lambda,arguments,body>;

  typed_lambda(arguments a, expression e) : base_type{a,e} {}
  arguments const& get_arguments()const {return static_cast<arguments const &>(*this);}

  body const& get_body()const {return static_cast<body const&>(*this); }

};

template <class expression, class... Ids, class... Types>
typed_lambda(args<decl<Types, Ids>...>, expression)
    -> typed_lambda<args<decl<Types, Ids>...>, expression>;



template <class environment, class... expression> class temp_lambda;

template <class... expressions, class expression, class... Ids, class... Types>
class temp_lambda<args<decl<Types, Ids>...>, typed_sequential_block<expression,expressions...>>: public dict<temp_lambda,args<decl<Types, Ids>...>,
                                                                                                                typed_sequential_block<expression,expressions...>>{
public:
  using arguments= args<decl<Types, Ids>...>;
  using body= typed_sequential_block<expression,expressions...>;
  using base_type=dict<temp_lambda,arguments,body>;

  temp_lambda(arguments a, expression e) : base_type{a,e} {}
  arguments const& get_arguments()const {return static_cast<arguments const &>(*this);}

  body const& get_body()const {return static_cast<body const&>(*this); }
};


template < class... Ids, class... Types>
class temp_lambda<args<decl<Types, Ids>...>>: public args<decl<Types, Ids>...>{
public:
  using arguments= args<decl<Types, Ids>...>;
  temp_lambda(arguments a) : arguments{a}{}

  arguments const& get_arguments()const {return static_cast<arguments const &>(*this);}


};


template <class... expressions, class expression, class... Ids, class... Types>
temp_lambda(args<decl<Types, Ids>...>, expression)
    -> temp_lambda<args<decl<Types, Ids>...>, typed_sequential_block<expression,expressions...>>;

template < class... Ids, class... Types>
temp_lambda(const args<decl<Types, Ids>...>&)
    -> temp_lambda<args<decl<Types, Ids>...>>;


template <class... expressions, class expression, class... Ids, class... Types>
auto push_back(temp_lambda<args<decl<Types, Ids>...>, typed_sequential_block<expressions...>>&& la, expression&& e)
{
  return temp_lambda(std::move(la.get_args()), push_back(std::move(la.get_body()),std::move(e)));
}

template < class expression, class... Ids, class... Types>
auto push_back(temp_lambda<args<decl<Types, Ids>...>>&& la, expression&& e)
{
  return temp_lambda(std::move(la.get_args()), typed_sequential_block(std::move(e)));
}

template < class... types, class ...expressions>
auto type_deduction(prod<typed_expression<types,expressions>...>&& p) {
  return typed_expression(type_deduction(map([](auto x){return x.get_type();},p),map([](auto x){return x.get_body();},p)));
}


template <class environment, class ...expression>
auto type_reduction(const environment &e, const prod<expression...>& p) {
  return type_deduction(map([&e](auto&& exp){return type_reduction(e,std::forward<decltype(exp)>(exp));},p));
}






template <class environment, class... expressions>
auto type_reduction(environment&& env, sequential_block<expressions...> const& body)
{
  return type_reduction(
      reduce(
          std::forward<environment>(env),
          [](auto&& acc_env,auto& expr){return push_back(std::move(acc_env),type_reduction(acc_env,expr));},
          body
          )
      );
}


template <class... expressions,  class... Ids, class... Types>
auto type_reduction(lambda<args<decl<Types, Ids>...>, sequential_block<expressions...>> const& la)
{
  return type_reduction(temp_lambda(la.get_arguments()),la.get_body());

}

template<class Output, class ...inputs>
struct typed_fun:  public  make_sequence_t<typed_fun,Output,inputs...>, public self_type_predictate<fun<Output,inputs...>>
{
  using base_type=make_sequence_t<typed_fun,Output,inputs...>;
  using self_type_predictate<typed_fun<Output,inputs...>>::operator();

  //   template<class otherOutput, class otherInput>
  //    constexpr auto operator()(fun<otherOutput,otherInput> f2)const {
  //    return f2.get_Output()(get_Input())&&get_Output()(f2.get_Input());
  //  }

  //   template <class expression, class... Ids, class... Types>
  //  constexpr auto operator() (lambda<args<decl<Types, Ids>...>, expression> la) {
  // //   return Output{}(la())(Input{});
  //         ;
  //  };


  typed_fun(Output o, inputs... i): base_type{o, i...} {}
  typed_fun() {}

  Output const & get_Output()const {return this->template get<0>();}

};




template<class environment, class Output, class ...inputs>
auto type_reduction(const environment& e, const fun<Output,inputs...>& f )
{
  return map<typed_fun>([&e](auto&& elem){return type_reduction(e,std::forward<decltype(elem)>(elem));},f);
}




#endif // TAPP_TYPE_REDUCTION_H
