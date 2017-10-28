/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginEditor.h"

//==============================================================================
NewProjectAudioProcessorEditor::NewProjectAudioProcessorEditor (NewProjectAudioProcessor& p)
    : AudioProcessorEditor (&p),
    processor (p),
    padfoot(p.padfoot),
    sampleArea(p.padfoot.sampleLoop),
    controlArea(p.padfoot)
{
    setResizable(true, true);
    setSize (700, 700);
    addAndMakeVisible(sampleArea);
    addAndMakeVisible(controlArea);
    resized();
}

void NewProjectAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll(Colours::darkgrey);
    sampleArea.repaint();
    controlArea.repaint();
}

void NewProjectAudioProcessorEditor::resized()
{
    float border = 0.01;
    sampleArea.setBoundsRelative(border, border, 1.0 - 2.0 * border, .5 - 2.0 * border);
    controlArea.setBoundsRelative(border, .5 + border, 1.0 - 2.0 * border, .5 - 2.0 * border);
}
