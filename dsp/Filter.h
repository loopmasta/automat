/*
 *  Filter.h
 *  Automat
 *
 *  Created by Stefan Beck on 09.04.06.
 *  Copyright (c) 2006-2008 Stefan Beck (stiwi@alphakanal, loopmasta). MIT License, see LICENSE.
 *
 */

// -----------------------------------------------------------------------------

#ifndef FILTER_H
#define FILTER_H

#include "AUBuffer.h"

#include "Constants.h"
#include "Inlines.h"

#include "Biquad.h"

// -----------------------------------------------------------------------------

class Filter
{
	enum
	{
		kModulation_CutOff,
		kModulation_Resonance,
	};

	enum
	{
		kFilter_Off,		//	0
		
		kFilter_LD_LP06,	//	1
		kFilter_LD_LP12,
		kFilter_LD_LP18,
		kFilter_LD_LP24,
				
		kFilter_MM_LP12,	//	5
		kFilter_MM_HP12,
		kFilter_MM_BP12,
		kFilter_MM_BR12,
		kFilter_MM_PK12,

		kFilter_MM_LP24,	//	10
		kFilter_MM_HP24,
		kFilter_MM_BP24,
		kFilter_MM_BR24,
		kFilter_MM_PK24,

		kFilter_AP_4,		//	15
		kFilter_AP_8,
		
		kFilter_VOWEL,		//	17
		
		kFilter_Size		= 10
	};
	
public:
	
			Filter();
				
	void	initialize(Float32 sampleRate);

	void	setParameter(UInt32 filterType, Float32 cutOff, Float32 resonance, Float32 envelopeDepth, Float32 keyTrackDepth, Float32 velocityDepth);
	void	getFilter(UInt32 bufferSize, Float32* buffer, Float32 envelope, Float32* modulation);
	void	getFilter(UInt32 bufferSize, Float32* inBuffer, Float32* outBuffer, Float32 envelope, Float32* modulation);
	
	void	start(Float32 keyTrack, Float32 velocity);
	void	update(Float32 keyTrack);
	
	void	reset();
	void	clear();

private:

	void	getLadderFilter(UInt32 bufferSize, Float32* inBuffer, Float32* outBuffer, Float32 cutOff, Float32 resonance);
	void	get12dbMultiModeFilter(UInt32 bufferSize, Float32* inBuffer, Float32* outBuffer, Float32 cutOff, Float32 resonance);
	void	get24dbMultiModeFilter(UInt32 bufferSize, Float32* inBuffer, Float32* outBuffer, Float32 cutOff, Float32 resonance);
	void	get4StageAPFilter(UInt32 bufferSize, Float32* inBuffer, Float32* outBuffer, Float32 cutOff, Float32 resonance);
	void	get8StageAPFilter(UInt32 bufferSize, Float32* inBuffer, Float32* outBuffer, Float32 cutOff, Float32 resonance);
	void	getFormantFilter(UInt32 bufferSize, Float32* inBuffer, Float32* outBuffer, Float32 cutOff, Float32 resonance);
	
	Float32	mSampleRate;

	UInt32	mFilterType, mDriveType;
	Float32	mKeyTrack, mVelocity;
	Float32	mCutOff, mResonance, mEnvelopeDepth, mKeyTrackDepth, mVelocityDepth;
	Float32	mLastCutOff, mLastResonance;

	FlData	mIO[kFilter_Size];
	
	Biquad	mBiquad[kNumOfFormants];
};

// -----------------------------------------------------------------------------

#endif

// -----------------------------------------------------------------------------

