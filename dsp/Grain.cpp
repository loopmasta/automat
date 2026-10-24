/*
 *  Grain.cpp
 *  Automat
 *
 *  Created by Stefan Beck on 23.07.07.
 *  Copyright 2007 alphakanal. All rights reserved.
 *
 */

// -----------------------------------------------------------------------------

#include "Grain.h"

// -----------------------------------------------------------------------------

Grain::Grain()
{
	reset();
}

// -----------------------------------------------------------------------------

void Grain::start(Float32 phase)
{
	if (mStatus != kGrain_Done)
		return;

	mPhase		= phase;
	mStatus		= kGrain_Attack;
	
	mLevel		= 0.0;
	mLastLevel	= 0.0;

	mWait		= 0.0;
}

// -----------------------------------------------------------------------------

void Grain::sample(Float32 phaseInc, Float32 attack, Float32 hold, Float32 decay, Float32 wait, UInt32 dataSize, Float32* data, UInt32 bufferSize, Float32* buffer)
{
	if (mStatus == kGrain_Done)
		return;
		
	switch(mStatus)
	{
		case kGrain_Attack:
		{
			mLevel += attack;
	
			if (mLevel > 1.0)
			{
				mLevel	= 1.0;
				mStatus	= (hold == 1.0) ? kGrain_Decay : kGrain_Hold;
			}
		}
		break;
		
		case kGrain_Hold:
		{
			mWait += hold;
			
			if (mWait > 1.0)
			{
				mWait	= 0.0;
				mStatus = kGrain_Decay;
			}
		}
		break;
		
		case kGrain_Decay:
		{
			mLevel -= decay;
			
			if (mLevel < 0.0)
			{
				mLevel	= 0.0;
				mStatus	= (wait == 1.0) ? kGrain_Done : kGrain_Wait;
			}
		}
		break;

		case kGrain_Wait:
		{
			mWait += wait;
			
			if (mWait > 1.0)
			{
				mWait	= 0.0;
				mStatus = kGrain_Done;
			}
		}
		break;
	}
	
	Float32		ramp			= 1.0 / bufferSize;
	
	Float32		l_phase			= mPhase;
	
	Float32		l_phaseInc		= mLastPhaseInc;
	Float32		phaseDelta		= (phaseInc - l_phaseInc) * ramp;

	Float32		l_level			= mLastLevel;
	Float32		levelDelta		= (mLevel - l_level) * ramp;

	SInt32		readM1;
	UInt32		read0, read1, read2;
	Float32		fract, wave;

	while (bufferSize--)
	{
		l_phaseInc	+= phaseDelta;
		l_phase		+= l_phaseInc;
		l_level		+= levelDelta;
		
		if (l_phase >= dataSize)
			l_phase -= dataSize;
		
		read0	= (int)(l_phase);
		fract   = l_phase - read0;
		
		readM1	= read0 - 1;
		
		if (readM1 < 0)
			readM1 += dataSize;
			
		read1		 = (read0 + 1) % dataSize;
		read2		 = (read1 + 1) % dataSize;
		wave		 = cubic(fract, data[readM1], data[read0], data[read1], data[read2]);
					
		*buffer++	+= wave * l_level;
	}
	
	mPhase			= l_phase;
	mLastPhaseInc	= l_phaseInc;
	mLastLevel		= l_level;
}

// -----------------------------------------------------------------------------

bool Grain::decay()
{
	return mStatus == kGrain_Decay;
}

// -----------------------------------------------------------------------------

bool Grain::done()
{
	return mStatus == kGrain_Done;
}

// -----------------------------------------------------------------------------

void Grain::reset()
{
	mPhase			= 0.0;
	mStatus			= kGrain_Done;
	
	mLevel			= 0.0;
	mLastLevel		= 0.0;

	mWait			= 0.0;
}

// -----------------------------------------------------------------------------
