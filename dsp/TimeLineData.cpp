/*
 *  TimeLineData.cpp
 *  Automat
 *
 *  Created by Stefan Beck on 09.04.06.
 *  Copyright 2006 alphakanal. All rights reserved.
 *
 */

// -----------------------------------------------------------------------------

#include "TimeLineData.h"

// -----------------------------------------------------------------------------

TimeLineData::TimeLineData()
{
	mNumberOfUnits	= 0;
	mBeatsPerSecond	= 2.0,	
	mTLData			= NULL;
}

// -----------------------------------------------------------------------------

TimeLineData::~TimeLineData()
{
	cleanup();
}

// -----------------------------------------------------------------------------

void TimeLineData::initialize(UInt32 numberOfUnits)
{
	//	printf("TimeLineData::initialize numberOfUnits %ld\n", numberOfUnits);

	mNumberOfUnits = numberOfUnits;

	cleanup();

	mTLData = new TLData[mNumberOfUnits];

	reset();
}

// -----------------------------------------------------------------------------

void TimeLineData::setPreset(UInt32 unit, UInt32 preset)
{
	//	printf("TimeLineData::setPreset %ld %ld\n", unit, preset);

	bool	handled = true;
	TLData*	data	= &mTLData[unit];
	
	switch(preset)
	{
		case kModPreset_Flat:
		{
			data->numOfActivePoints		= 2;
			
			data->syncOffsetTime		= 0.000000;
			data->leftMarkerTime		= 0.000000;
			data->rightMarkerTime		= 1.000000;
			
			data->points[0].time		= 0.000000;
			data->points[0].value		= 0.500000;
			data->points[1].time		= 1.000000;
			data->points[1].value		= 0.500000;
		}
		break;
		
		case kModPreset_Sine:
		{
			data->numOfActivePoints		= 17;
			
			data->syncOffsetTime		= 0.000000;
			data->leftMarkerTime		= 0.000000;
			data->rightMarkerTime		= 1.000000;
			
			Float32 scale = 1.0 / (data->numOfActivePoints - 1);
			
			for (UInt32 i = 0; i < data->numOfActivePoints; i++)
			{
				data->points[i].time	= i * scale;
				data->points[i].value	= 0.5 + sin(2.0 * M_PI * data->points[i].time) * 0.5;				
			}
		}
		break;
			
		case kModPreset_Triangle:
		{
			data->numOfActivePoints		= 3;
			
			data->syncOffsetTime		= 0.000000;
			data->leftMarkerTime		= 0.000000;
			data->rightMarkerTime		= 1.000000;
			
			data->points[0].time		= 0.000000;
			data->points[0].value		= 1.000000;
			data->points[1].time		= 0.500000;
			data->points[1].value		= 0.000000;
			data->points[2].time		= 1.000000;
			data->points[2].value		= 1.000000;
		}
		break;
			
		case kModPreset_Rect:
		{
			data->numOfActivePoints		= 4;
			
			data->syncOffsetTime		= 0.000000;
			data->leftMarkerTime		= 0.000000;
			data->rightMarkerTime		= 1.000000;
			
			data->points[0].time		= 0.000000;
			data->points[0].value		= 1.000000;
			data->points[1].time		= 0.500000;
			data->points[1].value		= 1.000000;
			data->points[2].time		= 0.501742;
			data->points[2].value		= 0.000000;
			data->points[3].time		= 1.000000;
			data->points[3].value		= 0.000000;
		}
		break;
			
		case kModPreset_Saw:
		{
			data->numOfActivePoints		= 17;
			
			data->syncOffsetTime		= 0.000000;
			data->leftMarkerTime		= 0.000000;
			data->rightMarkerTime		= 1.000000;
			
			Float32 scale = 1.0 / (data->numOfActivePoints - 1);
			Float32	x;
			
			for (UInt32 i = 0; i < data->numOfActivePoints; i++)
			{
				x						= i * scale;
				data->points[i].time	= x;
				data->points[i].value	= 1.0 - x;				
			}
		}
		break;
						
		case kModPreset_Quad:
		{
			data->numOfActivePoints		= 17;
			
			data->syncOffsetTime		= 0.000000;
			data->leftMarkerTime		= 0.000000;
			data->rightMarkerTime		= 1.000000;
			
			Float32	scale = 1.0 / (data->numOfActivePoints - 1);
			Float32	x;
			
			for (UInt32 i = 0; i < data->numOfActivePoints; i++)
			{
				x						= i * scale;
				data->points[i].time	= x;
				data->points[i].value	= quadDownNegative(x);			
			}
		}
		break;
								
		case kModPreset_Bell:
		{
			data->numOfActivePoints		= 17;
			
			data->syncOffsetTime		= 0.000000;
			data->leftMarkerTime		= 0.000000;
			data->rightMarkerTime		= 1.000000;
			
			Float32	scale = 1.0 / (data->numOfActivePoints - 1);
			Float32	x;
			
			for (UInt32 i = 0; i < data->numOfActivePoints; i++)
			{
				x						= i * scale;
				data->points[i].time	= x;
				x						= x * 2.0 - 1.0;
				data->points[i].value	= pow2(x);			
			}
		}
		break;
			
		case kModPreset_Tanh:
		{
			data->numOfActivePoints		= 17;
			
			data->syncOffsetTime		= 0.000000;
			data->leftMarkerTime		= 0.000000;
			data->rightMarkerTime		= 1.000000;
			
			Float32 scale = 1.0f / (data->numOfActivePoints - 1);
			
			for (UInt32 i = 0; i < data->numOfActivePoints; i++)
			{
				data->points[i].time	= i * scale;
				data->points[i].value	= 0.5 + tanh(data->points[i].time * 6.0 - 3.0) * 0.5;				
			}
		}
		break;
			
		case kModPreset_Peak:
		{
			data->numOfActivePoints		= 33;
			
			data->syncOffsetTime		= 0.000000;
			data->leftMarkerTime		= 0.000000;
			data->rightMarkerTime		= 1.000000;
			
			Float32 scale = 1.0f / (data->numOfActivePoints - 1);
			
			for (UInt32 i = 0; i < data->numOfActivePoints; i++)
			{
				data->points[i].time	= i * scale;
				data->points[i].value	= 0.5 + sin(8.0 * M_PI * data->points[i].time) * 0.5 * sin(M_PI * data->points[i].time);				
			}
		}
		break;
		
		case kModPreset_Decay:
		{
			data->numOfActivePoints		= 33;
			
			data->syncOffsetTime		= 0.000000;
			data->leftMarkerTime		= 0.000000;
			data->rightMarkerTime		= 1.000000;
			
			Float32 scale = 1.0f / (data->numOfActivePoints - 1);
			
			for (UInt32 i = 0; i < data->numOfActivePoints; i++)
			{
				data->points[i].time	= i * scale;
				data->points[i].value	= 0.5 + sin(8.0 * M_PI * data->points[i].time) * 0.5 * (1.0 - data->points[i].time);				
			}
		}
		break;

		case kModPreset_Beat1:
		{
			data->numOfActivePoints		= 11;
			
			data->syncOffsetTime		= 0.000000;
			data->rightMarkerTime		= 0.000000;
			data->rightMarkerTime		= 1.000000;
			
			data->points[0].time		= 0.000000;
			data->points[0].value		= 1.000000;
			data->points[1].time		= 0.125461;
			data->points[1].value		= 0.502242;
			data->points[2].time		= 0.127306;
			data->points[2].value		= 0.000000;
			data->points[3].time		= 0.376384;
			data->points[3].value		= 0.000000;
			data->points[4].time		= 0.378229;
			data->points[4].value		= 0.757847;
			data->points[5].time		= 0.501845;
			data->points[5].value		= 0.246637;
			data->points[6].time		= 0.503690;
			data->points[6].value		= 0.000000;
			data->points[7].time		= 0.750923;
			data->points[7].value		= 0.000000;
			data->points[8].time		= 0.752768;
			data->points[8].value		= 0.502242;
			data->points[9].time		= 0.880074;
			data->points[9].value		= 0.000000;
			data->points[10].time		= 1.000000;
			data->points[10].value		= 0.000000;
		}
		break;
			
		case kModPreset_Beat2:
		{
			data->numOfActivePoints		= 16;
			
			data->syncOffsetTime		= 0.000000;
			data->leftMarkerTime		= 0.000000;
			data->rightMarkerTime		= 1.000000;
			
			data->points[0].time		= 0.000000;
			data->points[0].value		= 1.000000;
			data->points[1].time		= 0.123616;
			data->points[1].value		= 1.000000;
			data->points[2].time		= 0.125461;
			data->points[2].value		= 0.004484;
			data->points[3].time		= 0.249077;
			data->points[3].value		= 0.004484;
			data->points[4].time		= 0.250923;
			data->points[4].value		= 0.753363;
			data->points[5].time		= 0.374539;
			data->points[5].value		= 0.748879;
			data->points[6].time		= 0.376384;
			data->points[6].value		= 0.000000;
			data->points[7].time		= 0.500000;
			data->points[7].value		= 0.000000;
			data->points[8].time		= 0.501742;
			data->points[8].value		= 0.495327;
			data->points[9].time		= 0.625436;
			data->points[9].value		= 0.495327;
			data->points[10].time		= 0.627178;
			data->points[10].value		= 0.000000;
			data->points[11].time		= 0.749129;
			data->points[11].value		= 0.000000;
			data->points[12].time		= 0.750923;
			data->points[12].value		= 0.251121;
			data->points[13].time		= 0.878229;
			data->points[13].value		= 0.251121;
			data->points[14].time		= 0.880074;
			data->points[14].value		= 0.000000;
			data->points[15].time		= 1.000000;
			data->points[15].value		= 0.000000;
		}
		break;
			
		case kModPreset_Beat3:
		{
			data->numOfActivePoints		= 12;
			
			data->syncOffsetTime		= 0.000000;
			data->leftMarkerTime		= 0.000000;
			data->rightMarkerTime		= 1.000000;
			
			data->points[0].time		= 0.000000;
			data->points[0].value		= 1.000000;
			data->points[1].time		= 0.123616;
			data->points[1].value		= 1.000000;
			data->points[2].time		= 0.123616;
			data->points[2].value		= 0.000000;
			data->points[3].time		= 0.500000;
			data->points[3].value		= 0.000000;
			data->points[4].time		= 0.501845;
			data->points[4].value		= 0.497758;
			data->points[5].time		= 0.625461;
			data->points[5].value		= 0.497758;
			data->points[6].time		= 0.627306;
			data->points[6].value		= 0.000000;
			data->points[7].time		= 0.750923;
			data->points[7].value		= 0.000000;
			data->points[8].time		= 0.752768;
			data->points[8].value		= 0.246637;
			data->points[9].time		= 0.878229;
			data->points[9].value		= 0.246637;
			data->points[10].time		= 0.880074;
			data->points[10].value		= 0.000000;
			data->points[11].time		= 1.000000;
			data->points[11].value		= 0.000000;
		}
		break;
			
		case kModPreset_Beat4:
		{
			data->numOfActivePoints		= 17;
			
			data->syncOffsetTime		= 0.500000;
			data->rightMarkerTime		= 0.000000;
			data->rightMarkerTime		= 1.000000;
			
			data->points[0].time		= 0.000000;
			data->points[0].value		= 1.000000;
			data->points[1].time		= 0.123616;
			data->points[1].value		= 1.000000;
			data->points[2].time		= 0.125461;
			data->points[2].value		= 0.004484;
			data->points[3].time		= 0.156827;
			data->points[3].value		= 0.502242;
			data->points[4].time		= 0.250923;
			data->points[4].value		= 0.000000;
			data->points[5].time		= 0.313653;
			data->points[5].value		= 0.246637;
			data->points[6].time		= 0.374539;
			data->points[6].value		= 0.000000;
			data->points[7].time		= 0.457565;
			data->points[7].value		= 0.497758;
			data->points[8].time		= 0.500000;
			data->points[8].value		= 0.000000;
			data->points[9].time		= 0.501845;
			data->points[9].value		= 0.497758;
			data->points[10].time		= 0.625461;
			data->points[10].value		= 0.497758;
			data->points[11].time		= 0.627306;
			data->points[11].value		= 0.000000;
			data->points[12].time		= 0.750923;
			data->points[12].value		= 0.000000;
			data->points[13].time		= 0.752768;
			data->points[13].value		= 0.246637;
			data->points[14].time		= 0.878229;
			data->points[14].value		= 0.246637;
			data->points[15].time		= 0.880074;
			data->points[15].value		= 0.000000;
			data->points[16].time		= 1.000000;
			data->points[16].value		= 0.000000;
		}
		break;
			
		case kModPreset_Beat5:
		{
			data->numOfActivePoints		= 21;
			
			data->syncOffsetTime		= 0.000000;
			data->leftMarkerTime		= 0.000000;
			data->rightMarkerTime		= 1.000000;
			
			data->points[0].time		= 0.000000;
			data->points[0].value		= 1.000000;
			data->points[1].time		= 0.059041;
			data->points[1].value		= 0.497758;
			data->points[2].time		= 0.125461;
			data->points[2].value		= 0.246637;
			data->points[3].time		= 0.250923;
			data->points[3].value		= 0.000000;
			data->points[4].time		= 0.376384;
			data->points[4].value		= 0.000000;
			data->points[5].time		= 0.376384;
			data->points[5].value		= 0.251121;
			data->points[6].time		= 0.431734;
			data->points[6].value		= 0.251121;
			data->points[7].time		= 0.431734;
			data->points[7].value		= 0.000000;
			data->points[8].time		= 0.500000;
			data->points[8].value		= 0.000000;
			data->points[9].time		= 0.501845;
			data->points[9].value		= 0.497758;
			data->points[10].time		= 0.560886;
			data->points[10].value		= 0.497758;
			data->points[11].time		= 0.562731;
			data->points[11].value		= 0.000000;
			data->points[12].time		= 0.750923;
			data->points[12].value		= 0.000000;
			data->points[13].time		= 0.750923;
			data->points[13].value		= 0.251121;
			data->points[14].time		= 0.819188;
			data->points[14].value		= 0.251121;
			data->points[15].time		= 0.819188;
			data->points[15].value		= 0.000000;
			data->points[16].time		= 0.876384;
			data->points[16].value		= 0.000000;
			data->points[17].time		= 0.878229;
			data->points[17].value		= 0.497758;
			data->points[18].time		= 0.940959;
			data->points[18].value		= 0.497758;
			data->points[19].time		= 0.942804;
			data->points[19].value		= 0.000000;
			data->points[20].time		= 1.000000;
			data->points[20].value		= 0.000000;
		}
		break;
			
		default:
			handled = false;
	}
	
	if (handled)
		TTimeLineControl::updateTLData(data);
}

