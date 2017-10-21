/*
  ==============================================================================

    Looper.cpp
    Created: 9 Oct 2017 9:42:36pm
    Author:  Doron Roberts-Kedes

  ==============================================================================
*/

#include "SampleLoop.h"

///////////////////////////////////////
// SampleLoop
///////////////////////////////////////
SampleLoop::SampleLoop()
{}

// TODO: race condition? used during audio callback
void SampleLoop::updateData(AudioFormatReader &audioReader)
{
    begin = 0;
    end = (int) audioReader.lengthInSamples;
    data.clear();
    data.setSize(2, end);
    audioReader.read(&data, 0, end, 0, true, true);
    dataSampleRate = audioReader.sampleRate;
    midiRootNote = 74;
}

// TODO: race condition? used during audio callback
void SampleLoop::setOutputSampleRate(double rate) {
    outputSampleRate = rate;
}

bool SampleLoop::movingForwardAtPosition(int position) const
{
    int len = end - begin;
    return (position / len) % 2 == 0;
}

float SampleLoop::getLoopedSampleNum(int sampleNum) const
{
    int len = end - begin;
    int offset = sampleNum % len;
    if (!forward) {
        offset = end - offset;
    }
    int sampleWithLoopMode = begin + offset;
    if (mode == TWO_WAY) {
        bool inForwardPortion = movingForwardAtPosition(sampleNum);
        if (!inForwardPortion) {
            sampleWithLoopMode = end - offset;
        }
    }
    return sampleWithLoopMode;
}

float SampleLoop::getSampleInterpolated(int chan, double position) const
{
    int loopedSampleNum = getLoopedSampleNum((int) position);
    float val = data.getSample(chan, loopedSampleNum);
    float valNext = data.getSample(chan, loopedSampleNum + 1);
    double alpha = position - (int) position;
    double invAlpha = 1.0 - alpha;
    return val * invAlpha + valNext * alpha;
}

float SampleLoop::getXFadeGain(int position) const
{
    int beginEdge = begin + xfadeSamples;
    int endEdge = end - xfadeSamples;
    
    if (forward) {
        int xfadeBegin = end - xfadeSamples;
        if (xfadeposition > xfadeBegin) {
            double offset = xfadeposition - xfadeBegin;
            return offset / xfadeSamples;
        }
    } else {
        int xfadeBegin = begin + xfa
    }
    return 0.0;
}

// TODO: rename these getting functions
// TODO: save CPU by understanding xfadeBegin only changes when xfadeSamples changes
// TODO: save CPU by only calculating xfade stuff if xfadeSamples > 0
// TODO: decide what makes sense to store as samples, ms, seconds
    // honestly, all state inside sample loop should probably be an integer referring to a sample index or length
    // operations into sample buffer can take and return floats because outside state is floatey
float SampleLoop::getAmplitudeAtPosition(int chan, double position) const
{
    float amp = getSampleInterpolated(chan, position);
    float ampXFade = getSampleInterpolated(chan, position + xfadeSamples);
    float xFadeGain = getXFadeGain(position + xfadeSamples);
    return amp * (1.0 - xFadeGain) + ampXFade * xFadeGain;
}


double SampleLoop::deltaForNote(int midiNoteNumber) const
{
    int noteDelta = midiNoteNumber - midiRootNote;
    double sampleFactor = dataSampleRate / outputSampleRate;
    return pow (2.0, noteDelta / 12.0) * sampleFactor;
}

void SampleLoop::setRange(double beginFrac, double endFrac)
{
    jassert(beginFrac < endFrac &&
            beginFrac >= 0.0 &&
            endFrac <= 1.0);
    int numSamples = data.getNumSamples();
    begin = beginFrac * numSamples;
    end = endFrac * numSamples;
}

std::pair<double, double> SampleLoop::getRange()
{
    int numSamples = data.getNumSamples();
    double rangeBegin = begin / (double) numSamples;
    double rangeEnd = end / (double) numSamples;
    return std::pair<double, double>{rangeBegin, rangeEnd};
}

// TODO: UI
void SampleLoop::setLoopMode(LoopMode mode_)
{
    mode = mode_;
}

// TODO: UI
void SampleLoop::setForward(bool forward_)
{
    forward = forward_;
}

// TODO: UI
void SampleLoop::setXFadeMilliseconds(int ms)
{
    double samplesPerMs = dataSampleRate / 1000.0;
    xfadeSamples = ms * samplesPerMs;
}

