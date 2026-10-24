/*
 *  Shaper.h
 *  Automat
 *
 *  Created by Stefan Beck on 09.04.06.
 *  Copyright 2006 alphakanal. All rights reserved.
 *
 */

// -----------------------------------------------------------------------------

#ifndef SHAPER_H
#define SHAPER_H

#include "Constants.h"
#include "Inlines.h"

// -----------------------------------------------------------------------------

class Shaper
{
	enum
	{
		kModulation_Bias,
		kModulation_Drive,
	};

	enum
	{
		kShape_Off,
		kShape_Tube,
		kShape_Silicon,
		kShape_Digital,
		kShape_AmMod,
		kShape_FmMod,
	};

	enum
	{
		kSample_Size = 2
	};

public:
	
			Shaper();

	void	initialize(Float32 sampleRate);
	void	setParameter(UInt32 type, Float32 bias, Float32 drive);
	void	getShape(UInt32 bufferSize, Float32* buffer, Float32* modulation);
	
	void	start(Float32 keyTrack, Float32 velocity);
	void	update(Float32 keyTrack);

	void	reset();
	void	clear();
	
private:

	void	getTube(UInt32 bufferSize, Float32* buffer, Float32 bias, Float32 drive);
	void	getSilicon(UInt32 bufferSize, Float32* buffer, Float32 bias, Float32 drive);
	void	getDigital(UInt32 bufferSize, Float32* buffer, Float32 bias, Float32 drive);
	void	getAmMod(UInt32 bufferSize, Float32* buffer, Float32 bias, Float32 drive);
	void	getFmMod(UInt32 bufferSize, Float32* buffer, Float32 bias, Float32 drive);
	
	UInt32	mType;
	Float32	mBias, mDrive;
	Float32	mLastBias, mLastDrive;
	
	Float32	mSampleRate, mKeyTrack, mVelocity;
	
	Float32	mSample[kSample_Size];
	
	Float32	b1[kMaxRenderSlice] __attribute__ ((aligned (16)));	
	Float32	b2[kMaxRenderSlice] __attribute__ ((aligned (16)));	
	Float32	b3[kMaxRenderSlice] __attribute__ ((aligned (16)));	
	Float32	b4[kMaxRenderSlice] __attribute__ ((aligned (16)));	
	Float32	b5[kMaxRenderSlice] __attribute__ ((aligned (16)));	
};

// -----------------------------------------------------------------------------

#endif

// -----------------------------------------------------------------------------

