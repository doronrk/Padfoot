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
    virtual void reset();
    virtual float getAmplitude(int chan, double position) const;
    virtual void setRange(double begin, double len);
    std::pair<double, double> getRange();
    virtual void setLoopMode(LoopMode mode);
    virtual void setForward(bool forward);
    
    void setBegin(int begin);
    
    double deltaForNote(int midiNoteNumber) const;
    
    const AudioSampleBuffer &data;
    
protected:
    int begin;
    int len;
    bool forward{true};
    LoopMode mode{ONE_WAY};
    

private:
    inline int applyDirectionToOffset(int offset) const;
    inline int applySustainModeToDoubleLenOffset(int doubleLenOffset) const;
    inline int getIndexForPosition(int position) const;
    inline float getAmplitudeForPosition(int chan, int position) const;
    inline float getAmplitudeForPosition(int chan, double position) const;
    
};

class SampleLoopCrossFader : public SampleLoop
{
public:
    SampleLoopCrossFader(const AudioSampleBuffer &data);
    float getAmplitude(int chan, double position) const override;
    void setRange(double begin, double len) override;
    void setLoopMode(LoopMode mode) override;
    void setForward(bool forward) override;
    void setCrossfadeLen(int len);

private:
    void boundCrossfadeLen();
    void updateSecondaryBegin();
    
    SampleLoop secondary;
    int crossfadeLen{22050};
};
