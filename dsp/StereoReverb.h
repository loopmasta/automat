/*
 *  StereoReverb.h
 *  Automat
 *
 *  Created by Stefan Beck on 08.07.07.
 *  Copyright 2006 alphakanal. All rights reserved.
 *
 */

// -----------------------------------------------------------------------------

#ifndef STEREOREVERB_H
#define STEREOREVERB_H

#include "Constants.h"
#include "Buffer.h"
#include "Inlines.h"
#include "StereoFilter.h"

// -----------------------------------------------------------------------------

#define kNumComb		8

#define kSampleRate		44100.0
#define kStereoSpread	23.0

static const Float32 kCombDelays[kNumComb][2] = {
    {1116.0 / kSampleRate, (1116.0 + kStereoSpread) / kSampleRate},
    {1188.0 / kSampleRate, (1188.0 + kStereoSpread) / kSampleRate},
    {1277.0 / kSampleRate, (1277.0 + kStereoSpread) / kSampleRate},
    {1356.0 / kSampleRate, (1356.0 + kStereoSpread) / kSampleRate},
    {1422.0 / kSampleRate, (1422.0 + kStereoSpread) / kSampleRate},
    {1491.0 / kSampleRate, (1491.0 + kStereoSpread) / kSampleRate},
    {1557.0 / kSampleRate, (1557.0 + kStereoSpread) / kSampleRate},
    {1617.0 / kSampleRate, (1617.0 + kStereoSpread) / kSampleRate}
};

static const Float32 kPreDelay = 11025.0 / kSampleRate;

// -----------------------------------------------------------------------------

class StereoReverb
{	
	enum
	{
		kModulation_Size,
		kModulation_HiCut,
		kModulation_LoCut,
		kModulation_Mix,
	};

	enum
	{
		kRev_Off	= 0,
		kRev_On		= 1,
	};

public:
	
					StereoReverb();
				   
	void			initialize(Float32 sampleRate);

	void			setParameter(UInt32 mode, Float32 size, Float32 hicut, Float32 loCut, Float32 delay, Float32 mix);
	void			getEffect(UInt32 bufferSize, Float32* leftBuffer, Float32* rightBuffer, Float32* modulation);
	
	void			reset();
	void			clear();
	
private:
			
	void			getPreDelay(UInt32 bufferSize, Float32* leftInBuffer, Float32* rightInBuffer, Float32* leftOutBuffer, Float32* rightOutBuffer, Float32 delay);

	Float32			mSampleRate;
	
	UInt32			mMode;
	Float32			mSize, mHiCut, mLoCut, mDelay, mMix;
	Float32			mLastSize, mLastMix;
	
	Buffer			mDelayL;
	Buffer			mDelayR;
	UInt32			mDelaySize;
	UInt32			mDelayRead;

	Buffer			mCombL[kNumComb];
	UInt32			mCombLSize[kNumComb];
	UInt32			mCombLRead[kNumComb];

	Buffer			mCombR[kNumComb];
	UInt32			mCombRSize[kNumComb];
	UInt32			mCombRRead[kNumComb];

	StereoFilter	mFilter;

	Float32			mFilterL[kMaxRenderSlice] __attribute__ ((aligned (16)));	
	Float32			mFilterR[kMaxRenderSlice] __attribute__ ((aligned (16)));		
};

// -----------------------------------------------------------------------------

#endif

// -----------------------------------------------------------------------------
