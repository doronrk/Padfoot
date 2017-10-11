/*
  ==============================================================================

    Looper.cpp
    Created: 9 Oct 2017 9:42:36pm
    Author:  Doron Roberts-Kedes

  ==============================================================================
*/

#include "Looper.h"

#define LEFT 0
#define RIGHT 1


///////////////////////////////////////
// SampleLoop
///////////////////////////////////////
SampleLoop::SampleLoop(const AudioSampleBuffer &data_,
                       double dataSampleRate_,
                       double outSampleRate_,
                       int root) :
    data(data_),
    dataSampleRate(dataSampleRate_),
    outSampleRate(outSampleRate_),
    midiRootNote(root),
    begin(0),
    end(data.getNumSamples()) {}

void SampleLoop::setOutSampleRate(double rate) {
    outSampleRate = rate;
}

float SampleLoop::getSample(int chan, int sampleNum) const
{
    int wrapped = sampleNum % data.getNumSamples();
    return data.getSample(chan, wrapped);
}

float SampleLoop::getSampleInterp(int chan, double position) const
{
    int sampleNum = (int) position;
    float val = getSample(chan, sampleNum);
    float valNext = getSample(chan, sampleNum + 1);
    double alpha = position - sampleNum;
    double invAlpha = 1.0 - alpha;
    return val * invAlpha + valNext * alpha; //TODO double check this
}

double SampleLoop::deltaForNote(int midiNoteNumber) const
{
    int noteDelta = midiNoteNumber - midiRootNote;
    double sampleFactor = dataSampleRate / outSampleRate;
    return pow (2.0, noteDelta / 12.0) * sampleFactor;
}

///////////////////////////////////////
// PadfootVoice
///////////////////////////////////////
PadfootVoice::PadfootVoice(const SampleLoop &sampleLoop_) :
    position(0.0),
    midiNoteNumber(0),
    sampleLoop(sampleLoop_)
{
    
}

void PadfootVoice::setPosition(double pos) {
    position = pos;
}

void PadfootVoice::setMidiNoteNumber(int num) {
    midiNoteNumber = num;
}

void PadfootVoice::reset() {
    setPosition(0.0);
}

void PadfootVoice::renderNextBlock(AudioSampleBuffer & output, int startSample, int numSamples)
{
    float *outL = output.getWritePointer(LEFT, startSample);
    float *outR = output.getWritePointer(RIGHT, startSample);
    
    while (--numSamples >= 0) {
        *outL++ = sampleLoop.getSampleInterp(LEFT, position);
        *outR++ = sampleLoop.getSampleInterp(RIGHT, position);
        position += sampleLoop.deltaForNote(midiNoteNumber);
    }
}

///////////////////////////////////////
// PadfootNote
///////////////////////////////////////
PadfootNote::PadfootNote (const SampleLoop &sampleLoop) :
    voice(sampleLoop) {}

void PadfootNote::startNote (int midiNoteNumber, float velocity)
{
    voice.setMidiNoteNumber(midiNoteNumber);
    voice.setPosition(0.0);
    on = true;
}

void PadfootNote::stopNote (float velocity)
{
    voice.reset();
    on = false;
}

void PadfootNote::renderNextBlock (AudioSampleBuffer&
   outputBuffer, int startSample, int numSamples)
{
    if (on) {
        voice.renderNextBlock(outputBuffer, startSample, numSamples);
    }
}
