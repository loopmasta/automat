/*
 *  Biquad.cpp
 *  Automat
 *
 *  Created by Stefan Beck on 23.07.07.
 *  Copyright (c) 2006-2008 Stefan Beck (stiwi@alphakanal, loopmasta). MIT License, see LICENSE.
 *
 */

// -----------------------------------------------------------------------------

#include "Biquad.h"

// -----------------------------------------------------------------------------

Biquad::Biquad()
{
	reset();
	clear();
}

// -----------------------------------------------------------------------------

void Biquad::setParameter(UInt32 bufferSize, UInt32 type, Float32 cutOff, Float32 resonance, Float32 level)
{
	FlData	ramp	= 1.0 / bufferSize;
	
	FlData	w		= M_2PI * cutOff;
	FlData	sn		= sin(w);
	FlData	cs		= cos(w);
	FlData	alpha	= sn * sinh(M_LN2_2 * (1.0 - resonance) * w / sn);
	FlData	a0r		= 1.0 / (1.0 + alpha);

	FlData	l_b0, l_b1, l_b2, l_a1, l_a2;

	switch (type)
	{
		default:
		case kBiquad_LP:
		{
			l_b0 = a0r * (1.0 - cs) * 0.5;
			l_b1 = a0r * (1.0 - cs);
			l_b2 = l_b0;
			l_a1 = a0r * (2.0 * cs);
			l_a2 = a0r * (alpha - 1.0);
			break;
		}
			
		case kBiquad_HP:
		{
			l_b0 = a0r * (1.0 + cs) * 0.5;
			l_b1 = a0r * -(1.0 + cs);
			l_b2 = l_b0;
			l_a1 = a0r * (2.0 * cs);
			l_a2 = a0r * (alpha - 1.0);
			break;
		}

		case kBiquad_BP:
		{
			l_b0 = a0r * alpha;
			l_b1 = 0.0;
			l_b2 = a0r * -alpha;
			l_a1 = a0r * (2.0 * cs);
			l_a2 = a0r * (alpha - 1.0);
			break;
		}
		
		case kBiquad_BR:
		{
			l_b0 = a0r;
			l_b1 = a0r * (-2.0 * cs);
			l_b2 = a0r;
			l_a1 = a0r * (2.0 * cs);
			l_a2 = a0r * (alpha - 1.0);
			break;
		}
	}

	mB0_D	= (l_b0 - mB0) * ramp;
	mB1_D	= (l_b1 - mB1) * ramp;
	mB2_D	= (l_b2 - mB2) * ramp;
	mA1_D	= (l_a1 - mA1) * ramp;
	mA2_D	= (l_a2 - mA2) * ramp;
	
	mLv_D	= (level - mLv) * ramp;
}

// -----------------------------------------------------------------------------

void Biquad::reset()
{
	mB0_D	= 0.0;
	mB1_D	= 0.0;
	mB2_D	= 0.0;
	mA1_D	= 0.0;
	mA2_D	= 0.0;
}
	
// -----------------------------------------------------------------------------

void Biquad::clear()
{
	mX1_L	= 0.0; 
	mX2_L	= 0.0;
	mY1_L	= 0.0; 
	mY2_L	= 0.0; 

	mX1_R	= 0.0; 
	mX2_R	= 0.0;
	mY1_R	= 0.0; 
	mY2_R	= 0.0; 
}
	
// -----------------------------------------------------------------------------
