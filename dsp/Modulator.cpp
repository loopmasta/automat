/*
 *  Modulator.cpp
 *  Automat
 *
 *  Created by Stefan Beck on 09.04.06.
 *  Copyright 2006 alphakanal. All rights reserved.
 *
 */

// -----------------------------------------------------------------------------

#include "Modulator.h"

// -----------------------------------------------------------------------------

Modulator::Modulator()
{
	mVelocity	= 0.0;
	mKeyTrack	= 0.0;
	
	mState		= NULL;
	mOutput		= NULL;
}

// -----------------------------------------------------------------------------

Modulator::~Modulator()
{
	cleanup();
}

// -----------------------------------------------------------------------------

void Modulator::initialize(Float32 sampleRate, UInt32 numberOfTargets, TimeLineData* timeLineData, UInt32 firstDataSet, UInt32 lastDataSet, UInt32 firstMod2Mod)
{
	mSampleRate			= sampleRate;
	mTimeLineData		= timeLineData;
	mNumberOfTargets	= numberOfTargets;
	
	mFirstDataSet		= firstDataSet;
	mLastDataSet		= lastDataSet;
	mNumberOfInputs		= mLastDataSet - mFirstDataSet + 1;	//	mTimeLineData->mNumberOfUnits
	
	mFirstMod2Mod		= firstMod2Mod;
	
	//	printf("Modulator::initialize numberOfTargets %ld, firstDataSet %ld, lastDataSet %ld, mNumberOfInputs %ld\n", numberOfTargets, firstDataSet, lastDataSet, mNumberOfInputs);

	cleanup();

	mState				= new ModulatorData[mNumberOfInputs];
	mOutput				= new Float32[mNumberOfTargets];
	
	reset();
	clear();
}

// -----------------------------------------------------------------------------

void Modulator::setParameter(UInt32 modulator, UInt32 target, UInt32 sync, UInt32 timing, Float32 speed, Float32 keyTrackDepth, Float32 velocityDepth, Float32 level)
{
	if (mState && modulator < mNumberOfInputs)
	{
		ModulatorData* state	= &mState[modulator];

		state->target			= target;
		state->sync				= sync;
		state->timing			= timing;
		state->speed			= 0.01 + pow3(speed) * 0.99;
		state->keyTrackDepth	= keyTrackDepth;
		state->velocityDepth	= velocityDepth;
		state->level			= level;	
	}
}

// -----------------------------------------------------------------------------

Float32* Modulator::getModulation(UInt32 chunkSize)
{
	mKeyDown = false;

	if (mState && mOutput)		
	{
		clear();
		
		for (UInt32 i = 0; i < mNumberOfInputs; i++)
		{
			ModulatorData*	state	= &mState[i];
			UInt32			target	= state->target;
			
			if (target < mFirstMod2Mod)
				continue;
				
			TLData*	data	= &mTimeLineData->mTLData[mFirstDataSet + i];
			Float32 signal	= getOutput(chunkSize, state, data, NULL);
			
			mOutput[target] *= signal;
		}

		for (UInt32 i = 0; i < mNumberOfInputs; i++)
		{
			ModulatorData*	state	= &mState[i];
			UInt32			target	= state->target;
	
			if (target == kModTarget_Off)
				continue;

			TLData*	data	= &mTimeLineData->mTLData[mFirstDataSet + i];
			Float32	signal	= getOutput(chunkSize, state, data, &mOutput[mFirstMod2Mod + i]);
			
			mOutput[target] *= signal;
		}
	}
		
	return &mOutput[0];
}

// -----------------------------------------------------------------------------

