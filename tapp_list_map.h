#ifndef TAPP_LIST_MAP_H
#define TAPP_LIST_MAP_H


#include "tapp_lisp_base.h"

template<class F,template<class...>class Op,class... types, std::size_t...Is, class...environments>
auto map( F f,sequence<Op,std::index_sequence<Is...>,types...> list, environments&&...e)
{
    return Op(f(list.template get<Is>(),std::forward<environments>(e)...)...);
}

template<template<class...>class Op1,class F,template<class...>class Op,class... types, std::size_t...Is, class...environments>
auto map( F f,sequence<Op,std::index_sequence<Is...>,types...> list, environments&&...e)
{
    return Op1(f(list.template get<Is>(),std::forward<environments>(e)...)...);
}

template<class Op1,class F,template<class...>class Op,class... types, std::size_t...Is, class...environments>
auto apply_on_map(Op1 op, F f,sequence<Op,std::index_sequence<Is...>,types...> list, environments&&...e)
{
    return op(f(list.template get<Is>(),std::forward<environments>(e)...)...);
}


#endif // TAPP_LIST_MAP_H
