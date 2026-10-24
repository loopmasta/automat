/*
 *  Voice.h
 *  Automat
 *
 *  Created by Stefan Beck on 09.04.06.
 *  Copyright (c) 2006-2008 Stefan Beck (stiwi@alphakanal, loopmasta). MIT License, see LICENSE.
 *
 */

// -----------------------------------------------------------------------------

#ifndef VOICE_H
#define VOICE_H

#include "TTimeLineControl.h"

#include "AUInstrumentBase.h"
#include "SynthNote.h"

#include "Constants.h"
#include "Envelope.h"
#include "FilterShaper.h"
#include "MainFilter.h"
#include "Mixer.h"
#include "BlockLimiter.h"
#include "Modulator.h"
#include "Oscillator.h"
#include "Parameter.h"
#include "SampleData.h"
#include "StereoAmp.h"
#include "TuningData.h"
#include "TimeLineData.h"

// -----------------------------------------------------------------------------

class Voice : public SynthNote
{
public:

	void				initialize(Float32 sampleRate, SampleData* sampleData, TuningData* tuningData, TimeLineData* timeLineData);
	
	virtual void		Attack(const MusicDeviceNoteParams &inParams);
	virtual OSStatus	Render(UInt32 inNumFrames, AudioBufferList &inBufferList){return noErr;};
	
	//	SDKPatch_stiwi
	virtual OSStatus	Render(UInt32 inNumFrames, AudioBufferList &inBufferList, bool hasLineIn, AudioBufferList &inputList);
	virtual void		Legato(const MusicDeviceNoteParams &inParams);
	//	SDKPatch_stiwi
	
	virtual Float32		Amplitude();
	
	void				reset();
	void				clear();
	
private:

	void				updateParameter();

	Oscillator			mOscillator1;
	Oscillator			mOscillator2;
	Oscillator			mOscillator3;

	FilterShaper		mFilterShaper1;
	FilterShaper		mFilterShaper2;
	FilterShaper		mFilterShaper3;

	Mixer				mMixer;
	BlockLimiter		mBlockLimiter;
	
	Envelope			mFltEnvelope;
	MainFilter			mMainFilter;
	
	Envelope			mAmpEnvelope;
	StereoAmp			mAmplifier;
	
	Modulator			mModulator;
	
	Float32				mSampleRate;
	TuningData*			mTuningData;
};

// -----------------------------------------------------------------------------

#endif

// -----------------------------------------------------------------------------
