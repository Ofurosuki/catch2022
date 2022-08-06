#pragma once

#include <catchPosition.h>
#include <get3.h>

#include <tinyfsm.hpp>

using fsmList = tinyfsm::FsmList<CatchPosition, Get3>;

template <typename E>
void send_event(E const& event) {
  fsm_list::template dispatch<E>(event);
}