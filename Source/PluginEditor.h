/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"


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


//==============================================================================
class NewProjectAudioProcessorEditor  : public AudioProcessorEditor
{
public:
    NewProjectAudioProcessorEditor (NewProjectAudioProcessor&);

    void paint (Graphics&) override;
    void resized() override;

private:
    NewProjectAudioProcessor& processor;
    Padfoot& padfoot;
    
    SampleArea sampleArea;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NewProjectAudioProcessorEditor)
};
