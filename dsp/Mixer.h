/*
 *  Mixer.h
 *  Automat
 *
 *  Created by Stefan Beck on 09.04.06.
 *  Copyright 2006 alphakanal. All rights reserved.
 *
 */

// -----------------------------------------------------------------------------

#ifndef MIXER_H
#define MIXER_H

#include "Buffer.h"
#include "Constants.h"
#include "Inlines.h"

// -----------------------------------------------------------------------------

class Mixer
{
	enum
	{
		kModulation_Panorama,
		kModulation_Level,
	};

	typedef struct MixerData
	{
		Float32	pan;
		Float32	lastPan;
		
		Float32	level;
		Float32	lastLevel;
	}
	MixerData;

public:

				Mixer();
			   ~Mixer();

	void		initialize(UInt32 maxChanels);
		
	void		setParameter(UInt32 chanel, Float32 pan, Float32 level);
	
	void		clearAllChanels();
	void		getMix(UInt32 bufferSize, Float32* modulation);
	
	void		addToOut(UInt32 bufferSize, Float32* leftOut, Float32* rightOut);
	
	Float32*	getChanel(UInt32 chanel);
	Float32*	getLeftOut();
	Float32*	getRightOut();
	
	void		reset();
	void		clear();

private:

	void		cleanup();
	
	UInt32		mMaxChanels;

	Buffer		mChanelBuffer;
	Buffer		mLeftBuffer;
	Buffer		mRightBuffer;
	
	MixerData*	mMixerData;

	Float32	b1[kMaxRenderSlice] __attribute__ ((aligned (16)));	
	Float32	b2[kMaxRenderSlice] __attribute__ ((aligned (16)));	
	Float32	b3[kMaxRenderSlice] __attribute__ ((aligned (16)));	
	Float32	b4[kMaxRenderSlice] __attribute__ ((aligned (16)));	
	Float32	b5[kMaxRenderSlice] __attribute__ ((aligned (16)));	
	Float32	b6[kMaxRenderSlice] __attribute__ ((aligned (16)));	
	Float32	b7[kMaxRenderSlice] __attribute__ ((aligned (16)));	
	
	Float32	hp[4];
};

// -----------------------------------------------------------------------------

#endif

// -----------------------------------------------------------------------------
