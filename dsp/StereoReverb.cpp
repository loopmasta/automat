/*
 *  StereoReverb.cpp
 *  Automat
 *
 *  Created by Stefan Beck on 08.07.07.
 *  Copyright 2006 alphakanal. All rights reserved.
 *
 */

// -----------------------------------------------------------------------------

#include "StereoReverb.h"

// -----------------------------------------------------------------------------

StereoReverb::StereoReverb()
{
	mMode		= kRev_Off;
	mSize		= 0.5;
	mHiCut		= 0.0;
	mLoCut		= 0.0;
	mDelay		= 0.5;
	mMix		= 0.5;

	reset();
}

// -----------------------------------------------------------------------------

void StereoReverb::initialize(Float32 sampleRate)
{ 
	if (sampleRate < 44100.0)
		sampleRate = 44100.0;

	mSampleRate = sampleRate;
	
	mDelaySize	= (UInt32)(kPreDelay * sampleRate + 0.5);
	mDelayRead	= 0;
	
	mDelayL.allocate(mDelaySize);
	mDelayR.allocate(mDelaySize);

	for (UInt32 i = 0; i < kNumComb; i++)
	{
		mCombLSize[i]	= (UInt32)(kCombDelays[i][0] * sampleRate + 0.5);
		mCombLRead[i]	= 0;
		
		mCombL[i].allocate(mCombLSize[i]);
		
		mCombRSize[i]	= (UInt32)(kCombDelays[i][1] * sampleRate + 0.5);
		mCombRRead[i]	= 0;
		
		mCombR[i].allocate(mCombRSize[i]);
	}
	
	mFilter.initialize(sampleRate);

	reset();
	clear();
}

// -----------------------------------------------------------------------------

void StereoReverb::setParameter(UInt32 mode, Float32 size, Float32 hicut, Float32 loCut, Float32 delay, Float32 mix)
{
	if (mode != mMode)
	{
		reset();
		clear();
	}

	mMode	= mode;
	mSize	= size;
	mHiCut	= hicut;
	mLoCut	= loCut;
	mDelay	= delay;
	mMix	= mix;
}

// -----------------------------------------------------------------------------

