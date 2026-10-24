/*
 *  StereoFilter.cpp
 *  Automat
 *
 *  Created by Stefan Beck on 09.04.06.
 *  Copyright (c) 2006-2008 Stefan Beck (stiwi@alphakanal, loopmasta). MIT License, see LICENSE.
 *
 */

// -----------------------------------------------------------------------------

#include "StereoFilter.h"

// -----------------------------------------------------------------------------

StereoFilter::StereoFilter()
{
	mFilterType		= kFilter_Off;
	
	mCutOff			= 0.0;
	mResonance		= 0.0;
	
	mEnvelopeDepth	= 0.5;
	mKeyTrackDepth	= 0.5;
	mVelocityDepth	= 0.5;

	mKeyTrack		= 0.0;
	mVelocity		= 0.0;

	reset();
}

// -----------------------------------------------------------------------------

void StereoFilter::initialize(Float32 sampleRate)
{ 
	if (sampleRate < 44100.0)
		sampleRate = 44100.0;

	mSampleRate = sampleRate;

	reset();
	clear();
}

// -----------------------------------------------------------------------------

void StereoFilter::setParameter(UInt32 filterType, Float32 cutOff, Float32 resonance, Float32 envelopeDepth, Float32 keyTrackDepth, Float32 velocityDepth)
{
	if (filterType != mFilterType)
	{
		reset();
		clear();
	}

	mFilterType		= filterType;
	mCutOff			= cutOff;
	mResonance		= resonance;
	mEnvelopeDepth	= envelopeDepth;
	mKeyTrackDepth	= keyTrackDepth;
	mVelocityDepth	= velocityDepth;
}

// -----------------------------------------------------------------------------

void StereoFilter::getFilter(UInt32 bufferSize, Float32* leftInBuffer, Float32* rightInBuffer, Float32 envelope, Float32* modulation)
{
	getFilter(bufferSize, leftInBuffer, rightInBuffer, leftInBuffer, rightInBuffer, envelope, modulation);
}

// -----------------------------------------------------------------------------

void StereoFilter::getFilter(UInt32 bufferSize, Float32* leftInBuffer, Float32* rightInBuffer, Float32* leftOutBuffer, Float32* rightOutBuffer, Float32 envelope, Float32* modulation)
{
	if (mFilterType == kFilter_Off)
		return;
		
	Float32 keyTrack	= 1.0;
	Float32 velocity	= 1.0;

	if (mKeyTrackDepth < 0.5)
		keyTrack = linear(mKeyTrackDepth * 2.0, 1.0 / mKeyTrack, 1.0);
	else if (mKeyTrackDepth > 0.5)
		keyTrack = linear((mKeyTrackDepth - 0.5) * 2.0, 1.0, mKeyTrack);

	if (mVelocityDepth < 0.5)
		velocity = linear(mVelocityDepth * 2.0, 1.0 - mVelocity, 1.0);
	else if (mVelocityDepth > 0.5)
		velocity = linear((mVelocityDepth - 0.5) * 2.0, 1.0, mVelocity);

	Float32	cutOff		= clamp_f(mCutOff * velocity + envelope * (mEnvelopeDepth * 2.0 - 1.0), 0.0, 1.0);
	Float32	resonance	= mResonance;

	if (modulation)
	{
		cutOff		*= modulation[kModulation_CutOff];
		resonance	*= modulation[kModulation_Resonance];
	}
	
	switch (mFilterType)
	{
		case kFilter_LD_LP06:
		case kFilter_LD_LP12:
		case kFilter_LD_LP18:
		case kFilter_LD_LP24:
		{
			Float32	minHz	= 2.0 * (40.0 / mSampleRate);
			Float32	maxHz	= 2.0 * (19000.0 / mSampleRate);
			
			cutOff	= minHz + pow3(cutOff) * (maxHz - minHz);
			cutOff	= clamp_f(cutOff * keyTrack, minHz, maxHz);
					
			getLadderFilter(bufferSize, leftInBuffer, rightInBuffer, leftOutBuffer, rightOutBuffer, cutOff, resonance);
			break;
		}

		case kFilter_MM_LP12:
		case kFilter_MM_HP12:
		case kFilter_MM_BP12:
		case kFilter_MM_BR12:
		case kFilter_MM_PK12:
		{
			Float32		minHz	= 2.0 * (40.0 / mSampleRate);
			Float32		maxHz	= 2.0 * (19000.0 / mSampleRate);
			
			cutOff		= minHz + pow3(cutOff) * (maxHz - minHz);
			cutOff		= clamp_f(cutOff * keyTrack, minHz, maxHz);
			resonance	= 1.0 - resonance;

			get12dbMultiModeFilter(bufferSize, leftInBuffer, rightInBuffer, leftOutBuffer, rightOutBuffer, cutOff, resonance);
			break;
		}

		case kFilter_MM_LP24:
		case kFilter_MM_HP24:
		case kFilter_MM_BP24:
		case kFilter_MM_BR24:
		case kFilter_MM_PK24:
		{
			Float32		minHz	= 2.0 * (40.0 / mSampleRate);
			Float32		maxHz	= 2.0 * (19000.0 / mSampleRate);
			
			cutOff		= minHz + pow3(cutOff) * (maxHz - minHz);
			cutOff		= clamp_f(cutOff * keyTrack, minHz, maxHz);
			resonance	= 1.0 - resonance;

			get24dbMultiModeFilter(bufferSize, leftInBuffer, rightInBuffer, leftOutBuffer, rightOutBuffer, cutOff, resonance);
			break;
		}

		case kFilter_AP_4:
		{
			cutOff		= 1.0 - clamp_f(pow3(cutOff) * keyTrack, 0.0, 1.0);
			resonance	= resonance * 0.92;
			
			get4StageAPFilter(bufferSize, leftInBuffer, rightInBuffer, leftOutBuffer, rightOutBuffer, cutOff, resonance);
			break;
		}

		case kFilter_AP_8:
		{
			cutOff		= 1.0 - clamp_f(pow3(cutOff) * keyTrack, 0.0, 1.0);
			resonance	= resonance * 0.92;
			
			get8StageAPFilter(bufferSize, leftInBuffer, rightInBuffer, leftOutBuffer, rightOutBuffer, cutOff, resonance);
			break;
		}

		case kFilter_VOWEL:
		{
			cutOff = clamp_f(cutOff * keyTrack, 0.0, 1.0);
			
			getFormantFilter(bufferSize, leftInBuffer, rightInBuffer, leftOutBuffer, rightOutBuffer, cutOff, resonance);
			break;
		}
	}
}

