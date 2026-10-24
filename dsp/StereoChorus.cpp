/*
 *  StereoChorus.cpp
 *  Automat
 *
 *  Created by Stefan Beck on 09.04.06.
 *  Copyright 2006 alphakanal. All rights reserved.
 *
 */

// -----------------------------------------------------------------------------

#include "StereoChorus.h"

// -----------------------------------------------------------------------------

StereoChorus::StereoChorus()
{
	mType		= kSpt_Off;
	mMode		= kSpt_Mod;
	mSpeed1		= 0.0;
	mSpeed2		= 0.0;
	mFeedBack	= 0.0;
	mDepth		= 0.0;
	mMix		= 0.0;
	
	reset();
}

// -----------------------------------------------------------------------------
/*
StereoChorus::~StereoChorus()
{
	cleanup();
}
*/
// -----------------------------------------------------------------------------

void StereoChorus::initialize(Float32 sampleRate, Float32 maxSpeed)
{
	mLfoTuning		= (kMaxRenderSlice / sampleRate) * maxSpeed;
	
	mDelaySize		= lrintf(kSptMaxDelay * sampleRate);
	mDelayOffset	= (Float32)lrintf(kSptOffDelay * sampleRate);
	mDelayMod		= (Float32)(mDelaySize - mDelayOffset) - 1.0;
		
	//	printf("StereoChorus::initialize mDelaySize %ld, mDelayOffset %f, mDelayMod %f\n", mDelaySize, mDelayOffset, mDelayMod);

	//	cleanup();

	mRingBufferL.allocate(mDelaySize);
	mRingBufferR.allocate(mDelaySize);
	
	reset();	
	clear();
}

// -----------------------------------------------------------------------------

void StereoChorus::setParameter(UInt32 type, UInt32 mode, Float32 speed1, Float32 speed2, Float32 feedBack, Float32 depth, Float32 mix)
{
	//	printf("StereoChorus::setParameter type %ld, mode %ld, speed1 %f, speed2 %f, feedBack %f, depth %f, mix %f\n", type, mode, speed1, speed2, feedBack, depth, mix);

	if (type != mType || mode != mMode)
	{
		reset();
		clear();
	}

	mType		= type;
	mMode		= mode;
	mSpeed1		= speed1;
	mSpeed2		= speed2;
	mFeedBack	= feedBack;
	mDepth		= depth;
	mMix		= mix;
}


// -----------------------------------------------------------------------------

void StereoChorus::getEffect(UInt32 bufferSize, Float32* leftBuffer, Float32* rightBuffer, Float32* modulation)
{
	if (mType == kSpt_Off)
		return;
		
	Float32	speed1		= mSpeed1;
	Float32	speed2		= mSpeed2;
	Float32	feedBack	= mFeedBack;
	Float32	depth		= mDepth;
	Float32	mix			= mMix;
	
	if (modulation)
	{
		speed1		*= modulation[kModulation_Speed1];
		speed2		*= modulation[kModulation_Speed2];
		feedBack	*= modulation[kModulation_FeedBack];
		depth		*= modulation[kModulation_Depth];
		mix			*= modulation[kModulation_Mix];
	}
	
	speed1		= pow3(speed1);
	speed2		= pow3(speed2);
	depth		= pow3(depth);

	switch(mType)
	{
		case kSpt_Chorus: 
			getChorus(bufferSize, leftBuffer, rightBuffer, speed1, speed2, feedBack, depth, mix);
			break;
			
		case kSpt_Phaser: 
			getPhaser(bufferSize, leftBuffer, rightBuffer, speed1, speed2, feedBack, depth, mix);
			break;
	}
}

// -----------------------------------------------------------------------------

