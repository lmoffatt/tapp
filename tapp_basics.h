#ifndef TAPP_BASICS_H
#define TAPP_BASICS_H
namespace tapp{


template <class... statements>
struct block{
  block(statements...){}
};


template<class Type,typename Identifier>
struct decl{
  decl(Type,Identifier){}
};

template <typename Identifier, class Type, class Value>
struct def{
  def(Type,Identifier,Value){}
};



template<class Type, class Predicate>
struct sub{
  sub(Type, Predicate){}
};


template<class... Kinds>
struct sum{
  sum(Kinds...){}
};



template<class... Kinds>
struct prod{
  prod(Kinds...){}
};

template<class Output, class...Inputs>
struct fun{
  fun(Output, Inputs...){}
};

template<class expression, class...vars>
struct lambda;

template<class expression, class...Ids, class... Types>
struct lambda<expression,decl<Types,Ids>...>
{
  lambda(expression, decl<Types,Ids>...){}
};
















} // namespace tapp






#endif // TAPP_BASICS_H
