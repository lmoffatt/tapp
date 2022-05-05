#ifndef TAPP_LISP_RUN_H
#define TAPP_LISP_RUN_H


#include "tapp_lisp.h"
#include "tapp_lisp_global_env.h"
#include "tapp_list_map.h"

#include <functional>
struct Run_value{
    template<class ...Ts>
    auto operator()(Ts&& ...t)
    {
        return run(std::forward<Ts>(t)...)();
    }

};




template<class... environment>
constexpr auto run(int x,const environment &...)
{
    return vtv(integer_type{},x);
}

template<class... environment>
constexpr auto run(double x,environment...)
{

    return vtv(real_type{},x);
}


template<class environment, class... environments, class type>
constexpr auto run(Identifier<type> t, environment && e,environments &&...es)
{
    if constexpr(not_Nothing_v<decltype(e[type{}])>)
        return std::forward<environment>(e)[type{}]();
    else
        return run(t,std::forward<environments>(es)...);
}

template< class type>
constexpr auto run(Identifier<type> )
{
    return type{};
}

template<class environment, class... environments, class Id, class type, class value>
constexpr auto verify(type, Def<Id,vtv<type,value>> typed_value,const environment &, const environments&... )

{
    return typed_value();
}



template<class environment, class... environments, class type, class value>
constexpr auto verify(type, vtv<type,value> typed_value,const environment &, const environments&... )

{
    return typed_value;
}




template<class... environments, class type, class value>
constexpr auto run(tv<type,value> typed_value,environments&&... es)
{
    auto run_value=run(typed_value(),std::forward<environments>(es)...);
    return verify(typed_value.get_type(), run_value,std::forward<environments>(es)...);
}

struct Invoke
{
    template<class ...Ts>
    auto operator()(Ts&&...t)const
    {
        return std::invoke(std::forward<Ts>(t)...);
    }
};

template<class... environments, class fun,class... value,class func, class lambda>
constexpr auto run_apply(vtv<func,lambda> f,apply<fun,value...> closure,environments&&... es)
{
    return apply_on_map(f(),[](auto x, auto...ees){return run(x,ees...);},closure,es...);
}



template<class... environments, class fun,class... value>
constexpr auto run(apply<fun,value...> closure,environments&&... es)
{
    auto f=run(closure.get_function(),es...);
    return run_apply(f,closure,es...);
}






template<class... environments, class Identifier, class type, class value, typename=std::enable_if_t<(!has_Id_v<environments,Identifier>&&...)>>
constexpr auto run(Def<Identifier,tv<type,value>> x, environments&&... es)
{
    auto identifier= x.get_Identifier();
    auto typed_value= x();
    auto run_value=run(typed_value,std::forward<environments>(es)...);
    return Def(std::move(identifier),std::move(run_value));
}



template<class... environments,class... expressions, class...all_expressions>
constexpr auto run_index(std::index_sequence<>,
                                    const seq<all_expressions...>& ,
                                    seq<expressions...>&& exp,
                                    const environments&... )
{
    return std::move(exp);
}





template<std::size_t I, std::size_t ...Is,class... environments,class... expressions, class...all_expressions>
constexpr auto run_index(std::index_sequence<I, Is...>,
                                    const seq<all_expressions...>& s,
                                    seq<expressions...>&& exp,
                                     environments&&... e)
{
    auto reduced_expression=run(s.template get<I>(),exp,e...);
    return run_index(std::index_sequence<Is...>(),s,push_back(std::move(exp),std::move(reduced_expression)),std::forward<environments>(e)...);
}

template<class... environments, class...expressions>
constexpr auto run( const seq<expressions...>& s, environments&&... e)
{
    return run_index(std::index_sequence_for<expressions...>(),s,seq<>(),std::forward<environments>(e)...);
}



#endif // TAPP_LISP_RUN_H
