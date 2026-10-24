/*
 *  FilterShaper.cpp
 *  Automat
 *
 *  Created by Stefan Beck on 27.12.07.
 *  Copyright 2007 alphakanal. All rights reserved.
 *
 */

// -----------------------------------------------------------------------------

#include "FilterShaper.h"

// -----------------------------------------------------------------------------

FilterShaper::FilterShaper()
{
	mLayout			= kFSh_Shape_Pre;
	
	mShaperMod[0]	= 1.0;
	mShaperMod[1]	= 1.0;

	mFilterMod[0]	= 1.0;
	mFilterMod[1]	= 1.0;
}

// -----------------------------------------------------------------------------

void FilterShaper::initialize(Float32 sampleRate)
{ 
	mFilter.initialize(sampleRate);
	mShaper.initialize(sampleRate);
}

// -----------------------------------------------------------------------------

void FilterShaper::setParameter(UInt32 layout, UInt32 filterType, UInt32 shapeType, Float32 cutOff, Float32 resonance, Float32 bias, Float32 drive, Float32 keyTrackDepth, Float32 velocityDepth)
{
	mLayout = layout;
	
	mFilter.setParameter(filterType, cutOff, resonance, 0.5, keyTrackDepth, velocityDepth);
	mShaper.setParameter(shapeType, bias, drive);
}

// -----------------------------------------------------------------------------

void FilterShaper::getEffect(UInt32 bufferSize, Float32* buffer, Float32* modulation)
{
	if (modulation)
	{
		mShaperMod[0]	= modulation[kModulation_Bias];
		mShaperMod[1]	= modulation[kModulation_Drive];

		mFilterMod[0]	= modulation[kModulation_CutOff];
		mFilterMod[1]	= modulation[kModulation_Resonance];
	}

	switch (mLayout)
	{
		case kFSh_Shape_Pre:
			mShaper.getShape(bufferSize, buffer, &mShaperMod[0]);
			mFilter.getFilter(bufferSize, buffer, 0.0, &mFilterMod[0]);
			break;
			
		case kFSh_Shape_Post:
			mFilter.getFilter(bufferSize, buffer, 0.0, &mFilterMod[0]);
			mShaper.getShape(bufferSize, buffer, &mShaperMod[0]);
			break;
	}
}

// -----------------------------------------------------------------------------

void FilterShaper::start(Float32 keyTrack, Float32 velocity)
{
	mFilter.start(keyTrack, velocity);
	mShaper.start(keyTrack, velocity);
}

// -----------------------------------------------------------------------------

void FilterShaper::update(Float32 keyTrack)
{
	mFilter.update(keyTrack);
	mShaper.update(keyTrack);
}

// -----------------------------------------------------------------------------

void FilterShaper::reset()
{
	mFilter.reset();
	mShaper.reset();
}

// -----------------------------------------------------------------------------

void FilterShaper::clear()
{
	mFilter.clear();
	mShaper.clear();
}

// -----------------------------------------------------------------------------