// -----------------------------------------------------------------------------

void TimeLineData::editData(UInt32 unit, UInt32 mode)
{
	//	printf("TimeLineData::editData unit %ld, mode %ld\n", unit, mode);

	bool	handled = true;
	TLData*	data	= &mTLData[unit];
	
	switch (mode)
	{
		case kModEdit_HFlip:
		{
			for (UInt32 i = 0; i < data->numOfActivePoints; i++)
				data->points[i].value = 1.0 - data->points[i].value;
		}
		break;
			
		case kModEdit_VFlip:
		{
			TLData* clip = &mClipBoard[0];
			memcpy(clip, data, sizeof(TLData));

			UInt32 j = data->numOfActivePoints;

			for (UInt32 i = 0; i < data->numOfActivePoints; i++)
			{
				j--;

				data->points[i].time	= 1.0 - clip->points[j].time;
				data->points[i].value	= clip->points[j].value;				
			}
		}
		break;
		
		case kModEdit_Copy:
		{
			TLData* clip = &mClipBoard[1];
			memcpy(clip, data, sizeof(TLData));
		}
		break;

		case kModEdit_Paste:
		{
			TLData* clip = &mClipBoard[1];
			memcpy(data, clip, sizeof(TLData));
		}
		break;
			
		case kModEdit_Quantize:
		{
			//	TODO:
			
			Float32	xSteps = 96.0;
			Float32	ySteps = 48.0;
			
			for (UInt32 i = 0; i < data->numOfActivePoints; i++)
			{
				data->points[i].time	= roundf(data->points[i].time * xSteps) / xSteps;
				data->points[i].value	= roundf(data->points[i].value * ySteps) / ySteps;		
			}
		}
		break;
		
		case kModEdit_Rand:
		{
			randomize(data, kModRandomize_Random);
		}
		break;
		
		default:
			handled = false;
	}
	
	if (handled)
		TTimeLineControl::updateTLData(data);
}