void StereoReverb::getEffect(UInt32 bufferSize, Float32* leftBuffer, Float32* rightBuffer, Float32* modulation)
{
	if (mMode == kRev_Off)
		return;

	Float32 size	= mSize;
	Float32 hiCut	= mHiCut;
	Float32 loCut	= mLoCut;
	Float32 delay	= mDelay;
	Float32 mix		= mMix;

	if (modulation)
	{
		size		*= modulation[kModulation_Size];
		hiCut		*= modulation[kModulation_HiCut];
		loCut		*= modulation[kModulation_LoCut];
		mix			*= modulation[kModulation_Mix];
	}

	size	= quadUpPositive(size);
	delay	= 1.0 - delay;
	mix		= pow2(mix);
	
	Float32		ramp			= 1.0 / bufferSize;

	Float32		l_size			= mLastSize;	
	Float32		sizeDelta		= (size - mLastSize) * ramp;

	Float32		l_mix			= mLastMix;	
	Float32		mixDelta		= (mix - mLastMix) * ramp;
	
	UInt32		l_combLSize_1	= mCombLSize[0];
	UInt32		l_combLRead_1	= mCombLRead[0];
	Float32*	l_combLBuffer_1	= mCombL[0].getAlignedBuffer();

	UInt32		l_combLSize_2	= mCombLSize[1];
	UInt32		l_combLRead_2	= mCombLRead[1];
	Float32*	l_combLBuffer_2	= mCombL[1].getAlignedBuffer();

	UInt32		l_combLSize_3	= mCombLSize[2];
	UInt32		l_combLRead_3	= mCombLRead[2];
	Float32*	l_combLBuffer_3	= mCombL[2].getAlignedBuffer();

	UInt32		l_combLSize_4	= mCombLSize[3];
	UInt32		l_combLRead_4	= mCombLRead[3];
	Float32*	l_combLBuffer_4	= mCombL[3].getAlignedBuffer();

	UInt32		l_combLSize_5	= mCombLSize[4];
	UInt32		l_combLRead_5	= mCombLRead[4];
	Float32*	l_combLBuffer_5	= mCombL[4].getAlignedBuffer();

	UInt32		l_combLSize_6	= mCombLSize[5];
	UInt32		l_combLRead_6	= mCombLRead[5];
	Float32*	l_combLBuffer_6	= mCombL[5].getAlignedBuffer();

	UInt32		l_combLSize_7	= mCombLSize[6];
	UInt32		l_combLRead_7	= mCombLRead[6];
	Float32*	l_combLBuffer_7	= mCombL[6].getAlignedBuffer();

	UInt32		l_combLSize_8	= mCombLSize[7];
	UInt32		l_combLRead_8	= mCombLRead[7];
	Float32*	l_combLBuffer_8	= mCombL[7].getAlignedBuffer();

	UInt32		l_combRSize_1	= mCombRSize[0];
	UInt32		l_combRRead_1	= mCombRRead[0];
	Float32*	l_combRBuffer_1	= mCombR[0].getAlignedBuffer();

	UInt32		l_combRSize_2	= mCombRSize[1];
	UInt32		l_combRRead_2	= mCombRRead[1];
	Float32*	l_combRBuffer_2	= mCombR[1].getAlignedBuffer();

	UInt32		l_combRSize_3	= mCombRSize[2];
	UInt32		l_combRRead_3	= mCombRRead[2];
	Float32*	l_combRBuffer_3	= mCombR[2].getAlignedBuffer();

	UInt32		l_combRSize_4	= mCombRSize[3];
	UInt32		l_combRRead_4	= mCombRRead[3];
	Float32*	l_combRBuffer_4	= mCombR[3].getAlignedBuffer();

	UInt32		l_combRSize_5	= mCombRSize[4];
	UInt32		l_combRRead_5	= mCombRRead[4];
	Float32*	l_combRBuffer_5	= mCombR[4].getAlignedBuffer();

	UInt32		l_combRSize_6	= mCombRSize[5];
	UInt32		l_combRRead_6	= mCombRRead[5];
	Float32*	l_combRBuffer_6	= mCombR[5].getAlignedBuffer();

	UInt32		l_combRSize_7	= mCombRSize[6];
	UInt32		l_combRRead_7	= mCombRRead[6];
	Float32*	l_combRBuffer_7	= mCombR[6].getAlignedBuffer();

	UInt32		l_combRSize_8	= mCombRSize[7];
	UInt32		l_combRRead_8	= mCombRRead[7];
	Float32*	l_combRBuffer_8	= mCombR[7].getAlignedBuffer();
	
	Float32*	l_filterL		= &mFilterL[0];
	Float32*	l_filterR		= &mFilterR[0];

	getPreDelay(bufferSize, leftBuffer, rightBuffer, l_filterL, l_filterR, delay);

	mFilter.setParameter(18, hiCut, loCut, 0.5, 0.5, 0.5);
	mFilter.getFilter(bufferSize, l_filterL, l_filterR, 0.0, NULL);

	Float32		inL, inR, dL, dR, cL, cR, outL, outR;
		
	while(bufferSize--)
	{
		l_size					       += sizeDelta;
		l_mix						   += mixDelta;

		inL								= *leftBuffer;
		inR								= *rightBuffer;

		dL								= *l_filterL++;
		dR								= *l_filterR++;

		outL							= 0.0;
		outR							= 0.0;
		
		cL								= l_combLBuffer_1[l_combLRead_1];
		l_combLBuffer_1[l_combLRead_1]	= dL + (cL * l_size);
		l_combLRead_1					= (l_combLRead_1 + 1) % l_combLSize_1;
		
		outL							= cL;
		
		cL								= l_combLBuffer_2[l_combLRead_2];
		l_combLBuffer_2[l_combLRead_2]	= dL + (cL * l_size);
		l_combLRead_2					= (l_combLRead_2 + 1) % l_combLSize_2;

		outL						   += cL;
		
		cL								= l_combLBuffer_3[l_combLRead_3];
		l_combLBuffer_3[l_combLRead_3]	= dL + (cL * l_size);
		l_combLRead_3					= (l_combLRead_3 + 1) % l_combLSize_3;

		outL						   += cL;
		
		cL								= l_combLBuffer_4[l_combLRead_4];
		l_combLBuffer_4[l_combLRead_4]	= dL + (cL * l_size);
		l_combLRead_4					= (l_combLRead_4 + 1) % l_combLSize_4;

		outL						   += cL;
		
		cL								= l_combLBuffer_5[l_combLRead_5];
		l_combLBuffer_5[l_combLRead_5]	= dL + (cL * l_size);
		l_combLRead_5					= (l_combLRead_5 + 1) % l_combLSize_5;

		outL						   += cL;
		
		cL								= l_combLBuffer_6[l_combLRead_6];
		l_combLBuffer_6[l_combLRead_6]	= dL + (cL * l_size);
		l_combLRead_6					= (l_combLRead_6 + 1) % l_combLSize_6;

		outL						   += cL;
		
		cL								= l_combLBuffer_7[l_combLRead_7];
		l_combLBuffer_7[l_combLRead_7]	= dL + (cL * l_size);
		l_combLRead_7					= (l_combLRead_7 + 1) % l_combLSize_7;

		outL						   += cL;
		
		cL								= l_combLBuffer_8[l_combLRead_8];
		l_combLBuffer_8[l_combLRead_8]	= dL + (cL * l_size);
		l_combLRead_8					= (l_combLRead_8 + 1) % l_combLSize_8;

		outL						   += cL;
		
		//	-----

		cR								= l_combRBuffer_1[l_combRRead_1];
		l_combRBuffer_1[l_combRRead_1]	= dR + (cR * l_size);
		l_combRRead_1					= (l_combRRead_1 + 1) % l_combRSize_1;
		
		outR							= cR;
		
		cR								= l_combRBuffer_2[l_combRRead_2];
		l_combRBuffer_2[l_combRRead_2]	= dR + (cR * l_size);
		l_combRRead_2					= (l_combRRead_2 + 1) % l_combRSize_2;

		outR						   += cR;
		
		cR								= l_combRBuffer_3[l_combRRead_3];
		l_combRBuffer_3[l_combRRead_3]	= dR + (cR * l_size);
		l_combRRead_3					= (l_combRRead_3 + 1) % l_combRSize_3;

		outR						   += cR;
		
		cR								= l_combRBuffer_4[l_combRRead_4];
		l_combRBuffer_4[l_combRRead_4]	= dR + (cR * l_size);
		l_combRRead_4					= (l_combRRead_4 + 1) % l_combRSize_4;

		outR						   += cR;
		
		cR								= l_combRBuffer_5[l_combRRead_5];
		l_combRBuffer_5[l_combRRead_5]	= dR + (cR * l_size);
		l_combRRead_5					= (l_combRRead_5 + 1) % l_combRSize_5;

		outR						   += cR;
		
		cR								= l_combRBuffer_6[l_combRRead_6];
		l_combRBuffer_6[l_combRRead_6]	= dR + (cR * l_size);
		l_combRRead_6					= (l_combRRead_6 + 1) % l_combRSize_6;

		outR						   += cR;
		
		cR								= l_combRBuffer_7[l_combRRead_7];
		l_combRBuffer_7[l_combRRead_7]	= dR + (cR * l_size);
		l_combRRead_7					= (l_combRRead_7 + 1) % l_combRSize_7;

		outR						   += cR;
		
		cR								= l_combRBuffer_8[l_combRRead_8];
		l_combRBuffer_8[l_combRRead_8]	= dR + (cR * l_size);
		l_combRRead_8					= (l_combRRead_8 + 1) % l_combRSize_8;

		outR						   += cR;

		*leftBuffer++					= quadMix(inL, outL * 0.25, l_mix);		
		*rightBuffer++					= quadMix(inR, outR * 0.25, l_mix);
	}
	
	mLastSize			= l_size;
	mLastMix			= l_mix;

	mCombLRead[0]		= l_combLRead_1;
	mCombLRead[1]		= l_combLRead_2;
	mCombLRead[2]		= l_combLRead_3;
	mCombLRead[3]		= l_combLRead_4;
	mCombLRead[4]		= l_combLRead_5;
	mCombLRead[5]		= l_combLRead_6;
	mCombLRead[6]		= l_combLRead_7;
	mCombLRead[7]		= l_combLRead_8;

	mCombRRead[0]		= l_combRRead_1;
	mCombRRead[1]		= l_combRRead_2;
	mCombRRead[2]		= l_combRRead_3;
	mCombRRead[3]		= l_combRRead_4;
	mCombRRead[4]		= l_combRRead_5;
	mCombRRead[5]		= l_combRRead_6;
	mCombRRead[6]		= l_combRRead_7;
	mCombRRead[7]		= l_combRRead_8;
}

