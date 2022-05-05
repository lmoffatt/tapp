#ifndef TAPP_LISP_BASE_SERIAL_H
#define TAPP_LISP_BASE_SERIAL_H

#include "tapp_utility.h"
#include <utility>

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




#endif // TAPP_LISP_BASE_SERIAL_H
