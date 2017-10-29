/*
  ==============================================================================

    Looper.h
    Created: 9 Oct 2017 9:42:36pm
    Author:  Doron Roberts-Kedes

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

enum LoopMode {
    ONE_WAY,
    TWO_WAY
};

class SampleLoop
{
public:
    SampleLoop(const AudioSampleBuffer &data);
    virtual ~SampleLoop() {};
    virtual float getAmplitude(int chan, double position) const;
    /*
    virtual void setRange(double begin, double len);
    std::pair<double, double> getRange();
    virtual void setLoopMode(LoopMode mode);
    virtual void setForward(bool forward);
     */
    int getNumSamples() const;
    
    // These functions should only be called by SampleLoopCrossFader
    void setBegin(int begin);
    void setLen(int len);
    
    const AudioSampleBuffer &data;
    ValueTree state;

protected:
    // TODO updating any of these values should synchronize w audio callback thread
    var beginVar{0};
    var lenVar{0};
    var forwardVar{true};
    var oneWayVar{true};
    var numSamplesVar;
    
    inline int getBegin() const;
    inline int getLen() const;
    inline bool getForward() const;
    inline bool getOneWay() const;

private:
    inline int applyDirectionToOffset(int offset) const;
    inline int applySustainModeToDoubleLenOffset(int doubleLenOffset) const;
    inline int getIndexForPosition(int position) const;
    inline float getAmplitudeForPosition(int chan, int position) const;
    inline float getAmplitudeForPosition(int chan, double position) const;
};

class SampleLoopCrossFader : public SampleLoop, private Value::Listener
{
public:
    SampleLoopCrossFader(const AudioSampleBuffer &data);
    
    float getAmplitude(int chan, double position) const override;
    
private:
    void boundCrossfadeLen();
    void updateSecondary();
    void valueChanged(juce::Value &value) override;
    
    SampleLoop secondary;
    int crossfadeLen{0};
};
