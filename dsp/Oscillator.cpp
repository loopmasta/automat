/*
 *  Oscillator.cpp
 *  Automat
 *
 *  Created by Stefan Beck on 09.04.06.
 *  Copyright 2006 alphakanal. All rights reserved.
 *
 */

// -----------------------------------------------------------------------------

#include "Oscillator.h"

// -----------------------------------------------------------------------------

Oscillator::Oscillator()
{
	mKey				= 0;
	mFrequency			= 0.0;
	mWave				= kWave_Off;
	mMorph				= 0.0;
	mSync				= kSync_Off;
	mSyncOffset			= 0.0;
	mStack				= 0;
	mStackCount			= mStack + 1;
	mStackOffset		= 0.0;
	
	mSetupReal			= NULL;
	mLog2NumOfSamples	= 0;
	
	reset();
}

// -----------------------------------------------------------------------------

Oscillator::~Oscillator()
{
	cleanup();
}

// -----------------------------------------------------------------------------

void Oscillator::initialize(Float32 sampleRate, SampleData* sampleData)
{ 
	mSampleRate	= sampleRate;
	mSampleData = sampleData;

	reset();
	clear();
}

// -----------------------------------------------------------------------------

void Oscillator::setParameter(Float32 pitchBend, UInt32 wave, Float32 morph, Float32 tune, Float32 detune, Float32 glide, UInt32 sync, Float32 syncOffset, UInt32 stack, Float32 stackOffset)
{	
	//	printf("Oscillator::setParameter pitchBend %f morph %f tune %f detune %f glide %f\n", pitchBend, morph, tune, detune, glide);

	if (wave != mWave || sync != mSync)
	{
		reset();
		clear();
	}
		
	mWave			= wave;
	mMorph			= morph;
	mSync			= sync;
	mSyncOffset		= syncOffset;
	mStack			= stack;
	mStackCount		= stack + 1;
	mStackScale		= 1.0 / mStackCount;
	mStackOffset	= stackOffset;
	
	Float32	pitch	= pow(2.0, ((tune - 24.0) + (detune - 0.5) + (pitchBend * 12)) / 12.0);
	
	mPhaseInc += pow3(1.0 - glide) * ((mFrequency / mSampleRate * pitch) - mPhaseInc);
}

// -----------------------------------------------------------------------------

void Oscillator::getWave(UInt32 bufferSize, Float32* buffer, Float32* lineIn, Float32* modulation)
{
	Float32	morph		= mMorph;
	Float32	phaseInc	= mPhaseInc;
	Float32	syncOffset	= mSyncOffset;
	Float32	stackOffset	= mStackOffset;
	
	if (modulation)
	{
		morph		*= modulation[kModulation_Width];
		phaseInc	*= modulation[kModulation_Tune];
		syncOffset	*= modulation[kModulation_Phase];
		stackOffset	*= modulation[kModulation_Spread];
	}
	
	if (mResetPhase && mSync == kSync_Key)
	{		
		reset();
		clear();

		for (UInt32 i = 0; i < mStackCount; i++)
		{
			Float32 phase = 0.0;
			
			if (mStackCount == 1)
				phase = syncOffset;
			else
				phase = mStackScale * i * syncOffset; 
			
			mOscData[i].phase = phase;
		}
	}
	
	switch(mWave)
	{
		case kWave_Off:
			break;
			
		case kWave_Sine:
			getSine(bufferSize, buffer, morph, phaseInc, syncOffset, stackOffset);
			break;
			
		case kWave_Triangle:
			getTriangle(bufferSize, buffer, morph, phaseInc, syncOffset, stackOffset);
			break;
			
		case kWave_Rectangle:
			getRectangle(bufferSize, buffer, morph, phaseInc, syncOffset, stackOffset);
			break;

		case kWave_Sawtooth:
			getSawtooth(bufferSize, buffer, morph, phaseInc, syncOffset, stackOffset);
			break;

		case kWave_Table:
			getTable(bufferSize, buffer, morph, phaseInc, syncOffset, stackOffset);
			break;

		case kWave_Sample:
			if (mSampleData != NULL && mSampleData->getSampleReady())
			{
				getSample(bufferSize, buffer, morph, phaseInc, syncOffset, stackOffset);
				getFFT(bufferSize, buffer, buffer);
			}
			break;

		case kWave_Grain:
			if (mSampleData != NULL && mSampleData->getSampleReady())
				getGrain(bufferSize, buffer, morph, phaseInc, syncOffset, stackOffset);
			break;
			
		case kWave_Noise:
			getRandomNoise(bufferSize, buffer, morph, phaseInc, syncOffset, stackOffset);
			break;
			
		case kWave_Clicks:
			if (mSampleData != NULL && mSampleData->getSampleReady())
				getClicks(bufferSize, buffer, morph, phaseInc, syncOffset, stackOffset);
			break;

		case kWave_LinIn:
			getLineIn(bufferSize, buffer, lineIn);
			break;
	}
}

