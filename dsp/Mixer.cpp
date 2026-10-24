/*
 *  Mixer.cpp
 *  Automat
 *
 *  Created by Stefan Beck on 09.04.06.
 *  Copyright (c) 2006-2008 Stefan Beck (stiwi@alphakanal, loopmasta). MIT License, see LICENSE.
 *
 */

// -----------------------------------------------------------------------------

#include "Mixer.h"

// -----------------------------------------------------------------------------

Mixer::Mixer()
{
	mMaxChanels	= 0;
	mMixerData	= NULL;
	
	reset();
}

// -----------------------------------------------------------------------------

Mixer::~Mixer()
{
	cleanup();
}

// -----------------------------------------------------------------------------

void Mixer::initialize(UInt32 maxChanels)
{
	mMaxChanels	= maxChanels;

	cleanup();
	
	mChanelBuffer.allocateClear(maxChanels * kMaxRenderSlice);
	
	//	printf("Mixer::initialize maxChanels %ld, kMaxRenderSlice %ld, total %ld\n", maxChanels, kMaxRenderSlice, maxChanels * kMaxRenderSlice);

	mLeftBuffer.allocateClear(kMaxRenderSlice);
	mRightBuffer.allocateClear(kMaxRenderSlice);
	
	mMixerData = new MixerData[maxChanels];
	
	reset();
	clear();
}

// -----------------------------------------------------------------------------

void Mixer::setParameter(UInt32 chanel, Float32 pan, Float32 level)
{
	if (mMixerData)
	{
		mMixerData[chanel].pan		= pan;	
		mMixerData[chanel].level	= level;
	}
}

// -----------------------------------------------------------------------------

void Mixer::clearAllChanels()
{
	mChanelBuffer.clear();
}

// -----------------------------------------------------------------------------

void Mixer::getMix(UInt32 bufferSize, Float32* modulation)
{
	mLeftBuffer.clear();
	mRightBuffer.clear();
		
	Float32*	left	= mLeftBuffer.getAlignedBuffer();
	Float32*	right	= mRightBuffer.getAlignedBuffer();

	const int size		= bufferSize;
	
	Float32*	vPan	= (Float32*)&b1;
	Float32*	vNegPan	= (Float32*)&b2;
	Float32*	vLevel	= (Float32*)&b3;

	Float32*	vSignal	= (Float32*)&b4;
	Float32*	vLeft	= (Float32*)&b5;
	Float32*	vRight	= (Float32*)&b6;

	Float32		one		= 1.0;
	Float32*	vOne	= (Float32*)&b7;
	
	Float32		ramp	= 1.0 / size;

	for (UInt32 chanel = 0; chanel < mMaxChanels; chanel++)
	{
		Float32*	buffer		= mChanelBuffer.getAlignedBuffer() + (chanel * kMaxRenderSlice);
		MixerData*	mixer		= &mMixerData[chanel]; 
					
		Float32		pan			= mixer->pan;
		Float32		level		= mixer->level;

		Float32		l_pan		= mixer->lastPan;
		Float32		l_level		= mixer->lastLevel;
			
		if (modulation)
		{
			UInt32 modOffset = chanel * 2;
				
			pan		*= modulation[modOffset + kModulation_Panorama];
			level	*= modulation[modOffset + kModulation_Level];
		}
				
		Float32	panDelta	= (pan - l_pan) * ramp;
		Float32	levelDelta	= (level - l_level) * ramp;
		
		vDSP_vramp(&l_pan, &panDelta, vPan, 1, size);
		vDSP_vramp(&l_level, &levelDelta, vLevel, 1, size);

		vDSP_vfill(&one, vOne, 1, size);
		vDSP_vsub(vPan, 1, vOne, 1, vNegPan, 1, size);			//	1.0 - vPan -> vNegPan

		vDSP_vmul(buffer, 1, vLevel, 1, vSignal, 1, size);		//	buffer * vLevel -> vSignal
		vDSP_vmul(vSignal, 1, vNegPan, 1, vLeft, 1, size);		//	vSignal * vNegPan -> vLeft
		vDSP_vmul(vSignal, 1, vPan, 1, vRight, 1, size);		//	vSignal * vPan -> vRight

		vDSP_vadd(vLeft, 1, left, 1, left, 1, size);			//	vLeft + left -> left
		vDSP_vadd(vRight, 1, right, 1, right, 1, size);			//	vRight + right -> right

		mixer->lastPan		= pan;
		mixer->lastLevel	= level;
	}
}

// -----------------------------------------------------------------------------

void Mixer::addToOut(UInt32 bufferSize, Float32* leftOut, Float32* rightOut)
{ 
	//	printf("Mixer::addToOut bufferSize %ld\n", bufferSize);

	Float32*	left	= mLeftBuffer.getAlignedBuffer();
	Float32*	right	= mRightBuffer.getAlignedBuffer();
	
	vDSP_vadd(leftOut, 1, left, 1, leftOut, 1, bufferSize);		//	leftOut + left -> leftOut
	vDSP_vadd(rightOut, 1, right, 1, rightOut, 1, bufferSize);	//	vTemp2 + right -> vTemp2
}

// -----------------------------------------------------------------------------

Float32* Mixer::getChanel(UInt32 chanel)
{
	return mChanelBuffer.getAlignedBuffer() + (chanel * kMaxRenderSlice);
}

// -----------------------------------------------------------------------------

Float32* Mixer::getLeftOut()
{
	return mLeftBuffer.getAlignedBuffer();
}

// -----------------------------------------------------------------------------

Float32* Mixer::getRightOut()
{
	return mRightBuffer.getAlignedBuffer();
}

// -----------------------------------------------------------------------------

void Mixer::reset()
{
	if (mMixerData)
	{
		for (UInt32 i = 0; i < mMaxChanels; i++)
		{
			mMixerData[i].lastPan	= 0.5;
			mMixerData[i].lastLevel	= 0.0;
		}
	}
}

// -----------------------------------------------------------------------------

void Mixer::clear()
{
	mChanelBuffer.clear();
	
	mLeftBuffer.clear();
	mRightBuffer.clear();

	memset(&hp[0], 0, 4 * sizeof(Float32));
}

// -----------------------------------------------------------------------------

void Mixer::cleanup()
{
	/*
	mChanelBuffer.deallocate();

	mLeftBuffer.deallocate();
	mRightBuffer.deallocate();
	*/
	if (mMixerData)
		delete[] mMixerData;
}

// -----------------------------------------------------------------------------
