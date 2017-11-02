/*
  ==============================================================================

    Padfoot.cpp
    Created: 6 Oct 2017 7:06:41pm
    Author:  Doron Roberts-Kedes

  ==============================================================================
*/

#include "Padfoot.h"

#define LEFT 0
#define RIGHT 1

///////////////////////////////////////
// PadfootVoice
///////////////////////////////////////
PadfootVoice::PadfootVoice(const SampleLoop &sampleLoop_, const VoicePlaybackInfo &vpi) :
position(0.0),
midiNoteNumber(0),
sampleLoop(sampleLoop_),
playbackInfo(vpi)
{
    
}

void PadfootVoice::setPosition(double pos) {
    position = pos;
}

void PadfootVoice::setMidiNoteNumber(int num) {
    midiNoteNumber = num;
}

void PadfootVoice::reset() {
    setPosition(0.0);
}

void PadfootVoice::renderNextBlock(AudioSampleBuffer & output, int startSample, int numSamples)
{
    float *outL = output.getWritePointer(LEFT, startSample);
    float *outR = output.getWritePointer(RIGHT, startSample);
    
    while (--numSamples >= 0) {
        *outL++ += sampleLoop.getAmplitude(LEFT, position);
        *outR++ += sampleLoop.getAmplitude(RIGHT, position);
        position += playbackInfo.deltaForNote(midiNoteNumber);
    }
}

///////////////////////////////////////
// PadfootNote
///////////////////////////////////////
PadfootNote::PadfootNote (const SampleLoop &sampleLoop, const VoicePlaybackInfo &vpi) :
voice(sampleLoop, vpi) {}

void PadfootNote::startNote (int midiNoteNumber, float velocity)
{
    voice.setMidiNoteNumber(midiNoteNumber);
    voice.setPosition(0.0);
    on = true;
}

void PadfootNote::stopNote (float velocity)
{
    voice.reset();
    on = false;
}

void PadfootNote::renderNextBlock (AudioSampleBuffer&
                                   outputBuffer, int startSample, int numSamples)
{
    if (on) {
        voice.renderNextBlock(outputBuffer, startSample, numSamples);
    }
}

Padfoot::Padfoot()
: sampleLoop(data)
{
    // TODO move this drag and drop logic into Sample Component
    WavAudioFormat wavFormat;
    File f("~/Desktop/padfoot.wav");
    FileInputStream *fstream = new FileInputStream(f);
    ScopedPointer<AudioFormatReader> afr(wavFormat.createReaderFor(fstream, true));
    int len = (int) afr->lengthInSamples;
    data.clear();
    data.setSize(2, len);
    afr->read(&data, 0, len, 0, true, true);
    voicePlaybackInfo.dataSampleRate = afr->sampleRate;
    voicePlaybackInfo.dataMidiRootNote = 74;
    
    // TODO: make adjustable
    int polyphony = 6;
    
    for (int i = 0; i < polyphony; i++) {
        inactiveNotes.push_back(std::make_unique<PadfootNote>(sampleLoop, voicePlaybackInfo));
    }
    sampleLoop.stateTree.at("num_samples")->set(len);
}

Padfoot::~Padfoot()
{}

void Padfoot::renderNotes (AudioSampleBuffer &buffer, int startSample, int numSamples)
{
    for (const auto &entry : activeNotes) {
        entry.second->renderNextBlock(buffer, startSample, numSamples);
    }
}

std::unique_ptr<PadfootNote> Padfoot::getInactiveNote()
{
    // TODO: deactivate an active note
    if (inactiveNotes.empty()) {
        return nullptr;
    }
    std::unique_ptr<PadfootNote> note = std::move(inactiveNotes.back());
    inactiveNotes.pop_back();
    return note;
}

void Padfoot::handleMidiEvent (const MidiMessage &m)
{
    int noteNum = m.getNoteNumber();
    float vel = m.getFloatVelocity();
    if (m.isNoteOn()) {
        std::unique_ptr<PadfootNote> note = getInactiveNote();
        note->startNote(noteNum, vel);
        activeNotes[noteNum] = std::move(note);
    } else if (m.isNoteOff()) {
        size_t count = activeNotes.count(noteNum);
        if (count == 0) return;
        std::unique_ptr<PadfootNote> note = std::move(activeNotes[noteNum]);
        activeNotes.erase(noteNum);
        note->stopNote(vel);
        inactiveNotes.push_back(std::move(note));
    }
}

void Padfoot::processBlock (AudioSampleBuffer& outputAudio, MidiBuffer& midiData)
{
    // TODO move midi logic into helper
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
    voicePlaybackInfo.outputSampleRate = sampleRate;
}

