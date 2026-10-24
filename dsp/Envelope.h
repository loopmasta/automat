/*
 *  Envelope.h
 *  Automat
 *
 *  Created by Stefan Beck on 09.04.06.
 *  Copyright (c) 2006-2008 Stefan Beck (stiwi@alphakanal, loopmasta). MIT License, see LICENSE.
 *
 */

// -----------------------------------------------------------------------------

#ifndef ENVELOPE_H
#define ENVELOPE_H

#include "SynthNote.h"

#include "Constants.h"
#include "Inlines.h"

// -----------------------------------------------------------------------------

class Envelope
{
	enum
	{
		kEnvelope_Attack	= 0,
		kEnvelope_Hold		= 1,
		kEnvelope_Decay		= 2,
		kEnvelope_Sustain	= 3,
		kEnvelope_Done		= 4,
	};

public:

			Envelope();
			
	void	initialize(Float32 sampleRate, Float32 maxTime);

	void	setParameter(Float32 attack, Float32 hold, Float32 decay, Float32 sustain, Float32 release, Float32 velo_depth);
	Float32	getEnvelope(UInt32 chunkSize, UInt32 state);
	
	Float32	getLevel();
	bool	isDone();
	UInt32	getEndFrame();
	
	void	start(Float32 velocity);
	void	reset();
	
private:

	Float32	mSampleRate, mMaxTime;
	
	Float32	mLevel, mWait;
	Float32	mVelocity, mVeloAmount;
	Float32	mAttack, mHold, mDecay, mSustain, mRelease;
	
	UInt32	mEndFrame, mMode;
};

// -----------------------------------------------------------------------------

#endif

// -----------------------------------------------------------------------------