// -----------------------------------------------------------------------------

void TimeLineData::randomize(UInt32 unit, UInt32 mode)
{
	randomize(&mTLData[unit], mode);
}

// -----------------------------------------------------------------------------

void TimeLineData::randomize(TLData* data, UInt32 mode)
{
	//	printf("TimeLineData::randomize unit %ld, mode %ld\n", unit, mode);

	bool handled = true;
	
	switch (mode)
	{
		case kModRandomize_Random:
		{
			UInt32	points	= 2 + (UInt32)(drand48() * (Float32)(kTlc_NumberOfPoints / 4.0));
			UInt32	last	= points - 1;
			Float32	delta	= 1.0 / (Float32)points;
			
			data->syncOffsetTime	= drand48();
			data->leftMarkerTime	= drand48() * 0.5;
			data->rightMarkerTime	= 0.5 + drand48() * 0.5;
			
			data->numOfActivePoints	= points;
			
			for (UInt32 i = 0; i < last; i++)
			{
				data->points[i].time	= delta * i;
				data->points[i].value	= drand48();
			}
			
			data->points[last].time		= 1.0;
			data->points[last].value	= data->points[0].value;			
		}
		break;
			
		case kModRandomize_Evolve:
		{
			UInt32 last = data->numOfActivePoints - 1;
			
			for (UInt32 i = 0; i < last; i++)
				data->points[i].value = clamp_f(data->points[i].value * (1.2 - drand48() * 0.4), 0.0, 1.0);
			
			data->points[last].value = data->points[0].value;
		}
		break;

		default:
			handled = false;
	}
	
	if (handled)
		TTimeLineControl::updateTLData(data);
}

