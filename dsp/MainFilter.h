/*
 *  MainFilter.h
 *  Automat
 *
 *  Created by Stefan Beck on 27.12.07.
 *  Copyright (c) 2006-2008 Stefan Beck (stiwi@alphakanal, loopmasta). MIT License, see LICENSE.
 *
 */

// -----------------------------------------------------------------------------

#ifndef MAINFILTER_H
#define MAINFILTER_H

#include "Constants.h"
#include "Inlines.h"
#include "StereoFilter.h"
#include "StereoShaper.h"

// -----------------------------------------------------------------------------

class MainFilter
{
	enum
	{
		kModulation_Drive,
		kModulation_CutOff,
		kModulation_Resonance,
	};

	enum
	{
		kMFl_Shape_Off,
		kMFl_Shape_Pre,
		kMFl_Shape_Post,
	};

public:
	
			MainFilter();

	void	initialize(Float32 sampleRate);

	void	setParameter(UInt32 layout, UInt32 filterType, Float32 drive, Float32 cutOff, Float32 resonance, Float32 envelopeDepth, Float32 keyTrackDepth);
	void	getFilter(UInt32 bufferSize, Float32* leftBuffer, Float32* rightBuffer, Float32 envelope, Float32* modulation);
	
	void	start(Float32 keyTrack, Float32 velocity);
	void	update(Float32 keyTrack);
	
	void	reset();
	void	clear();

private:

	UInt32			mLayout;
	
	StereoFilter	mFilter;
	StereoShaper	mShaper;
	
	Float32			mFilterMod[2];
	Float32			mShaperMod[2];
};

// -----------------------------------------------------------------------------

#endif

// -----------------------------------------------------------------------------
