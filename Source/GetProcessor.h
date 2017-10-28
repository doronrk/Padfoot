/*
  ==============================================================================

    GetProcessor.h
    Created: 28 Oct 2017 2:22:06pm
    Author:  Doron Roberts-Kedes

  ==============================================================================
*/

#pragma once

#include "PluginEditor.h"
#include "../JuceLibraryCode/JuceHeader.h"


static std::shared_ptr<NewProjectAudioProcessor> processor;


static void *redrawEditor() {
    if (processor != nullptr) {
        AudioProcessorEditor *editor = processor->getActiveEditor();
        if (editor != nullptr) {
            editor->repaint();
        }
    }
}


