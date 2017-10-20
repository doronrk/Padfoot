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


void SampleLoop::setOutputSampleRate(double rate) {
    outputSampleRate = rate;
}

float SampleLoop::getSample(int chan, int sampleNum) const
{
    // TODO: are begin + end really the right fields? should it be begin and length?
    int len = end - begin;
    int offset = sampleNum % len;
    if (!forward) {
        offset = end - offset;
    }
    int sampleWithLoopMode = begin + offset;
    if (mode == TWO_WAY) {
        bool inForwardPortion = (sampleNum / len) % 2 == 0;
        if (!inForwardPortion) {
            sampleWithLoopMode = end - offset;
        }
    }
    return data.getSample(chan, sampleWithLoopMode);
}

float SampleLoop::getSampleInterpolated(int chan, double position) const
{
    int sampleNum = (int) position;
    float val = getSample(chan, sampleNum);
    float valNext = getSample(chan, sampleNum + 1);
    double alpha = position - sampleNum;
    double invAlpha = 1.0 - alpha;
    return val * invAlpha + valNext * alpha;
}

// TODO: rename these getting functions
// TODO: save CPU by understanding xfadeBegin only changes when xfadeSamples changes
// TODO: save CPU by only calculating xfade stuff if xfadeSamples > 0
float SampleLoop::getSampleFinal(int chan, double position) const
{
    int output = getSampleInterpolated(chan, position);
    /*
    int xfadeoutput = getSampleInterpolated(chan, position + xfadeSamples);
    float xfadegain = 0.0;
    if (xfadeSamples > 0) {
        if (forward) {
            int xfadeBegin = end - xfadeSamples;
            if (position > xfadeBegin)
        }
        int factor = forward ? 1 : -1;
        position + factor * xfadeSamples;
    }
    return output * (1.0 - xfadegain) + xfadeoutput * xfadegain;
     */
    return output;
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

