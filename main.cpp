#include "tapp_run.h"
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

// struct channel_state_count : public tapp::def<channel_state_count,

struct hehe : public tapp::Identifier<hehe> {};
struct hehe2 : public tapp::Identifier<hehe2> {};
struct hehe3 : public tapp::Identifier<hehe3> {};

struct f1 : public tapp::Identifier<f1> {};


int main() {
  auto data = tapp::env(tapp::x(hehe{}, 10));

  auto d =
      tapp::sequential_block(
          tapp::def(hehe2{}, tapp::integer_literal{}, hehe{} * 2),
          tapp::def(hehe3{}, tapp::integer_literal{}, hehe2{} * 2.54)
          );
  auto f=tapp::lambda(tapp::args{tapp::decl{tapp::integer_literal{},hehe{}}},hehe{}*2);
  auto r = tapp::run(data, d);
  return 0;
}