// -----------------------------------------------------------------------------

void StereoReverb::reset()
{
	mLastSize	= 0.0;
	mLastMix	= 0.0;

	mDelayRead	= 0;

	for (UInt32 i = 0; i < kNumComb; i++)
	{
		mCombLRead[i] = 0;
		mCombRRead[i] = 0;
	}
	
	mFilter.reset();
}

// -----------------------------------------------------------------------------

void StereoReverb::clear()
{
	mDelayL.clear();
	mDelayR.clear();

	for (UInt32 i = 0; i < kNumComb; i++)
	{
		mCombL[i].clear();
		mCombR[i].clear();
	}
	
	mFilter.clear();
}

// -----------------------------------------------------------------------------

void StereoReverb::getPreDelay(UInt32 bufferSize, Float32* leftInBuffer, Float32* rightInBuffer, Float32* leftOutBuffer, Float32* rightOutBuffer, Float32 delay)
{
	UInt32		l_delay			= (UInt32)((mDelaySize - 1) * delay);
	UInt32		l_delaysize		= mDelaySize;
	UInt32		l_delayread		= mDelayRead;
	Float32*	l_delayLBuffer	= mDelayL.getAlignedBuffer();
	Float32*	l_delayRBuffer	= mDelayR.getAlignedBuffer();

	UInt32		di;

	while(bufferSize--)
	{
		di								= (l_delayread + l_delay) % l_delaysize;
		
		*leftOutBuffer++				= l_delayLBuffer[di];
		l_delayLBuffer[l_delayread]		= *leftInBuffer++;

		*rightOutBuffer++				= l_delayRBuffer[di];
		l_delayRBuffer[l_delayread]		= *rightInBuffer++;
		
		l_delayread						= (l_delayread + 1) % l_delaysize;
	}
	
	mDelayRead = l_delayread;
}

// -----------------------------------------------------------------------------
