/*
 *  Filter.cpp
 *  Automat
 *
 *  Created by Stefan Beck on 09.04.06.
 *  Copyright (c) 2006-2008 Stefan Beck (stiwi@alphakanal, loopmasta). MIT License, see LICENSE.
 *
 */

// -----------------------------------------------------------------------------

#include "Filter.h"

// -----------------------------------------------------------------------------

Filter::Filter()
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

void Filter::initialize(Float32 sampleRate)
{ 
	if (sampleRate < 44100.0)
		sampleRate = 44100.0;

	mSampleRate = sampleRate;

	reset();
	clear();
}

// -----------------------------------------------------------------------------

void Filter::setParameter(UInt32 filterType, Float32 cutOff, Float32 resonance, Float32 envelopeDepth, Float32 keyTrackDepth, Float32 velocityDepth)
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

void Filter::getFilter(UInt32 bufferSize, Float32* buffer, Float32 envelope, Float32* modulation)
{
	getFilter(bufferSize, buffer, buffer, envelope, modulation);
}

// -----------------------------------------------------------------------------

void Filter::getFilter(UInt32 bufferSize, Float32* inBuffer, Float32* outBuffer, Float32 envelope, Float32* modulation)
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
			
			getLadderFilter(bufferSize, inBuffer, outBuffer, cutOff, resonance);
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

			get12dbMultiModeFilter(bufferSize, inBuffer, outBuffer, cutOff, resonance);
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

			get24dbMultiModeFilter(bufferSize, inBuffer, outBuffer, cutOff, resonance);
			break;
		}

		case kFilter_AP_4:
		{
			cutOff		= 1.0 - clamp_f(pow3(cutOff) * keyTrack, 0.0, 1.0);
			resonance	= resonance * 0.92;
			
			get4StageAPFilter(bufferSize, inBuffer, outBuffer, cutOff, resonance);
			break;
		}

		case kFilter_AP_8:
		{
			cutOff		= 1.0 - clamp_f(pow3(cutOff) * keyTrack, 0.0, 1.0);
			resonance	= resonance * 0.92;
			
			get8StageAPFilter(bufferSize, inBuffer, outBuffer, cutOff, resonance);
			break;
		}
		
		case kFilter_VOWEL:
		{
			cutOff = clamp_f(cutOff * keyTrack, 0.0, 1.0);
			
			getFormantFilter(bufferSize, inBuffer, outBuffer, cutOff, resonance);
			break;
		}
	}
}

// -----------------------------------------------------------------------------

void Filter::start(Float32 keyTrack, Float32 velocity)
{
	mKeyTrack	= keyTrack;
	mVelocity	= velocity;
	
	//	clear();
}

// -----------------------------------------------------------------------------

void Filter::update(Float32 keyTrack)
{
	mKeyTrack = keyTrack;
}

// -----------------------------------------------------------------------------

void Filter::reset()
{
	mLastCutOff		= 0.0;
	mLastResonance	= 0.0;
	
	for (UInt32 i = 0; i < kNumOfFormants; i++)
		mBiquad[i].reset();
}

// -----------------------------------------------------------------------------

void Filter::clear()
{
	memset(&mIO[0], 0, kFilter_Size * sizeof(FlData));

	for (UInt32 i = 0; i < kNumOfFormants; i++)
		mBiquad[i].clear();
}

// -----------------------------------------------------------------------------

