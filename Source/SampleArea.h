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
    LoopSelector(SampleLoop &sampleLoop);
    
    void paint (Graphics&) override;
    void resized() override;
    
    void mouseDown(const MouseEvent &event) override;
    void mouseUp(const MouseEvent &event) override;
    void mouseDrag(const MouseEvent &event) override;
    
private:
    SampleLoop &sampleLoop;
    bool dragInProgress;
    int dragBegin;
    int dragCurrent;
};

//==============================================================================
class Waveform : public Component
{
public:
    Waveform(SampleLoop &sampleLoop);
    
    void paint (Graphics&) override;
    void resized() override;
    
    void updateThumbnail();
private:
    SampleLoop &sampleLoop;
    
    AudioFormatManager formatManager;
    AudioThumbnailCache thumbnailCache;                  // [1]
    AudioThumbnail thumbnail;
};

//==============================================================================
class SampleArea : public Component
{
public:
    SampleArea(SampleLoop &sampleLoop);
    
    void resized() override;
private:
    LoopSelector loopSelector;
    Waveform waveform;
};
