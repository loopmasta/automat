/*
 *  SampleData.h
 *  Automat
 *
 *  Created by Stefan Beck on 09.04.06.
 *  Copyright 2006 alphakanal. All rights reserved.
 *
 */

// -----------------------------------------------------------------------------

#ifndef SAMPLEDATA_H
#define SAMPLEDATA_H

#include <math.h>

#include "CAAudioFile.h"
#include "CAAudioBufferList.h"

#include "Buffer.h"
#include "Inlines.h"

// -----------------------------------------------------------------------------

class SampleData
{
	enum
	{
		kSample_Sine,
		kSample_Triangle,
		kSample_Sawtooth,
		kSample_Rectangle,
	};
	
public:

				SampleData();	
			   //~SampleData();	
			
	void		readSampleData(CFURLRef url);
	void		createWaveData(UInt32 wave, UInt32 numberOfSamples);
	
	Float32*	getSampleData();
	UInt32		getSampleSize();
	Float32		getSampleRate();
	Float32		getSampleKey();
	bool		getSampleReady();

	void		reset();
	void		clear();
	
private:

	//void		cleanup();	

	Buffer		mSampleData;
	UInt32		mNumberOfSamples;
	Float32		mSampleRate;
	Float32		mSampleKey;
	bool		mReady;
};

#endif

// -----------------------------------------------------------------------------

