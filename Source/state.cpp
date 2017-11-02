/*
  ==============================================================================

    state.cpp
    Created: 1 Nov 2017 6:21:17pm
    Author:  Doron Roberts-Kedes

  ==============================================================================
*/

#include "state.h"

State::State() {}

State::State(const Validator& v) :
validator(v) {}

void State::operator= (int v) {
    if (!validator(v)) {
        return;
    }
    value = v;
    for (const Callback& cb : callbacks) {
        cb(*this);
    }
}

int State::get() const {
    return value;
}

void State::addCallback(const Callback& cb) {
    callbacks.push_back(cb);
}
