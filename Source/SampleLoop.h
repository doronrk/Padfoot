/*
  ==============================================================================

    Looper.h
    Created: 9 Oct 2017 9:42:36pm
    Author:  Doron Roberts-Kedes

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include <unordered_map>
#include <memory>
#include "State.h"

using StateTree = std::unordered_map<std::string, std::shared_ptr<State>>;

enum LoopMode {
    ONE_WAY,
    TWO_WAY
};

class SampleLoop //: private ValueTree::Listener
{
public:
    SampleLoop(const AudioSampleBuffer &data);
    virtual ~SampleLoop() {};
    virtual float getAmplitude(int chan, double position) const;
    
    const AudioSampleBuffer &data;
    std::unordered_map<std::string, std::shared_ptr<State>> stateTree;

protected:
    
    inline bool getForward() const;
    inline bool getOneWay() const;

private:
    inline int applyDirectionToOffset(int offset) const;
    inline int applySustainModeToDoubleLenOffset(int doubleLenOffset) const;
    inline int getIndexForPosition(int position) const;
    inline float getAmplitudeForPosition(int chan, int position) const;
    inline float getAmplitudeForPosition(int chan, double position) const;
    
    bool validateBegin(int value) const;
    bool validateLen(int value) const;
    
    std::shared_ptr<State> begin;
    std::shared_ptr<State> len;
    // TODO make this a callback union type
    std::shared_ptr<State> numSamples;
    // TODO make these States
    bool forward;
    bool oneWay;
};

class SampleLoopCrossFader : public SampleLoop
{
public:
    SampleLoopCrossFader(const AudioSampleBuffer &data);
    float getAmplitude(int chan, double position) const override;
    
private:
    void boundCrossfadeLen();
    void updateSecondary();
    
    // TODO: changes to primary data must be propagated to secondary.stateTree
    // achieve this via adding some callbacks to this.
    SampleLoop secondary;
    std::shared_ptr<State> crossfadeLen;
};
