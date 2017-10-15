/*
  ==============================================================================

    SampleComponent.h
    Created: 14 Oct 2017 6:33:29pm
    Author:  Doron Roberts-Kedes

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Looper.h"

class SampleComponent : public Component
{
public:
    SampleComponent (SampleLoop &sampleLoop);
    ~SampleComponent();
    
    void paint (Graphics&) override;
    void resized() override;
    
    void resetThumbnail();
    
private:
    SampleLoop &sampleLoop;

    AudioFormatManager formatManager;
    AudioThumbnailCache thumbnailCache;                  // [1]
    AudioThumbnail thumbnail;
    
};
