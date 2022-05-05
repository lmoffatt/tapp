#ifndef TAPP_LISP_TYPE_REDUCTION_H
#define TAPP_LISP_TYPE_REDUCTION_H

#include "tapp_lisp.h"
#include "tapp_lisp_global_env.h"





template<class... environment>
constexpr auto beta_reduction(int x,const environment &...)
{
  return x;
}

template<class... environment>
constexpr auto beta_reduction(double x,environment...)
{

  return x;
}


template<class environment, class... environments, class type>
constexpr auto beta_reduction(Identifier<type> t, environment && e,environments &&...es)
{
    if constexpr(not_Nothing_v<decltype(e[type{}])>)
        return std::forward<environment>(e)[type{}];
  else
   return beta_reduction(t,std::forward<environments>(es)...);
}

template< class type>
constexpr auto beta_reduction(Identifier<type> )
{
  return type{};
}


template<class type,class type_value,class value>
auto  build_verified_typed_value(type type_expression,type_value type_definition, value reduced_value)
{
  return error(type_expression,type_definition,reduced_value);
}

template<class type,class value>
auto  build_verified_typed_value(type type_expression,vtv<primitive_type,Type<value>> , value reduced_value)
{
  return vtv(type_expression,reduced_value);
}




template<class environment, class... environments, class type, class value>
constexpr auto beta_reduction(tv<type,value> typed_value,const environment &e, const environments&... es)
{
  auto type_expression=typed_value.get_type();
   auto type_definition=beta_reduction(type_expression,e,es...);
   auto reduced_value=beta_reduction(typed_value(),e,es...);

   return build_verified_typed_value(type_expression,type_definition(),reduced_value);
}

template<class... environments, class Identifier, class type, class value, typename=std::enable_if_t<(!has_Id_v<environments,Identifier>&&...)>>
constexpr auto beta_reduction(Def<Identifier,tv<type,value>> x, environments&&... es)
{
  auto identifier= x.get_Identifier();
  auto typed_value= x();
  auto reduced_identifier=beta_reduction(identifier,std::forward<environments>(es)...);
  auto reduced_typed_value=beta_reduction(typed_value,std::forward<environments>(es)...);
  return Def(std::move(reduced_identifier),std::move(reduced_typed_value));
}



template<class... environments,class... expressions, class...all_expressions>
constexpr auto beta_reduction_index(std::index_sequence<>,
                                    const seq<all_expressions...>& ,
                                    seq<expressions...>&& exp,
                                    const environments&... )
{
  return std::move(exp);
}





template<std::size_t I, std::size_t ...Is,class... environments,class... expressions, class...all_expressions>
constexpr auto beta_reduction_index(std::index_sequence<I, Is...>,
                                    const seq<all_expressions...>& s,
                                    seq<expressions...>&& exp,
                                    const environments&... e)
{
  auto reduced_expression=beta_reduction(s.template get<I>(),exp,e...);
  return beta_reduction_index(std::index_sequence<Is...>(),s,push_back(std::move(exp),std::move(reduced_expression)),e...);
 }

template<class... environments, class...expressions>
constexpr auto beta_reduction( const seq<expressions...>& s, const environments&... e)
{
   return beta_reduction_index(std::index_sequence_for<expressions...>(),s,seq<>(),e...);
}



#endif // TAPP_LISP_TYPE_REDUCTION_H
