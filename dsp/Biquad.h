/*
 *  Biquad.h
 *  Automat
 *
 *  Created by Stefan Beck on 23.07.07.
 *  Copyright 2007 alphakanal. All rights reserved.
 *
 */

// -----------------------------------------------------------------------------

#ifndef BIQUAD_H
#define BIQUAD_H

#include "Inlines.h"

// -----------------------------------------------------------------------------

#define	kNumOfVowels	5
#define	kNumOfFormants	5

typedef double FlData;

typedef struct Vowel
{
	char*	name;
	Float32	formant[kNumOfFormants];
}
Vowel;

//	http://ecmc.rochester.edu/onlinedocs/Csound/Appendices/table3.html

static const Vowel kFormantData[kNumOfVowels] =
{
	{"tenor a", {650, 1080, 2650, 2900, 3250}},
	{"tenor e", {400, 1700, 2600, 3200, 3580}},
	{"tenor i", {290, 1870, 2800, 3250, 3540}},
	{"tenor o", {400,  800, 2600, 2800, 3000}},
	{"tenor u", {350,  600, 2700, 2900, 3300}}
};

// -----------------------------------------------------------------------------

class Biquad
{
	enum
	{
		kBiquad_LP,
		kBiquad_HP,
		kBiquad_BP,
		kBiquad_BR
	};

public:
					Biquad();

	void			setParameter(UInt32 bufferSize, UInt32 type, Float32 cutOff, Float32 resonance, Float32 level);
	
	inline	void	filter(Float32 input, Float32* output);
	inline	void	filter(Float32 leftIn, Float32* leftOut, Float32 rightIn, Float32* rightOut);

	void			reset();
	void			clear();
	
private:
	
	FlData			mB0, mB1, mB2, mA1, mA2, mLv;
	FlData			mB0_D, mB1_D, mB2_D, mA1_D, mA2_D, mLv_D;
		
	FlData			mX1_L, mX2_L, mY1_L, mY2_L;
	FlData			mX1_R, mX2_R, mY1_R, mY2_R;
};

// -----------------------------------------------------------------------------

inline void Biquad::filter(Float32 input, Float32* output)
{
	mB0	+= mB0_D;
	mB1	+= mB1_D;
	mB2	+= mB2_D;
	mA1	+= mA1_D;
	mA2	+= mA2_D;

	mLv	+= mLv_D;
	
	FlData	in	= input * mLv;
	FlData	out	= mB0 * in + mB1 * mX1_L + mB2 * mX2_L + mA1 * mY1_L + mA2 * mY2_L;

	mX2_L		= mX1_L;
	mX1_L		= in;
	mY2_L		= mY1_L;
	mY1_L		= out;

	*output		= out;
}

// -----------------------------------------------------------------------------

inline void Biquad::filter(Float32 leftIn, Float32* leftOut, Float32 rightIn, Float32* rightOut)
{
	mB0	+= mB0_D;
	mB1	+= mB1_D;
	mB2	+= mB2_D;
	mA1	+= mA1_D;
	mA2	+= mA2_D;

	mLv	+= mLv_D;
	
	FlData	in	= leftIn * mLv;
	FlData	out	= mB0 * in + mB1 * mX1_L + mB2 * mX2_L + mA1 * mY1_L + mA2 * mY2_L;

	mX2_L		= mX1_L;
	mX1_L		= in;
	mY2_L		= mY1_L;
	mY1_L		= out;

	*leftOut	= out;

	//	-----

	in			= rightIn * mLv;
	out			= mB0 * in + mB1 * mX1_R + mB2 * mX2_R + mA1 * mY1_R + mA2 * mY2_R;

	mX2_R		= mX1_R;
	mX1_R		= in;
	mY2_R		= mY1_R;
	mY1_R		= out;

	*rightOut	= out;
}

// -----------------------------------------------------------------------------

#endif
