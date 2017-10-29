/*
  ==============================================================================

    Padfoot.h
    Created: 6 Oct 2017 7:06:41pm
    Author:  Doron Roberts-Kedes

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "SampleLoop.h"

#include <unordered_map>

//==============================================================================
struct VoicePlaybackInfo
{
    double deltaForNote(int midiNoteNumber) const
    {
        int noteDelta = midiNoteNumber - dataMidiRootNote;
        double sampleFactor = dataSampleRate / outputSampleRate;
        return pow (2.0, noteDelta / 12.0) * sampleFactor;
    }
    
    double dataSampleRate;
    double outputSampleRate;
    int dataMidiRootNote;
};

//==============================================================================
class PadfootVoice
{
public:
    PadfootVoice(const SampleLoop &, const VoicePlaybackInfo &vpi);
    void renderNextBlock (AudioSampleBuffer &, int startSample, int numSamples);
    
    void setPosition(double pos);
    void setMidiNoteNumber(int num);
    
    void reset();
    
private:
    
    double position;
    int midiNoteNumber;
    const SampleLoop &sampleLoop;
    const VoicePlaybackInfo &playbackInfo;
};

//==============================================================================
class PadfootNote
{
public:
    PadfootNote(const SampleLoop &sampleLoop, const VoicePlaybackInfo &vpi);
    
    void startNote (int midiNoteNumber, float velocity);
    void stopNote (float velocity);
    
    void renderNextBlock (AudioSampleBuffer&, int startSample, int numSamples);
private:
    PadfootVoice voice;
    bool on;
};

//==============================================================================
class Padfoot
{
public:
    Padfoot();
    ~Padfoot();
    
    void processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages);
    void prepareToPlay (double sampleRate, int samplesPerBlock);
        
    SampleLoopCrossFader sampleLoop;
    AudioSampleBuffer data;
    
private:
    
    std::unique_ptr<PadfootNote> getInactiveNote();

    void handleMidiEvent(const MidiMessage &m);
    void renderNotes(AudioSampleBuffer &buffer, int startSample, int numSamples);
    
    std::vector<std::unique_ptr<PadfootNote>> inactiveNotes;
    std::unordered_map<int, std::unique_ptr<PadfootNote>> activeNotes;

    VoicePlaybackInfo voicePlaybackInfo;
};
