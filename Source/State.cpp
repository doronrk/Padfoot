/*
  ==============================================================================

    State.cpp
    Created: 1 Nov 2017 9:06:57pm
    Author:  Doron Roberts-Kedes

  ==============================================================================
*/

#include "State.h"

State::State(const Validator& v) : validator(v) {}

int State::get() const {
    return value;
}

void State::set(int v) {
    if (!validator(v)) {
        return;
    }
    value = v;
    for (const Callback& cb : callbacks) {
        cb(*this);
    }
}

void State::addCallback(const Callback &cb) {
    callbacks.push_back(cb);
}
