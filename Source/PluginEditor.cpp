/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Looper.h"

//==============================================================================
LoopSelector::LoopSelector(SampleLoop &sl) :
sampleLoop(sl)
{
    
}

void LoopSelector::paint(Graphics& g)
{
    Rectangle<int> bounds = getBounds();
    g.setColour(Colours::orange);
    g.drawRect(bounds);
    // draw the selected region
    std::pair<double, double> range = sampleLoop.getRange();
    double begin = range.first;
    double end = range.second;
    g.setColour(Colours::green);
    Rectangle<double> selectedProp{begin, 0.0, end - begin, 1.0};
    Rectangle<int> selectedRect = bounds.getProportion(selectedProp);
    g.fillAll(Colours::orange);
    g.fillRect(selectedRect);
}

void LoopSelector::resized()
{
    
}

void LoopSelector::mouseDown(const MouseEvent &event)
{
    
}

void LoopSelector::mouseUp(const MouseEvent &event)
{
    // TODO continue here
    event.getDistanceFromDragStartX();
}

//==============================================================================
#define THUMB_RES 512

Waveform::Waveform(SampleLoop &sl) :
sampleLoop(sl),
thumbnailCache(5),
thumbnail(THUMB_RES, formatManager, thumbnailCache)
{
    updateThumbnail();
}

void Waveform::paint (Graphics& g)
{
    g.fillAll(Colours::black);
    g.setColour(Colours::lightcyan);
    
    Rectangle<int> thumbBounds = getBounds();
    int len = thumbnail.getTotalLength();
    thumbnail.drawChannels(g, thumbBounds, 0.0, len, 1.0f);
}

void Waveform::resized()
{
    repaint();
}

void Waveform::updateThumbnail()
{
    const AudioSampleBuffer &newData = sampleLoop.data;
    int n = newData.getNumSamples();
    // Note sample rate of 1 is just used to avoid bad design choice of making thumbnail aware of sample rate
    thumbnail.reset(2, /*sr*/ 1, n);
    thumbnail.addBlock(0, newData, 0, n);
}

//==============================================================================
SampleArea::SampleArea(SampleLoop &sampleLoop) :
loopSelector(sampleLoop),
waveform(sampleLoop)
{
    addAndMakeVisible(loopSelector);
    addAndMakeVisible(waveform);
    resized();
}

void SampleArea::resized()
{
    const float selectHeight = 0.1f;
    loopSelector.setBoundsRelative(0.0, 0.0, 1.0, selectHeight);
    waveform.setBoundsRelative(0.0, selectHeight, 1.0, 1.0 - selectHeight);
}

//==============================================================================
NewProjectAudioProcessorEditor::NewProjectAudioProcessorEditor (NewProjectAudioProcessor& p)
    : AudioProcessorEditor (&p),
    processor (p),
    padfoot(p.padfoot),
    sampleArea(p.padfoot.sampleLoop)
{
    setResizable(true, true);
    setSize (700, 700);
    addAndMakeVisible(sampleArea);
    resized();
}

void NewProjectAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll(Colours::darkgrey);
}

void NewProjectAudioProcessorEditor::resized()
{
    float border = 0.01;
    sampleArea.setBoundsRelative(border, border, 1.0 - 2.0 * border, .5 - 2.0 * border);
}
