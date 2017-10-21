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
    len = (int) audioReader.lengthInSamples;
    data.clear();
    data.setSize(2, len);
    audioReader.read(&data, 0, len, 0, true, true);
    dataSampleRate = audioReader.sampleRate;
    midiRootNote = 74;
}

// TODO: race condition? used during audio callback
void SampleLoop::setOutputSampleRate(double rate) {
    outputSampleRate = rate;
}

bool SampleLoop::movingForwardAtPosition(int position) const
{
    return (position / len) % 2 == 0;
}

int SampleLoop::applyDirectionToOffset(int offset) const
{
    jassert(offset < len);
    if (forward) {
        return offset;
    }
    return len - offset;
}

int SampleLoop::applySustainModeToDoubleLenOffset(int doubleLenOffset) const
{
    jassert(doubleLenOffset < len * 2);
    int singleLenOffset = doubleLenOffset % len;
    if (mode == ONE_WAY) {
        return singleLenOffset;
    }
    // mode == TWO_WAY
    if (doubleLenOffset < len) {
        return singleLenOffset;
    }
    return len - singleLenOffset - 1;
}

int SampleLoop::getIndexForPosition(int position) const
{
    int doubleLenOffset = position % (len * 2);
    int sustainModeOffset = applySustainModeToDoubleLenOffset(doubleLenOffset);
    int directionOffest = applyDirectionToOffset(sustainModeOffset);
    return begin + directionOffest;
}

float SampleLoop::getAmplitudeForPosition(int chan, int position) const
{
    int index = getIndexForPosition(position);
    return data.getSample(chan, index);
}

static inline float interpolate(float a, float b, double alpha)
{
    return a * (1.0 - alpha) + b * (alpha);
}

float SampleLoop::getAmplitudeForPosition(int chan, double position) const
{
    int positionFloor = (int) position;
    double alpha = position - positionFloor;
    float floorAmp = getAmplitudeForPosition(chan, positionFloor);
    float ceilingAmp = getAmplitudeForPosition(chan, positionFloor + 1);
    return interpolate(floorAmp, ceilingAmp, alpha);
}

double SampleLoop::deltaForNote(int midiNoteNumber) const
{
    int noteDelta = midiNoteNumber - midiRootNote;
    double sampleFactor = dataSampleRate / outputSampleRate;
    return pow (2.0, noteDelta / 12.0) * sampleFactor;
}

void SampleLoop::setRange(double beginFrac, double lenFrac)
{
    jassert(beginFrac < lenFrac &&
            beginFrac >= 0.0 &&
            lenFrac <= 1.0);
    int numSamples = data.getNumSamples();
    begin = beginFrac * numSamples;
    len = lenFrac * numSamples;
}

std::pair<double, double> SampleLoop::getRange()
{
    int numSamples = data.getNumSamples();
    double rangeBegin = begin / (double) numSamples;
    double rangeLen = len / (double) numSamples;
    return std::pair<double, double>{rangeBegin, rangeLen};
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