// -----------------------------------------------------------------------------

void Oscillator::getSine(UInt32 bufferSize, Float32* buffer, Float32 morph, Float32 phaseInc, Float32 syncOffset, Float32 stackOffset)
{
	Float32	level			= kOscStackLevel[mStack];
	Float32	ramp			= 1.0 / bufferSize;
	
	Float32	l_morph			= quadInOut(morph);
	Float32	l_stackOffset	= pow3(stackOffset);

	Float32	keytrack		= mKey / 128.0;
	
	for (UInt32 i = 0; i < mStackCount; i++)
	{		
		OscData*	data		= &mOscData[i];
		UInt32		size		= bufferSize;
		Float32*	out			= buffer;

		Float32		p			= data->phase;
		Float32		w			= data->lastPhaseInc;
		Float32		b			= data->lastMorph;
	
		Float32		l_phaseInc	= phaseInc + phaseInc * kOscStackOffset[i] * l_stackOffset;
				
		Float32		dw			= (l_phaseInc - w) * ramp;
		Float32		db			= (l_morph - b) * ramp;

		Float32		lastOut		= data->output;
		
		while (size--)
		{
			w += dw; 
			b += db;
			p += w;
			
			if (p >= 1.0)
				p -= 1.0;

			lastOut	 = fastSin(((p * 2.0 - 1.0) * M_PI) + lastOut * (b - 0.5f) * (2.1 - keytrack));
			*out++	+= lastOut * level * 0.5;
			
			//	printf("Oscillator::getSine out %f\n", lastOut * level * 0.5);
		}
		
		data->output		= lastOut;
		
		data->phase			= p;
		data->lastPhaseInc	= w;
		data->lastMorph		= b;
	}
}

// -----------------------------------------------------------------------------

