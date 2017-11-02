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
class LoopSelector : public Component
{
public:
    LoopSelector(StateTree& stateTree);
    
    void paint (Graphics&) override;
    void resized() override;
    
    void mouseDown(const MouseEvent &event) override;
    void mouseUp(const MouseEvent &event) override;
    void mouseDrag(const MouseEvent &event) override;
    
    void crossfadeCallback(const State& s);
    
private:

    StateTree& stateTree;
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
    SampleArea(StateTree& stateTree, AudioSampleBuffer &data);
    
    void resized() override;
private:
    LoopSelector loopSelector;
    Waveform waveform;
};