// -----------------------------------------------------------------------------

void StereoFilter::start(Float32 keyTrack, Float32 velocity)
{
	mKeyTrack	= keyTrack;
	mVelocity	= velocity;
	
	//	clear();
}

// -----------------------------------------------------------------------------

void StereoFilter::update(Float32 keyTrack)
{
	mKeyTrack = keyTrack;
}

// -----------------------------------------------------------------------------

void StereoFilter::reset()
{
	mLastCutOff		= 0.0;
	mLastResonance	= 0.0;
	
	for (UInt32 i = 0; i < kNumOfFormants; i++)
		mBiquad[i].reset();
}

// -----------------------------------------------------------------------------

void StereoFilter::clear()
{
	memset(&mIO_L[0], 0, kFilter_Size * sizeof(FlData));
	memset(&mIO_R[0], 0, kFilter_Size * sizeof(FlData));

	for (UInt32 i = 0; i < kNumOfFormants; i++)
		mBiquad[i].clear();
}

// -----------------------------------------------------------------------------

void StereoFilter::getLadderFilter(UInt32 bufferSize, Float32* leftInBuffer, Float32* rightInBuffer, Float32* leftOutBuffer, Float32* rightOutBuffer, Float32 cutOff, Float32 resonance)
{
	FlData		l_cut		= mLastCutOff;
	FlData		l_reso		= mLastResonance;

	FlData		l_low1_L	= mIO_L[0];
	FlData		l_low2_L	= mIO_L[1];
	FlData		l_low3_L	= mIO_L[2];
	FlData		l_low4_L	= mIO_L[3];
	FlData		l_mem0_L	= mIO_L[4];
	
	FlData		l_low1_R	= mIO_R[0];
	FlData		l_low2_R	= mIO_R[1];
	FlData		l_low3_R	= mIO_R[2];
	FlData		l_low4_R	= mIO_R[3];
	FlData		l_mem0_R	= mIO_R[4];

	FlData		ramp		= 1.0 / bufferSize;

	FlData		maxReso		= 4.0;
	
	FlData		cutDelta	= (cutOff - l_cut) * ramp;
	FlData		resoDelta	= (maxReso * resonance - l_reso) * ramp;
		
	FlData		l_low0, in, levelfix;
	FlData*		out_L;
	FlData*		out_R;

	switch(mFilterType)
	{
		default:
		case kFilter_LD_LP06:	out_L = &l_low1_L; out_R = &l_low1_R; break;
		case kFilter_LD_LP12:	out_L = &l_low2_L; out_R = &l_low2_R; break;
		case kFilter_LD_LP18:	out_L = &l_low3_L; out_R = &l_low3_R; break;
		case kFilter_LD_LP24:	out_L = &l_low4_L; out_R = &l_low4_R; break;
	}

	while(bufferSize--)
	{
		l_cut	   += cutDelta;   
		l_reso	   += resoDelta;
		
		in			= *leftInBuffer++;
		levelfix	= 1.0 + (l_reso * 0.3) + ((1.0 - l_cut) * 0.2);

		l_low0		= in - l_reso * l_mem0_L;

		l_low1_L	= l_low1_L + l_cut * (l_low0   - l_low1_L);
		l_low2_L	= l_low2_L + l_cut * (l_low1_L - l_low2_L);
		l_low3_L	= l_low3_L + l_cut * (l_low2_L - l_low3_L);
		l_low4_L	= l_low4_L + l_cut * (l_low3_L - l_low4_L);
		
		l_mem0_L	= (l_low4_L + l_mem0_L) * 0.5;
		l_low0		= in - l_reso * l_mem0_L;

		l_low1_L	= l_low1_L + l_cut * (l_low0   - l_low1_L);
		l_low2_L	= l_low2_L + l_cut * (l_low1_L - l_low2_L);
		l_low3_L	= l_low3_L + l_cut * (l_low2_L - l_low3_L);
		l_low4_L	= l_low4_L + l_cut * (l_low3_L - l_low4_L);
		
		l_mem0_L	= (l_low4_L + l_mem0_L) * 0.5;
		l_low0		= in - l_reso * l_mem0_L;
		
		l_low1_L	= l_low1_L + l_cut * (l_low0   - l_low1_L);
		l_low2_L	= l_low2_L + l_cut * (l_low1_L - l_low2_L);
		l_low3_L	= l_low3_L + l_cut * (l_low2_L - l_low3_L);
		l_low4_L	= l_low4_L + l_cut * (l_low3_L - l_low4_L);
		
		l_mem0_L	= (l_low4_L + l_mem0_L) * 0.5;

		*leftOutBuffer++ = *out_L * levelfix;

		//	-----

		in			= *rightInBuffer++;				
		l_low0		= in - l_reso * l_mem0_R;

		l_low1_R	= l_low1_R + l_cut * (l_low0   - l_low1_R);
		l_low2_R	= l_low2_R + l_cut * (l_low1_R - l_low2_R);
		l_low3_R	= l_low3_R + l_cut * (l_low2_R - l_low3_R);
		l_low4_R	= l_low4_R + l_cut * (l_low3_R - l_low4_R);		
		
		l_mem0_R	= (l_low4_R + l_mem0_R) * 0.5;
		l_low0		= in - l_reso * l_mem0_R;

		l_low1_R	= l_low1_R + l_cut * (l_low0   - l_low1_R);
		l_low2_R	= l_low2_R + l_cut * (l_low1_R - l_low2_R);
		l_low3_R	= l_low3_R + l_cut * (l_low2_R - l_low3_R);
		l_low4_R	= l_low4_R + l_cut * (l_low3_R - l_low4_R);
		
		l_mem0_R	= (l_low4_R + l_mem0_R) * 0.5;
		l_low0		= in - l_reso * l_mem0_R;

		l_low1_R	= l_low1_R + l_cut * (l_low0   - l_low1_R);
		l_low2_R	= l_low2_R + l_cut * (l_low1_R - l_low2_R);
		l_low3_R	= l_low3_R + l_cut * (l_low2_R - l_low3_R);
		l_low4_R	= l_low4_R + l_cut * (l_low3_R - l_low4_R);
		
		l_mem0_R	= (l_low4_R + l_mem0_R) * 0.5;

		*rightOutBuffer++ = *out_R * levelfix;
	}

	mLastCutOff		= l_cut;
	mLastResonance	= l_reso;
	
	mIO_L[0]		= l_low1_L;
	mIO_L[1]		= l_low2_L;
	mIO_L[2]		= l_low3_L;
	mIO_L[3]		= l_low4_L;
	mIO_L[4]		= l_mem0_L;

	mIO_R[0]		= l_low1_R;
	mIO_R[1]		= l_low2_R;
	mIO_R[2]		= l_low3_R;
	mIO_R[3]		= l_low4_R;
	mIO_R[4]		= l_mem0_R;
}