void Oscillator::getTriangle(UInt32 bufferSize, Float32* buffer, Float32 morph, Float32 phaseInc, Float32 syncOffset, Float32 stackOffset)
{		
	Float32	level			= kOscStackLevel[mStack];
	Float32	ramp			= 1.0 / bufferSize;
	
	Float32 l_morph			= (0.01 + quadInOut(morph) * 0.98);
	Float32	l_stackOffset	= pow3(stackOffset);
	
	for (UInt32 i = 0; i < mStackCount; i++)
	{	
		OscData*	data		= &mOscData[i];
		UInt32		size		= bufferSize;
		Float32*	out			= buffer;
		 
		Float32*	f			= data->fillBuffer;
		Float32*	sync		= &mSyncBuffer[0];
	
		Float32		p			= data->phase;
		Float32		w			= data->lastPhaseInc;
		Float32		b			= data->lastMorph;
		UInt32		j			= data->index;
		UInt32		k			= data->outputState;
	
		Float32		l_phaseInc	= phaseInc + phaseInc * kOscStackOffset[i] * l_stackOffset;
		
		if (mSync == kSync_Osc)
		{
			getSync(size, sync, data, l_phaseInc);
			l_phaseInc += l_phaseInc * syncOffset * 2.0;
		}
		
		Float32	dw	= (l_phaseInc - w) * ramp;
		Float32	db	= (l_morph - b) * ramp;
		
		Float32	b1, x;
		
		while (size--)
		{
			w += dw; 
			b += db;
			b1 = 1.0 - b;
			p += w;
			
			if (*sync >= 1e-20)
			{
				Float32 eof_offset = (*sync - 1e-20) * w;
				Float32 p_at_reset = p - eof_offset;
				
				p = eof_offset;
				
				if (!k)
				{
					x = -0.5 + p_at_reset / b;
					
					if (p_at_reset >= b)
					{
						x = 0.5 - (p_at_reset - b) / b1;
						placeSlope(f, j, p_at_reset - b + eof_offset, w, -1.0 / b1 - 1.0 / b);
						k = 1;
					}
					
					if (p_at_reset >= 1.0)
					{
						p_at_reset -= 1.0;
						x = -0.5f + p_at_reset / b;
						placeSlope(f, j, p_at_reset + eof_offset, w, 1.0 / b + 1.0 / b1);
						k = 0;
					}
				}
				else
				{
					x = 0.5f - (p_at_reset - b) / b1;
					
					if (p_at_reset >= 1.0)
					{
						p_at_reset -= 1.0;
						x = -0.5 + p_at_reset / b;
						placeSlope(f, j, p_at_reset + eof_offset, w, 1.0 / b + 1.0 / b1);
						k = 0;
					}
					
					if (!k && p_at_reset >= b)
					{
						x = 0.5 - (p_at_reset - b) / b1;
						placeSlope(f, j, p_at_reset - b + eof_offset, w, -1.0 / b1 - 1.0 / b);
						k = 1;
					}
				}
				
				if (k)
					placeSlope(f, j, p, w, 1.0 / b + 1.0 / b1);
				
				placeStep(f, j, p, w, -0.5 - x);
				x = -0.5 + p / b;
				k = 0;
				
				if (p >= b)
				{
					x = 0.5 - (p - b) / b1;
					placeSlope(f, j, p - b, w, -1.0 / b1 - 1.0 / b);
					k = 1;
				}
			}
			else if (!k)
			{
				x = -0.5 + p / b;
				
				if (p >= b)
				{
					x = 0.5 - (p - b) / b1;
					placeSlope(f, j, p - b, w, -1.0 / b1 - 1.0 / b);
					k = 1;
				}
				
				if (p >= 1.0)
				{
					p -= 1.0;
					x = -0.5 + p / b;
					placeSlope(f, j, p, w, 1.0 / b + 1.0 / b1);
					k = 0;
				}
			}
			else
			{
				x = 0.5 - (p - b) / b1;
				
				if (p >= 1.0)
				{
					p -= 1.0;
					x = -0.5 + p / b;
					placeSlope(f, j, p, w, 1.0 / b + 1.0 / b1);
					k = 0;
				}
				
				if (!k && p >= b)
				{
					x = 0.5 - (p - b) / b1;
					placeSlope(f, j, p - b, w, -1.0 / b1 - 1.0 / b);
					k = 1;
				}
			}
			
			f[j + DD_SAMPLE_DELAY] += x;
			
			*out++ += f[j] * level;
			
			sync++;
			
			if (++j == FILL_LENGTH)
			{ 
				j = 0;
				
				memcpy(f, f + FILL_LENGTH, STEP_DD_PULSE_LENGTH * sizeof(Float32));
				memset(f + STEP_DD_PULSE_LENGTH, 0,  FILL_LENGTH * sizeof(Float32));
			}
		}
	
		data->phase			= p;
		data->lastPhaseInc	= w;
		data->lastMorph		= b;
		data->index			= j;
		data->outputState	= k;
	}
}

// -----------------------------------------------------------------------------

