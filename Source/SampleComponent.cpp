/*
  ==============================================================================

    SampleComponent.cpp
    Created: 14 Oct 2017 6:33:29pm
    Author:  Doron Roberts-Kedes

  ==============================================================================
*/

#include "SampleComponent.h"


SampleComponent::SampleComponent (SampleLoop &sl)
: sampleLoop(sl),
thumbnailCache(5),
thumbnail(512, formatManager, thumbnailCache)
{
    resetThumbnail();
}

SampleComponent::~SampleComponent()
{
    
}

void SampleComponent::paint (Graphics& g)
{
    // TODO: draw if no source selected
    thumbnail.drawChannels (g,                                      // [9]
                            getBounds(),
                            0.0,                                    // start time
                            thumbnail.getTotalLength(),             // end time
                            1.0f);
}

void SampleComponent::resized()
{
    
}

void SampleComponent::resetThumbnail()
{
    const AudioSampleBuffer *newData = sampleLoop.data;
    int n = newData->getNumSamples();
    thumbnail.reset(1, 512, n);
    thumbnail.addBlock(0, *newData, 0, n);
}


