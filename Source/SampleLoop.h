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
    virtual ~SampleLoop() {};
    virtual float getAmplitude(int chan, double position) const;
    virtual void setRange(double begin, double len);
    std::pair<double, double> getRange();
    virtual void setLoopMode(LoopMode mode);
    virtual void setForward(bool forward);
    
    void setBegin(int begin);
    
    void setOutputSampleRate(double rate);
    void updateData(AudioFormatReader &);
    double deltaForNote(int midiNoteNumber) const;
    // TODO figure out elegant way to expose this to SampleComponent class w/o making it public
    // and without this class having to know about the concept of a Thumbnail
    AudioSampleBuffer data;
    

    
protected:
    int begin;
    int len;
    bool forward{true};
    
    
private:
    inline bool movingForwardAtPosition(int position) const;
    inline int applyDirectionToOffset(int offset) const;
    inline int applySustainModeToDoubleLenOffset(int doubleLenOffset) const;
    inline int getIndexForPosition(int position) const;
    inline float getAmplitudeForPosition(int chan, int position) const;
    inline float getAmplitudeForPosition(int chan, double position) const;
    
    LoopMode mode{TWO_WAY};

    double dataSampleRate;
    double outputSampleRate;
    int midiRootNote;
};

class SampleLoopCrossFader : public SampleLoop
{
public:
    float getAmplitude(int chan, double position) const override;
    void setRange(double begin, double len) override;
    void setLoopMode(LoopMode mode) override;
    void setForward(bool forward) override;
    void setCrossfadeLen(int len);

private:
    void boundCrossfadeLen();
    void updateSecondaryBegin();
    
    SampleLoop secondary;
    int crossfadeLen{0};
};