void Oscillator::getRectangle(UInt32 bufferSize, Float32* buffer, Float32 morph, Float32 phaseInc, Float32 syncOffset, Float32 stackOffset)
{
	Float32	level			= kOscStackLevel[mStack];
	Float32	ramp			= 1.0 / bufferSize;

	Float32 l_morph			= (0.01 + quadInOut(morph) * 0.98);
	Float32	l_stackOffset	= pow3(stackOffset);
		
	for (UInt32 i = 0; i < mStackCount; i++)
	{
		OscData*	data		= &mOscData[i];
		UInt32		size		= bufferSize;
		Float32*	out			= buffer;
		 
		Float32*	f			= data->fillBuffer;
		Float32*	sync		= &mSyncBuffer[0];

		Float32		p			= data->phase;
		Float32		w			= data->lastPhaseInc;
		Float32		b			= data->lastMorph;
		UInt32		j			= data->index;
		UInt32		k			= data->outputState;
		Float32		x			= data->output;
		
		Float32		l_phaseInc	= phaseInc + phaseInc * kOscStackOffset[i] * l_stackOffset;
		
		if (mSync == kSync_Osc)
		{
			getSync(size, sync, data, l_phaseInc);
			l_phaseInc += l_phaseInc * syncOffset * 2.0;
		}
			 
		Float32	dw	= (l_phaseInc - w) * ramp;
		Float32	db	= (l_morph - b) * ramp;
		
		while (size--)
		{
			w += dw; 
			b += db;
			p += w;
			
			if (*sync >= 1e-20)
			{
				Float32 eof_offset = (*sync - 1e-20) * w;
				Float32 p_at_reset = p - eof_offset;
				
				p = eof_offset;
				
				if (!k)
				{
					if (p_at_reset >= b)
					{
						placeStep(f, j, p_at_reset - b + eof_offset, w, -1.0);
						k = 1;
						x = -0.5;
					}
					
					if (p_at_reset >= 1.0)
					{
						p_at_reset -= 1.0;
						placeStep(f, j, p_at_reset + eof_offset, w, 1.0);
						k = 0;
						x = 0.5;
					}
				}
				else
				{
					if (p_at_reset >= 1.0)
					{
						p_at_reset -= 1.0;
						placeStep(f, j, p_at_reset + eof_offset, w, 1.0);
						k = 0;
						x = 0.5;
					}
					
					if (!k && p_at_reset >= b)
					{
						placeStep(f, j, p_at_reset - b + eof_offset, w, -1.0);
						k = 1;
						x = -0.5f;
					}
				}
				
				if (k)
				{
					placeStep(f, j, p, w, 1.0);
					k = 0;
					x = 0.5;
				}
				
				if (p >= b)
				{
					placeStep(f, j, p - b, w, -1.0);
					k = 1;
					x = -0.5;
				}
			}
			else if (!k)
			{
				if (p >= b)
				{
					placeStep(f, j, p - b, w, -1.0);
					k = 1;
					x = -0.5;
				}
				
				if (p >= 1.0)
				{
					p -= 1.0;
					placeStep(f, j, p, w, 1.0);
					k = 0;
					x = 0.5;
				}
			}
			else
			{
				if (p >= 1.0)
				{
					p -= 1.0;
					placeStep(f, j, p, w, 1.0);
					k = 0;
					x = 0.5;
				}
				
				if (!k && p >= b)
				{
					placeStep(f, j, p - b, w, -1.0);
					k = 1;
					x = -0.5;
				}
			}
			
			f[j + DD_SAMPLE_DELAY] += x;
			
			*out++ += f[j] * level;
			
			sync++;
			
			if (++j == FILL_LENGTH)
			{ 
				j = 0;
				
				memcpy(f, f + FILL_LENGTH, STEP_DD_PULSE_LENGTH * sizeof(Float32));
				memset(f + STEP_DD_PULSE_LENGTH, 0,  FILL_LENGTH * sizeof(Float32));
			}
		}

		data->phase			= p;
		data->lastPhaseInc	= w;
		data->lastMorph		= b;
		data->index			= j;
		data->outputState	= k;
		data->output		= x;	
	}	
}

// -----------------------------------------------------------------------------

