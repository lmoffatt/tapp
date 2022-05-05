#ifndef TAPP_LISP_BASE_H
#define TAPP_LISP_BASE_H

#include <type_traits>
#include <utility>

#include "tapp_utility.h"

struct Nothing {
    template<class Something>
    friend auto operator||(Nothing, Something&&s) {return std::forward<Something>(s);}

    template<class Something>
    friend auto operator||(Something&&s, Nothing) {return std::forward<Something>(s);}

    friend auto operator||(Nothing, Nothing) {return Nothing{};}

};

template <class T> constexpr auto not_Nothing = true;

template <> constexpr auto not_Nothing<Nothing> = false;

template <class T> constexpr auto not_Nothing_v = not_Nothing<std::decay_t<T>>;


template <std::size_t I, class type_name, typename = void> class v
{
  type_name x_;

public:
  using type = type_name;
  constexpr v(type x) : x_{x} {}
  constexpr v() : x_{} {}
  constexpr auto &operator()() const { return x_; }
  constexpr auto &operator()() { return x_; }
  template <std::size_t J, typename = std::enable_if_t<I == J>>
  constexpr auto &get() const {
    return x_;
  }

  template <std::size_t J, typename = std::enable_if_t<I == J>>
  constexpr auto &get() {
    return x_;
  }
};

template <std::size_t I, class type_name>
class v<I, type_name, std::enable_if_t<std::is_empty_v<type_name>&& std::is_default_constructible_v<type_name>>> {
public:
  using type = type_name;
  constexpr v(type) {}
  constexpr v() {}
  constexpr auto operator()() const { return type{}; }
  template <std::size_t J, typename = std::enable_if_t<I == J>>
  constexpr auto get() const {
    return type{};
  }
};

template <class type, class Id, typename = void>
struct has_Id : public std::false_type {};

template <class type, class Id>
    struct has_Id<type, Id, std::enable_if_t<not_Nothing<decltype(type{}[Id{}])>>>
    : public std::true_type{};
template <class type, class Id>
constexpr bool has_Id_v = has_Id<type, Id>::value;

template <template <class...> class, class...> struct sequence;

template <template <class...> class prod, class... types, std::size_t... Is>
struct sequence<prod, std::index_sequence<Is...>, types...>
    : public v<Is, types>... {
  using v<Is, types>::get...;

  template<class...Types,typename=std::enable_if_t<(sizeof...(Types)>0)&&(std::is_same_v<Types,types>&&...)>>
  constexpr sequence(Types... ts) : v<Is, types>{ts}... {}
  constexpr sequence() {}

  template <std::size_t I> struct get_type {
      using type = std::decay_t<decltype(sequence{}.template get<I>())>;
  };

  template <std::size_t I> using get_type_t = typename get_type<I>::type;

  template <std::size_t I, class Id>
  constexpr auto extract(Id) const -> std::enable_if_t<has_Id<get_type_t<I>, Id>::value,decltype(this->template get<I>()[Id{}])>
  {
    return this->template get<I>()[Id{}];
  }

  template <std::size_t I, class Id,
            typename = std::enable_if_t<!has_Id<get_type_t<I>, Id>::value>>
  constexpr auto extract(Id) const {

      if constexpr (I > 0)
      return extract<I - 1>(Id{});
    else
      return Nothing{};
  }

  template <class Id> constexpr auto operator[](Id) const {
    return this->template extract<sizeof...(types) - 1>(Id{});
  }
};

template <template <class...> class Op, class... types>
using make_sequence_t =
    sequence<Op, std::index_sequence_for<types...>, types...>;

template <template <class...> class Op, class... types, std::size_t... Is,
          class new_type>
auto push_back(sequence<Op, std::index_sequence<Is...>, types...> list,
               new_type new_data) {
  return Op(list.template get<Is>()..., new_data);
}

#endif // TAPP_LISP_BASE_H
