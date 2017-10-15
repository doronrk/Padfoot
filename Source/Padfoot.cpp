/*
  ==============================================================================

    Padfoot.cpp
    Created: 6 Oct 2017 7:06:41pm
    Author:  Doron Roberts-Kedes

  ==============================================================================
*/

#include "Padfoot.h"

Padfoot::Padfoot()
{
    WavAudioFormat wavFormat;
    File f("~/Desktop/padfoot.wav");
    FileInputStream fstream(f);
    ScopedPointer<AudioFormatReader> afr(wavFormat.createReaderFor(&fstream, true));
    sampleLoop.updateData(*afr);

    /* TODO: pre-allocate collection of padfoot notes.
     each padfoot note pre-allocates some number of voices
     (1 for now)
     
     there is a collection of in-use notes, and free notes.
     
     on note-on,
         grab and move a note from free to in-use
         set its note number, velocity
     
     on note-off
         reset the note
         move from in-use to free
     
     each process block, call render on every in-use note
     */
    note = new PadfootNote(sampleLoop);
}

Padfoot::~Padfoot() {}

void Padfoot::renderNotes (AudioSampleBuffer &buffer, int startSample, int numSamples)
{
    note->renderNextBlock(buffer, startSample, numSamples);
}

void Padfoot::handleMidiEvent (const MidiMessage &m)
{
    if (m.isNoteOn()) {
        int noteNum = m.getNoteNumber();
        float vel = m.getFloatVelocity();
        note->startNote(noteNum, vel);
    } else if (m.isNoteOff()) {
        float vel = m.getFloatVelocity();
        note->stopNote(vel);
    }
}

void Padfoot::processBlock (AudioSampleBuffer& outputAudio, MidiBuffer& midiData)
{
    int startSample = 0;
    int numSamples = outputAudio.getNumSamples();
    MidiBuffer::Iterator midiIterator (midiData);
    midiIterator.setNextSamplePosition (startSample);
    
    bool firstEvent = true;
    int midiEventPos;
    MidiMessage m;
    
    // TODO Wtf?
    bool subBlockSubdivisionIsStrict = false;
    int minimumSubBlockSize = 32;
    
    while (numSamples > 0)
    {
        if (! midiIterator.getNextEvent (m, midiEventPos))
        {
            renderNotes (outputAudio, startSample, numSamples);
            return;
        }
        
        const int samplesToNextMidiMessage = midiEventPos - startSample;
        
        if (samplesToNextMidiMessage >= numSamples)
        {
            renderNotes (outputAudio, startSample, numSamples);
            handleMidiEvent (m);
            break;
        }
        
        if (samplesToNextMidiMessage < ((firstEvent && ! subBlockSubdivisionIsStrict) ? 1 : minimumSubBlockSize))
        {
            handleMidiEvent (m);
            continue;
        }
        
        firstEvent = false;
        
        renderNotes (outputAudio, startSample, samplesToNextMidiMessage);
        
        handleMidiEvent (m);
        startSample += samplesToNextMidiMessage;
        numSamples  -= samplesToNextMidiMessage;
    }
    
    while (midiIterator.getNextEvent (m, midiEventPos))
        handleMidiEvent (m);
}



void Padfoot::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    sampleLoop.setOutSampleRate(sampleRate);
}

