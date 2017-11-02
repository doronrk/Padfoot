/*
  ==============================================================================

    State.h
    Created: 1 Nov 2017 9:06:57pm
    Author:  Doron Roberts-Kedes

  ==============================================================================
*/

#pragma once

#include <vector>
#include <functional>

class State {
    using Callback = std::function<void(const State& s)>;
    using Validator = std::function<bool(int v)>;
public:
    State() {};
    State(const Validator& v);
    int get() const;
    void set(int v);
    void addCallback(const Callback& cb);
private:
    int value{0};
    Validator validator{[](int /*v*/){return true;}};
    std::vector<Callback> callbacks;
};