// -----------------------------------------------------------------------------

void StereoFilter::get12dbMultiModeFilter(UInt32 bufferSize, Float32* leftInBuffer, Float32* rightInBuffer, Float32* leftOutBuffer, Float32* rightOutBuffer, Float32 cutOff, Float32 resonance)
{
	FlData		l_cut		= mLastCutOff;
	FlData		l_reso		= mLastResonance;

	FlData		l_hig1_L	= mIO_L[0];
	FlData		l_mid1_L	= mIO_L[1];
	FlData		l_low1_L	= mIO_L[2];	
	FlData		l_rej1_L	= mIO_L[3];
	FlData		l_pek1_L	= mIO_L[4];
	
	FlData		l_hig1_R	= mIO_R[0];
	FlData		l_mid1_R	= mIO_R[1];
	FlData		l_low1_R	= mIO_R[2];	
	FlData		l_rej1_R	= mIO_R[3];
	FlData		l_pek1_R	= mIO_R[4];

	FlData		ramp		= 1.0 / bufferSize;
	//	FlData		damp		= 0.01;

	FlData		cutDelta	= (cutOff- l_cut) * ramp;
	FlData		resoDelta	= (resonance - l_reso) * ramp;
	
	FlData		in;

	FlData*		out1_L;
	FlData*		out1_R;

	switch(mFilterType)
	{
		default:
		case kFilter_MM_LP12:	out1_L = &l_low1_L; out1_R = &l_low1_R;break;
		case kFilter_MM_HP12:	out1_L = &l_hig1_L; out1_R = &l_hig1_R;break;
		case kFilter_MM_BP12:	out1_L = &l_mid1_L; out1_R = &l_mid1_R;break;
		case kFilter_MM_BR12:	out1_L = &l_rej1_L; out1_R = &l_rej1_R;break;
		case kFilter_MM_PK12:	out1_L = &l_pek1_L; out1_R = &l_pek1_R; break;
	}
	
	while(bufferSize--)
	{
		l_cut		+= cutDelta;   
		l_reso		+= resoDelta;

		in			 = *leftInBuffer++;

		l_rej1_L	 = in - (l_reso * l_mid1_L);
		l_low1_L	+= l_mid1_L * l_cut;
		l_hig1_L	 = l_rej1_L - l_low1_L;
		l_mid1_L	+= l_hig1_L * l_cut;	//	 - pow3(l_mid1_L) * damp;

		l_rej1_L	 = in - (l_reso * l_mid1_L);
		l_low1_L	+= l_mid1_L * l_cut;
		l_hig1_L	 = l_rej1_L - l_low1_L;
		l_mid1_L	+= l_hig1_L * l_cut;	//	 - pow3(l_mid1_L) * damp;
		
		l_rej1_L	 = in - (l_reso * l_mid1_L);
		l_low1_L	+= l_mid1_L * l_cut;
		l_hig1_L	 = l_rej1_L - l_low1_L;
		l_mid1_L	+= l_hig1_L * l_cut;	//	 - pow3(l_mid1_L) * damp;
		
		l_pek1_L	 = (l_low1_L - l_hig1_L) * 0.65;
			
		*leftOutBuffer++ = *out1_L;

		//	-----

		in			 = *rightInBuffer++;

		l_rej1_R	 = in - (l_reso * l_mid1_R);
		l_low1_R	+= l_mid1_R * l_cut;
		l_hig1_R	 = l_rej1_R - l_low1_R;
		l_mid1_R	+= l_hig1_R * l_cut;	//	 - pow3(l_mid1_R) * damp;

		l_rej1_R	 = in - (l_reso * l_mid1_R);
		l_low1_R	+= l_mid1_R * l_cut;
		l_hig1_R	 = l_rej1_R - l_low1_R;
		l_mid1_R	+= l_hig1_R * l_cut;	//	 - pow3(l_mid1_R) * damp;
		
		l_rej1_R	 = in - (l_reso * l_mid1_R);
		l_low1_R	+= l_mid1_R * l_cut;
		l_hig1_R	 = l_rej1_R - l_low1_R;
		l_mid1_R	+= l_hig1_R * l_cut;	//	 - pow3(l_mid1_R) * damp;
		
		l_pek1_R	 = (l_low1_R - l_hig1_R) * 0.65;
			
		*rightOutBuffer++ = *out1_R;
	}

	mLastCutOff		= l_cut;
	mLastResonance	= l_reso;

	mIO_L[0]		= l_hig1_L;
	mIO_L[1]		= l_mid1_L;
	mIO_L[2]		= l_low1_L;
	mIO_L[3]		= l_rej1_L;
	mIO_L[4]		= l_pek1_L;
	
	mIO_R[0]		= l_hig1_R;
	mIO_R[1]		= l_mid1_R;
	mIO_R[2]		= l_low1_R;
	mIO_R[3]		= l_rej1_R;
	mIO_R[4]		= l_pek1_R;
}

