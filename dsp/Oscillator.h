/*
 *  Oscillator.h
 *  Automat
 *
 *  Created by Stefan Beck on 09.04.06.
 *  Copyright 2006 alphakanal. All rights reserved.
 *
 */

// -----------------------------------------------------------------------------

#ifndef OSCILLATOR_H
#define OSCILLATOR_H

#include "minblep_tables.h"
#include "wave_data.h"

#include "Constants.h"
#include "Grain.h"
#include "Inlines.h"
#include "SampleData.h"

// -----------------------------------------------------------------------------

class Oscillator
{
	enum
	{
		kModulation_Width,
		kModulation_Tune,
		kModulation_Phase,
		kModulation_Spread,
	};

	enum
	{
		kWave_Off,
		kWave_Sine,
		kWave_Triangle,
		kWave_Rectangle,
		kWave_Sawtooth,
		kWave_Table,
		kWave_Sample,
		kWave_Grain,
		kWave_Noise,
		kWave_Clicks,
		kWave_LinIn
	};

	enum
	{
		kSync_Off,
		kSync_Key,
		kSync_Osc,
	};
	
	typedef struct OscData
	{
		Float32	phase;
		Float32 lastPhaseInc;
		
		Float32 syncPhase;
		Float32 lastSyncPhaseInc;
				
		Float32	lastMorph;
		
		UInt32	index;
		UInt32	outputState;
		Float32	output;
		
		Float32	hold;
		
		Float32	fillBuffer[FILL_LENGTH + STEP_DD_PULSE_LENGTH];
	}
	OscData;
		
public:

					Oscillator();
				   ~Oscillator();
	
	void			initialize(Float32 sampleRate, SampleData* sampleData);

	void			setParameter(Float32 pitchBend, UInt32 wave, Float32 morph, Float32 tune, Float32 detune, Float32 glide, UInt32 sync, Float32 syncOffset, UInt32 stack, Float32 stackOffset);
	void			getWave(UInt32 bufferSize, Float32* buffer, Float32* lineIn,  Float32* modulation);

	void			getSine(UInt32 bufferSize, Float32* buffer, Float32 morph, Float32 phaseInc, Float32 syncOffset, Float32 stackOffset);
	void			getTriangle(UInt32 bufferSize, Float32* buffer, Float32 morph, Float32 phaseInc, Float32 syncOffset, Float32 stackOffset);
	void			getRectangle(UInt32 bufferSize, Float32* buffer, Float32 morph, Float32 phaseInc, Float32 syncOffset, Float32 stackOffset);
	void			getSawtooth(UInt32 bufferSize, Float32* buffer, Float32 morph, Float32 phaseInc, Float32 syncOffset, Float32 stackOffset);
	void			getTable(UInt32 bufferSize, Float32* buffer, Float32 morph, Float32 phaseInc, Float32 syncOffset, Float32 stackOffset);
	void			getSample(UInt32 bufferSize, Float32* buffer, Float32 morph, Float32 phaseInc, Float32 syncOffset, Float32 stackOffset);
	void			getGrain(UInt32 bufferSize, Float32* buffer, Float32 morph, Float32 phaseInc, Float32 syncOffset, Float32 stackOffset);
	void			getRandomNoise(UInt32 bufferSize, Float32* buffer, Float32 morph, Float32 phaseInc, Float32 syncOffset, Float32 stackOffset);
	void			getClicks(UInt32 bufferSize, Float32* buffer, Float32 morph, Float32 phaseInc, Float32 syncOffset, Float32 stackOffset);
	void			getLineIn(UInt32 bufferSize, Float32* buffer, Float32* lineIn);
	
	void			start(UInt32 key, Float32 frequency);
	void			update(UInt32 key, Float32 frequency);
	
	void			reset();
	void			clear();

private:

	void			cleanup();
	
	void			getSync(UInt32 bufferSize, Float32* buffer, OscData* data, Float32 syncPhaseInc);
	void			getFFT(UInt32 bufferSize, Float32* inBuffer, Float32* outBuffer);
	
	inline void		placeStep(Float32* buffer, UInt32 index, Float32 phase, Float32 w, Float32 scale);
	inline void		placeSlope(Float32* buffer, UInt32 index, Float32 phase, Float32 w, Float32 slope_delta);

	Float32			mFrequency, mMorph, mPhaseInc, mSyncOffset, mStackOffset, mStackScale;
	UInt32			mKey, mWave, mSync, mStack, mStackCount;
	bool			mResetPhase;
		
	OscData			mOscData[kOscNumberOfStacks];
	Float32			mSyncBuffer[kMaxRenderSlice];
	
	Float32			mSampleRate;
	SampleData*		mSampleData;
	
	Grain			mGrain[kOscNumberOfStacks];
	
	FFTSetup		mSetupReal;
	UInt32			mLog2NumOfSamples;
};

// -----------------------------------------------------------------------------

inline void Oscillator::placeStep(Float32* buffer, UInt32 index, Float32 phase, Float32 w, Float32 scale)
{
	Float32	r;
	UInt32	i;
	
	r	 = MINBLEP_PHASES * phase / w;
	i	 = lrintf(r - 0.5);
	r	-= (Float32)i;
	
	i	&= MINBLEP_PHASE_MASK;
	
	/*
	while (i < 0)
	{
		i += MINBLEP_PHASES;
		index++;
	}
	*/
	
	while (i < MINBLEP_PHASES * STEP_DD_PULSE_LENGTH)
	{
		buffer[index]	+= scale * (step_dd_table[i].value + r * step_dd_table[i].delta);
		i				+= MINBLEP_PHASES;
		
		index++;
	}
}

inline void Oscillator::placeSlope(Float32* buffer, UInt32 index, Float32 phase, Float32 w, Float32 slope_delta)
{
	Float32	r;
	UInt32	i;
	
	r	 = MINBLEP_PHASES * phase / w;
	i	 = lrintf(r - 0.5);
	r	-= (Float32)i;
	i	&= MINBLEP_PHASE_MASK;
	
	slope_delta *= w;
	
	while (i < MINBLEP_PHASES * SLOPE_DD_PULSE_LENGTH)
	{
		buffer[index]	+= slope_delta * (slope_dd_table[i] + r * (slope_dd_table[i + 1] - slope_dd_table[i]));
		i				+= MINBLEP_PHASES;
		
		index++;
	}
}

// -----------------------------------------------------------------------------

#endif

// -----------------------------------------------------------------------------