void Filter::getLadderFilter(UInt32 bufferSize, Float32* inBuffer, Float32* outBuffer, Float32 cutOff, Float32 resonance)
{	
	FlData		l_cut		= mLastCutOff;
	FlData		l_reso		= mLastResonance;

	FlData		l_low1		= mIO[0];
	FlData		l_low2		= mIO[1];
	FlData		l_low3		= mIO[2];
	FlData		l_low4		= mIO[3];
	FlData		l_mem0		= mIO[4];
	
	FlData		ramp		= 1.0 / bufferSize;

	FlData		maxReso		= 4.0;
	
	FlData		cutDelta	= (cutOff - l_cut) * ramp;
	FlData		resoDelta	= (maxReso * resonance - l_reso) * ramp;
	
	FlData		l_low0, in, levelfix;
	FlData*		out;

	switch(mFilterType)
	{
		default:
		case kFilter_LD_LP06:	out = &l_low1; break;
		case kFilter_LD_LP12:	out = &l_low2; break;
		case kFilter_LD_LP18:	out = &l_low3; break;
		case kFilter_LD_LP24:	out = &l_low4; break;
	}

	while(bufferSize--)
	{
		l_cut	   += cutDelta;   
		l_reso	   += resoDelta;
				
		in			= *inBuffer++;
		levelfix	= 1.0 + (l_reso * 0.3) + ((1.0 - l_cut) * 0.2);
		
		l_low0		= in - l_reso * l_mem0;

		l_low1		= l_low1 + l_cut * (l_low0 - l_low1);
		l_low2		= l_low2 + l_cut * (l_low1 - l_low2);
		l_low3		= l_low3 + l_cut * (l_low2 - l_low3);
		l_low4		= l_low4 + l_cut * (l_low3 - l_low4);

		l_mem0		= (l_low4 + l_mem0) * 0.5;				
		l_low0		= in - l_reso * l_mem0;

		l_low1		= l_low1 + l_cut * (l_low0 - l_low1);
		l_low2		= l_low2 + l_cut * (l_low1 - l_low2);
		l_low3		= l_low3 + l_cut * (l_low2 - l_low3);
		l_low4		= l_low4 + l_cut * (l_low3 - l_low4);
		
		l_mem0		= (l_low4 + l_mem0) * 0.5;
		l_low0		= in - l_reso * l_mem0;

		l_low1		= l_low1 + l_cut * (l_low0 - l_low1);
		l_low2		= l_low2 + l_cut * (l_low1 - l_low2);
		l_low3		= l_low3 + l_cut * (l_low2 - l_low3);
		l_low4		= l_low4 + l_cut * (l_low3 - l_low4);
		
		l_mem0		= (l_low4 + l_mem0) * 0.5;

		*outBuffer++ = *out * levelfix;
	}

	mLastCutOff		= l_cut;
	mLastResonance	= l_reso;
	
	mIO[0]			= l_low1;
	mIO[1]			= l_low2;
	mIO[2]			= l_low3;
	mIO[3]			= l_low4;
	mIO[4]			= l_mem0;
}

// -----------------------------------------------------------------------------

void Filter::get12dbMultiModeFilter(UInt32 bufferSize, Float32* inBuffer, Float32* outBuffer, Float32 cutOff, Float32 resonance)
{
	FlData		l_cut		= mLastCutOff;
	FlData		l_reso		= mLastResonance;

	FlData		l_hig		= mIO[0];
	FlData		l_mid		= mIO[1];
	FlData		l_low		= mIO[2];
	FlData		l_rej		= mIO[3];
	FlData		l_pek		= mIO[4];
	
	FlData		ramp		= 1.0 / bufferSize;
	//	FlData		damp		= 0.01;

	FlData		cutDelta	= (cutOff - l_cut) * ramp;
	FlData		resoDelta	= (resonance - l_reso) * ramp;
	
	FlData		in;
	FlData*		out;

	switch(mFilterType)
	{
		default:
		case kFilter_MM_LP12:	out = &l_low; break;
		case kFilter_MM_HP12:	out = &l_hig; break;
		case kFilter_MM_BP12:	out = &l_mid; break;
		case kFilter_MM_BR12:	out = &l_rej; break;
		case kFilter_MM_PK12:	out = &l_pek; break;
	}
	
	while(bufferSize--)
	{
		l_cut		+= cutDelta;   
		l_reso		+= resoDelta;
				
		in			 = *inBuffer++;
		
		l_rej		 = in - (l_reso * l_mid);
		l_low		+= l_mid * l_cut;
		l_hig		 = l_rej - l_low;
		l_mid		+= l_hig * l_cut;	//	- pow3(l_mid) * damp;
		
		l_rej		 = in - (l_reso * l_mid);
		l_low		+= l_mid * l_cut;
		l_hig		 = l_rej - l_low;
		l_mid		+= l_hig * l_cut;	//	- pow3(l_mid) * damp;

		l_rej		 = in - (l_reso * l_mid);
		l_low		+= l_mid * l_cut;
		l_hig		 = l_rej - l_low;
		l_mid		+= l_hig * l_cut;	//	- pow3(l_mid) * damp;

		l_pek		 = (l_low - l_hig) * 0.65;
			
		*outBuffer++ = *out;
	}

	mLastCutOff		= l_cut;
	mLastResonance	= l_reso;
	
	mIO[0]			= l_hig;
	mIO[1]			= l_mid;
	mIO[2]			= l_low;
	mIO[3]			= l_rej;
	mIO[4]			= l_pek;
}

