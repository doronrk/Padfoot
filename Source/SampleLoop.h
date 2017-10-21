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
    /* Applies range, sustain mode, interpolation, and crossfade. */
    float getAmplitudeForPosition(int chan, double position) const;
    double deltaForNote(int midiNoteNumber) const;
    
    void setRange(double begin, double len);
    std::pair<double, double> getRange();
    
    void setLoopMode(LoopMode mode);
    void setForward(bool forward);
    void setCrossfadeFactor(double factor);
    
    // TODO figure out elegant way to expose this to SampleComponent class w/o making it public
    // and without this class having to know about the concept of a Thumbnail
    AudioSampleBuffer data;
private:
    inline bool movingForwardAtPosition(int position) const;
    inline int applyDirectionToOffset(int offset) const;
    inline int applySustainModeToDoubleLenOffset(int doubleLenOffset) const;
    inline int getIndexForPosition(int position) const;
    inline float getAmplitudeForPosition(int chan, int position) const;
    
    LoopMode mode{TWO_WAY};
    bool forward{true};

    double dataSampleRate;
    double outputSampleRate;
    int midiRootNote;
    int begin;
    int len;
};
