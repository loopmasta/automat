/*
 *  MainFilter.cpp
 *  Automat
 *
 *  Created by Stefan Beck on 27.12.07.
 *  Copyright (c) 2006-2008 Stefan Beck (stiwi@alphakanal, loopmasta). MIT License, see LICENSE.
 *
 */

// -----------------------------------------------------------------------------

#include "MainFilter.h"

// -----------------------------------------------------------------------------

MainFilter::MainFilter()
{
	mLayout			= kMFl_Shape_Pre;

	mShaperMod[0]	= 1.0;
	mShaperMod[1]	= 1.0;

	mFilterMod[0]	= 1.0;
	mFilterMod[1]	= 1.0;
}

// -----------------------------------------------------------------------------

void MainFilter::initialize(Float32 sampleRate)
{ 
	mFilter.initialize(sampleRate);
	mShaper.initialize(sampleRate);
}

// -----------------------------------------------------------------------------

void MainFilter::setParameter(UInt32 layout, UInt32 filterType, Float32 drive, Float32 cutOff, Float32 resonance, Float32 envelopeDepth, Float32 keyTrackDepth)
{
	mLayout = layout;
	
	mFilter.setParameter(filterType, cutOff, resonance, envelopeDepth, keyTrackDepth, 0.5);
	mShaper.setParameter(1, 0.0, drive);
}

// -----------------------------------------------------------------------------

void MainFilter::getFilter(UInt32 bufferSize, Float32* leftBuffer, Float32* rightBuffer, Float32 envelope, Float32* modulation)
{
	if (modulation)
	{
		mShaperMod[0]	= 1.0;
		mShaperMod[1]	= modulation[kModulation_Drive];

		mFilterMod[0]	= modulation[kModulation_CutOff];
		mFilterMod[1]	= modulation[kModulation_Resonance];
	}
	
	switch (mLayout)
	{
		case kMFl_Shape_Off:
			mFilter.getFilter(bufferSize, leftBuffer, rightBuffer, envelope, &mFilterMod[0]);
			break;

		case kMFl_Shape_Pre:
			mShaper.getShape(bufferSize, leftBuffer, rightBuffer, &mShaperMod[0]);
			mFilter.getFilter(bufferSize, leftBuffer, rightBuffer, envelope, &mFilterMod[0]);
			break;
			
		case kMFl_Shape_Post:
			mFilter.getFilter(bufferSize, leftBuffer, rightBuffer, envelope, &mFilterMod[0]);
			mShaper.getShape(bufferSize, leftBuffer, rightBuffer, &mShaperMod[0]);
			break;
	}
}

// -----------------------------------------------------------------------------

void MainFilter::start(Float32 keyTrack, Float32 velocity)
{
	mFilter.start(keyTrack, velocity);
	mShaper.start(keyTrack, velocity);
}

// -----------------------------------------------------------------------------

void MainFilter::update(Float32 keyTrack)
{
	mFilter.update(keyTrack);
	mShaper.update(keyTrack);
}

// -----------------------------------------------------------------------------

void MainFilter::reset()
{
	mFilter.reset();
	mShaper.reset();
}

// -----------------------------------------------------------------------------

void MainFilter::clear()
{
	mFilter.clear();
	mShaper.clear();	
}

// -----------------------------------------------------------------------------
