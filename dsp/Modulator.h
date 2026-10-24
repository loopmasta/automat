/*
 *  Modulator.h
 *  Automat
 *
 *  Created by Stefan Beck on 09.04.06.
 *  Copyright (c) 2006-2008 Stefan Beck (stiwi@alphakanal, loopmasta). MIT License, see LICENSE.
 *
 */

// -----------------------------------------------------------------------------

#ifndef MODULATOR_H
#define MODULATOR_H

#include "TTimeLineControl.h"
#include "SynthNote.h"

#include "Constants.h"
#include "Inlines.h"
#include "TimeLineData.h"

// -----------------------------------------------------------------------------

class Modulator
{

	enum
	{	
		kModulation_Speed,
		kModulation_Level,
	};
	
	enum
	{	
		kModSync_Off			= 0,
		kModSync_Loop			= 1,
		kModSync_Ppong			= 2,
		kModSync_OneShot		= 3,
		kModSync_Envelope		= 4,

		kModTimeInSeconds		= 1,
	};

	typedef struct ModulatorData
	{
		//	State
		UInt32	lookup;
		Float32	time;
		SInt8	direction;
		
		//	Parameter
		UInt32	target;
		UInt32	sync;
		UInt32	timing;
		Float32	speed;
		Float32	keyTrackDepth;
		Float32	velocityDepth;
		Float32	level;	
	}
	ModulatorData;

public:
	
					Modulator();
				   ~Modulator();
	
	void			initialize(Float32 sampleRate, UInt32 numberOfTargets, TimeLineData* timeLineData, UInt32 firstDataSet, UInt32 lastDataSet, UInt32 firstMod2Mod);
	
	void			setParameter(UInt32 modulator, UInt32 target, UInt32 sync, UInt32 timing, Float32 speed, Float32 keyTrackDepth, Float32 velocityDepth, Float32 level);	
	Float32*		getModulation(UInt32 chunkSize);
	Float32*		getModulationWithState(UInt32 chunkSize, UInt32 state);
	
	void			start(Float32 keyTrack, Float32 velocity);
	void			update(Float32 keyTrack);
	
	void			reset();
	void			clear();
	
private:
			
	void			cleanup();
	
	Float32			getOutput(UInt32 chunkSize, ModulatorData* state, TLData* data, Float32* modulation);
	void			printModulationData();
	
	bool			mKeyDown, mResetMod;
	Float32			mSampleRate, mKeyTrack, mVelocity;
	
	TimeLineData*	mTimeLineData;	
	UInt32			mNumberOfTargets, mFirstDataSet, mLastDataSet, mNumberOfInputs, mFirstMod2Mod;

	ModulatorData*	mState;
	Float32*		mOutput;
};

#endif

// -----------------------------------------------------------------------------