// -----------------------------------------------------------------------------

void Filter::get24dbMultiModeFilter(UInt32 bufferSize, Float32* inBuffer, Float32* outBuffer, Float32 cutOff, Float32 resonance)
{
	FlData		l_cut		= mLastCutOff;
	FlData		l_reso		= mLastResonance;

	FlData		l_hig1		= mIO[0];
	FlData		l_mid1		= mIO[1];
	FlData		l_low1		= mIO[2];	
	FlData		l_rej1		= mIO[3];
	FlData		l_pek1		= mIO[4];
	
	FlData		l_hig2		= mIO[5];
	FlData		l_mid2		= mIO[6];
	FlData		l_low2		= mIO[7];
	FlData		l_rej2		= mIO[8];
	FlData		l_pek2		= mIO[9];
	
	FlData		ramp		= 1.0 / bufferSize;
	//	FlData		damp		= 0.01;

	FlData		cutDelta	= (cutOff- l_cut) * ramp;
	FlData		resoDelta	= (resonance - l_reso) * ramp;
	
	FlData		in;

	FlData*		out1;
	FlData*		out2;

	switch(mFilterType)
	{
		default:
		case kFilter_MM_LP24: out1 = &l_low1; out2 = &l_low2; break;
		case kFilter_MM_HP24: out1 = &l_hig1; out2 = &l_hig2; break;
		case kFilter_MM_BP24: out1 = &l_mid1; out2 = &l_mid2; break;
		case kFilter_MM_BR24: out1 = &l_rej1; out2 = &l_rej2; break;
		case kFilter_MM_PK24: out1 = &l_pek1; out2 = &l_pek2; break;
	}
	
	while(bufferSize--)
	{
		l_cut		+= cutDelta;   
		l_reso		+= resoDelta;

		in			 = *inBuffer++;

		l_rej1		 = in - l_mid1;
		l_low1		+= l_mid1 * l_cut;
		l_hig1		 = l_rej1 - l_low1;
		l_mid1		+= l_hig1 * l_cut;	//	- pow3(l_mid1) * damp;
		l_pek1		 = (l_low1 - l_hig1) * 0.65;
		l_rej2		 = *out1 - (l_reso * l_mid2);
		l_low2		+= l_mid2 * l_cut;
		l_hig2		 = l_rej2 - l_low2;
		l_mid2		+= l_hig2 * l_cut;	//	- pow3(l_mid2) * damp;

		l_rej1		 = in - l_mid1;
		l_low1		+= l_mid1 * l_cut;
		l_hig1		 = l_rej1 - l_low1;
		l_mid1		+= l_hig1 * l_cut;	//	- pow3(l_mid1) * damp;
		l_pek1		 = (l_low1 - l_hig1) * 0.65;
		l_rej2		 = *out1 - (l_reso * l_mid2);
		l_low2		+= l_mid2 * l_cut;
		l_hig2		 = l_rej2 - l_low2;
		l_mid2		+= l_hig2 * l_cut;	//	- pow3(l_mid2) * damp;
		
		l_rej1		 = in - l_mid1;
		l_low1		+= l_mid1 * l_cut;
		l_hig1		 = l_rej1 - l_low1;
		l_mid1		+= l_hig1 * l_cut;	//	- pow3(l_mid1) * damp;
		l_pek1		 = (l_low1 - l_hig1) * 0.65;
		l_rej2		 = *out1 - (l_reso * l_mid2);
		l_low2		+= l_mid2 * l_cut;
		l_hig2		 = l_rej2 - l_low2;
		l_mid2		+= l_hig2 * l_cut;	//	- pow3(l_mid2) * damp;

		l_pek2		 = (l_low2 - l_hig2) * 0.65;
			
		*outBuffer++ = *out2;
	}

	mLastCutOff		= l_cut;
	mLastResonance	= l_reso;

	mIO[0]			= l_hig1;
	mIO[1]			= l_mid1;
	mIO[2]			= l_low1;
	mIO[3]			= l_rej1;
	mIO[4]			= l_pek1;
	
	mIO[5]			= l_hig2;
	mIO[6]			= l_mid2;
	mIO[7]			= l_low2;
	mIO[8]			= l_rej2;
	mIO[9]			= l_pek2;
}