void Oscillator::getSawtooth(UInt32 bufferSize, Float32* buffer, Float32 morph, Float32 phaseInc, Float32 syncOffset, Float32 stackOffset)
{		
	Float32	level			= kOscStackLevel[mStack];
	Float32	ramp			= 1.0 / bufferSize;

	Float32 l_morph			= (0.01 + quadInOut(morph) * 0.98);
	Float32	l_stackOffset	= pow3(stackOffset);
		
	for (UInt32 i = 0; i < mStackCount; i++)
	{
		OscData*	data		= &mOscData[i];
		UInt32		size		= bufferSize;
		Float32*	out			= buffer;
		 
		Float32*	f			= data->fillBuffer;
		Float32*	sync		= &mSyncBuffer[0];

		Float32		p			= data->phase;
		Float32		w			= data->lastPhaseInc;
		Float32		b			= data->lastMorph;
		UInt32		j			= data->index;
	
		Float32		l_phaseInc	= phaseInc + phaseInc * kOscStackOffset[i] * l_stackOffset;
		
		if (mSync == kSync_Osc)
		{
			getSync(size, sync, data, l_phaseInc);
			l_phaseInc += l_phaseInc * syncOffset * 2.0;
		}
			 
		Float32	dw	= (l_phaseInc - w) * ramp;
		Float32	db	= (l_morph - b) * ramp;
		
		while (size--)
		{
			w += dw; 
			b += db;
			p += w;
			
			if (*sync >= 1e-20)
			{
				Float32 eof_offset = (*sync - 1e-20) * w;
				Float32 p_at_reset = p - eof_offset;
				
				p = eof_offset;
				
				if (p_at_reset >= 1.0)
				{
					p_at_reset -= 1.0;
					placeStep(f, j, p_at_reset + eof_offset, w, 1.0);
				}
				
				placeStep(f, j, p, w, p_at_reset);
			}
			else if (p >= 1.0)
			{
				p -= 1.0;
				placeStep(f, j, p, w, 1.0);
			}
			
			f[j + DD_SAMPLE_DELAY] += 0.5 - p;
			
			*out++ += f[j] * level;
			
			sync++;
			
			if (++j == FILL_LENGTH)
			{ 
				j = 0;
				
				memcpy(f, f + FILL_LENGTH, STEP_DD_PULSE_LENGTH * sizeof(Float32));
				memset(f + STEP_DD_PULSE_LENGTH, 0,  FILL_LENGTH * sizeof(Float32));
			}
		}
		
		data->phase			= p;
		data->lastPhaseInc	= w;
		data->lastMorph		= b;
		data->index			= j;
	}
}

// -----------------------------------------------------------------------------