// -----------------------------------------------------------------------------

void StereoFilter::get24dbMultiModeFilter(UInt32 bufferSize, Float32* leftInBuffer, Float32* rightInBuffer, Float32* leftOutBuffer, Float32* rightOutBuffer, Float32 cutOff, Float32 resonance)
{
	FlData		l_cut		= mLastCutOff;
	FlData		l_reso		= mLastResonance;

	FlData		l_hig1_L	= mIO_L[0];
	FlData		l_mid1_L	= mIO_L[1];
	FlData		l_low1_L	= mIO_L[2];	
	FlData		l_rej1_L	= mIO_L[3];
	FlData		l_pek1_L	= mIO_L[4];
	
	FlData		l_hig2_L	= mIO_L[5];
	FlData		l_mid2_L	= mIO_L[6];
	FlData		l_low2_L	= mIO_L[7];
	FlData		l_rej2_L	= mIO_L[8];
	FlData		l_pek2_L	= mIO_L[9];
	
	FlData		l_hig1_R	= mIO_R[0];
	FlData		l_mid1_R	= mIO_R[1];
	FlData		l_low1_R	= mIO_R[2];	
	FlData		l_rej1_R	= mIO_R[3];
	FlData		l_pek1_R	= mIO_R[4];
	
	FlData		l_hig2_R	= mIO_R[5];
	FlData		l_mid2_R	= mIO_R[6];
	FlData		l_low2_R	= mIO_R[7];
	FlData		l_rej2_R	= mIO_R[8];
	FlData		l_pek2_R	= mIO_R[9];

	FlData		ramp		= 1.0 / bufferSize;
	//	FlData		damp		= 0.01;
	
	FlData		cutDelta	= (cutOff- l_cut) * ramp;
	FlData		resoDelta	= (resonance - l_reso) * ramp;
	
	FlData		in;

	FlData*		out1_L;
	FlData*		out2_L;
	FlData*		out1_R;
	FlData*		out2_R;

	switch(mFilterType)
	{
		default:
		case kFilter_MM_LP24:	out1_L = &l_low1_L; out2_L = &l_low2_L; out1_R = &l_low1_R; out2_R = &l_low2_R;break;
		case kFilter_MM_HP24:	out1_L = &l_hig1_L; out2_L = &l_hig2_L; out1_R = &l_hig1_R; out2_R = &l_hig2_R;break;
		case kFilter_MM_BP24:	out1_L = &l_mid1_L; out2_L = &l_mid2_L; out1_R = &l_mid1_R; out2_R = &l_mid2_R;break;
		case kFilter_MM_BR24:	out1_L = &l_rej1_L; out2_L = &l_rej2_L; out1_R = &l_rej1_R; out2_R = &l_rej2_R;break;
		case kFilter_MM_PK24:	out1_L = &l_pek1_L; out2_L = &l_pek2_L; out1_R = &l_pek1_R; out2_R = &l_pek2_R;break;
	}
	
	while(bufferSize--)
	{
		l_cut			+= cutDelta;   
		l_reso			+= resoDelta;

		in			 = *leftInBuffer++;

		l_rej1_L	 = in - l_mid1_L;
		l_low1_L	+= l_mid1_L * l_cut;
		l_hig1_L	 = l_rej1_L - l_low1_L;
		l_mid1_L	+= l_hig1_L * l_cut;	//	 - pow3(l_mid1_L) * damp;
		l_pek1_L	 = (l_low1_L - l_hig1_L) * 0.65;
		l_rej2_L	 = *out1_L - (l_reso * l_mid2_L);
		l_low2_L	+= l_mid2_L * l_cut;
		l_hig2_L	 = l_rej2_L - l_low2_L;
		l_mid2_L	+= l_hig2_L * l_cut;	//	 - pow3(l_mid2_L) * damp;

		l_rej1_L	 = in - l_mid1_L;
		l_low1_L	+= l_mid1_L * l_cut;
		l_hig1_L	= l_rej1_L - l_low1_L;
		l_mid1_L	 += l_hig1_L * l_cut;	//	 - pow3(l_mid1_L) * damp;
		l_pek1_L	 = (l_low1_L - l_hig1_L) * 0.65;
		l_rej2_L	 = *out1_L - (l_reso * l_mid2_L);
		l_low2_L	+= l_mid2_L * l_cut;
		l_hig2_L	 = l_rej2_L - l_low2_L;
		l_mid2_L	+= l_hig2_L * l_cut;	//	 - pow3(l_mid2_L) * damp;
		
		l_rej1_L	 = in - l_mid1_L;
		l_low1_L	+= l_mid1_L * l_cut;
		l_hig1_L	 = l_rej1_L - l_low1_L;
		l_mid1_L	+= l_hig1_L * l_cut;	//	 - pow3(l_mid1_L) * damp;
		l_pek1_L	 = (l_low1_L - l_hig1_L) * 0.65;
		l_rej2_L	 = *out1_L - (l_reso * l_mid2_L);
		l_low2_L	+= l_mid2_L * l_cut;
		l_hig2_L	 = l_rej2_L - l_low2_L;
		l_mid2_L	+= l_hig2_L * l_cut;	//	 - pow3(l_mid2_L) * damp;

		l_pek2_L	 = (l_low2_L - l_hig2_L) * 0.65;
			
		*leftOutBuffer++ = *out2_L;

		//	-----

		in			 = *rightInBuffer++;

		l_rej1_R	 = in - l_mid1_R;
		l_low1_R	+= l_mid1_R * l_cut;
		l_hig1_R	 = l_rej1_R - l_low1_R;
		l_mid1_R	+= l_hig1_R * l_cut;	//	 - pow3(l_mid1_R) * damp;
		l_pek1_R	 = (l_low1_R - l_hig1_R) * 0.65;
		l_rej2_R	 = *out1_R - (l_reso * l_mid2_R);
		l_low2_R	+= l_mid2_R * l_cut;
		l_hig2_R	 = l_rej2_R - l_low2_R;
		l_mid2_R	+= l_hig2_R * l_cut;	//	 - pow3(l_mid2_R) * damp;

		l_rej1_R	 = in - l_mid1_R;
		l_low1_R	+= l_mid1_R * l_cut;
		l_hig1_R	 = l_rej1_R - l_low1_R;
		l_mid1_R	+= l_hig1_R * l_cut;	//	 - pow3(l_mid1_R) * damp;
		l_pek1_R	 = (l_low1_R - l_hig1_R) * 0.65;
		l_rej2_R	 = *out1_R - (l_reso * l_mid2_R);
		l_low2_R	+= l_mid2_R * l_cut;
		l_hig2_R	 = l_rej2_R - l_low2_R;
		l_mid2_R	+= l_hig2_R * l_cut;	//	 - pow3(l_mid2_R) * damp;
		
		l_rej1_R	 = in - l_mid1_R;
		l_low1_R	+= l_mid1_R * l_cut;
		l_hig1_R	 = l_rej1_R - l_low1_R;
		l_mid1_R	+= l_hig1_R * l_cut;	//	 - pow3(l_mid1_R) * damp;
		l_pek1_R	 = (l_low1_R - l_hig1_R) * 0.65;
		l_rej2_R	 = *out1_R - (l_reso * l_mid2_R);
		l_low2_R	+= l_mid2_R * l_cut;
		l_hig2_R	 = l_rej2_R - l_low2_R;
		l_mid2_R	+= l_hig2_R * l_cut;	//	 - pow3(l_mid2_R) * damp;

		l_pek2_R	 = (l_low2_R - l_hig2_R) * 0.65;
			
		*rightOutBuffer++ = *out2_R;
	}

	mLastCutOff		= l_cut;
	mLastResonance	= l_reso;

	mIO_L[0]		= l_hig1_L;
	mIO_L[1]		= l_mid1_L;
	mIO_L[2]		= l_low1_L;
	mIO_L[3]		= l_rej1_L;
	mIO_L[4]		= l_pek1_L;
	
	mIO_L[5]		= l_hig2_L;
	mIO_L[6]		= l_mid2_L;
	mIO_L[7]		= l_low2_L;
	mIO_L[8]		= l_rej2_L;
	mIO_L[9]		= l_pek2_L;

	mIO_R[0]		= l_hig1_R;
	mIO_R[1]		= l_mid1_R;
	mIO_R[2]		= l_low1_R;
	mIO_R[3]		= l_rej1_R;
	mIO_R[4]		= l_pek1_R;
	
	mIO_R[5]		= l_hig2_R;
	mIO_R[6]		= l_mid2_R;
	mIO_R[7]		= l_low2_R;
	mIO_R[8]		= l_rej2_R;
	mIO_R[9]		= l_pek2_R;
}

