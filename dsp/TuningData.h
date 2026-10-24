/*
 *  TuningData.h
 *  Automat
 *
 *  Created by Stefan Beck on 09.04.06.
 *  Copyright (c) 2006-2008 Stefan Beck (stiwi@alphakanal, loopmasta). MIT License, see LICENSE.
 *
 */

// -----------------------------------------------------------------------------

#ifndef TUNINGDATA_H
#define TUNINGDATA_H

#include <stdio.h>
#include <fstream>

#include "Inlines.h"

// -----------------------------------------------------------------------------

class TuningData
{
	enum eSection
	{
		SEC_None		= 0,
		SEC_Unknown		= 1,
		SEC_Tuning		= 2,
		SEC_ExactTuning	= 3
	};
	
public:
	
				TuningData();
	
	void		readTuningData(CFURLRef url);
	
	Float32		getBaseFreq();						// BaseFreq in Hz
	Float32		getNoteFreq(UInt8 noteIndex);		// Absolute tune in Hz
	Float32		getRelativeTune(UInt8 noteIndex);	// Relative tune in cents
	
	void		reset();
	
private:
		
	bool		noteInRange(int noteIndex);	
	void		debug();
	
	Float32		mTunes[128];	// Unit: Cents
	Float32		mBaseFreq;		// Unit: Hz
};

// -----------------------------------------------------------------------------

#endif

// -----------------------------------------------------------------------------

