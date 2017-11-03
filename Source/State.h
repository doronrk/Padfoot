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
    using Callback = std::function<void(const State& s)>;
    using Validator = std::function<bool(const State& s)>;
    
    State() {};
    State(const Validator& v);
    /*
    int get() const;
    void set(int v);
     */
    void addCallback(const Callback& cb);
    
    State& operator= (int value);
    State& operator= (bool value);
    
    var x;
    
    operator int() const noexcept;
    operator bool() const noexcept;
    
private:
    Validator validator{[](int /*v*/){return true;}};
    std::vector<Callback> callbacks;
    mpark::variant<int, bool> value_;
};