void Oscillator::getTable(UInt32 bufferSize, Float32* buffer, Float32 morph, Float32 phaseInc, Float32 syncOffset, Float32 stackOffset)
{
	Float32	level			= kOscStackLevel[mStack];
	Float32	ramp			= 1.0 / bufferSize;

	Float32 l_morph			= morph;
	Float32	l_stackOffset	= pow3(stackOffset);
	
	Float32	shortToFloat	= 1.0 / -65534.0;
	Float32	tableCount		= (Float32)(WAVETABLE_COUNT - 2);
	Float32	tablePoints		= (Float32)WAVETABLE_POINTS;
	
	UInt32	dataIndex		= mKey < 47 ? 0 : mKey > 127 ? 11 : ((mKey + 1) / 8) - 5;

	for (UInt32 i = 0; i < mStackCount; i++)
	{
		OscData*		data		= &mOscData[i];
		UInt32			size		= bufferSize;
		Float32*		out			= buffer;

		Float32*		f			= data->fillBuffer;
		Float32*		sync		= &mSyncBuffer[0];
		 
		Float32			p			= data->phase;
		Float32			w			= data->lastPhaseInc;
		Float32			b			= data->lastMorph;
		UInt32			j			= data->index;
	
		Float32			l_phaseInc	= phaseInc + phaseInc * kOscStackOffset[i] * l_stackOffset;
		
		if (mSync == kSync_Osc)
		{
			getSync(size, sync, data, l_phaseInc);
			l_phaseInc += l_phaseInc * syncOffset * 2.0;
		}
			 
		Float32			dw			= (l_phaseInc - w) * ramp;
		Float32			db			= (l_morph - b) * ramp;
	
		UInt32			readM1, read0, read1, read2, waveIndex0, waveIndex1;
		Float32			fractTable, fractWave, wave, wave0, wave1;

		WaveSet*		waveSet0;
		WaveSet*		waveSet1;
		
		signed short*	data0;
		signed short*	data1;
		
		while (size--)
		{
			w			+= dw; 
			b			+= db;
			p			+= w;
			
			fractTable	 = b * tableCount;
			waveIndex0	 = (int)(fractTable);
			waveIndex1	 = waveIndex0 + 1;
			fractTable	-= waveIndex0;
			
			waveSet0	 = &wavetable[waveIndex0];
			waveSet1	 = &wavetable[waveIndex1];
			
			//	printf("Oscillator::getTable wave1 %s, wave2 %s, key %ld, index %ld\n", waveSet0->name, waveSet1->name, mKey, dataIndex);

			data0	= waveSet0->wave[dataIndex].data;
			data1	= waveSet1->wave[dataIndex].data;

			if (*sync >= 1e-20)
			{
				Float32 eof_offset = (*sync - 1e-20) * w;
				Float32 p_at_reset = p - eof_offset;
				
				p = eof_offset;
				
				if (p_at_reset >= 1.0)
					p_at_reset -= 1.0;

				fractWave	 = p_at_reset * tablePoints;
				read0		 = (int)(fractWave);
				readM1		 = read0 - 1;
				read1		 = read0 + 1;
				read2		 = read1 + 1;
				fractWave	-= read0;
				
				wave0		 = cubic(fractWave, (Float32)data0[readM1], (Float32)data0[read0], (Float32)data0[read1], (Float32)data0[read2]) * shortToFloat;
				wave1		 = cubic(fractWave, (Float32)data1[readM1], (Float32)data1[read0], (Float32)data1[read1], (Float32)data1[read2]) * shortToFloat;			
				wave		 = linear(fractTable, wave0, wave1);
				
				placeStep(f, j, p, w, wave);
			}
			else if (p >= 1.0)
			{
				p -= 1.0;
			}			
			
			fractWave	 = p * tablePoints;
			read0		 = (int)(fractWave);
			readM1		 = read0 - 1;
			read1		 = read0 + 1;
			read2		 = read1 + 1;
            fractWave	-= read0;
			
            wave0		 = cubic(fractWave, (Float32)data0[readM1], (Float32)data0[read0], (Float32)data0[read1], (Float32)data0[read2]) * shortToFloat;
            wave1		 = cubic(fractWave, (Float32)data1[readM1], (Float32)data1[read0], (Float32)data1[read1], (Float32)data1[read2]) * shortToFloat;			
			wave		 = linear(fractTable, wave0, wave1);
			
			f[j + DD_SAMPLE_DELAY] += wave;
			
			*out++ += f[j] * level;
			
			sync++;
			
			if (++j == FILL_LENGTH)
			{ 
				j = 0;
				
				memcpy(f, f + FILL_LENGTH, STEP_DD_PULSE_LENGTH * sizeof(Float32));
				memset(f + STEP_DD_PULSE_LENGTH, 0,  FILL_LENGTH * sizeof(Float32));
			}
		}
	
		data->phase			= p;
		data->lastPhaseInc	= w;
		data->lastMorph		= b;
		data->index			= j;
	}
}

// -----------------------------------------------------------------------------

void Oscillator::getSample(UInt32 bufferSize, Float32* buffer, Float32 morph, Float32 phaseInc, Float32 syncOffset, Float32 stackOffset)
{
	Float32		level			= kOscStackLevel[mStack];
	Float32		ramp			= 1.0 / bufferSize;

	Float32		l_morph			= morph;
	Float32		l_stackOffset	= pow3(stackOffset);

	Float32*	sampleData		= mSampleData->getSampleData();
	UInt32		sampleSize		= mSampleData->getSampleSize();
	
	phaseInc *= mSampleData->getSampleRate() / mSampleData->getSampleKey();
	
	for (UInt32 i = 0; i < mStackCount; i++)
	{
		OscData*	data		= &mOscData[i];
		UInt32		size		= bufferSize;
		Float32*	out			= buffer;

		Float32		p			= data->phase;
		Float32		w			= data->lastPhaseInc;
		Float32		b			= data->lastMorph;
	
		Float32		l_phaseInc	= phaseInc + phaseInc * kOscStackOffset[i] * l_stackOffset;
				
		Float32		dw			= (l_phaseInc - w) * ramp;
		Float32		db			= (l_morph - b) * ramp;
	
		SInt32		readM1;
		UInt32		read0, read1, read2;
		Float32		fract, wave;
			
		while (size--)
		{
			w += dw; 
			b += db;
			p += w;
			
			if (p >= sampleSize)
				p -= sampleSize;
			
			read0	= (int)(p);
			fract   = p - read0;
			
			readM1	= read0 - 1;
			
			if (readM1 < 0)
				readM1 += sampleSize;
				
			read1	= (read0 + 1) % sampleSize;
			read2	= (read1 + 1) % sampleSize;
			
			wave	= cubic(fract, sampleData[readM1], sampleData[read0], sampleData[read1], sampleData[read2]);
						
			*out++ += wave * level;			
		}
	
		data->phase			= p;
		data->lastPhaseInc	= w;
		data->lastMorph		= b;
	}
}

