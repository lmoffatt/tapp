//#include "tapp_run.h"
//#include "tapp_type_reduction.h"
#include "tapp_lisp.h"
#include "tapp_lisp_run.h"
#include <iostream>

/***
 *   como test voy a tratar de plantear la ecuacion
 *   dp/dt = p Q
 *
 *   y que p(t)= p(0) * exp(Q t)
 *  la resuelve
 *
 *
 * primero debo poder escribir p(s) y p(s,t)
 *
 * para esto tengo que poder escribir los distintos estados de un canal
 *
 * y la probabilidad de un estado
 *
 * para ello tengo que definir el tipo suma de estados
 *
 *
 *
 * 1 como declaro una variable de tipo channel_state de un numero fijo de
 * canales antes debo declarar una variable tipo numero de channel_states|
 *
 *
 */

// struct channel_state_count : public def<channel_state_count,

int main() {
  struct hehe : public Identifier<hehe> {};
  struct hehe2 : public Identifier<hehe2> {};
  struct hehe3 : public Identifier<hehe3> {};
  struct h : public Identifier<h> {};

  struct f1 : public Identifier<f1> {};

  constexpr auto d = lambda(
      seq(),
      seq(Def(hehe2{}, tv(integer_type{}, 2)),
          Def(hehe3{}, tv(real_type{}, hehe2{} * 2.54)),
          Def(f1{}, tv(fun(integer_type{}, integer_type{}),
                       lambda(seq{Decl{integer_type{}, h{}}}, seq{},
                              seq(Def(out{}, tv(integer_type{}, h{} * 2))))))),
      seq(Def(h(), tv(integer_type{}, apply(f1{}, hehe{})))));

  auto vd = seq(Def(hehe2{}, tv(integer_type{}, 1)),
                Def(hehe3{}, tv(real_type{}, hehe2{} * 2.54)));
  using aa = typename decltype(vd)::get_type<0>::type;
  aa r{};

  auto vd2 = run(tv(integer_type{}, 2), global_env);

  auto vd3 = run(Def(hehe2{}, tv(integer_type{}, 2)), global_env);

  auto vd1 = run(seq(Def(hehe2{}, tv(integer_type{}, 2)),
                     Def(hehe3{}, tv(integer_type{}, hehe2{} * 2))),
                 global_env);

  return 0;
}
