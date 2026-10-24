/*
 *  Grain.h
 *  Automat
 *
 *  Created by Stefan Beck on 23.07.07.
 *  Copyright (c) 2006-2008 Stefan Beck (stiwi@alphakanal, loopmasta). MIT License, see LICENSE.
 *
 */

// -----------------------------------------------------------------------------

#ifndef GRAIN_H
#define GRAIN_H

#include "Inlines.h"

// -----------------------------------------------------------------------------

class Grain
{
	enum
	{
		kGrain_Attack,
		kGrain_Hold,
		kGrain_Decay,
		kGrain_Wait,
		kGrain_Done,
	};

public:
				Grain();

	void		start(Float32 phase);
	void		sample(Float32 phaseInc, Float32 attack, Float32 hold, Float32 decay, Float32 wait, UInt32 dataSize, Float32* data, UInt32 bufferSize, Float32* buffer);
	
	bool		decay();
	bool		done();
	
	void		reset();
	
private:

	UInt32		mStatus;
	
	Float32		mPhase, mPhaseInc, mLevel, mWait, mAttack, mHold, mDecay;
	Float32		mLastLevel, mLastPhaseInc;
};

// -----------------------------------------------------------------------------

#endif