// -----------------------------------------------------------------------------

void Filter::get4StageAPFilter(UInt32 bufferSize, Float32* inBuffer, Float32* outBuffer, Float32 cutOff, Float32 resonance)
{
	FlData		l_cut		= mLastCutOff;
	FlData		l_reso		= mLastResonance;
	
	FlData		l_mem1		= mIO[0];
	FlData		l_mem2		= mIO[1];
	FlData		l_mem3		= mIO[2];
	FlData		l_mem4		= mIO[3];

	FlData		l_out		= mIO[4];

	FlData		ramp		= 1.0 / bufferSize;
	
	FlData		cutDelta	= (cutOff - l_cut) * ramp;
	FlData		resoDelta	= (resonance - l_reso) * ramp;
	
	FlData		in, stage, out, invCut;

	while(bufferSize--)
	{
		l_cut	   += cutDelta;   
		l_reso	   += resoDelta;
		
		invCut		= -l_cut;
		
		in			= *inBuffer++;
		
		stage		= in + l_out * l_reso;
		out			= stage * invCut + l_mem1;
		l_mem1		= out * l_cut + stage;
		
		stage		= out;
		out			= stage * invCut + l_mem2;
		l_mem2		= out * l_cut + stage;
		
		stage		= out;
		out			= stage * invCut + l_mem3;
		l_mem3		= out * l_cut + stage;

		stage		= out;
		out			= stage * invCut + l_mem4;
		l_mem4		= out * l_cut + stage;

		l_out		= out;

		*outBuffer++ = (in + out) * 0.5;
	}
	
	mLastCutOff		= l_cut;
	mLastResonance	= l_reso;

	mIO[0]			= l_mem1;
	mIO[1]			= l_mem2;
	mIO[2]			= l_mem3;
	mIO[3]			= l_mem4;

	mIO[4]			= l_out;
}

// -----------------------------------------------------------------------------

void Filter::get8StageAPFilter(UInt32 bufferSize, Float32* inBuffer, Float32* outBuffer, Float32 cutOff, Float32 resonance)
{
	FlData		l_cut		= mLastCutOff;
	FlData		l_reso		= mLastResonance;
	
	FlData		l_mem1		= mIO[0];
	FlData		l_mem2		= mIO[1];
	FlData		l_mem3		= mIO[2];
	FlData		l_mem4		= mIO[3];

	FlData		l_mem5		= mIO[4];
	FlData		l_mem6		= mIO[5];
	FlData		l_mem7		= mIO[6];
	FlData		l_mem8		= mIO[7];

	FlData		l_out		= mIO[8];

	FlData		ramp		= 1.0 / bufferSize;
	
	FlData		cutDelta	= (cutOff - l_cut) * ramp;
	FlData		resoDelta	= (resonance - l_reso) * ramp;
	
	FlData		in, stage, out, invCut;

	while(bufferSize--)
	{
		l_cut	   += cutDelta;   
		l_reso	   += resoDelta;
		
		invCut		= -l_cut;
		
		in			= *inBuffer++;
		
		stage		= in + l_out * l_reso;
		out			= stage * invCut + l_mem1;
		l_mem1		= out * l_cut + stage;
		
		stage		= out;
		out			= stage * invCut + l_mem2;
		l_mem2		= out * l_cut + stage;
		
		stage		= out;
		out			= stage * invCut + l_mem3;
		l_mem3		= out * l_cut + stage;

		stage		= out;
		out			= stage * invCut + l_mem4;
		l_mem4		= out * l_cut + stage;
		
		stage		= out;
		out			= stage * invCut + l_mem5;
		l_mem5		= out * l_cut + stage;
		
		stage		= out;
		out			= stage * invCut + l_mem6;
		l_mem6		= out * l_cut + stage;
		
		stage		= out;
		out			= stage * invCut + l_mem7;
		l_mem7		= out * l_cut + stage;

		stage		= out;
		out			= stage * invCut + l_mem8;
		l_mem8		= out * l_cut + stage;
	
		l_out		= out;
		
		*outBuffer++ = (in + out) * 0.5;
	}
	
	mLastCutOff		= l_cut;
	mLastResonance	= l_reso;

	mIO[0]			= l_mem1;
	mIO[1]			= l_mem2;
	mIO[2]			= l_mem3;
	mIO[3]			= l_mem4;

	mIO[4]			= l_mem5;
	mIO[5]			= l_mem6;
	mIO[6]			= l_mem7;
	mIO[7]			= l_mem8;

	mIO[8]			= l_out;
}