// -----------------------------------------------------------------------------

void StereoFilter::get4StageAPFilter(UInt32 bufferSize, Float32* leftInBuffer, Float32* rightInBuffer, Float32* leftOutBuffer, Float32* rightOutBuffer, Float32 cutOff, Float32 resonance)
{
	FlData		l_cut		= mLastCutOff;
	FlData		l_reso		= mLastResonance;
	
	FlData		l_mem1_L	= mIO_L[0];
	FlData		l_mem2_L	= mIO_L[1];
	FlData		l_mem3_L	= mIO_L[2];
	FlData		l_mem4_L	= mIO_L[3];

	FlData		l_out_L		= mIO_L[4];

	FlData		l_mem1_R	= mIO_R[0];
	FlData		l_mem2_R	= mIO_R[1];
	FlData		l_mem3_R	= mIO_R[2];
	FlData		l_mem4_R	= mIO_R[3];

	FlData		l_out_R		= mIO_R[4];

	FlData		ramp		= 1.0 / bufferSize;
	
	FlData		cutDelta	= (cutOff - l_cut) * ramp;
	FlData		resoDelta	= (resonance - l_reso) * ramp;
	
	FlData		in, stage, out, invCut;

	while(bufferSize--)
	{
		l_cut		+= cutDelta;   
		l_reso		+= resoDelta;
		
		invCut		 = -l_cut;
		
		in			 = *leftInBuffer++;
		
		stage		 = in + l_out_L * l_reso;
		out			 = stage * invCut + l_mem1_L;
		l_mem1_L	= out * l_cut + stage;
		
		stage		 = out;
		out			 = stage * invCut + l_mem2_L;
		l_mem2_L	 = out * l_cut + stage;
		
		stage		 = out;
		out			 = stage * invCut + l_mem3_L;
		l_mem3_L	 = out * l_cut + stage;

		stage		 = out;
		out			 = stage * invCut + l_mem4_L;
		l_mem4_L	 = out * l_cut + stage;
	
		l_out_L		 = out;

		*leftOutBuffer++ = (in + out) * 0.5;

		//	-----

		in			 = *rightInBuffer++;
		
		stage		 = in + l_out_R * l_reso;
		out			 = stage * invCut + l_mem1_R;
		l_mem1_R	 = out * l_cut + stage;
		
		stage		 = out;
		out			 = stage * invCut + l_mem2_R;
		l_mem2_R	 = out * l_cut + stage;
		
		stage		 = out;
		out			 = stage * invCut + l_mem3_R;
		l_mem3_R	 = out * l_cut + stage;

		stage		 = out;
		out			 = stage * invCut + l_mem4_R;
		l_mem4_R	 = out * l_cut + stage;
		
		l_out_R		 = out;

		*rightOutBuffer++ = (in + out) * 0.5;
	}
	
	mLastCutOff		= l_cut;
	mLastResonance	= l_reso;

	mIO_L[0]		= l_mem1_L;
	mIO_L[1]		= l_mem2_L;
	mIO_L[2]		= l_mem3_L;
	mIO_L[3]		= l_mem4_L;

	mIO_L[4]		= l_out_L;
	
	mIO_R[0]		= l_mem1_R;
	mIO_R[1]		= l_mem2_R;
	mIO_R[2]		= l_mem3_R;
	mIO_R[3]		= l_mem4_R;

	mIO_R[4]		= l_out_R;
}

