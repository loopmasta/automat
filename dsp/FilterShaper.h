/*
 *  FilterShaper.h
 *  Automat
 *
 *  Created by Stefan Beck on 27.12.07.
 *  Copyright 2007 alphakanal. All rights reserved.
 *
 */

// -----------------------------------------------------------------------------

#ifndef FILTERSHAPER_H
#define FILTERSHAPER_H

#include "Constants.h"
#include "Inlines.h"
#include "Filter.h"
#include "Shaper.h"

// -----------------------------------------------------------------------------

class FilterShaper
{
	enum
	{
		kModulation_CutOff,
		kModulation_Resonance,
		kModulation_Bias,
		kModulation_Drive,
	};

	enum
	{
		kFSh_Shape_Pre,
		kFSh_Shape_Post,
	};

public:
	
			FilterShaper();

	void	initialize(Float32 sampleRate);

	void	setParameter(UInt32 layout, UInt32 filterType, UInt32 shapeType, Float32 cutOff, Float32 resonance, Float32 bias, Float32 drive, Float32 keyTrackDepth, Float32 velocityDepth);
	void	getEffect(UInt32 bufferSize, Float32* buffer, Float32* modulation);
	
	void	start(Float32 keyTrack, Float32 velocity);
	void	update(Float32 keyTrack);
	
	void	reset();
	void	clear();

private:

	UInt32	mLayout;
	
	Filter	mFilter;
	Shaper	mShaper;

	Float32	mFilterMod[2];
	Float32	mShaperMod[2];
};

// -----------------------------------------------------------------------------

#endif

// -----------------------------------------------------------------------------
