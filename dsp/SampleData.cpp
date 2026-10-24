/*
 *  SampleData.cpp
 *  Automat
 *
 *  Created by Stefan Beck on 09.04.06.
 *  Copyright (c) 2006-2008 Stefan Beck (stiwi@alphakanal, loopmasta). MIT License, see LICENSE.
 *
 */

// -----------------------------------------------------------------------------

#include "SampleData.h"

// -----------------------------------------------------------------------------

SampleData::SampleData()
{
	reset();
}

// -----------------------------------------------------------------------------
/*
SampleData::~SampleData()
{
	cleanup();
}
*/
// -----------------------------------------------------------------------------

void SampleData::readSampleData(CFURLRef url)
{
	reset();
	
	/*
	createWaveData(kSample_Sawtooth, 4096);
	return;
	*/
	
	try
	{   
		//	printf("SampleData::readWaveData try\n");
		
		FSRef						fsRef;
		CAAudioFile					audioFile;
		CAStreamBasicDescription	dataFormat;
		
		CFURLGetFSRef(url, &fsRef);
		audioFile.Open(fsRef);
				
		dataFormat.mSampleRate = audioFile.GetFileDataFormat().mSampleRate;
		dataFormat.SetCanonical(1, false);
				
		audioFile.SetClientFormat(dataFormat, NULL);
		
		/*
		printf("SampleData::readSampleData : SampleRate    %lf\n", audioFile.GetFileDataFormat().mSampleRate);
		printf("SampleData::readSampleData : Num channels  %ld\n", audioFile.GetClientDataFormat().NumberChannels());
		printf("SampleData::readSampleData : File duration %lf\n", audioFile.GetDurationSeconds());
		printf("SampleData::readSampleData : Frames        %lld\n", audioFile.GetNumberFrames());
		*/
		
		UInt32 readSamples = (UInt32)audioFile.GetNumberFrames();
		
		mSampleData.allocateClear(readSamples);
		Float32* fBuffer = mSampleData.getAlignedBuffer();
		
		AudioBufferList *buffers = CAAudioBufferList::Create(1);

		buffers->mBuffers[0].mData			= fBuffer;
		buffers->mBuffers[0].mDataByteSize	= readSamples * sizeof(Float32);
		
		audioFile.Read(readSamples, buffers);
		
		CAAudioBufferList::Destroy(buffers);

		mNumberOfSamples	= readSamples;
		mSampleRate			= audioFile.GetFileDataFormat().mSampleRate;
		mReady				= true;

		audioFile.Close();
	}
	catch (...)
	{
		//	printf("SampleData::readWaveData catch\n");
		
		reset();
		clear();
	}
}

// -----------------------------------------------------------------------------

void SampleData::createWaveData(UInt32 wave, UInt32 numberOfSamples)
{
	reset();

	mNumberOfSamples	= numberOfSamples;
	mSampleRate			= 44100.0;
	mSampleKey			= mSampleRate / mNumberOfSamples;
	
	mSampleData.allocateClear(mNumberOfSamples);
	Float32* b = mSampleData.getAlignedBuffer();

	//	printf("SampleData::createWaveData : wave %ld, mNumberOfSamples %ld \n",  wave, mNumberOfSamples);

	switch(wave)
	{
		case kSample_Sine:
			for(UInt32 i = 0; i < mNumberOfSamples; i++)
			{
				Float32 t = (i + 0.5) / mNumberOfSamples;

				t = t * 2.0 - 1.0;
				b[i] = t * ( 2.0 - fabsf(t * 2.0));
			}
			break;
			
		case kSample_Triangle:
			for(UInt32 i = 0; i < mNumberOfSamples; i++)
			{
				Float32 t = (i + 0.5) / mNumberOfSamples;
				b[i] = 0.5 - 2.0 * fabs(0.5 - t);
			}
			break;
			
		case kSample_Sawtooth:
			for(UInt32 i = 0; i < mNumberOfSamples; i++)
			{
				Float32 t = (i + 0.5) / mNumberOfSamples;
				b[i] = t - 0.5;
			}
			break;
			
		case kSample_Rectangle:
			for(UInt32 i = 0; i < mNumberOfSamples; i++)
			{
				Float32 t = (i + 0.5) / mNumberOfSamples;
				b[i] = (t > 0.5) ? 0.5 : -0.5;
			}
			break;
	}

	mReady = true;
}

// -----------------------------------------------------------------------------

Float32* SampleData::getSampleData()
{
	return mSampleData.getAlignedBuffer();
}

// -----------------------------------------------------------------------------

UInt32 SampleData::getSampleSize()
{
	return mNumberOfSamples;
}

// -----------------------------------------------------------------------------

float SampleData::getSampleRate()
{
	return mSampleRate;
}

// -----------------------------------------------------------------------------

float SampleData::getSampleKey()
{
	return mSampleKey;
}

// -----------------------------------------------------------------------------

bool SampleData::getSampleReady()
{
	return mReady;
}

// -----------------------------------------------------------------------------

void SampleData::reset()
{
	mNumberOfSamples	= 0;	
	mSampleRate			= 0.0f;
	mSampleKey			= 440.0;
	mReady				= false;
}

// -----------------------------------------------------------------------------

void SampleData::clear()
{
	mSampleData.clear();
}

// -----------------------------------------------------------------------------
/*
void SampleData::cleanup()
{
	mSampleData.deallocate();
}
*/
// -----------------------------------------------------------------------------
