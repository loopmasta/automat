/*
 *  Envelope.cpp
 *  Automat
 *
 *  Created by Stefan Beck on 09.04.06.
 *  Copyright (c) 2006-2008 Stefan Beck (stiwi@alphakanal, loopmasta). MIT License, see LICENSE.
 *
 */

// -----------------------------------------------------------------------------

#include "Envelope.h"

// -----------------------------------------------------------------------------

Envelope::Envelope()
{
	reset();
}

// -----------------------------------------------------------------------------

void Envelope::initialize(Float32 sampleRate, Float32 maxTime)
{ 
	mSampleRate	= sampleRate;
	mMaxTime	= maxTime;
}

// -----------------------------------------------------------------------------

void Envelope::setParameter(Float32 attack, Float32 hold, Float32 decay, Float32 sustain, Float32 release, Float32 velo_depth)
{ 	
	//	printf("Envelope::setParameter attack %f, hold %f,  decay %f, sustain %f,  release %f, velo_amount %f\n", attack, hold, decay, sustain,  release, velo_depth);

	Float32	time	= (mSampleRate / kMaxRenderSlice) * mMaxTime;
	
	mAttack			= 1.0 / (1.0 + pow3(attack) * time);
	mHold			= 1.0 / (1.0 + pow3(hold) * time);
	mDecay			= 1.0 / (1.0 + pow3(decay) * time);
	mSustain		= sustain;
	mRelease		= 1.0 / (1.0 + pow3(release) * time);

	mVeloAmount		= 1.0;

	if (velo_depth < 0.5)
		mVeloAmount = linear(velo_depth * 2.0, 1.0 - mVelocity, 1.0);
	else if (velo_depth > 0.5)
		mVeloAmount = linear((velo_depth - 0.5) * 2.0, 1.0, mVelocity);
}

// -----------------------------------------------------------------------------

Float32 Envelope::getEnvelope(UInt32 chunkSize, UInt32 state)
{ 
	//	printf("Envelope::getEnvelope state %ld, mode %ld\n", state, mMode);

	mEndFrame += chunkSize;
	
	switch(state)
	{
		case kNoteState_Attacked:
		case kNoteState_Sostenutoed:
		case kNoteState_ReleasedButSostenutoed:
		case kNoteState_ReleasedButSustained:
		{
			//	printf("Envelope::getEnvelope attacked -> %ld, mode %ld\n", state, mMode);

			switch(mMode)
			{
				case kEnvelope_Attack:
				{
					mLevel += mAttack;
			
					if (mLevel > 1.0)
					{
						mLevel	= 1.0;
						mMode	= (mHold == 1.0) ? kEnvelope_Decay : kEnvelope_Hold;
					}
				}
				break;
				
				case kEnvelope_Hold:
				{
					mWait += mHold;
					
					if (mWait > 1.0)
						mMode = kEnvelope_Decay;
				}
				break;
				
				case kEnvelope_Decay:
				{
					mLevel -= mDecay;
					
					if (mLevel < mSustain)
					{
						mLevel	= mSustain;
						mMode	= kEnvelope_Sustain;
					}
				
				}
				break;
			
				case kEnvelope_Sustain:
				{
					mLevel = mSustain;
				}
				break;

				default:
					break;
			}			
		}
		break;
			
		case kNoteState_Released:
		case kNoteState_FastReleased:
		{
			mLevel -= mRelease;
			
			if (mLevel < 0.0)
			{
				mLevel	= 0.0;
				mMode	= kEnvelope_Done;
			}

			//	printf("Envelope::getEnvelope released -> state %ld, level %lf, delta %lf, mode %i\n", state, mLevel, mRelease, mMode);
		}
		break;

		default:
			break;
	}
	
	return mLevel * mVeloAmount;
}

// -----------------------------------------------------------------------------

Float32 Envelope::getLevel()
{
	return mLevel;
}

// -----------------------------------------------------------------------------

bool Envelope::isDone()
{
	return mMode == kEnvelope_Done;
}

// -----------------------------------------------------------------------------

UInt32 Envelope::getEndFrame()
{
	return mEndFrame;
}

// -----------------------------------------------------------------------------

void Envelope::start(Float32 velocity)
{ 
	//	printf("Envelope::startEnvelope velocity%f\n", velocity);

	mLevel		= 0.0;
	mWait		= 0.0;
	
	mVelocity	= velocity;
	
	mEndFrame	= 0xFFFFFFFF;
	mMode		= kEnvelope_Attack;
}

// -----------------------------------------------------------------------------

void Envelope::reset()
{
	mLevel		= 0.0;
	mWait		= 0.0;
	
	mVelocity	= 0.0;
	
	mEndFrame	= 0xFFFFFFFF;
	mMode		= kEnvelope_Done;
}

// -----------------------------------------------------------------------------
