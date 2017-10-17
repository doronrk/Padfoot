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
    return data.getSample(chan, begin + offset);
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