// -----------------------------------------------------------------------------

void StereoFilter::get8StageAPFilter(UInt32 bufferSize, Float32* leftInBuffer, Float32* rightInBuffer, Float32* leftOutBuffer, Float32* rightOutBuffer, Float32 cutOff, Float32 resonance)
{
	FlData		l_cut		= mLastCutOff;
	FlData		l_reso		= mLastResonance;
	
	FlData		l_mem1_L	= mIO_L[0];
	FlData		l_mem2_L	= mIO_L[1];
	FlData		l_mem3_L	= mIO_L[2];
	FlData		l_mem4_L	= mIO_L[3];

	FlData		l_mem5_L	= mIO_L[4];
	FlData		l_mem6_L	= mIO_L[5];
	FlData		l_mem7_L	= mIO_L[6];
	FlData		l_mem8_L	= mIO_L[7];

	FlData		l_out_L		= mIO_L[8];

	FlData		l_mem1_R	= mIO_R[0];
	FlData		l_mem2_R	= mIO_R[1];
	FlData		l_mem3_R	= mIO_R[2];
	FlData		l_mem4_R	= mIO_R[3];

	FlData		l_mem5_R	= mIO_R[4];
	FlData		l_mem6_R	= mIO_R[5];
	FlData		l_mem7_R	= mIO_R[6];
	FlData		l_mem8_R	= mIO_R[7];

	FlData		l_out_R		= mIO_R[8];

	FlData		ramp		= 1.0 / bufferSize;
	
	FlData		cutDelta	= (cutOff - l_cut) * ramp;
	FlData		resoDelta	= (resonance - l_reso) * ramp;
	
	FlData		in, stage, out, invCut;

	while(bufferSize--)
	{
		l_cut		+= cutDelta;   
		l_reso		+= resoDelta;
		
		invCut		 = -l_cut;
		
		in			 = *leftInBuffer++;
		
		stage		 = in + l_out_L * l_reso;
		out			 = stage * invCut + l_mem1_L;
		l_mem1_L	 = out * l_cut + stage;
		
		stage		 = out;
		out			 = stage * invCut + l_mem2_L;
		l_mem2_L	 = out * l_cut + stage;
		
		stage		 = out;
		out			 = stage * invCut + l_mem3_L;
		l_mem3_L	 = out * l_cut + stage;

		stage		 = out;
		out			 = stage * invCut + l_mem4_L;
		l_mem4_L	 = out * l_cut + stage;
		
		stage		 = out;
		out			 = stage * invCut + l_mem5_L;
		l_mem5_L	 = out * l_cut + stage;
		
		stage		 = out;
		out			 = stage * invCut + l_mem6_L;
		l_mem6_L	 = out * l_cut + stage;
		
		stage		 = out;
		out			 = stage * invCut + l_mem7_L;
		l_mem7_L	 = out * l_cut + stage;

		stage		 = out;
		out			 = stage * invCut + l_mem8_L;
		l_mem8_L	 = out * l_cut + stage;
	
		l_out_L		 = out;
		
		*leftOutBuffer++ = (in + out) * 0.5;

		//	-----

		in			 = *rightInBuffer++;
		
		stage		 = in + l_out_R * l_reso;
		out			 = stage * invCut + l_mem1_R;
		l_mem1_R	 = out * l_cut + stage;
		
		stage		 = out;
		out			 = stage * invCut + l_mem2_R;
		l_mem2_R	 = out * l_cut + stage;
		
		stage		 = out;
		out			 = stage * invCut + l_mem3_R;
		l_mem3_R	 = out * l_cut + stage;

		stage		 = out;
		out			 = stage * invCut + l_mem4_R;
		l_mem4_R	 = out * l_cut + stage;
		
		stage		 = out;
		out			 = stage * invCut + l_mem5_R;
		l_mem5_R	 = out * l_cut + stage;
		
		stage		 = out;
		out			 = stage * invCut + l_mem6_R;
		l_mem6_R	 = out * l_cut + stage;
		
		stage		 = out;
		out			 = stage * invCut + l_mem7_R;
		l_mem7_R	 = out * l_cut + stage;

		stage		 = out;
		out			 = stage * invCut + l_mem8_R;
		l_mem8_R	 = out * l_cut + stage;
	
		l_out_R		 = out;


		*rightOutBuffer++ = (in + out) * 0.5;
	}
	
	mLastCutOff		= l_cut;
	mLastResonance	= l_reso;

	mIO_L[0]		= l_mem1_L;
	mIO_L[1]		= l_mem2_L;
	mIO_L[2]		= l_mem3_L;
	mIO_L[3]		= l_mem4_L;

	mIO_L[4]		= l_mem5_L;
	mIO_L[5]		= l_mem6_L;
	mIO_L[6]		= l_mem7_L;
	mIO_L[7]		= l_mem8_L;

	mIO_L[8]		= l_out_L;
	
	mIO_R[0]		= l_mem1_R;
	mIO_R[1]		= l_mem2_R;
	mIO_R[2]		= l_mem3_R;
	mIO_R[3]		= l_mem4_R;

	mIO_R[4]		= l_mem5_R;
	mIO_R[5]		= l_mem6_R;
	mIO_R[6]		= l_mem7_R;
	mIO_R[7]		= l_mem8_R;

	mIO_R[8]		= l_out_R;
}

