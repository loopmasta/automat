/*
 *  StereoChorus.h
 *  Automat
 *
 *  Created by Stefan Beck on 09.04.06.
 *  Copyright 2006 alphakanal. All rights reserved.
 *
 */

// -----------------------------------------------------------------------------

#ifndef STEREOCHORUS_H
#define STEREOCHORUS_H

#include "Buffer.h"
#include "Constants.h"
#include "Inlines.h"

// -----------------------------------------------------------------------------

const	Float32	kSptMaxDelay	= 0.04;	//	40ms
const	Float32	kSptOffDelay	= 0.01;	//	10ms

// -----------------------------------------------------------------------------

class StereoChorus
{
	enum
	{
		kModulation_Speed1,
		kModulation_Speed2,
		kModulation_FeedBack,
		kModulation_Depth,
		kModulation_Mix,
	};

	enum
	{
		kSpt_Off		= 0,
		kSpt_Chorus		= 1,
		kSpt_Phaser		= 2,
		
		kSpt_Mod		= 0,
		kSpt_Fix		= 1,
	};

public:
	
				StereoChorus();
			   //~StereoChorus();
	
	void		initialize(Float32 sampleRate, Float32 maxSpeed);
	
	void		setParameter(UInt32 type, UInt32 mode, Float32 speed1, Float32 speed2, Float32 feedBack, Float32 depth, Float32 mix);
	void		getEffect(UInt32 bufferSize, Float32* leftBuffer, Float32* rightBuffer, Float32* modulation);
	
	void		reset();
	void		clear();
	
private:
		
	//void		cleanup();

	void		getChorus(UInt32 bufferSize, Float32 *leftBuffer, Float32 *rightBuffer, Float32 speed1, Float32 speed2, Float32 feedBack, Float32 depth, Float32 mix);
	void		getPhaser(UInt32 bufferSize, Float32 *leftBuffer, Float32 *rightBuffer, Float32 speed1, Float32 speed2, Float32 feedBack, Float32 depth, Float32 mix);
	
	UInt32		mType, mMode;
	Float32		mSpeed1, mSpeed2, mFeedBack, mDepth, mMix;
	Float32		mLastFeedBack, mLastMix;
		
	Float32		mLfoTuning, mDelayOffset, mDelayMod;
	UInt32		mDelaySize, mRingPointer;
	
	Buffer		mRingBufferL;
	Buffer		mRingBufferR;
	
	sinOsc		mOsc[2];
	Float32		mState[6];
};

#endif

// -----------------------------------------------------------------------------
