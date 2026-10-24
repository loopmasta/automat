/*
 *  StereoAmp.cpp
 *  Automat
 *
 *  Created by Stefan Beck on 09.04.06.
 *  Copyright 2006 alphakanal. All rights reserved.
 *
 */

// -----------------------------------------------------------------------------

#include "StereoAmp.h"

// -----------------------------------------------------------------------------

StereoAmp::StereoAmp()
{
	mLevel	= 0.0;
	mSpread	= 0.0;

	reset();
}

// -----------------------------------------------------------------------------

void StereoAmp::setParameter(Float32 level)
{ 
	mLevel = level;
}

// -----------------------------------------------------------------------------

void StereoAmp::setParameter(Float32 level, Float32 spread)
{ 
	mLevel	= level;
	mSpread	= spread;
}

// -----------------------------------------------------------------------------

void StereoAmp::getAmp(UInt32 bufferSize, Float32* leftBuffer, Float32* rightBuffer, Float32 envelope, Float32* modulation)
{ 
	Float32	level = mLevel;
	
	if (modulation)
		level *= modulation[kModulation_Level];

	level = pow3(envelope * level);
	
	//	printf("StereoAmp::getAmp level %f, envelope %f\n", level, envelope);

	const int size		= bufferSize;

	Float32	ramp		= 1.0 / size;

	Float32 *vLevel		= (Float32*)&b1;
	Float32	levelDelta	= (level - mLastLevel) * ramp;
	
	//	out = in * level * modulation * envelope;

    vDSP_vramp(&mLastLevel, &levelDelta, vLevel, 1, size);
		
	vDSP_vmul(leftBuffer, 1, vLevel, 1, leftBuffer, 1, size);	//	leftBuffer * vLevel -> leftBuffer
	vDSP_vmul(rightBuffer, 1, vLevel, 1, rightBuffer, 1, size);	//	rightBuffer * vLevel -> rightBuffer
				
	mLastLevel = level;
}

// -----------------------------------------------------------------------------

void StereoAmp::getAmpWithSpread(UInt32 bufferSize, Float32* leftBuffer, Float32* rightBuffer, Float32 envelope, Float32* modulation)
{ 
	Float32	level	= mLevel;
	Float32	spread	= mSpread;
	
	if (modulation)
	{
		level	*= modulation[kModulation_Level];
		spread	*= modulation[kModulation_Spread];
	}

	level	= pow3(envelope * level);
	spread	= (0.5 + spread * 0.5);
	
	//	printf("StereoAmp::getAmp level %f, spread %f, envelope %f\n", level, spread, envelope);

	const int size		= bufferSize;

	Float32 *vLevel		= (Float32*)&b1;
	Float32 *vSpread	= (Float32*)&b2;
	Float32 *vNegSpr	= (Float32*)&b3;

	Float32 *vLeft		= (Float32*)&b4;
	Float32 *vRight		= (Float32*)&b5;
	Float32 *vMix		= (Float32*)&b6;
	
	Float32	one			= 1.0;
	Float32	*vOne		= (Float32*)&b7;
	
	Float32	ramp		= 1.0 / size;
	
	Float32	levelDelta	= (level - mLastLevel) * ramp;
	Float32	spreadDelta	= (spread - mLastSpread) * ramp;
	
	//	out = mix(right, left, spread) * level;

    vDSP_vramp(&mLastLevel, &levelDelta, vLevel, 1, size);
    vDSP_vramp(&mLastSpread, &spreadDelta, vSpread, 1, size);
	
	vDSP_vfill(&one, vOne, 1, size);
	vDSP_vsub(vSpread, 1, vOne, 1, vNegSpr, 1, size);		//	1.0 - vSpread -> vNegSpr
	
	vDSP_vmul(leftBuffer, 1, vSpread, 1, vLeft, 1, size);	//	leftBuffer * vSpread -> vLeft
	vDSP_vmul(rightBuffer, 1, vNegSpr, 1, vRight, 1, size);	//	rightBuffer * vNegSpr -> vRight
	vDSP_vadd(vLeft, 1, vRight, 1, vMix, 1, size);			//	vLeft + vRight -> vMix
	vDSP_vmul(vMix, 1, vLevel, 1, leftBuffer, 1, size);		//	vMix * vLevel -> leftBuffer	

	vDSP_vmul(leftBuffer, 1, vNegSpr, 1, vLeft, 1, size);	//	leftBuffer * vNegSpr -> vLeft
	vDSP_vmul(rightBuffer, 1, vSpread, 1, vRight, 1, size);	//	rightBuffer * vSpread -> vRight
	vDSP_vadd(vLeft, 1, vRight, 1, vMix, 1, size);			//	vLeft + vRight -> vMix
	vDSP_vmul(vMix, 1, vLevel, 1, rightBuffer, 1, size);	//	vMix * vLevel -> rightBuffer	
				
	mLastLevel	= level;
	mLastSpread	= spread;
}

// -----------------------------------------------------------------------------

void StereoAmp::reset()
{
	mLastLevel	= 0.0;
	mLastSpread	= 0.0;	
}

// -----------------------------------------------------------------------------
