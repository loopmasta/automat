/*
 *  StereoDelay.cpp
 *  Automat
 *
 *  Created by Stefan Beck on 09.04.06.
 *  Copyright (c) 2006-2008 Stefan Beck (stiwi@alphakanal, loopmasta). MIT License, see LICENSE.
 *
 */

// -----------------------------------------------------------------------------

#include "StereoDelay.h"

// -----------------------------------------------------------------------------

StereoDelay::StereoDelay()
{
	mLeftTime		= kDly_Off;
	mRightTime		= kDly_Off;
	
	mFilterType		= kDlyFilter_Off;
	mFilterTarget	= kDlyFilter_Delay;
	mCutOff			= 0.0;
	mResonance		= 0.0;

	mFeedBack		= 0.0;
	mCrossFeed		= 0.0;	
	mMix			= 0.0;
	
	mSecondsPerBeat	= 0.5;
	mRingPointer	= 0;
	
	reset();
}

// -----------------------------------------------------------------------------
/*
StereoDelay::~StereoDelay()
{
	cleanup();
}
*/
// -----------------------------------------------------------------------------

void StereoDelay::initialize(Float32 sampleRate, Float32 maxSeconds)
{
	if (sampleRate < 44100.0)
		sampleRate = 44100.0;

	mSampleRate = sampleRate;
	mRingSize	= lrintf(sampleRate * maxSeconds);
		
	//	cleanup();
	
	//	printf("StereoDelay::initialize mRingSize %ld\n", mRingSize);

	mLeftRingBuffer.allocate(mRingSize);
	mRightRingBuffer.allocate(mRingSize);
	
	mLeftDelay.allocate(kMaxRenderSlice);
	mRightDelay.allocate(kMaxRenderSlice);

	mLeftFBack.allocate(kMaxRenderSlice);
	mRightFBack.allocate(kMaxRenderSlice);
	
	mFilter.initialize(sampleRate);
	
	reset();
	clear();
}

// -----------------------------------------------------------------------------

void StereoDelay::setParameter(UInt32 leftTime, UInt32 rightTime, UInt32 filterType, UInt32 filterTarget, Float32 cutOff, Float32 resonance, Float32 feedBack, Float32 crossFeed, Float32 mix)
{
	if (leftTime != mLeftTime || rightTime != mRightTime || filterType != mFilterType)
	{
		reset();
		clear();
	}

	mLeftTime		= leftTime;
	mRightTime		= rightTime;
	mFilterType		= filterType;
	mFilterTarget	= filterTarget;
	mCutOff			= cutOff;
	mResonance		= resonance;
	mFeedBack		= feedBack;
	mCrossFeed		= crossFeed;	
	mMix			= mix;
}

// -----------------------------------------------------------------------------

void StereoDelay::setBPM(Float64 bpm)
{
	mSecondsPerBeat = (60.0 / bpm) * mSampleRate;
}

// -----------------------------------------------------------------------------

void StereoDelay::getEffect(UInt32 bufferSize, Float32* leftBuffer, Float32* rightBuffer, Float32* modulation)
{	
	if (mLeftTime == kDly_Off && mRightTime == kDly_Off)
		return;
		
	Float32 cutOff		= mCutOff;
	Float32 resonance	= mResonance;
	Float32 feedBack	= mFeedBack;
	Float32 crossFeed	= mCrossFeed;	
	Float32 mix			= mMix;
	
	if (modulation)
	{
		cutOff		*= modulation[kModulation_CutOff];
		resonance	*= modulation[kModulation_Reso];
		feedBack	*= modulation[kModulation_FeedBack];
		crossFeed	*= modulation[kModulation_CrossFeed];
		mix			*= modulation[kModulation_Mix];
	}
	
	mix = pow2(mix);

	mFilter.setParameter(mFilterType, cutOff, resonance, 0.5, 0.5, 0.5);

	switch (mFilterTarget)
	{
		default:
		case kDlyFilter_Delay:
			readDelay(bufferSize);
			getFeedBack(bufferSize, feedBack, crossFeed);
			writeDelay(bufferSize, leftBuffer, rightBuffer);
			mFilter.getFilter(bufferSize, mLeftDelay.getAlignedBuffer(), mRightDelay.getAlignedBuffer(), 0.0, NULL);
			break;
			
		case kDlyFilter_FeedBack:
			readDelay(bufferSize);			
			getFeedBack(bufferSize, feedBack, crossFeed);
			mFilter.getFilter(bufferSize, mLeftFBack.getAlignedBuffer(), mRightFBack.getAlignedBuffer(), 0.0, NULL);
			writeDelay(bufferSize, leftBuffer, rightBuffer);
			break;

		case kDlyFilter_Both:
			readDelay(bufferSize);			
			mFilter.getFilter(bufferSize, mLeftDelay.getAlignedBuffer(), mRightDelay.getAlignedBuffer(), 0.0, NULL);
			getFeedBack(bufferSize, feedBack, crossFeed);			
			writeDelay(bufferSize, leftBuffer, rightBuffer);
			break;
	}

	writeToOut(bufferSize, leftBuffer, rightBuffer, mix);	

}

