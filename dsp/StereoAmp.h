/*
 *  StereoAmp.h
 *  Automat
 *
 *  Created by Stefan Beck on 09.04.06.
 *  Copyright (c) 2006-2008 Stefan Beck (stiwi@alphakanal, loopmasta). MIT License, see LICENSE.
 *
 */

// -----------------------------------------------------------------------------

#ifndef AMPLIFIER_H
#define AMPLIFIER_H

#include "Constants.h"
#include "Inlines.h"

// -----------------------------------------------------------------------------

class StereoAmp
{
	enum
	{
		kModulation_Level,
		kModulation_Spread,
	};

public:

			StereoAmp();

	void	setParameter(Float32 level);
	void	setParameter(Float32 level, Float32 spread);
	
	void	getAmp(UInt32 bufferSize, Float32* leftBuffer, Float32* rightBuffer, Float32 envelope, Float32* modulation);
	void	getAmpWithSpread(UInt32 bufferSize, Float32* leftBuffer, Float32* rightBuffer, Float32 envelope, Float32* modulation);

	void	reset();

private:

	void	cleanup();	

	Float32	mLevel, mSpread;
	Float32	mLastLevel, mLastSpread;

	Float32	b1[kMaxRenderSlice] __attribute__ ((aligned (16)));	
	Float32	b2[kMaxRenderSlice] __attribute__ ((aligned (16)));	
	Float32	b3[kMaxRenderSlice] __attribute__ ((aligned (16)));	
	Float32	b4[kMaxRenderSlice] __attribute__ ((aligned (16)));	
	Float32	b5[kMaxRenderSlice] __attribute__ ((aligned (16)));	
	Float32	b6[kMaxRenderSlice] __attribute__ ((aligned (16)));	
	Float32	b7[kMaxRenderSlice] __attribute__ ((aligned (16)));	
};

// -----------------------------------------------------------------------------

#endif

// -----------------------------------------------------------------------------
