/*
  ==============================================================================

    Looper.h
    Created: 9 Oct 2017 9:42:36pm
    Author:  Doron Roberts-Kedes

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class SampleLoop
{
public:
    enum LoopMode {
        ONE_WAY,
        TWO_WAY
    };
    SampleLoop();
    void updateData(AudioFormatReader &);
    void setOutputSampleRate(double rate);
    float getSampleInterpolated(int chan, double position) const;
    float getSampleFinal(int chan, double position) const;
    double deltaForNote(int midiNoteNumber) const;
    
    void setRange(double begin, double end);
    std::pair<double, double> getRange();
    
    void setLoopMode(LoopMode mode);
    void setForward(bool forward);
    void setXFadeMilliseconds(int ms);
    
    // TODO figure out elegant way to expose this to SampleComponent class w/o making it public
    // and without this class having to know about the concept of a Thumbnail
    AudioSampleBuffer data;
private:
    float getSample(int chan, int sampleNum) const;
    
    LoopMode mode{TWO_WAY};
    bool forward{true};
    double xfadeSamples{0.0};
    
    double dataSampleRate;
    double outputSampleRate;
    int midiRootNote;
    int begin; // inclusive
    int end;   // exclusive
};