// -----------------------------------------------------------------------------

void StereoDelay::readDelay(UInt32 bufferSize)
{	
	if (mLeftTime)
	{
		Float32*	leftDelay		= mLeftDelay.getAlignedBuffer();
		UInt32		leftOffset		= clamp_i(lrintf(mSecondsPerBeat * kDlySlices[mLeftTime]), 1, mRingSize);
		
		UInt32		l_size			= bufferSize;
		UInt32		l_ringPointer	= mRingPointer;
		
		UInt32		delay;
		Float32*	leftRingBuffer	= mLeftRingBuffer.getAlignedBuffer();
		
		while(l_size--)
		{
			delay			= ((l_ringPointer - leftOffset) + mRingSize) % mRingSize;		
			*leftDelay++	= leftRingBuffer[delay];

			l_ringPointer	= (l_ringPointer + 1) % mRingSize;
		}
	}
	
	if (mRightTime)
	{
		Float32*	rightDelay		= mRightDelay.getAlignedBuffer();
		UInt32		rightOffset		= clamp_i(lrintf(mSecondsPerBeat * kDlySlices[mRightTime]), 1, mRingSize);

		UInt32		l_size			= bufferSize;
		UInt32		l_ringPointer	= mRingPointer;
		
		UInt32		delay;
		Float32*	rightRingBuffer	= mRightRingBuffer.getAlignedBuffer();

		while(l_size--)
		{
			delay			= ((l_ringPointer - rightOffset) + mRingSize) % mRingSize;		
			*rightDelay++	= rightRingBuffer[delay];

			l_ringPointer	= (l_ringPointer + 1) % mRingSize;
		}
	}
}

// -----------------------------------------------------------------------------

void StereoDelay::getFeedBack(UInt32 bufferSize, Float32 feedBack, Float32 crossFeed)
{
	Float32*	leftDelay		= mLeftDelay.getAlignedBuffer();
	Float32*	rightDelay		= mRightDelay.getAlignedBuffer();

	Float32*	leftFBack		= mLeftFBack.getAlignedBuffer();
	Float32*	rightFBack		= mRightFBack.getAlignedBuffer();

	const int size				= bufferSize;

	Float32*	vFeedBack		= (Float32*)&b1;
	Float32*	vCrossFeed		= (Float32*)&b2;
	Float32*	vNegCrFeed		= (Float32*)&b3;

	Float32*	vLeft			= (Float32*)&b4;
	Float32*	vRight			= (Float32*)&b5;
	Float32*	vFbLeft			= (Float32*)&b6;
	Float32*	vFbRight		= (Float32*)&b7;
	
	Float32		one				= 1.0;
	Float32*	vOne			= (Float32*)&b8;
	
	Float32		ramp			= 1.0 / size;
	
	Float32		feedBackDelta	= (feedBack - mLastFeedBack) * ramp;
	Float32		crossFeedDelta	= (crossFeed - mLastCrossFeed) * ramp;
	
	//	out = mix(right, left, crossFeed) * feedBack;

    vDSP_vramp(&mLastFeedBack, &feedBackDelta, vFeedBack, 1, size);
    vDSP_vramp(&mLastCrossFeed, &crossFeedDelta, vCrossFeed, 1, size);
	
	vDSP_vfill(&one, vOne, 1, size);
	vDSP_vsub(vCrossFeed, 1, vOne, 1, vNegCrFeed, 1, size);		//	1.0 - vCrossFeed -> vNegCrFeed
	
	vDSP_vmul(leftDelay, 1, vFeedBack, 1, vLeft, 1, size);		//	leftDelay * vFeedBack -> vLeft
	vDSP_vmul(rightDelay, 1, vFeedBack, 1, vRight, 1, size);	//	rightDelay * vFeedBack -> vRight

	vDSP_vmul(vLeft, 1, vNegCrFeed, 1, vFbLeft, 1, size);		//	vLeft * vNegCrFeed -> vFbLeft
	vDSP_vmul(vRight, 1, vCrossFeed, 1, vFbRight, 1, size);		//	vRight * vCrossFeed -> vFbRight
	
	vDSP_vadd(vFbLeft, 1, vFbRight, 1, leftFBack, 1, size);		//	vFbLeft + vFbRight -> leftFBack

	vDSP_vmul(vLeft, 1, vCrossFeed, 1, vFbLeft, 1, size);		//	vLeft * vCrossFeed -> vFbLeft
	vDSP_vmul(vRight, 1, vNegCrFeed, 1, vFbRight, 1, size);		//	vRight * vNegCrFeed -> vFbRight
	
	vDSP_vadd(vFbLeft, 1, vFbRight, 1, rightFBack, 1, size);	//	vFbLeft + vFbRight -> rightFBack

	mLastFeedBack	= feedBack;
	mLastCrossFeed	= crossFeed;	
}