// -----------------------------------------------------------------------------

void Oscillator::getGrain(UInt32 bufferSize, Float32* buffer, Float32 morph, Float32 phaseInc, Float32 syncOffset, Float32 stackOffset)
{
	Float32*	sampleData	= mSampleData->getSampleData();
	UInt32		sampleSize	= mSampleData->getSampleSize();

	phaseInc *= mSampleData->getSampleRate() / mSampleData->getSampleKey();

	Float32		time		= (mSampleRate / kMaxRenderSlice);
	
	Float32		smooth		= 1.0 / (1.0 + pow3(syncOffset) * time);
	Float32		hold		= 1.0 / (1.0 + pow3(morph) * time);
	Float32		wait		= 1.0 / (1.0 + pow3(stackOffset) * time);

	for (UInt32 i = 0; i < mStackCount; i++)
	{
		Grain* grain = &mGrain[i];
		
		if (grain->done())
			grain->start(drand48() * sampleSize);
			
		grain->sample(phaseInc, smooth, hold, smooth, wait, sampleSize, sampleData, bufferSize, buffer);
	}
}

// -----------------------------------------------------------------------------

void Oscillator::getRandomNoise(UInt32 bufferSize, Float32* buffer, Float32 morph, Float32 phaseInc, Float32 syncOffset, Float32 stackOffset)
{
	while (bufferSize--)
	{
		*buffer++ = drand48() - 0.5;
	}
}

// -----------------------------------------------------------------------------

void Oscillator::getClicks(UInt32 bufferSize, Float32* buffer, Float32 morph, Float32 phaseInc, Float32 syncOffset, Float32 stackOffset)
{
	Float32		ramp		= 1.0 / bufferSize;
	OscData*	data		= &mOscData[0];

	Float32		p			= data->phase;
	Float32		w			= data->lastPhaseInc;
	Float32		b			= data->lastMorph;
	Float32		h			= data->hold;

	Float32		dw			= (phaseInc - w) * ramp;
	Float32		db			= (morph - b) * ramp;

	if (h >= 1.0)
	{
		Float32		amplitude	= drand48();
		Float32		envelope	= 0.0;
		
		Float32		dur1		= ramp;
		Float32		dur2		= pow2(dur1);
		Float32		slope		= 4.0 * amplitude * (dur1 - dur2);
		Float32		curve		= -8.0 * amplitude * dur2;
			
		while (bufferSize--)
		{
			w += dw; 
			b += db;
			p += w;
			
			if (p >= 1.0)
				p -= 1.0;
			
			envelope   += slope;
			slope	   += curve;

			*buffer++	= (drand48() - 0.5) * envelope;		
		}
		
		h = drand48();
	}
	else
	{
		w += phaseInc; 
		b += morph;
		p += w;
		
		if (p >= 1.0)
			p -= 1.0;

		h += phaseInc;			
	}

	data->phase			= p;
	data->lastPhaseInc	= w;
	data->lastMorph		= b;
	
	data->hold			= h;
}

// -----------------------------------------------------------------------------

void Oscillator::getLineIn(UInt32 bufferSize, Float32* buffer, Float32* lineIn)
{
	if (lineIn)
		memcpy(buffer, lineIn, bufferSize * sizeof(Float32));
}

