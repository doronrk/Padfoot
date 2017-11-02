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
#include <unordered_map>
#include <memory>
#include <string>

class State {

public:
    using Callback = std::function<void(const State& s)>;
    using Validator = std::function<bool(int v)>;
    
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
