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

SampleLoop::SampleLoop(const AudioSampleBuffer &data_)
:data(data_) {}

// TODO: race condition? used during audio callback
void SampleLoop::reset()
{
    begin = 0;
    len = data.getNumSamples();
    // TODO: create a bound function for begin & len kind of like the crossfade one
    // this can be called here and re-used when setRange is called.
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

float SampleLoop::getAmplitude(int chan, double position) const
{
    return getAmplitudeForPosition(chan, position);
}

void SampleLoop::setRange(double beginFrac, double lenFrac)
{
    jassert(beginFrac < 1.0 &&
            lenFrac <= 1.0 &&
            beginFrac >= 0.0 &&
            lenFrac >= 0.0);
    int numSamples = data.getNumSamples();
    begin = beginFrac * numSamples;
    len = lenFrac * numSamples;
    if (len == 0) {
        len = 1;
    }
    
    int maxLen = numSamples - begin;
    len = jmin(len, maxLen);
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

void SampleLoop::setBegin(int newbegin)
{
    begin = newbegin;
}


///////////////////////////////////////
// SampleLoopCrossFader
///////////////////////////////////////
SampleLoopCrossFader::SampleLoopCrossFader(const AudioSampleBuffer &data)
: SampleLoop(data), secondary(data)
{
    
}

float SampleLoopCrossFader::getAmplitude(int chan, double position) const
{
    // TODO: support TWO_WAY crossfading
    float primaryAmp = SampleLoop::getAmplitude(chan, position);
    if (crossfadeLen == 0 || mode == TWO_WAY) {
        return primaryAmp;
    }
    int offset = ((int) position) % len;
    int crossfadeProgress = len - crossfadeLen + offset;
    if (crossfadeProgress < 0) {
        return primaryAmp;
    }
    float secondaryAmp = secondary.getAmplitude(chan, position + crossfadeLen);
    float alpha = crossfadeProgress / (float) crossfadeLen;
    return interpolate(primaryAmp, secondaryAmp, alpha);
}

void SampleLoopCrossFader::setRange(double begin, double len)
{
    SampleLoop::setRange(begin, len);
    boundCrossfadeLen();
}

void SampleLoopCrossFader::setLoopMode(LoopMode mode)
{
    secondary.setLoopMode(mode);
    SampleLoop::setLoopMode(mode);
}

void SampleLoopCrossFader::setForward(bool forward)
{
    secondary.setForward(forward);
    SampleLoop::setForward(forward);
    boundCrossfadeLen();
}

// TODO: UI
void SampleLoopCrossFader::setCrossfadeLen(int len)
{
    crossfadeLen = len;
    boundCrossfadeLen();
}

void SampleLoopCrossFader::boundCrossfadeLen()
{
    // crossfadeLen cannot exceed length of the range
    crossfadeLen = jmin(crossfadeLen, len);
    if (forward) {
        crossfadeLen = jmin(crossfadeLen, begin);
    } else {
        int remaining = data.getNumSamples() - (begin + len) - 1;
        crossfadeLen = jmin(crossfadeLen, remaining);
    }
    updateSecondaryBegin();
}

void SampleLoopCrossFader::updateSecondaryBegin()
{
    int offset = forward ? -crossfadeLen : crossfadeLen;
    secondary.setBegin(begin + offset);
}
