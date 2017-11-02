/*
  ==============================================================================

    SampleArea.cpp
    Created: 16 Oct 2017 9:11:07pm
    Author:  Doron Roberts-Kedes

  ==============================================================================
*/

#include "SampleArea.h"

//==============================================================================
LoopSelector::LoopSelector(StateTree& st) :
    stateTree(st),
    dragInProgress(false),
    dragBegin(0),
    dragCurrent(0)
{
    //state.addListener(this);
    repaint();
}

void LoopSelector::valueTreePropertyChanged (ValueTree &treeWhosePropertyHasChanged, const Identifier &property) {
/*    if (property == Identifier("begin")) {
        std::cout << "going to repaint" << std::endl;
        repaint();
    }*/
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
        int numSamples = stateTree.at("num_samples")->get();
        int begin = stateTree.at("begin")->get();
        int len = stateTree.at("len")->get();
        float beginProp = begin / (float) numSamples;
        float lenProp = len / (float) numSamples;
        
        Rectangle<double> selectedProp{beginProp, 0.0, lenProp, 1.0};
        Rectangle<int> selectedRect = bounds.getProportion(selectedProp);
        g.fillRect(selectedRect);
        /*
        int crossfadeLen = sampleLoop.getCrossfadeLen();
        int nSamples = sampleLoop.getNumSamples();
        double crossOffset = crossfadeLen / (double) nSamples;
        g.drawLine(begin * getWidth() - crossOffset * getWidth(), getHeight(), begin * getWidth(), 0, 1.0);
         */
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
    // TODO handle case of drag from right to left
    int xDelta = event.getDistanceFromDragStartX();
    int xEnd = event.x;
    int xBegin = xEnd - xDelta;
    int width = getWidth();
    double beginProp = xBegin / (double) width;
    double lenProp = xDelta / (double) width;
    
    int numSamples = stateTree.at("num_samples")->get();
    int begin = beginProp * numSamples;
    int len = lenProp * numSamples;
    stateTree["begin"]->set(begin);
    stateTree["len"]->set(len);
    dragInProgress = false;
}

//==============================================================================
#define THUMB_RES 512

Waveform::Waveform(AudioSampleBuffer &data) :
data(data),
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
    int n = data.getNumSamples();
    // Note sample rate of 1 is just used to avoid bad design choice of making thumbnail aware of sample rate
    thumbnail.reset(2, /*sr*/ 1, n);
    thumbnail.addBlock(0, data, 0, n);
}

//==============================================================================
SampleArea::SampleArea(StateTree& stateTree, AudioSampleBuffer &data) :
loopSelector(stateTree),
waveform(data)
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
