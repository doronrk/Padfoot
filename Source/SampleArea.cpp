/*
  ==============================================================================

    SampleArea.cpp
    Created: 16 Oct 2017 9:11:07pm
    Author:  Doron Roberts-Kedes

  ==============================================================================
*/

#include "SampleArea.h"

//==============================================================================
LoopSelector::LoopSelector(SampleLoop &sl) :
    sampleLoop(sl),
    dragInProgress(false),
    dragBegin(0),
    dragCurrent(0)
{
    
}

void LoopSelector::paint(Graphics& g)
{
    Rectangle<int> bounds = getBounds();
    g.setColour(Colours::orange);
    g.drawRect(bounds);
    // draw the selected region
    if (dragInProgress) {
        Rectangle<int> selectedRect{dragBegin, 0, dragCurrent - dragBegin, getHeight()};
        g.fillRect(selectedRect);
    } else {
        std::pair<double, double> range = sampleLoop.getRange();
        double begin = range.first;
        double end = range.second;
        Rectangle<double> selectedProp{begin, 0.0, end - begin, 1.0};
        Rectangle<int> selectedRect = bounds.getProportion(selectedProp);
        g.fillRect(selectedRect);
    }
}

void LoopSelector::resized()
{
    
}

void LoopSelector::mouseDown(const MouseEvent &event)
{
    dragInProgress = true;
    dragBegin = event.x;
}

void LoopSelector::mouseDrag(const MouseEvent &event)
{
    dragCurrent = event.x;
    repaint();
}

void LoopSelector::mouseUp(const MouseEvent &event)
{
    int xDelta = event.getDistanceFromDragStartX();
    int xEnd = event.x;
    int xBegin = xEnd - xDelta;
    int width = getWidth();
    double begin = xBegin / (double) width;
    double end = xEnd / (double) width;
    sampleLoop.setRange(begin, end);
    dragInProgress = false;
    repaint();
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
