/*
 *  StereoDelay.h
 *  Automat
 *
 *  Created by Stefan Beck on 09.04.06.
 *  Copyright (c) 2006-2008 Stefan Beck (stiwi@alphakanal, loopmasta). MIT License, see LICENSE.
 *
 */

// -----------------------------------------------------------------------------

#ifndef STEREODELAY_H
#define STEREODELAY_H

#include "Buffer.h"
#include "Constants.h"
#include "Inlines.h"
#include "StereoFilter.h"

// -----------------------------------------------------------------------------

class StereoDelay
{
	enum
	{
		kModulation_CutOff,
		kModulation_Reso,
		kModulation_FeedBack,
		kModulation_CrossFeed,
		kModulation_Mix,
	};

	enum
	{
		kDly_Off			= 0,
		
		kDlyFilter_Delay	= 0,
		kDlyFilter_FeedBack	= 1,
		kDlyFilter_Both		= 2,
	};

	enum
	{
		kDlyFilter_Off,		//	0
		
		kDlyFilter_LP12,	//	1
		kDlyFilter_HP12,
		kDlyFilter_BP12,
		kDlyFilter_BR12,
		kDlyFilter_PK12,
		
		kDlyFilter_AP4,		//	6
		
		kDlyFilter_Size = 5,
	};

public:

					StereoDelay();
				   //~StereoDelay();
	
	void			initialize(Float32 sampleRate, Float32 maxSeconds);

	void			setParameter(UInt32 leftTime, UInt32 rightTime, UInt32 filterType, UInt32 filterTarget, Float32 cutOff, Float32 resonance, Float32 feedBack, Float32 crossFeed, Float32 mix);	
	void			setBPM(Float64 bpm);
	void			getEffect(UInt32 bufferSize, Float32* leftBuffer, Float32* rightBuffer, Float32* modulation);
	
	void			reset();
	void			clear();
	
private:

	//void			cleanup();
	
	void			readDelay(UInt32 bufferSize);
	void			getFeedBack(UInt32 bufferSize, Float32 feedBack, Float32 crossFeed);
	void			writeDelay(UInt32 bufferSize, Float32* leftIn, Float32* rightIn);
	void			writeToOut(UInt32 bufferSize, Float32* leftBuffer, Float32* rightBuffer, Float32 mix);
	
	Float32			mSampleRate;
	
	UInt32			mLeftTime, mRightTime, mFilterType, mFilterTarget;	
	Float32			mCutOff, mResonance, mFeedBack, mCrossFeed, mMix;
	Float32			mLastCutOff, mLastResonance, mLastFeedBack, mLastCrossFeed, mLastMix;

	Float32			mSecondsPerBeat;
	
	UInt32			mRingSize, mRingPointer;
	
	Buffer			mLeftRingBuffer;
	Buffer			mRightRingBuffer;

	Buffer			mLeftDelay;
	Buffer			mRightDelay;

	Buffer			mLeftFBack;
	Buffer			mRightFBack;
	
	StereoFilter	mFilter;

	Float32	b1[kMaxRenderSlice] __attribute__ ((aligned (16)));	
	Float32	b2[kMaxRenderSlice] __attribute__ ((aligned (16)));	
	Float32	b3[kMaxRenderSlice] __attribute__ ((aligned (16)));	
	Float32	b4[kMaxRenderSlice] __attribute__ ((aligned (16)));	
	Float32	b5[kMaxRenderSlice] __attribute__ ((aligned (16)));	
	Float32	b6[kMaxRenderSlice] __attribute__ ((aligned (16)));	
	Float32	b7[kMaxRenderSlice] __attribute__ ((aligned (16)));	
	Float32	b8[kMaxRenderSlice] __attribute__ ((aligned (16)));
};

#endif

// -----------------------------------------------------------------------------