// -----------------------------------------------------------------------------

void Filter::getFormantFilter(UInt32 bufferSize, Float32* inBuffer, Float32* outBuffer, Float32 cutOff, Float32 resonance)
{
	Float32	vowel		= cutOff * (kNumOfVowels - 1.000001);
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
	
	Float32 in, out0, out1, out2, out3, out4;
	
	while(bufferSize--)
	{
		in = *inBuffer++;
		
		mBiquad[0].filter(in, &out0);
		mBiquad[1].filter(in, &out1);
		mBiquad[2].filter(in, &out2);
		mBiquad[3].filter(in, &out3);
		mBiquad[4].filter(in, &out4);
		
		*outBuffer++ = (out0 + out1 + out2 + out3 + out4);
	}
	
	mLastCutOff		= cutOff;
	mLastResonance	= resonance;
}

// -----------------------------------------------------------------------------

/*
void Filter::getBandwithLimiter(UInt32 bufferSize, Float32* inBuffer, Float32* outBuffer, Float32 loCut, Float32 hiCut)
{	
	FlData	l_cut1		= mLastCutOff;
	FlData	l_cut2		= mLastResonance;

	FlData	l_low1		= mIO[0];
	FlData	l_low2		= mIO[1];

	FlData	ramp		= 1.0 / bufferSize;
	
	FlData	cut1Delta	= (loCut - l_cut1) * ramp;
	FlData	cut2Delta	= (hiCut - l_cut2) * ramp;

	FlData	in;

	while(bufferSize--)
	{
		l_cut1	   += cut1Delta;   
		l_cut2	   += cut2Delta;
		
		in			= *inBuffer++;

		l_low1		= l_low1 + l_cut1 * (in		- l_low1);
		l_low2		= l_low2 + l_cut2 * (l_low1 - l_low2);

		l_low1		= l_low1 + l_cut1 * (in		- l_low1);
		l_low2		= l_low2 + l_cut2 * (l_low1 - l_low2);

		l_low1		= l_low1 + l_cut1 * (in		- l_low1);
		l_low2		= l_low2 + l_cut2 * (l_low1 - l_low2);
       
		*outBuffer++ = l_low1 - l_low2;
	}
	
	mLastCutOff		= l_cut1;
	mLastResonance	= l_cut2;

	mIO[0]			= l_low1;
	mIO[1]			= l_low2;
}

// -----------------------------------------------------------------------------

void Filter::getDCBlocker(UInt32 bufferSize, Float32* inBuffer, Float32* outBuffer)
{	
	FlData	x1	= mIO[0];
	FlData	x2	= mIO[1];

	FlData	in;

	while(bufferSize--)
	{
		in			= *inBuffer++;
		x1			= 0.999 * x1 + in - x2;
		x2			= in;
       
		*outBuffer++ = x1;
	}
	
	mIO[0]	= x1;
	mIO[1]	= x2;
}

// -----------------------------------------------------------------------------
*/
