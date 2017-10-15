/*
  ==============================================================================

    Padfoot.h
    Created: 6 Oct 2017 7:06:41pm
    Author:  Doron Roberts-Kedes

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Looper.h"

//==============================================================================
/**
 */
class Padfoot
{
public:
    Padfoot();
    ~Padfoot();
    
    void processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages);
    void prepareToPlay (double sampleRate, int samplesPerBlock);
    
    SampleLoop sampleLoop;
    PadfootNote *note;

private:
    void handleMidiEvent(const MidiMessage &m);
    void renderNotes(AudioSampleBuffer &buffer, int startSample, int numSamples);
};