void StereoChorus::getChorus(UInt32 bufferSize, Float32* leftBuffer, Float32* rightBuffer, Float32 speed1, Float32 speed2, Float32 feedBack, Float32 depth, Float32 mix)
{	
	//	printf("StereoChorus::getChorus bufferSize %ld, mDelaySize  %ld\n", bufferSize, mDelaySize);

	feedBack	= feedBack * 0.99;
	depth		= 0.2 + depth * 0.8;
	
	switch(mMode)
	{
		default:
		case kSpt_Mod:
		{
			speed1	*= mLfoTuning;
			speed2	*= mLfoTuning;
			
			triMod	mod;
			Float32	delay = mDelayMod * depth;
			
			so_triMod(&mOsc[0], speed1, &mOsc[1], speed2, &mod);
	
			mState[0]	= mDelayOffset + mod.center * delay;
			mState[1]	= mDelayOffset + mod.left * delay;
			mState[2]	= mDelayOffset + mod.right * delay;
		}
		break;
			
		case kSpt_Fix:
		{
			Float32	delay = mDelayMod * depth;

			mState[0] = mDelayOffset + 0.5 * delay;
			mState[1] = mDelayOffset + speed1 * delay;
			mState[2] = mDelayOffset + speed2 * delay;
		}
		break;
	}
							
	//	printf("StereoChorus::getChorus mState[0] %f, mState[1] %f, mState[2] %f\n", mState[0], mState[1], mState[2]);
	
	Float32		ramp		= 1.0 / bufferSize;

	Float32		l_delay0	= mState[3];
	Float32		delayDt0	= (mState[0] - l_delay0) * ramp;
	
	Float32		l_delay1	= mState[4];
	Float32		delayDt1	= (mState[1] - l_delay1) * ramp;
	
	Float32		l_delay2	= mState[5];
	Float32		delayDt2	= (mState[2] - l_delay2) * ramp;
		
	Float32		l_feedBack	= mLastFeedBack;
	Float32		fbDelta		= (feedBack - l_feedBack) * ramp;
	
	Float32		l_mix		= mLastMix;	
	Float32		mixDelta	= (mix - l_mix) * ramp;
	
	SInt32		readM1;
	UInt32		read0, read1, read2;
	Float32		pointer, offset, in, out;
		
	vFloat		vFloatRead	= (vFloat){0.0, 0.0, 0.0, 0.0};
	vFloat		vFractional	= (vFloat){0.0, 0.0, 0.0, 0.0};
	vSInt32		vIntRead;
	
	Float32		*fRead		= (Float32*)&vFloatRead;
	Float32		*fFrac		= (Float32*)&vFractional;
	SInt32		*iRead		= (SInt32*)&vIntRead;

	UInt32		l_delaySize	= mDelaySize;
	UInt32		l_pointer	= mRingPointer;
		
	Float32*	ringBufferL	= mRingBufferL.getAlignedBuffer();
	Float32*	ringBufferR	= mRingBufferR.getAlignedBuffer();

	while (bufferSize--)
	{
		l_delay0	+= delayDt0;
		l_delay1	+= delayDt1;		
		l_delay2	+= delayDt2;
		
		l_feedBack	+= fbDelta;
		l_mix		+= mixDelta;
		
		pointer		= l_pointer - 2.0;
		offset		= pointer - l_delay0;
		
		if (offset < 0.0)
			offset += l_delaySize;
		
		fRead[0]	= offset;
		offset		= pointer - l_delay1;
		
		if (offset < 0.0)
			offset += l_delaySize;
		
		fRead[1]	= offset;
		offset		= pointer - l_delay2;
		
		if (offset < 0.0)
			offset += l_delaySize;
		
		fRead[2]	= offset;
		fRead[3]	= 0.0;

		#ifdef __SSE__

		vIntRead	= _mm_cvttps_epi32(vFloatRead);
		vFractional	= _mm_sub_ps(vFloatRead, _mm_cvtepi32_ps(vIntRead));

		#else ifdef __VEC__

		vIntRead	= vec_cts(vFloatRead, 0);
		vFractional	= vec_sub(vFloatRead, vec_ctf(vIntRead, 0));

		#endif
		
		read0	= iRead[1];
		readM1	= read0 - 1;
		
		if (readM1 < 0)
			readM1 += l_delaySize;
			
		read1	= (read0 + 1) % l_delaySize;
		read2	= (read1 + 1) % l_delaySize;
		
		out		= cubic(fFrac[1], ringBufferL[readM1], ringBufferL[read0], ringBufferL[read1], ringBufferL[read2]);
		
		read0	= iRead[0];
		readM1	= read0 - 1;
		
		if (readM1 < 0)
			readM1 += l_delaySize;
			
		read1	= (read0 + 1) % l_delaySize;
		read2	= (read1 + 1) % l_delaySize;
		
		out		+= cubic(fFrac[0], ringBufferL[readM1], ringBufferL[read0], ringBufferL[read1], ringBufferL[read2]);
		out		*= 0.5;
		
		in						= *leftBuffer;
		ringBufferL[l_pointer]	= in + out * l_feedBack;
		*leftBuffer++			= quadMix(in, out, l_mix);
	
		out		= cubic(fFrac[0], ringBufferR[readM1], ringBufferR[read0], ringBufferR[read1], ringBufferR[read2]);
		
		read0	= iRead[2];
		readM1	= read0 - 1;
		
		if (readM1 < 0)
			readM1 += l_delaySize;
			
		read1	= (read0 + 1) % l_delaySize;
		read2	= (read1 + 1) % l_delaySize;
		
		out		+= cubic(fFrac[2], ringBufferR[readM1], ringBufferR[read0], ringBufferR[read1], ringBufferR[read2]);
		out		*= 0.5;
		
		in						= *rightBuffer;
		ringBufferR[l_pointer]	= in + out * l_feedBack;
		*rightBuffer++			= quadMix(in, out, l_mix);

		l_pointer				= (l_pointer + 1) % l_delaySize;
	}

	mState[3]		= l_delay0;
	mState[4]		= l_delay1;
	mState[5]		= l_delay2;
	
	mLastFeedBack	= l_feedBack;
	mLastMix		= l_mix;
	
	mRingPointer	= l_pointer;
}