// -----------------------------------------------------------------------------

void StereoDelay::writeDelay(UInt32 bufferSize, Float32* leftIn, Float32* rightIn)
{
	Float32*	leftFBack		= mLeftFBack.getAlignedBuffer();
	Float32*	righFBack		= mRightFBack.getAlignedBuffer();

	UInt32		l_ringPointer	= mRingPointer;
	UInt32		l_ringSize		= mRingSize;

	Float32*	leftRingBuffer	= mLeftRingBuffer.getAlignedBuffer();
	Float32*	rightRingBuffer	= mRightRingBuffer.getAlignedBuffer();

	while(bufferSize--)
	{
		leftRingBuffer[l_ringPointer]	= *leftIn++ + *leftFBack++;
		rightRingBuffer[l_ringPointer]	= *rightIn++ + *righFBack++;

		l_ringPointer = (l_ringPointer + 1) % l_ringSize;
	}
	
	mRingPointer = l_ringPointer;
}

// -----------------------------------------------------------------------------

void StereoDelay::writeToOut(UInt32 bufferSize, Float32* leftBuffer, Float32* rightBuffer, Float32 mix)
{
	Float32*	leftDelay	= mLeftDelay.getAlignedBuffer();
	Float32*	rightDelay	= mRightDelay.getAlignedBuffer();

	Float32		ramp		= 1.0 / bufferSize;

	Float32		l_mix		= mLastMix;	
	Float32		mixDelta	= (mix - mLastMix) * ramp;

	Float32		in, delay;
	
	while(bufferSize--)
	{
		l_mix			+= mixDelta;
	
		in				 = *leftBuffer;
		delay			 = *leftDelay++;
		*leftBuffer++	 = quadMix(in, delay, l_mix);
		
		in				 = *rightBuffer;
		delay			 = *rightDelay++;
		*rightBuffer++	 = quadMix(in, delay, l_mix);
	}
	
	mLastMix = l_mix;
}

// -----------------------------------------------------------------------------

void StereoDelay::reset()
{
	mLastCutOff		= 0.0;
	mLastResonance	= 0.0;
	mLastFeedBack	= 0.0;
	mLastCrossFeed	= 0.0;
	mLastMix		= 0.0;
	
	mRingPointer	= 0;
	
	mFilter.reset();
}

// -----------------------------------------------------------------------------

void StereoDelay::clear()
{
	mLeftRingBuffer.clear();
	mRightRingBuffer.clear();

	mLeftDelay.clear();
	mRightDelay.clear();

	mLeftFBack.clear();
	mRightFBack.clear();

	mFilter.clear();
}

// -----------------------------------------------------------------------------

/*
void StereoDelay::cleanup()
{
	mLeftRingBuffer.deallocate();
	mRightRingBuffer.deallocate();

	mLeftDelay.deallocate();
	mRightDelay.deallocate();

	mLeftFBack.deallocate();
	mRightFBack.deallocate();	
}
*/
// -----------------------------------------------------------------------------
