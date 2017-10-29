/*
  ==============================================================================

    ControlArea.cpp
    Created: 25 Oct 2017 8:40:01pm
    Author:  Doron Roberts-Kedes

  ==============================================================================
*/

#include "ControlArea.h"


//==============================================================================
ControlArea::ControlArea(ValueTree &state) :
state(state)
{
    /*
    addAndMakeVisible (s);
    s.setRange(0, sampleLoop.getMaxCrossfadeLen(), 1);
    s.setTextValueSuffix (" samples");
    s.addListener (this);
    // TODO why isn't this the initially displayed value?
    // A: its because at this point the actual allowed range is [0, 0]
    s.setValue(0.0);
    repaint();
     */
}

void ControlArea::paint (Graphics& g) {
    /*
    s.setRange(0, sampleLoop.getMaxCrossfadeLen(), 1);
    s.setValue(sampleLoop.getCrossfadeLen());
     */
}

void ControlArea::resized()
{
    s.setBounds(getLocalBounds());
}

void ControlArea::sliderValueChanged (Slider* slider)
{
    /*
    if (slider == &s)
        padfoot.sampleLoop.setCrossfadeLen((int) s.getValue());
     */
}
