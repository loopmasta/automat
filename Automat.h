/*
 *  Automat.h
 *  Automat
 *
 *  Created by Stefan Beck on 01.02.06.
 *  Copyright (c) 2006-2008 Stefan Beck (stiwi@alphakanal, loopmasta). MIT License, see LICENSE.
 *
 */

// -----------------------------------------------------------------------------

#ifndef AUTOMAT_H
#define AUTOMAT_H

#include <AudioUnit/AudioUnit.h>
#include <AudioUnit/AudioUnitCarbonView.h>

#include "AUInstrumentBase.h"

#include "AutomatVersion.h"

#include "Constants.h"
#include "Parameter.h"
#include "Inlines.h"

#include "Modulator.h"
#include "SampleData.h"
#include "StereoAmp.h"
#include "StereoChorus.h"
#include "StereoDelay.h"

#include "StereoReverb.h"

#include "TimeLineData.h"
#include "TuningData.h"
#include "Voice.h"

// -----------------------------------------------------------------------------

class Automat : public AUMonotimbralInstrumentBase
{
	enum
	{
		kRandomize_Random,
		kRandomize_Evolution
	};

	enum
	{
		kMode_Poly,
		kMode_Mono,
		kMode_Legato,
	};

public:

								Automat(ComponentInstance inComponentInstance);
	virtual					   ~Automat();
				
	virtual ComponentResult		Initialize();
	virtual ComponentResult		Version(){return kAutomatVersion;}
	
	virtual	ComponentResult		SaveState(CFPropertyListRef* outData);
	virtual	ComponentResult		RestoreState(CFPropertyListRef inData);

	virtual ComponentResult		GetParameterInfo(AudioUnitScope inScope, AudioUnitParameterID inParameterID, AudioUnitParameterInfo& outParameterInfo);
	virtual ComponentResult		GetParameterValueStrings(AudioUnitScope inScope, AudioUnitParameterID inParameterID, CFArrayRef* outStrings);

	virtual ComponentResult		GetPropertyInfo(AudioUnitPropertyID inID, AudioUnitScope inScope, AudioUnitElement inElement, UInt32& outDataSize, Boolean& outWritable);
	virtual ComponentResult		GetProperty(AudioUnitPropertyID inID, AudioUnitScope inScope, AudioUnitElement inElement, void* outData);
	virtual ComponentResult		SetProperty(AudioUnitPropertyID inID, AudioUnitScope inScope, AudioUnitElement inElement, const void* inData, UInt32 inDataSize);
	
	virtual ComponentResult		RealTimeStartNote(SynthGroupElement* inGroup, NoteInstanceID inNoteInstanceID, UInt32 inOffsetSampleFrame, const MusicDeviceNoteParams &inParams);
	
	virtual OSStatus			HandleAllNotesOff(UInt8 inChannel);
	virtual OSStatus			HandleAllSoundOff(UInt8 inChannel);

	virtual OSStatus			HandleControlChange(UInt8 inChannel, UInt8 inController, UInt8 inValue, UInt32 inStartFrame);
													
	virtual int					GetNumCustomUIComponents(){return 1;};
	virtual void				GetUIComponentDescs(ComponentDescription *inDescArray);

	virtual UInt32				SupportedNumChannels(const AUChannelInfo **outInfo);
	virtual	bool				ValidFormat(AudioUnitScope inScope, AudioUnitElement inElement, const CAStreamBasicDescription& inNewFormat);
	
	virtual CFURLRef			CopyIconLocation();

	virtual ComponentResult		Render(AudioUnitRenderActionFlags &ioActionFlags, const AudioTimeStamp &inTimeStamp, UInt32 inNumberFrames);
	
	void						reset();
	void						clear();

private:
	
	void						readSampleData(CFStringRef name);
	void						readTuningData(CFStringRef name);
	
	void						updateParameter();
	void						randomizeParameter(UInt32 mode);

	Voice						mVoice[kNumberOfNotes];

	SampleData					mSampleData;
	TuningData					mTuningData;
	TimeLineData				mTimeLineData;
		
	StereoChorus				mChorus;
	StereoDelay					mDelay;
	StereoReverb				mReverb;
	
	StereoAmp					mAmplifier;
	
	Modulator					mModulator;	
	
	SInt32						mCCData[kNumberOfMidiControls];
	SInt32						mCCLearning;
	
	Float64						mBeatsPerMinute;

	CFStringRef					mSample;
	CFStringRef					mTuning;	
};

// -----------------------------------------------------------------------------

#endif

// -----------------------------------------------------------------------------
