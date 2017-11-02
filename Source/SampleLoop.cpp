/*
  ==============================================================================

    Looper.cpp
    Created: 9 Oct 2017 9:42:36pm
    Author:  Doron Roberts-Kedes

  ==============================================================================
*/

#include "SampleLoop.h"
#include "State.h"

///////////////////////////////////////
// SampleLoop
///////////////////////////////////////

SampleLoop::SampleLoop(const AudioSampleBuffer &data_)
:data(data_){
    State::Validator beginValid = [this](int v) {return this->validateBegin(v);};
    begin = std::make_shared<State>(beginValid);
    stateTree["begin"] = begin;

    State::Validator lenValid = [this](int v) {return this->validateLen(v);};
    len = std::make_shared<State>(lenValid);
    stateTree["len"] = len;

    numSamples = std::make_shared<State>();
    stateTree["num_samples"] = numSamples;
}

bool SampleLoop::validateBegin(int value) const {
    std::cout << "validateBegin called" << std::endl;
    return value >= 0 && value < data.getNumSamples();
}

bool SampleLoop::validateLen(int value) const {
    std::cout << "validateLen called" << std::endl;
    int begin = stateTree.at("begin")->get();
    return value > 0 && value <= data.getNumSamples() - begin;
}

int SampleLoop::getBegin() const {
    return begin->get();
}
int SampleLoop::getLen() const {
    return len->get();
}
bool SampleLoop::getForward() const {
    return forward;
}
bool SampleLoop::getOneWay() const {
    return oneWay;
}

int SampleLoop::applyDirectionToOffset(int offset) const
{
    int len = getLen();
    jassert(offset < len);
    if (getForward()) {
        return offset;
    }
    return len - offset;
}

int SampleLoop::applySustainModeToDoubleLenOffset(int doubleLenOffset) const
{
    int len = getLen();
    jassert(doubleLenOffset < len * 2);
    int singleLenOffset = doubleLenOffset % len;
    if (getOneWay()) {
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

///////////////////////////////////////
// SampleLoopCrossFader
///////////////////////////////////////
SampleLoopCrossFader::SampleLoopCrossFader(const AudioSampleBuffer &data)
: SampleLoop(data), secondary(data)
{
    crossfadeLen = std::make_shared<State>();
    stateTree["crossfade_len"] = crossfadeLen;
}

float SampleLoopCrossFader::getAmplitude(int chan, double position) const
{
    int cl = crossfadeLen->get();
    // TODO: support TWO_WAY crossfading
    float primaryAmp = SampleLoop::getAmplitude(chan, position);
    if (crossfadeLen == 0 || !getOneWay()) {
        return primaryAmp;
    }
    int len = getLen();
    int offset = ((int) position) % len;
    int crossfadeProgress = offset - (len - cl);
    if (crossfadeProgress < 0) {
        return primaryAmp;
    }
    float secondaryAmp = secondary.getAmplitude(chan, position + cl);
    float alpha = crossfadeProgress / (float) cl;
    return interpolate(primaryAmp, secondaryAmp, alpha);
}
/*
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