Float32* Modulator::getModulationWithState(UInt32 chunkSize, UInt32 state)
{	
	switch(state)
	{
		case kNoteState_Released:
		case kNoteState_FastReleased:
			mKeyDown = false;
			break;
			
		default:
			mKeyDown = true;
			break;
	}
	
	if (mState && mOutput)		
	{
		clear();

		if (mResetMod)
		{			
			for (UInt32 i = 0; i < mNumberOfInputs; i++)
			{
				ModulatorData*	state	= &mState[i];
				TLData*			data	= &mTimeLineData->mTLData[mFirstDataSet + i];
				
				if (state->sync != kModSync_Off)
				{
					state->lookup		= data->syncOffsetIndex;
					state->time			= data->syncOffsetTime;
					state->direction	= 1.0;
				}
			}
			
			mResetMod = false;
		}

		for (UInt32 i = 0; i < mNumberOfInputs; i++)
		{
			ModulatorData*	state	= &mState[i];
			UInt32			target	= state->target;
			
			if (target < mFirstMod2Mod)
				continue;
				
			TLData*	data	 = &mTimeLineData->mTLData[mFirstDataSet + i];
			Float32	signal	 = getOutput(chunkSize, state, data, NULL);
			
			mOutput[target] *= signal;
		}
		
		for (UInt32 i = 0; i < mNumberOfInputs; i++)
		{
			ModulatorData*	state	= &mState[i];
			UInt32			target	= state->target;
			
			if (target == kModTarget_Off || target >= mFirstMod2Mod)
				continue;
			
			TLData*	data	= &mTimeLineData->mTLData[mFirstDataSet + i];
			Float32	signal	= getOutput(chunkSize, state, data, &mOutput[mFirstMod2Mod + i]);
			
			switch(target)
			{
				case kModTarget_Osc_Width:
					mOutput[kModTarget_Osc1_Width] *= signal;
					mOutput[kModTarget_Osc2_Width] *= signal;
					mOutput[kModTarget_Osc3_Width] *= signal;
					break;

				case kModTarget_Osc_Tune:
					mOutput[kModTarget_Osc1_Tune] *= signal;
					mOutput[kModTarget_Osc2_Tune] *= signal;
					mOutput[kModTarget_Osc3_Tune] *= signal;
					break;

				case kModTarget_Osc_Phase:
					mOutput[kModTarget_Osc1_Phase] *= signal;
					mOutput[kModTarget_Osc2_Phase] *= signal;
					mOutput[kModTarget_Osc3_Phase] *= signal;
					break;

				case kModTarget_Osc_Spread:
					mOutput[kModTarget_Osc1_Spread] *= signal;
					mOutput[kModTarget_Osc2_Spread] *= signal;
					mOutput[kModTarget_Osc3_Spread] *= signal;
					break;

				case kModTarget_Shp_CutOff:
					mOutput[kModTarget_Shp1_CutOff] *= signal;
					mOutput[kModTarget_Shp2_CutOff] *= signal;
					mOutput[kModTarget_Shp3_CutOff] *= signal;
					break;

				case kModTarget_Shp_Resonance:
					mOutput[kModTarget_Shp1_Resonance] *= signal;
					mOutput[kModTarget_Shp2_Resonance] *= signal;
					mOutput[kModTarget_Shp3_Resonance] *= signal;
					break;

				case kModTarget_Shp_Bias:
					mOutput[kModTarget_Shp1_Bias] *= signal;
					mOutput[kModTarget_Shp2_Bias] *= signal;
					mOutput[kModTarget_Shp3_Bias] *= signal;
					break;

				case kModTarget_Shp_Drive:
					mOutput[kModTarget_Shp1_Drive] *= signal;
					mOutput[kModTarget_Shp2_Drive] *= signal;
					mOutput[kModTarget_Shp3_Drive] *= signal;
					break;

				case kModTarget_Mix_Panorama:
					mOutput[kModTarget_Mix1_Panorama] *= signal;
					mOutput[kModTarget_Mix2_Panorama] *= signal;
					mOutput[kModTarget_Mix3_Panorama] *= signal;
					break;

				case kModTarget_Mix_Level:
					mOutput[kModTarget_Mix1_Level] *= signal;
					mOutput[kModTarget_Mix2_Level] *= signal;
					mOutput[kModTarget_Mix3_Level] *= signal;
					break;

				default:
					//	printf("Modulator::getModulationWithState signal %f\n", signal);

					mOutput[target] *= signal;
			}
		}
	}
	
	return &mOutput[0];
}

// -----------------------------------------------------------------------------

void Modulator::start(Float32 keyTrack, Float32 velocity)
{
	mKeyTrack	= keyTrack;
	mVelocity	= velocity;

	mResetMod	= true;
}

// -----------------------------------------------------------------------------

void Modulator::update(Float32 keyTrack)
{
	mKeyTrack = keyTrack;
}

// -----------------------------------------------------------------------------

void Modulator::reset()
{
	mKeyDown	= false;
	mResetMod	= false;

	if (mState)
	{
		//	printf("Modulator::reset\n");

		for (UInt32 i = 0; i < mNumberOfInputs; i++)
		{
			ModulatorData* state = &mState[i];

			state->lookup		= 0;
			state->time			= 0.0;
			state->direction	= 1.0;
		}
	}
}