// -----------------------------------------------------------------------------

void StereoChorus::getPhaser(UInt32 bufferSize, Float32* leftBuffer, Float32* rightBuffer, Float32 speed1, Float32 speed2, Float32 feedBack, Float32 depth, Float32 mix)
{
	feedBack	= feedBack * 0.92;
	depth		= depth * 0.99;

	switch(mMode)
	{
		default:
		case kSpt_Mod:
		{
			speed1 *= mLfoTuning;
			speed2 *= mLfoTuning;
			
			triMod	mod;
			
			so_triMod(&mOsc[0], speed1, &mOsc[1], speed2, &mod);
			
			mState[0] = 0.99 - mod.center * depth;
			mState[1] = 0.99 - mod.left * depth;
			mState[2] = 0.99 - mod.right * depth;
		}
		break;
			
		case kSpt_Fix:
		{
			mState[0] = 0.99 - 0.5 * depth;
			mState[1] = 0.99 - speed1 * depth;
			mState[2] = 0.99 - speed2 * depth;
		}
		break;
	}
		
	Float32		ramp		= 1.0 / bufferSize;

	Float32		l_mod0		= mState[3];
	Float32		modDt0		= (mState[0] - l_mod0) * ramp;
	
	Float32		l_mod1		= mState[4];
	Float32		modDt1		= (mState[1] - l_mod1) * ramp;
	
	Float32		l_mod2		= mState[5];
	Float32		modDt2		= (mState[2] - l_mod2) * ramp;
		
	Float32*	ringBufferL	= mRingBufferL.getAlignedBuffer();
	Float32*	ringBufferR	= mRingBufferR.getAlignedBuffer();

	Float32		l_lmem1		= ringBufferL[0];
	Float32		l_lmem2		= ringBufferL[1];
	Float32		l_lmem3		= ringBufferL[2];
	Float32		l_lmem4		= ringBufferL[3];
	Float32		l_lOut1		= ringBufferL[4];

	Float32		l_lmem5		= ringBufferL[5];
	Float32		l_lmem6		= ringBufferL[6];
	Float32		l_lmem7		= ringBufferL[7];
	Float32		l_lmem8		= ringBufferL[8];
	Float32		l_lOut2		= ringBufferL[9];
	
	Float32		l_rmem1		= ringBufferR[0];
	Float32		l_rmem2		= ringBufferR[1];
	Float32		l_rmem3		= ringBufferR[2];
	Float32		l_rmem4		= ringBufferR[3];
	Float32		l_rOut1		= ringBufferR[4];

	Float32		l_rmem5		= ringBufferR[5];
	Float32		l_rmem6		= ringBufferR[6];
	Float32		l_rmem7		= ringBufferR[7];
	Float32		l_rmem8		= ringBufferR[8];
	Float32		l_rOut2		= ringBufferR[9];

	Float32		l_feedBack	= mLastFeedBack;
	Float32		fbDelta		= (feedBack - l_feedBack) * ramp;
	
	Float32		l_mix		= mLastMix;	
	Float32		mixDelta	= (mix - l_mix) * ramp;
	
	Float32		in, out, l_inv0, l_inv1, l_inv2;
			
	while (bufferSize--)
	{		
		l_feedBack		+= fbDelta;
		l_mix			+= mixDelta;
		
		l_mod0			+= modDt0;
		l_mod1			+= modDt1;
		l_mod2			+= modDt2;

		l_inv0			 = -l_mod0;
		l_inv1			 = -l_mod1;
		l_inv2			 = -l_mod2;
	
		in				= *leftBuffer + l_lOut1 * l_feedBack;
		out				= in  * l_inv1 + l_lmem1;
		l_lmem1			= out * l_mod1 + in;
		
		in				= out;
		out				= in  * l_inv1 + l_lmem2;
		l_lmem2			= out * l_mod1 + in;

		in				= out;
		out				= in  * l_inv1 + l_lmem3;
		l_lmem3			= out * l_mod1 + in;
		
		in				= out;
		out				= in  * l_inv1 + l_lmem4;
		l_lmem4			= out * l_mod1 + in;
		
		l_lOut1			= out;

		in				= *leftBuffer + l_lOut2 * l_feedBack;
		out				= in  * l_inv0 + l_lmem5;
		l_lmem5			= out * l_mod0 + in;
		
		in				= out;
		out				= in  * l_inv0 + l_lmem6;
		l_lmem6			= out * l_mod0 + in;

		in				= out;
		out				= in  * l_inv0 + l_lmem7;
		l_lmem7			= out * l_mod0 + in;
		
		in				= out;
		out				= in  * l_inv0 + l_lmem8;
		l_lmem8			= out * l_mod0 + in;
		
		l_lOut2			= out;
		
		in				= *leftBuffer;
		out				= (l_lOut1 + l_lOut2) * 0.5;
		*leftBuffer++	= quadMix(in, out, l_mix);
		
		in				= *rightBuffer + l_rOut1 * l_feedBack;
		out				= in  * l_inv0 + l_rmem1;
		l_rmem1			= out * l_mod0 + in;

		in				= out;
		out				= in  * l_inv0 + l_rmem2;
		l_rmem2			= out * l_mod0 + in;		

		in				= out;
		out				= in  * l_inv0 + l_rmem3;
		l_rmem3			= out * l_mod0 + in;
		
		in				= out;
		out				= in  * l_inv0 + l_rmem4;
		l_rmem4			= out * l_mod0 + in;
		
		l_rOut1			= out;

		in				= *rightBuffer + l_rOut2 * l_feedBack;
		out				= in  * l_inv2 + l_rmem5;
		l_rmem5			= out * l_mod2 + in;
		
		in				= out;
		out				= in  * l_inv2 + l_rmem6;
		l_rmem6			= out * l_mod2 + in;

		in				= out;
		out				= in  * l_inv2 + l_rmem7;
		l_rmem7			= out * l_mod2 + in;
		
		in				= out;
		out				= in  * l_inv2 + l_rmem8;
		l_rmem8			= out * l_mod2 + in;
		
		l_rOut2			= out;
		
		in				= *rightBuffer;
		out				= (l_rOut1 + l_rOut2) * 0.5;
		*rightBuffer++	= quadMix(in, out, l_mix);
	}
	
	mState[3]		= l_mod0;
	mState[4]		= l_mod1;
	mState[5]		= l_mod2;

	ringBufferL[0]	= l_lmem1;
	ringBufferL[1]	= l_lmem2;
	ringBufferL[2]	= l_lmem3;
	ringBufferL[3]	= l_lmem4;
	ringBufferL[4]	= l_lOut1;
	
	ringBufferL[5]	= l_lmem5;
	ringBufferL[6]	= l_lmem6;
	ringBufferL[7]	= l_lmem7;
	ringBufferL[8]	= l_lmem8;
	ringBufferL[9]	= l_lOut2;

	ringBufferR[0]	= l_rmem1;
	ringBufferR[1]	= l_rmem2;
	ringBufferR[2]	= l_rmem3;
	ringBufferR[3]	= l_rmem4;
	ringBufferR[4]	= l_rOut1;

	ringBufferR[5]	= l_rmem5;
	ringBufferR[6]	= l_rmem6;
	ringBufferR[7]	= l_rmem7;
	ringBufferR[8]	= l_rmem8;
	ringBufferR[9]	= l_rOut2;

	mLastFeedBack	= l_feedBack;
	mLastMix		= l_mix;
}

// -----------------------------------------------------------------------------

void StereoChorus::reset()
{
	mRingPointer	= 0;
	
	mLastFeedBack	= 0.0f;
	mLastMix		= 0.0f;	
}

// -----------------------------------------------------------------------------

void StereoChorus::clear()
{
	mRingBufferL.clear();
	mRingBufferR.clear();
	
	so_clear(&mOsc[0]);
	so_clear(&mOsc[1]);
	
	memset(&mState[0], 0, 6 * sizeof(Float32));	
}

// -----------------------------------------------------------------------------
/*
void StereoChorus::cleanup()
{   
	mRingBufferL.deallocate();
	mRingBufferR.deallocate();
}
*/
// -----------------------------------------------------------------------------
