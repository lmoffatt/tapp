#ifndef TAPP_UTILITY_H
#define TAPP_UTILITY_H
#include <utility>



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

template<template<class ...>class V, class T>
constexpr bool is_this_template_class_v=is_this_template_class<V,T>::value;


template<class T,class...Ts>
constexpr bool not_these_types_v=(!std::is_same_v<std::decay_t<T>,Ts>&&...);

template <auto fun>
struct f;



#endif // TAPP_UTILITY_H
