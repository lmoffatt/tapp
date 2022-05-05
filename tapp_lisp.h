#ifndef TAPP_LISP_H
#define TAPP_LISP_H

#include "tapp_lisp_base.h"

template <class idName> struct Identifier {};

template <class value> struct Type {};

template <class Type, class Value, typename = void>
struct tv : make_sequence_t<tv, Type, Value> {
  using base_type = make_sequence_t<tv, Type, Value>;
  constexpr tv(Type &&t, Value &&v) : base_type{std::move(t), std::move(v)} {}
  constexpr tv() {}
  constexpr auto get_type() const { return this->template get<0>(); }
  constexpr auto operator()() const { return this->template get<1>(); }
};

template <class Type, class Value>
struct tv<Type, Value,
          std::enable_if_t<(std::is_empty_v<Type> && std::is_empty_v<Value>&& std::is_default_constructible_v<Value>)>> {
  constexpr tv(Type, Value) {}
  constexpr tv() {}
  constexpr auto get_type() const { return Type{}; }
  constexpr auto operator()() const { return Value{}; }
};

template <class Type, class Value, typename = std::enable_if_t<!(std::is_empty_v<Type> && std::is_empty_v<Value> && std::is_default_constructible_v<Value>)>>
struct vtv : make_sequence_t<vtv, Type, Value> {
  using base_type = make_sequence_t<vtv, Type, Value>;
  constexpr vtv(Type &&t, Value &&v) : base_type{std::move(t), std::move(v)} {}
  constexpr vtv(Type const &t, Value const &v) : base_type{t, v} {}
  constexpr vtv() {}
  constexpr decltype(auto) get_type() const { return this->template get<0>(); }
  constexpr decltype(auto) operator()() const { return this->template get<1>(); }
};

template <class Type, class Value>
struct vtv<
    Type, Value,
    std::enable_if_t<(std::is_empty_v<Type> && std::is_empty_v<Value> && std::is_default_constructible_v<Value>),int>> {
  constexpr vtv(Type, Value) {}
  constexpr vtv() {}
  constexpr auto get_type() const { return Type{}; }
  constexpr auto operator()() const { return Value{}; }
};

template <class Type, class... Value>
class error : make_sequence_t<error, Type, Value...> {
  using base_type = make_sequence_t<error, Type, Value...>;
  constexpr error(Type &&t, Value &&...v)
      : base_type{std::move(t), std::move(v)...} {}
  constexpr error(Type t, Value... v)
      : base_type{std::move(t), std::move(v)...} {}
};

template <class Id, class TypedValue, typename = void>
struct Def {
    Id id_;
    TypedValue value_;
  auto get_Identifier() const { return id_; }
  constexpr Def(Id &&id, TypedValue &&value)
      : id_ {id},value_{value}{}
  constexpr Def() {}
  constexpr auto &operator[](Id) const { return *this; }

  constexpr auto &operator()() const { return value_; }
};

template <class Id, class TypedValue>
struct Def<
    Id, TypedValue,
    std::enable_if_t<(std::is_empty_v<Id> && std::is_empty_v<TypedValue>&& std::is_default_constructible_v<TypedValue>)>> {
  auto get_Identifier() const { return Id{}; }
  constexpr Def(Id, TypedValue) {}
  constexpr Def() {}
  constexpr auto &operator[](Id) const { return *this; }

  constexpr auto operator()() const { return TypedValue{}; }
};

template <class Type, class Id>
struct Decl : public make_sequence_t<Decl, Type, Id> {
  using base_type = make_sequence_t<Decl, Type, Id>;
  constexpr Decl(Type t, Id id) : base_type{std::move(t), std::move(id)} {}
  constexpr auto &operator[](Id) const { return *this; }
};

template <class Id, class Value>
struct Mutate : public make_sequence_t<Decl, Id, Value> {
  using base_type = make_sequence_t<Decl, Id, Value>;
  constexpr Mutate(Value v, Id id) : base_type{v, id} {}
  constexpr auto &operator[](Id) const { return *this; }
};

template <class FunctionValue, class... argumentValues>
struct apply : public make_sequence_t<apply, argumentValues...> {
  using base_type = make_sequence_t<apply, argumentValues...>;

    FunctionValue f_;
  auto& get_function()const {return f_;}
  constexpr apply(FunctionValue&& f, argumentValues... args)
        : base_type{args...}, f_{std::move(f)} {}



};

template <class... statements>
struct seq : public make_sequence_t<seq, statements...> {
  using base_type = make_sequence_t<seq, statements...>;
    using base_type::operator[];
  constexpr seq(statements... s) : base_type{s...} {}
};

template <> struct seq<> : public make_sequence_t<seq> {
  template <class any> constexpr auto operator[](any) const {
    return Nothing{};
  }
};

struct out {};

template <class...> struct lambda;

template <class... Ids, class... Types, class... intermediate_staments,
          class... outputIds, class... outputTypes, class... outputValues>
struct lambda<seq<Decl<Ids, Types>...>, seq<intermediate_staments...>,
              seq<Def<outputIds, tv<outputTypes, outputValues>>...>>
    : public make_sequence_t<
          lambda, seq<Decl<Ids, Types>...>, seq<intermediate_staments...>,
          seq<Def<outputIds, tv<outputTypes, outputValues>>...>> {
  using arguments = seq<Decl<Ids, Types>...>;
  using body = seq<intermediate_staments...>;
  using output = seq<Def<outputIds, tv<outputTypes, outputValues>>...>;
  using base_type = make_sequence_t<lambda, arguments, body, output>;
  constexpr lambda(arguments a, body b, output o) : base_type{a, b, o} {}
};

template <class... Ids, class... Types, class... intermediate_staments,
          class... outputIds, class... outputTypes, class... outputValues>
lambda(seq<Decl<Ids, Types>...>, seq<intermediate_staments...>,
       seq<Def<outputIds, tv<outputTypes, outputValues>>...>)
    -> lambda<seq<Decl<Ids, Types>...>, seq<intermediate_staments...>,
              seq<Def<outputIds, tv<outputTypes, outputValues>>...>>;

template <class domain_type, class codomain_type>
struct fun : public make_sequence_t<fun, domain_type, codomain_type> {
  using base_type = make_sequence_t<fun, domain_type, codomain_type>;
  constexpr fun(domain_type d, codomain_type c) : base_type{d, c} {}
};

template <class... types> struct cp : public make_sequence_t<cp, types...> {
  using base_type = make_sequence_t<cp, types...>;
  constexpr cp(types... t) : base_type{t...} {}
};

template <class... types> struct du : public make_sequence_t<du, types...> {
  using base_type = make_sequence_t<du, types...>;
  constexpr du(types... t) : base_type{t...} {}
};

struct boolean_type : public Identifier<boolean_type> {};

struct real_type : public Identifier<real_type> {};
struct integer_type : public Identifier<integer_type> {};
struct string_type : public Identifier<string_type> {};

struct true_literal {};
struct false_literal {};

struct prod: public Identifier<prod> {};

template <typename idName, class Something>
constexpr auto operator*(Identifier<idName>, Something i) {
  return apply(prod{}, idName{}, i);
}

template <typename idName, class Something>
constexpr auto operator*(Something i, Identifier<idName>) {
  return apply(prod{}, i, idName{});
}

#endif // TAPP_LISP_H
