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
:data(data_), state("sample_loop_state") {
    numSamplesVar = [this](const var::NativeFunctionArgs&) {
        return data.getNumSamples();
    };
    state.setProperty(Identifier("begin"), beginVar, nullptr);
    state.setProperty(Identifier("len"), lenVar, nullptr);
    state.setProperty(Identifier("forward"), forwardVar, nullptr);
    state.setProperty(Identifier("oneway"), oneWayVar, nullptr);
    state.setProperty(Identifier("num_samples"), numSamplesVar, nullptr);
}

int SampleLoop::getBegin() const {
    return (int) beginVar;
}
int SampleLoop::getLen() const {
    return (int) lenVar;
}
bool SampleLoop::getForward() const {
    return (bool) forwardVar;
}
bool SampleLoop::getOneWay() const {
    return (bool) oneWayVar;
}

int SampleLoop::applyDirectionToOffset(int offset) const
{
    int len = getLen();
    jassert(offset < len);
    if (forwardVar) {
        return offset;
    }
    return len - offset;
}

int SampleLoop::applySustainModeToDoubleLenOffset(int doubleLenOffset) const
{
    int len = getLen();
    jassert(doubleLenOffset < len * 2);
    int singleLenOffset = doubleLenOffset % len;
    if (oneWayVar) {
        return singleLenOffset;
    }
    if (doubleLenOffset < len) {
        return singleLenOffset;
    }
    return len - singleLenOffset - 1;
}

int SampleLoop::getIndexForPosition(int position) const
{
    int doubleLenOffset = position % (getLen() * 2);
    int sustainModeOffset = applySustainModeToDoubleLenOffset(doubleLenOffset);
    int directionOffest = applyDirectionToOffset(sustainModeOffset);
    return getBegin() + directionOffest;
}

float SampleLoop::getAmplitudeForPosition(int chan, int position) const
{
    int index = getIndexForPosition(position);
    jassert(index >= 0 && index < data.getNumSamples());
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

/*
void SampleLoop::setRange(double beginFrac, double lenFrac)
{
    // TODO lock audio callback
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

void SampleLoop::setLen(int newLen)
{
    len = newLen;
}
 */

int SampleLoop::getNumSamples() const {
    return data.getNumSamples();
}


///////////////////////////////////////
// SampleLoopCrossFader
///////////////////////////////////////
SampleLoopCrossFader::SampleLoopCrossFader(const AudioSampleBuffer &data)
: SampleLoop(data), secondary(data)
{
    state.getPropertyAsValue("begin", nullptr).addListener(this);
}

void SampleLoopCrossFader::valueChanged(juce::Value &value) {
    
}

float SampleLoopCrossFader::getAmplitude(int chan, double position) const
{
    // TODO: support TWO_WAY crossfading
    float primaryAmp = SampleLoop::getAmplitude(chan, position);
    if (crossfadeLen == 0 || !oneWayVar) {
        return primaryAmp;
    }
    int len = getLen();
    int offset = ((int) position) % len;
    int crossfadeProgress = offset - (len - crossfadeLen);
    if (crossfadeProgress < 0) {
        return primaryAmp;
    }
    float secondaryAmp = secondary.getAmplitude(chan, position + crossfadeLen);
    float alpha = crossfadeProgress / (float) crossfadeLen;
    return interpolate(primaryAmp, secondaryAmp, alpha);
}

/*
void SampleLoopCrossFader::setRange(double beginFrac, double lenFrac)
{
    // TODO this should be cleaner
    SampleLoop::setRange(beginFrac, lenFrac);
    boundCrossfadeLen();
    redrawEditor();
}

void SampleLoopCrossFader::setLoopMode(LoopMode mode)
{
    secondary.setLoopMode(mode);
    SampleLoop::setLoopMode(mode);
    redrawEditor();
}

void SampleLoopCrossFader::setForward(bool forward)
{
    secondary.setForward(forward);
    SampleLoop::setForward(forward);
    boundCrossfadeLen();
    redrawEditor();
}

// TODO: UI
void SampleLoopCrossFader::setCrossfadeLen(int fadelen)
{
    crossfadeLen = fadelen;
    boundCrossfadeLen();
    redrawEditor();
}
 */
/*
void SampleLoopCrossFader::boundCrossfadeLen()
{
    int maxLen = getMaxCrossfadeLen();
    crossfadeLen = jmin(crossfadeLen, maxLen);
    updateSecondary();
}

void SampleLoopCrossFader::updateSecondary()
{
    int offset = getForward() ? -crossfadeLen : crossfadeLen;
    secondary.setBegin(getBegin() + offset);
    secondary.setLen(getLen());
}

int SampleLoopCrossFader::getMaxCrossfadeLen() const {
    int begin = getBegin();
    int len = getLen();
    if (getForward()) {
        return jmin(len, begin);
    } else {
        return jmin(len, getNumSamples() - (begin + len) - 1);
    }
}

*/