// -----------------------------------------------------------------------------

void TimeLineData::setBPM(Float64 bpm)
{
	//	printf("TimeLineData::setBPM %f\n", bpm);

	mBeatsPerSecond = bpm * 0.01666666667;	//	/ 60.0;
}

// -----------------------------------------------------------------------------

void TimeLineData::copyData(UInt32 unit, TLData* data)
{
	//	printf("TimeLineData::copyData unit %ld\n", unit);

	if (mTLData)	
		memcpy(data, &mTLData[unit], sizeof(TLData));
}

// -----------------------------------------------------------------------------

void TimeLineData::pasteData(UInt32 unit, TLData* data)
{
	//	printf("TimeLineData::pasteData unit %ld\n", unit);

	if (mTLData)	
		memcpy(&mTLData[unit], data, sizeof(TLData));
}

// -----------------------------------------------------------------------------

void TimeLineData::reset()
{
	if (mTLData)
	{
		for (UInt32 i = 0; i < mNumberOfUnits; i++)
			TTimeLineControl::initTLData(&mTLData[i]);
	}
	
	TTimeLineControl::initTLData(&mClipBoard[0]);
	TTimeLineControl::initTLData(&mClipBoard[1]);
}

// -----------------------------------------------------------------------------

void TimeLineData::cleanup()
{
	if (mTLData)
		delete[] mTLData;
}

// -----------------------------------------------------------------------------
