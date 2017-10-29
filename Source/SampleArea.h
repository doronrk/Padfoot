/*
  ==============================================================================

    SampleArea.h
    Created: 16 Oct 2017 9:11:07pm
    Author:  Doron Roberts-Kedes

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "SampleLoop.h"

//==============================================================================
class LoopSelector : public Component, private Value::Listener
{
public:
    LoopSelector(ValueTree& state);
    
    void paint (Graphics&) override;
    void resized() override;
    
    void mouseDown(const MouseEvent &event) override;
    void mouseUp(const MouseEvent &event) override;
    void mouseDrag(const MouseEvent &event) override;
    
private:
    void valueChanged(Value& value) override;
    
    ValueTree& state;
    bool dragInProgress;
    int dragBegin;
    int dragCurrent;
};

//==============================================================================
class Waveform : public Component
{
public:
    Waveform(AudioSampleBuffer &data);
    
    void paint (Graphics&) override;
    void resized() override;
    
    void updateThumbnail();
private:
    AudioSampleBuffer &data;
    
    AudioFormatManager formatManager;
    AudioThumbnailCache thumbnailCache;
    AudioThumbnail thumbnail;
};

//==============================================================================
class SampleArea : public Component
{
public:
    SampleArea(ValueTree& state, AudioSampleBuffer &data);
    
    void resized() override;
private:
    LoopSelector loopSelector;
    Waveform waveform;
};