// -----------------------------------------------------------------------------

void Modulator::clear()
{
	if (mOutput)
	{
		//	printf("Modulator::clear\n");
	
		for (UInt32 i = 0; i < mNumberOfTargets; i++)
			mOutput[i] = 1.0;
	}
}

// -----------------------------------------------------------------------------

void Modulator::cleanup()
{
	//	printf("Modulator::cleanup\n");

	if (mState)
		delete[] mState;
		
	if (mOutput)
		delete[] mOutput;
}

// -----------------------------------------------------------------------------

Float32 Modulator::getOutput(UInt32 chunkSize, ModulatorData* state, TLData* data, Float32* modulation)
{
	Float32	chunkRate	= chunkSize / mSampleRate;

	Float32 keyTrack	= 1.0;
	Float32 velocity	= 1.0;

	if (state->keyTrackDepth < 0.5)
		keyTrack = linear(state->keyTrackDepth * 2.0, 1.0 / mKeyTrack, 1.0);
	else if (state->keyTrackDepth > 0.5)
		keyTrack = linear((state->keyTrackDepth - 0.5) * 2.0, 1.0, mKeyTrack);

	if (state->velocityDepth < 0.5)
		velocity = linear(state->velocityDepth * 2.0, 1.0 - mVelocity, 1.0);
	else if (state->velocityDepth > 0.5)
		velocity = linear((state->velocityDepth - 0.5) * 2.0, 1.0, mVelocity);
		
	UInt32	now			= state->lookup;
	UInt32	next		= now + 1;
	
	Float32	fract		= (state->time - data->points[now].time) / data->timeDelta[now];
	Float32	value		= clamp_f(linear(fract, data->points[now].value, data->points[next].value), 0.0, 1.0) * velocity;
		
	//	if (now == data->syncOffsetIndex)
	//		printf("Modulator::getOutput now %ld fract %f value %f\n", now, fract, value);

	Float32	phaseInc	= chunkRate * kModSlices[state->timing];
	
	Float32	speed		= state->speed;
	Float32	level		= state->level;
	
	if (modulation)
	{
		speed *= modulation[kModulation_Speed];
		level *= modulation[kModulation_Level];
	}
		
	if (state->timing > kModTimeInSeconds)
		phaseInc *= mTimeLineData->mBeatsPerSecond;
	else
		phaseInc *= speed * keyTrack;
	
	state->time += phaseInc * state->direction;

	if (state->time < data->points[now].time)
	{
		switch(state->sync)
		{
			case kModSync_Ppong:
				if (state->time < data->leftMarkerTime)
				{
					state->time			= data->leftMarkerTime;		
					state->direction	= 1.0;
				}
				else 
					state->lookup--;
				break;
		}
	}
	else if (state->time > data->points[next].time)
	{
		switch(state->sync)
		{
			case kModSync_Off:
				if (state->time > 1.0f)
				{
					state->lookup = 0;
					state->time  -= 1.0;
				}
				else 
					state->lookup++;
				break;
			
			case kModSync_Loop:
				if (state->time > data->rightMarkerTime)
				{
					state->lookup = data->leftMarkerIndex;
					state->time  -= data->markerDelta;
				}
				else 
					state->lookup++;
				break;
			
			case kModSync_Ppong:
				if (state->time > data->rightMarkerTime)
				{
					state->time			= data->rightMarkerTime;
					state->direction	= -1.0;
				}
				else 
					state->lookup++;
				break;

			case kModSync_OneShot:
				if (state->time > 1.0)
					state->time = 1.0;
				else 
					state->lookup++;
				break;
					
			case kModSync_Envelope:
				if (state->time > data->rightMarkerTime)
				{
					if (mKeyDown)
					{
						state->lookup = data->leftMarkerIndex;
						state->time  -= data->markerDelta;
					}
					else if (state->time > 1.0)
						state->time = 1.0;
					else 
						state->lookup++;
				}
				else 
					state->lookup++;
				break;
		}
	}
				
	return linear(level, 1.0, value);
}

// -----------------------------------------------------------------------------

void Modulator::printModulationData()
{
	if (mOutput)
	{
		for (UInt32 i = 0; i < mNumberOfTargets; i++)
			printf("output[%ld] = %ff\n", i, mOutput[i]);

		printf("\n");
	}
}

// -----------------------------------------------------------------------------