// -----------------------------------------------------------------------------

void StereoFilter::getFormantFilter(UInt32 bufferSize, Float32* leftInBuffer, Float32* rightInBuffer, Float32* leftOutBuffer, Float32* rightOutBuffer, Float32 cutOff, Float32 resonance)
{
	Float32	vowel		= cutOff * (kNumOfVowels - 1.0000001);
	UInt32	vowel_1		= (UInt32)vowel;
	UInt32	vowel_2		= vowel_1 + 1;
	Float32	vowel_f		= vowel - vowel_1;

	Vowel	v1			= kFormantData[vowel_1];
	Vowel	v2			= kFormantData[vowel_2];

	for (int i = 0; i < kNumOfFormants; i++)
	{
		Float32	f1		= v1.formant[i];
		Float32	f2		= v2.formant[i];
		
		Float32	hz		= (f1 + (f2 - f1) * vowel_f);

		UInt32	type	= (i == 0) ? 0 : 2;	//	(i == 4) ? 1 : 2;
		Float32	cut		= hz / mSampleRate;
		Float32	reso	= resonance * 0.99 - (i * 0.05);
		Float32	level	= (i == 0) ? 0.5 : 0.5 + reso;
		
		mBiquad[i].setParameter(bufferSize, type, cut, reso, level);

		//	printf("Filter::getFormantFilter type %ld, cut %f, reso %f, level %f\n", type, cut, reso, level);
	}

	Float32 leftIn, out0_L, out1_L, out2_L, out3_L, out4_L;
	Float32	rightIn, out0_R, out1_R, out2_R, out3_R, out4_R;
	
	while(bufferSize--)
	{
		leftIn	= *leftInBuffer++;
		rightIn	= *rightInBuffer++;
		
		mBiquad[0].filter(leftIn, &out0_L, rightIn, &out0_R);
		mBiquad[1].filter(leftIn, &out1_L, rightIn, &out1_R);
		mBiquad[2].filter(leftIn, &out2_L, rightIn, &out2_R);
		mBiquad[3].filter(leftIn, &out3_L, rightIn, &out3_R);
		mBiquad[4].filter(leftIn, &out4_L, rightIn, &out4_R);
		
		*leftOutBuffer++	= (out0_L + out1_L + out2_L + out3_L + out4_L);
		*rightOutBuffer++	= (out0_R + out1_R + out2_R + out3_R + out4_R);
	}
	
	mLastCutOff		= cutOff;
	mLastResonance	= resonance;
}

// -----------------------------------------------------------------------------
