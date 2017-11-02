/*
  ==============================================================================

    state.h
    Created: 1 Nov 2017 6:21:17pm
    Author:  Doron Roberts-Kedes

  ==============================================================================
*/
#include <functional>
#include <vector>

#pragma once

class State {
using Validator = std::function<bool(int v)>;
using Callback = std::function<void(const State& s)>;
    
public:
    State();
    State(const Validator& v);
    void operator= (int value);
    int get() const;
    void addCallback(const Callback& cb);
private:
    int value{0};
    Validator validator;
    std::vector<Callback> callbacks;
};

class StateTree {
public:
    void operator[] (std::string key) const;
    
};
