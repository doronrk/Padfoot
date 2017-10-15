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
    SampleLoop();
    void updateData(AudioFormatReader &);
    void setOutSampleRate(double rate);
    float getSampleInterp(int chan, double position) const;
    double deltaForNote(int midiNoteNumber) const;
    void setLoop(double begin, double end);
    
    AudioSampleBuffer* data;

private:
    float getSample(int chan, int sampleNum) const;
    
    double dataSampleRate;
    double outSampleRate;
    int midiRootNote;
    int begin; // inclusive
    int end;   // exclusive
};

class PadfootVoice
{
public:
    PadfootVoice(const SampleLoop &);
    void renderNextBlock (AudioSampleBuffer &, int startSample, int numSamples);
    
    void setPosition(double pos);
    void setMidiNoteNumber(int num);
    
    void reset();
    
private:
    void advancePosition(int &, int &);
    
    double position;
    int midiNoteNumber;
    const SampleLoop &sampleLoop;
};

/*
 To begin with, each PadfootNote should have one PadfootVoice
 */
class PadfootNote
{
public:
    PadfootNote(const SampleLoop &sampleLoop);

    void startNote (int midiNoteNumber, float velocity);
    void stopNote (float velocity);

    void renderNextBlock (AudioSampleBuffer&, int startSample, int numSamples);
private:
    PadfootVoice voice;
    bool on;
};
