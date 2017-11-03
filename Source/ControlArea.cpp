/*
  ==============================================================================

    ControlArea.cpp
    Created: 25 Oct 2017 8:40:01pm
    Author:  Doron Roberts-Kedes

  ==============================================================================
*/

#include "ControlArea.h"


//==============================================================================
ControlArea::ControlArea(StateTree &st) :
stateTree(st)
{
    addAndMakeVisible (s);
    // TODO update to use proper key once lambdas supported
    int maxCrossfadeLen = *stateTree.at("num_samples");
    s.setRange(0, maxCrossfadeLen, 1);
    s.setTextValueSuffix (" samples");
    s.addListener (this);
    // TODO why isn't this the initially displayed value?
    // A: its because at this point the actual allowed range is [0, 0]
    s.setValue(0.0);
    repaint();
}

void ControlArea::paint (Graphics& g) {
    int maxCrossfadeLen = *stateTree.at("num_samples");
    s.setRange(0, maxCrossfadeLen, 1);
    int crossfadeLen = *stateTree.at("crossfade_len");
    s.setValue(crossfadeLen);
}

void ControlArea::resized()
{
    s.setBounds(getLocalBounds());
}

void ControlArea::sliderValueChanged (Slider* slider)
{
    if (slider == &s) {
        *stateTree.at("crossfade_len") = (int) s.getValue();
    }
}
