/*
 *  StereoShaper.h
 *  Automat
 *
 *  Created by Stefan Beck on 21.12.07.
 *  Copyright (c) 2006-2008 Stefan Beck (stiwi@alphakanal, loopmasta). MIT License, see LICENSE.
 *
 */

// -----------------------------------------------------------------------------

#ifndef STEREOSHAPER_H
#define STEREOSHAPER_H

#include "Constants.h"
#include "Inlines.h"

// -----------------------------------------------------------------------------

class StereoShaper
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
		kSample_Size = 4
	};

public:
	
			StereoShaper();

	void	initialize(Float32 sampleRate);
	void	setParameter(UInt32 type, Float32 bias, Float32 drive);
	void	getShape(UInt32 bufferSize, Float32* leftbuffer, Float32* rightbuffer, Float32* modulation);
	
	void	start(Float32 keyTrack, Float32 velocity);
	void	update(Float32 keyTrack);

	void	reset();
	void	clear();
	
private:

	void	getTube(UInt32 bufferSize, Float32* leftbuffer, Float32* rightbuffer, Float32 bias, Float32 drive);
	void	getSilicon(UInt32 bufferSize, Float32* leftbuffer, Float32* rightbuffer, Float32 bias, Float32 drive);
	void	getDigital(UInt32 bufferSize, Float32* leftbuffer, Float32* rightbuffer, Float32 bias, Float32 drive);
	void	getAmMod(UInt32 bufferSize, Float32* leftbuffer, Float32* rightbuffer, Float32 bias, Float32 drive);
	void	getFmMod(UInt32 bufferSize, Float32* leftbuffer, Float32* rightbuffer, Float32 bias, Float32 drive);
	
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

