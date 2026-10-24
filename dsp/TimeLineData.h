/*
 *  TimeLineData.h
 *  Automat
 *
 *  Created by Stefan Beck on 09.04.06.
 *  Copyright 2006 alphakanal. All rights reserved.
 *
 */

// -----------------------------------------------------------------------------

#ifndef TIMELINEDATA_H
#define TIMELINEDATA_H

#include "TTimeLineControl.h"

#include "Constants.h"
#include "Inlines.h"

// -----------------------------------------------------------------------------

class TimeLineData
{
	enum
	{	
		kModPreset_Flat,		//	0
		kModPreset_Sine,
		kModPreset_Triangle,
		kModPreset_Rect,
		kModPreset_Saw,
		kModPreset_Quad,		//	5
		kModPreset_Bell,
		kModPreset_Tanh,
		kModPreset_Peak,
		kModPreset_Decay,
		kModPreset_Beat1,		//	10
		kModPreset_Beat2,
		kModPreset_Beat3,
		kModPreset_Beat4,
		kModPreset_Beat5,
	};
		
	enum
	{	
		kModEdit_HFlip,
		kModEdit_VFlip,
		kModEdit_Copy,
		kModEdit_Paste,
		kModEdit_Quantize,
		kModEdit_Rand,
	};

	enum
	{	
		kModRandomize_Random,
		kModRandomize_Evolve,
	};
	
public:
	
			TimeLineData();
		   ~TimeLineData();
	
	void	initialize(UInt32 numberOfUnits);
	
	void	setPreset(UInt32 unit, UInt32 preset);
	void	editData(UInt32 unit, UInt32 mode);

	void	randomize(UInt32 unit, UInt32 mode);
	void	randomize(TLData* data, UInt32 mode);
	
	void	setBPM(Float64 bpm);
	
	void	copyData(UInt32 unit, TLData* data);
	void	pasteData(UInt32 unit, TLData* data);

	void	reset();
	
	UInt32	mNumberOfUnits;
	Float32	mBeatsPerSecond;
	
	TLData*	mTLData;
	TLData	mClipBoard[2];
	
private:

	void	cleanup();
};

#endif

// -----------------------------------------------------------------------------