// -----------------------------------------------------------------------------

void Oscillator::start(UInt32 key, Float32 frequency)
{
	//	printf("Oscillator::start key %ld frequency %f\n", key, frequency);

	mKey		= key;
	mFrequency	= frequency;
	
	mResetPhase = true;
}

// -----------------------------------------------------------------------------

void Oscillator::update(UInt32 key, Float32 frequency)
{
	//	printf("Oscillator::update key %ld frequency %f\n", key, frequency);

	mKey		= key;
	mFrequency	= frequency;
}

// -----------------------------------------------------------------------------


void Oscillator::reset()
{
	mPhaseInc	= 0.0;
	mResetPhase	= false;
}

// -----------------------------------------------------------------------------

void Oscillator::clear()
{
	memset(&mOscData[0], 0, kOscNumberOfStacks * sizeof(OscData));
	memset(&mSyncBuffer[0], 0, kMaxRenderSlice * sizeof(Float32));
	
	for (UInt32 i = 0; i < kOscNumberOfStacks; i++)
		mGrain[i].reset();
}

// -----------------------------------------------------------------------------

void Oscillator::cleanup()
{
	if (mSetupReal)
		destroy_fftsetup(mSetupReal);		
}

// -----------------------------------------------------------------------------

void Oscillator::getSync(UInt32 bufferSize, Float32* buffer, OscData* data, Float32 syncPhaseInc)
{
	Float32	p	= data->syncPhase;
	Float32	w	= data->lastSyncPhaseInc;
	
	Float32	dw	= (syncPhaseInc - w) / bufferSize;
	
	while (bufferSize--)
	{
		w += dw;
		p += w;
		
		if (p >= 1.0)
		{
			p -= 1.0;
			*buffer++ = p / w + 1e-20;
		}
		else
			*buffer++ = 0.0;
	}
	
	data->syncPhase			= p;
	data->lastSyncPhaseInc	= w;
}

// -----------------------------------------------------------------------------

void Oscillator::getFFT(UInt32 bufferSize, Float32* inBuffer, Float32* outBuffer)
{
	UInt32	log2NumOfSamples	= (UInt32)log2(bufferSize);
	UInt32	numberOfSamples		= 1 << log2NumOfSamples;
	
	log2NumOfSamples += 1;

	//	printf("Oscillator::getFFT : numberOfSamples %ld, log2NumOfSamples %ld\n",  numberOfSamples, log2NumOfSamples);

	if (mSetupReal && log2NumOfSamples != mLog2NumOfSamples)
	{
		destroy_fftsetup(mSetupReal);
		mSetupReal = NULL;
	}

	mLog2NumOfSamples = log2NumOfSamples;

	if (!mSetupReal)
		mSetupReal = create_fftsetup(mLog2NumOfSamples, FFT_RADIX2);

	COMPLEX_SPLIT	cps;
	Float32			real_a[numberOfSamples] __attribute__ ((aligned (16)));
	Float32			imag_a[numberOfSamples] __attribute__ ((aligned (16)));
	
	cps.realp	= real_a;
	cps.imagp	= imag_a;
	
	memcpy(&real_a, inBuffer, numberOfSamples * sizeof(Float32));
	memset(&imag_a, 0, numberOfSamples * sizeof(Float32));	
	
	//	FFT	
	fft_zrip(mSetupReal, &cps, 1, mLog2NumOfSamples, FFT_FORWARD);
	
	//	modify fft data
	
	for (UInt32 i = 0; i < numberOfSamples; i++)
	{			
		//	real_a[i]	= 0.0;
		//	imag_a[i]	= 0.0;

		//	printf("SampleData::fftResynthesis : real_a %f, imag_a %f, i%ld\n",  real_a[i], imag_a[i], i);
	}
	
	//	IFFT	
    fft_zrip(mSetupReal, &cps, 1, mLog2NumOfSamples, FFT_INVERSE);
	
	//	NORMALIZE
	Float32 scale = (1.0 / numberOfSamples) * 0.3;
	vsmul(cps.realp, 1, &scale, outBuffer, 1, numberOfSamples);
}

// -----------------------------------------------------------------------------

