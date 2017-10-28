/*
  ==============================================================================

    ControlArea.h
    Created: 25 Oct 2017 8:40:01pm
    Author:  Doron Roberts-Kedes

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Padfoot.h"



//==============================================================================
class ControlArea : public Component, public Slider::Listener
{
public:
    ControlArea(Padfoot &padfoot);
    
    void paint (Graphics&) override;
    void resized() override;
    
    void sliderValueChanged (Slider* slider) override;
    
private:
    Padfoot &padfoot;
    SampleLoopCrossFader &sampleLoop;
    Slider s;
};
