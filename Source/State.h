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
#include "Variant.h"
#include "../JuceLibraryCode/JuceHeader.h"

class State {
    
public:
    using Validator = std::function<bool(const State& s)>;
    using Callback = std::function<void(const State& s)>;
    
    State() {};
    State(int v) {value_ = v;};
    State(bool v) {value_ = v;};
    State(const Validator& v);
    
    void addCallback(const Callback& cb);
    
    State& operator= (int value);
    State& operator= (bool value);
    
    operator int() const noexcept;
    operator bool() const noexcept;
    
private:
    Validator validator{[](const State& /*s*/){return true;}};
    std::vector<Callback> callbacks;
    mpark::variant<int, bool> value_;
};
