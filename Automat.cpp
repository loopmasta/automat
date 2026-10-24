/*
 *  Automat.cpp
 *  Automat
 *
 *  Created by Stefan Beck on 01.02.06.
 *  Copyright 2006 alphakanal. All rights reserved.
 *
 */

// -----------------------------------------------------------------------------

#include "Automat.h"

// -----------------------------------------------------------------------------

COMPONENT_ENTRY(Automat)

// -----------------------------------------------------------------------------

Automat::Automat(ComponentInstance inComponentInstance):AUMonotimbralInstrumentBase(inComponentInstance, 1, 1)
{	
	//	printf("Automat::Automat\n");

	CreateElements();

	Globals()->UseIndexedParameters(kParameter_Count);
	
	for (UInt32 i = 0; i < kParameter_Count; i++)
		Globals()->SetParameter(i, kParameterInfo[i].defaultValue);

	for (UInt8 i = 0; i < kNumberOfMidiControls; ++i)
		mCCData[i] = -1;

	mCCLearning		= -1;
	mBeatsPerMinute = 120.0;

	mSample			= NULL;
	mTuning			= NULL;
}

// -----------------------------------------------------------------------------

Automat::~Automat()
{
	//	printf("Automat::~Automat\n");
}

// -----------------------------------------------------------------------------

ComponentResult Automat::Initialize()
{	
	//	printf("Automat::Initialize\n");

	ComponentResult	result = AUMonotimbralInstrumentBase::Initialize();
		
	if (result == noErr)
	{
		Float32 sampleRate = GetOutput(0)->GetStreamFormat().mSampleRate;
		
		mTimeLineData.initialize(kNumberOfTimeLines);
		
		for (UInt32 i = 0; i < kNumberOfNotes; i++)
			mVoice[i].initialize(sampleRate, &mSampleData, &mTuningData, &mTimeLineData);
		
		SetNotes(kNumberOfNotes, kMaxActiveNotes, mVoice, sizeof(Voice));
				
		mChorus.initialize(sampleRate, 4.0);
		mDelay.initialize(sampleRate, 4.0);		
		mReverb.initialize(sampleRate);
		
		mModulator.initialize(sampleRate, kModEfxTarget_Count, &mTimeLineData, 10, 13, kModEfxModule_Mod_1);
	}
	
	/*
	printWaveSet(3, 79);
	
	for (UInt32 i = 0; i < WAVETABLE_MAX_WAVES; i++)
		writeWaveSet(3, 39 + i * 8);
	*/
	
	return result;
}

// -----------------------------------------------------------------------------

ComponentResult Automat::SaveState(CFPropertyListRef* outData)
{
	//	printf("Automat::SaveState\n");

    ComponentResult result = AUMonotimbralInstrumentBase::SaveState(outData);
	
    if (result == noErr)
	{
		CFMutableDictionaryRef dict = (CFMutableDictionaryRef) *outData;

		//	printf("Automat::SaveState release\n");

		SInt32		value	= kAutomatVersion;
		CFNumberRef number	= CFNumberCreate (NULL, kCFNumberSInt32Type, &value);
		CFDictionarySetValue(dict, CFSTR("release"), number);
		CFRelease(number);

		//	printf("Automat::SaveState ccdata\n");

		CFMutableDataRef	buffer	= CFDataCreateMutable(0, 0);
		UInt32				size	= CFSwapInt32HostToBig(kNumberOfMidiControls);
		CFDataAppendBytes(buffer, (UInt8*)&size, sizeof(size));

		for (UInt32 i = 0; i < kNumberOfMidiControls; i++)
		{
			UInt32 value = CFSwapInt32HostToBig(mCCData[i]);
			CFDataAppendBytes(buffer, (UInt8*)&value, sizeof(value));
		}
		
		CFDictionarySetValue(dict, CFSTR("ccdata"), buffer);
		CFRelease(buffer);
				
		if (mTimeLineData.mTLData && mTimeLineData.mNumberOfUnits > 0)
		{
			//	printf("Automat::SaveState tldata\n");

			CFMutableDataRef	buffer	= CFDataCreateMutable(0, 0);
			UInt32				size	= CFSwapInt32HostToBig(kNumberOfTimeLines);
			CFDataAppendBytes(buffer, (UInt8*)&size, sizeof(size));
			
			for (UInt32 i = 0; i < kNumberOfTimeLines; i++)
			{
				UInt32	value;
				TLData*	data = &mTimeLineData.mTLData[i];
				
				value = CFSwapInt32HostToBig(*(UInt32*)&data->syncOffsetTime);
				CFDataAppendBytes(buffer, (UInt8*)&value, sizeof(value));

				value = CFSwapInt32HostToBig(*(UInt32*)&data->leftMarkerTime);
				CFDataAppendBytes(buffer, (UInt8*)&value, sizeof(value));

				value = CFSwapInt32HostToBig(*(UInt32*)&data->rightMarkerTime);
				CFDataAppendBytes(buffer, (UInt8*)&value, sizeof(value));
				
				value = CFSwapInt32HostToBig(data->syncOffsetIndex);
				CFDataAppendBytes(buffer, (UInt8*)&value, sizeof(value));		

				value = CFSwapInt32HostToBig(data->leftMarkerIndex);
				CFDataAppendBytes(buffer, (UInt8*)&value, sizeof(value));		

				value = CFSwapInt32HostToBig(data->rightMarkerIndex);
				CFDataAppendBytes(buffer, (UInt8*)&value, sizeof(value));		
				
				value = CFSwapInt32HostToBig(*(UInt32*)&data->markerDelta);
				CFDataAppendBytes(buffer, (UInt8*)&value, sizeof(value));

				value = CFSwapInt32HostToBig(data->numOfActivePoints);
				CFDataAppendBytes(buffer, (UInt8*)&value, sizeof(value));		

				value = CFSwapInt32HostToBig(data->activeIndex);
				CFDataAppendBytes(buffer, (UInt8*)&value, sizeof(value));
				
				size = CFSwapInt32HostToBig(kTlc_NumberOfPoints);
				CFDataAppendBytes(buffer, (UInt8*)&size, sizeof(size));
				
				for (UInt32 j = 0; j < kTlc_NumberOfPoints; j++)
				{
					UInt32 value;
					
					value = CFSwapInt32HostToBig(*(UInt32*)&data->points[j].time);
					CFDataAppendBytes(buffer, (UInt8*)&value, sizeof(value));

					value = CFSwapInt32HostToBig(*(UInt32*)&data->points[j].value);
					CFDataAppendBytes(buffer, (UInt8*)&value, sizeof(value));
				}
				
				size = CFSwapInt32HostToBig(kTlc_NumberOfPoints - 1);
				CFDataAppendBytes(buffer, (UInt8*)&size, sizeof(size));
				
				for (UInt32 j = 0; j < kTlc_NumberOfPoints - 1; j++)
				{
					UInt32 value = CFSwapInt32HostToBig(*(UInt32*)&data->timeDelta[j]);
					CFDataAppendBytes(buffer, (UInt8*)&value, sizeof(value));
				}
			}
			
			CFDictionarySetValue(dict, CFSTR("tldata"), buffer);
			CFRelease(buffer);
		}
		
		if (mSample)
		{
			//	printf("Automat::SaveState sample\n");
			CFDictionarySetValue(dict, CFSTR("sample"), mSample);
		}

		if (mTuning)
		{
			//	printf("Automat::SaveState tuning\n");
			CFDictionarySetValue(dict, CFSTR("tuning"), mTuning);
		}
	}
	
	return result;
}

// -----------------------------------------------------------------------------

ComponentResult Automat::RestoreState(CFPropertyListRef inData)
{
	//	printf("Automat::RestoreState\n");

	reset();
	clear();
	
    ComponentResult result = AUMonotimbralInstrumentBase::RestoreState(inData);
	
    if (result == noErr)
	{		
		CACFDictionary	dict(reinterpret_cast<CFDictionaryRef>(inData), false);
		CFDataRef		data;
		UInt32			version;
		CFStringRef		resource;
		
		dict.GetUInt32(CFSTR("release"), version);

		//	printf("Automat::RestoreState version %ld\n", version);

		if (dict.GetData(CFSTR("ccdata"), data))
		{
			const UInt8	*p = CFDataGetBytePtr(data);
				
			UInt32 numberOfMidiControls = CFSwapInt32BigToHost(*(UInt32*)p);
			p += sizeof(UInt32);
			
			//	printf("Automat::RestoreState numberOfMidiControls %ld\n", numberOfMidiControls);

			if (numberOfMidiControls <= kNumberOfMidiControls)
			{
				for (UInt32 i = 0; i < numberOfMidiControls; i++)
				{
					UInt32	value	= CFSwapInt32BigToHost(*(UInt32*)p);
					mCCData[i]		= *(SInt32*)&value;
					
					p += sizeof(SInt32);
				}
			}
		}

		if (dict.GetData(CFSTR("tldata"), data))
		{
			const UInt8	*p = CFDataGetBytePtr(data);
				
			UInt32 numberOfTimeLines = CFSwapInt32BigToHost(*(UInt32*)p);
			p += sizeof(UInt32);

			//	printf("Automat::RestoreState numberOfTimeLines %ld\n", numberOfTimeLines);
			
			if (mTimeLineData.mTLData && numberOfTimeLines <= mTimeLineData.mNumberOfUnits)
			{
				for (UInt32 i = 0; i < numberOfTimeLines; i++)
				{
					//	printf("Automat::RestoreState timeLine no %ld\n", i);

					UInt32	value;
					TLData*	data			= &mTimeLineData.mTLData[i];
					
					value					= CFSwapInt32BigToHost(*(UInt32*)p);
					data->syncOffsetTime	= *(Float32*)&value;
					p += sizeof(Float32);
					
					value					= CFSwapInt32BigToHost(*(UInt32*)p);
					data->leftMarkerTime	= *(Float32*)&value;
					p += sizeof(Float32);

					value					= CFSwapInt32BigToHost(*(UInt32*)p);
					data->rightMarkerTime	= *(Float32*)&value;
					p += sizeof(Float32);
					
					data->syncOffsetIndex	= CFSwapInt32BigToHost(*(UInt32*)p);
					p += sizeof(UInt32);
					
					data->leftMarkerIndex	= CFSwapInt32BigToHost(*(UInt32*)p);
					p += sizeof(UInt32);
					
					data->rightMarkerIndex	= CFSwapInt32BigToHost(*(UInt32*)p);
					p += sizeof(UInt32);

					value					= CFSwapInt32BigToHost(*(UInt32*)p);
					data->markerDelta		= *(Float32*)&value;
					p += sizeof(Float32);

					data->numOfActivePoints	= CFSwapInt32BigToHost(*(UInt32*)p);
					p += sizeof(UInt32);

					data->activeIndex		= CFSwapInt32BigToHost(*(UInt32*)p);
					p += sizeof(UInt32);
					
					UInt32 numberOfPoints	= CFSwapInt32BigToHost(*(UInt32*)p);
					p += sizeof(UInt32);
					
					//	printf("Automat::RestoreState numberOfPoints %ld\n", numberOfPoints);

					for (UInt32 j = 0; j < numberOfPoints; j++)
					{
						UInt32 value;
						
						value					= CFSwapInt32BigToHost(*(UInt32*)p);
						data->points[j].time	= *(Float32*)&value;
						p += sizeof(Float32);

						value					= CFSwapInt32BigToHost(*(UInt32*)p);
						data->points[j].value	= *(Float32*)&value;
						p += sizeof(Float32);
					}
					
					UInt32 numberOfTimeDeltas = CFSwapInt32BigToHost(*(UInt32*)p);
					p += sizeof(UInt32);
					
					//	printf("Automat::RestoreState numberOfTimeDeltas %ld\n", numberOfTimeDeltas);

					for (UInt32 j = 0; j < numberOfTimeDeltas; j++)
					{
						UInt32 value		= CFSwapInt32BigToHost(*(UInt32*)p);
						data->timeDelta[j]	= *(Float32*)&value;
						p += sizeof(Float32);
					}
					
					//	printf("Automat::RestoreState kProperty_TimeLine_Set %ld, i %ld\n", kProperty_TimeLine_Set, i);
					
					PropertyChanged(kProperty_TimeLine_Set + i, kAudioUnitScope_Global, (AudioUnitElement)0);
				}						
			}
		}			
		
		if (dict.GetString(CFSTR("sample"), resource))
		{
			readSampleData(resource);
			PropertyChanged(kProperty_File_Sample, kAudioUnitScope_Global, (AudioUnitElement)0);
		}
		
		if (dict.GetString(CFSTR("tuning"), resource))
		{
			readTuningData(resource);
			PropertyChanged(kProperty_File_Tuning, kAudioUnitScope_Global, (AudioUnitElement)0);
		}
	}
	
    return result;
}

// -----------------------------------------------------------------------------

ComponentResult Automat::GetParameterInfo(AudioUnitScope inScope, AudioUnitParameterID inParameterID, AudioUnitParameterInfo& outParameterInfo)
{
	//	printf("Automat::GetParameterInfo %i\n", (int)inParameterID);

    if (inScope == kAudioUnitScope_Global && inParameterID < kParameter_Count)
    {
		memcpy(&outParameterInfo, &kParameterInfo[inParameterID], sizeof(AudioUnitParameterInfo));
        return noErr;
    }
	
    return AUMonotimbralInstrumentBase::GetParameterInfo(inScope, inParameterID, outParameterInfo);
}

// -----------------------------------------------------------------------------

ComponentResult Automat::GetParameterValueStrings(AudioUnitScope inScope, AudioUnitParameterID inParameterID, CFArrayRef* outStrings)
{
	//	printf("Automat::GetParameterValueStrings %i\n", (int)inParameterID);
	
	ComponentResult result	= noErr;
	bool			handled	= false;
	
    if (inScope == kAudioUnitScope_Global)
    {	
		handled = true;
		
		switch (inParameterID)
		{
			case kOsc1_WaveParameter:
			case kOsc2_WaveParameter:
			case kOsc3_WaveParameter:
			{
				if (outStrings)
					*outStrings = CFArrayCreate(NULL, kOscWave, kOscNumberOfWaves, NULL);

				break;
			}
						
			case kOsc1_SyncParameter:
			case kOsc2_SyncParameter:
			case kOsc3_SyncParameter:
			{
				if (outStrings)
					*outStrings = CFArrayCreate(NULL, kOscSync, kOscNumberOfSyncs, NULL);

				break;
			}

			case kOsc1_StackParameter:
			case kOsc2_StackParameter:
			case kOsc3_StackParameter:
			{
				if (outStrings)
					*outStrings = CFArrayCreate(NULL, kOscStack, kOscNumberOfStacks, NULL);

				break;
			}

			case kShp1_FilterTypeParameter:
			case kShp2_FilterTypeParameter:
			case kShp3_FilterTypeParameter:
			{
				if (outStrings)
					*outStrings = CFArrayCreate(NULL, kShpFilter, kShpNumberOfFilters, NULL);

				break;
			}

			case kShp1_ShapeTypeParameter:
			case kShp2_ShapeTypeParameter:
			case kShp3_ShapeTypeParameter:
			{
				if (outStrings)
					*outStrings = CFArrayCreate(NULL, kShpShape, kShpNumberOfShapes, NULL);

				break;
			}
			
			case kFlt_FilterTypeParameter:
			{
				if (outStrings)
					*outStrings = CFArrayCreate(NULL, kFltType, kFltNumberOfTypes, NULL);

				break;
			}
			
			case kFlt_DriveTypeParameter:
			{
				if (outStrings)
					*outStrings = CFArrayCreate(NULL, kFltDriveType, kFltNumberOfDrives, NULL);

				break;
			}

			case kSfx_TypeParameter:
			{
				if (outStrings)
					*outStrings = CFArrayCreate(NULL, kSfxType, kSfxNumberOfTypes, NULL);

				break;
			}

			case kSfx_ModeParameter:
			{
				if (outStrings)
					*outStrings = CFArrayCreate(NULL, kSfxMode, kSfxNumberOfModes, NULL);

				break;
			}

			case kDly_LeftTimeParameter:
			case kDly_RightTimeParameter:
			{
				if (outStrings)
					*outStrings = CFArrayCreate(NULL, kDlyTime, kDlyNumberOfTimes, NULL);

				break;
			}

			case kDly_FilterTypeParameter:
			{
				if (outStrings)
					*outStrings = CFArrayCreate(NULL, kDlyFilter, kDlyNumberOfFilters, NULL);
				
				break;
			}
			
			case kDly_FilterTargetParameter:
			{
				if (outStrings)
					*outStrings = CFArrayCreate(NULL, kDlyTarget, kDlyNumberOfTargets, NULL);

				break;
			}

			case kRev_ModeParameter:
			{
				if (outStrings)
					*outStrings = CFArrayCreate(NULL, kRevMode, kRevNumberOfModes, NULL);

				break;
			}

			case kOut_ModeParameter:
			{
				if (outStrings)
					*outStrings = CFArrayCreate(NULL, kOutMode, kOutNumberOfModes, NULL);

				break;
			}

			case kOut_OctaveParameter:
			{
				if (outStrings)
					*outStrings = CFArrayCreate(NULL, kOutOctave, kOutNumberOfOctaves, NULL);

				break;
			}

			case kMod01_TargetParameter:
			case kMod02_TargetParameter:
			case kMod03_TargetParameter:
			case kMod04_TargetParameter:
			case kMod05_TargetParameter:
			case kMod06_TargetParameter:
			case kMod07_TargetParameter:
			case kMod08_TargetParameter:
			case kMod09_TargetParameter:
			case kMod10_TargetParameter:
			{
				if (outStrings)
					*outStrings = CFArrayCreate(NULL, kModTarget, kModNumberOfTargets, NULL);

				break;
			}
			
			case kMod11_TargetParameter:
			case kMod12_TargetParameter:
			case kMod13_TargetParameter:
			case kMod14_TargetParameter:
			{
				if (outStrings)
					*outStrings = CFArrayCreate(NULL, kModEfxTarget, kModNumberOfEfxTargets, NULL);

				break;
			}

			case kMod01_SyncParameter:
			case kMod02_SyncParameter:
			case kMod03_SyncParameter:
			case kMod04_SyncParameter:
			case kMod05_SyncParameter:
			case kMod06_SyncParameter:
			case kMod07_SyncParameter:
			case kMod08_SyncParameter:
			case kMod09_SyncParameter:
			case kMod10_SyncParameter:
			{
				if (outStrings)
					*outStrings = CFArrayCreate(NULL, kModSync, kModNumberOfSyncs, NULL);

				break;
			}
			
			case kMod11_SyncParameter:
			case kMod12_SyncParameter:
			case kMod13_SyncParameter:
			case kMod14_SyncParameter:
			{
				if (outStrings)
					*outStrings = CFArrayCreate(NULL, kModEfxSync, kModNumberOfEfxSyncs, NULL);

				break;
			}

			case kMod01_TimeParameter:
			case kMod02_TimeParameter:
			case kMod03_TimeParameter:
			case kMod04_TimeParameter:
			case kMod05_TimeParameter:
			case kMod06_TimeParameter:
			case kMod07_TimeParameter:
			case kMod08_TimeParameter:
			case kMod09_TimeParameter:
			case kMod10_TimeParameter:
			{
				if (outStrings)
					*outStrings = CFArrayCreate(NULL, kModTime, kModNumberOfTimes, NULL);

				break;
			}
			
			case kMod11_TimeParameter:
			case kMod12_TimeParameter:
			case kMod13_TimeParameter:
			case kMod14_TimeParameter:
			{
				if (outStrings)
					*outStrings = CFArrayCreate(NULL, kModTime, kModNumberOfTimes, NULL);

				break;
			}

			default:
				handled = false;
		}
	}
	
	if (!handled)
		result = AUMonotimbralInstrumentBase::GetParameterValueStrings(inScope, inParameterID, outStrings);

	return result;
}

// -----------------------------------------------------------------------------

/*
ComponentResult Automat::SetParameter(AudioUnitParameterID inID, AudioUnitScope inScope, AudioUnitElement inElement, AudioUnitParameterValue inValue, UInt32 inBufferOffsetInFrames)
{
	//	printf("Automat::SetParameter id%i, value %f\n", (int)inID, (Float32)inValue);

	return AUMonotimbralInstrumentBase::SetParameter(inID, inScope, inElement, inValue, inBufferOffsetInFrames);
}

// -----------------------------------------------------------------------------
*/

ComponentResult Automat::GetPropertyInfo(AudioUnitPropertyID inID, AudioUnitScope inScope, AudioUnitElement inElement, UInt32& outDataSize, Boolean& outWritable)
{
	//	printf("Automat::GetPropertyInfo %i\n", (int)inID);

	ComponentResult result = noErr;
	
	switch(inID) 
	{
        case kProperty_MidiLearn:
		{
            outDataSize = sizeof(SInt32);
            outWritable = false;
			
			break;
		}

		case kProperty_Randomize:
		{
            outDataSize = sizeof(UInt32);
            outWritable = false;
			
			break;
		}
		
		case kProperty_File_Sample:
		{
            outDataSize = sizeof(CFStringRef);
            outWritable = true;
			
			break;
		}

		case kProperty_File_Tuning:
		{
            outDataSize = sizeof(CFStringRef);
            outWritable = true;
			
			break;
		}

		case kProperty_TimeLine_0:
		case kProperty_TimeLine_1:
		case kProperty_TimeLine_2:
		case kProperty_TimeLine_3:
		case kProperty_TimeLine_4:
		case kProperty_TimeLine_5:
		case kProperty_TimeLine_6:
		case kProperty_TimeLine_7:
		case kProperty_TimeLine_8:
		case kProperty_TimeLine_9:
		case kProperty_TimeLine_10:
		case kProperty_TimeLine_11:
		case kProperty_TimeLine_12:
		case kProperty_TimeLine_13:
		{
            outDataSize = sizeof(TLData);
            outWritable = true;
			
			break;
		}
		
		case kProperty_TlPreset_0:
		case kProperty_TlPreset_1:
		case kProperty_TlPreset_2:
		case kProperty_TlPreset_3:
		case kProperty_TlPreset_4:
		case kProperty_TlPreset_5:
		case kProperty_TlPreset_6:
		case kProperty_TlPreset_7:
		case kProperty_TlPreset_8:
		case kProperty_TlPreset_9:
		case kProperty_TlPreset_10:
		case kProperty_TlPreset_11:
		case kProperty_TlPreset_12:
		case kProperty_TlPreset_13:
		{
            outDataSize = sizeof(UInt32);
            outWritable = false;
			
			break;
		}
		
		case kProperty_TlEdit_0:
		case kProperty_TlEdit_1:
		case kProperty_TlEdit_2:
		case kProperty_TlEdit_3:
		case kProperty_TlEdit_4:
		case kProperty_TlEdit_5:
		case kProperty_TlEdit_6:
		case kProperty_TlEdit_7:
		case kProperty_TlEdit_8:
		case kProperty_TlEdit_9:
		case kProperty_TlEdit_10:
		case kProperty_TlEdit_11:
		case kProperty_TlEdit_12:
		case kProperty_TlEdit_13:
		{
            outDataSize = sizeof(UInt32);
            outWritable = false;
			
			break;
		}

		default:
			result = AUMonotimbralInstrumentBase::GetPropertyInfo(inID, inScope, inElement, outDataSize, outWritable);
	}
	
	return result;
}

// -----------------------------------------------------------------------------

ComponentResult Automat::GetProperty(AudioUnitPropertyID inID, AudioUnitScope inScope, AudioUnitElement inElement, void* outData)
{
	//	printf("Automat::GetProperty %i\n", (int)inID);

	ComponentResult result = noErr;

	switch(inID) 
	{
		case kProperty_File_Sample:
			*(CFStringRef*)outData = mSample;
			break;
			
		case kProperty_File_Tuning:
			*(CFStringRef*)outData = mTuning;
			break;

		case kProperty_TimeLine_0:
		case kProperty_TimeLine_1:
		case kProperty_TimeLine_2:
		case kProperty_TimeLine_3:
		case kProperty_TimeLine_4:
		case kProperty_TimeLine_5:
		case kProperty_TimeLine_6:
		case kProperty_TimeLine_7:
		case kProperty_TimeLine_8:
		case kProperty_TimeLine_9:
		case kProperty_TimeLine_10:
		case kProperty_TimeLine_11:
		case kProperty_TimeLine_12:
		case kProperty_TimeLine_13:
			//	mTimeLineData.copyData(inID - kProperty_TimeLine_0, (TLData*)outData);
			*(TLData*)outData = mTimeLineData.mTLData[inID - kProperty_TimeLine_0];
			break;
			
		default:
			result = AUMonotimbralInstrumentBase::GetProperty(inID, inScope, inElement, outData);
	}
	
	return result;
}

// -----------------------------------------------------------------------------

ComponentResult Automat::SetProperty(AudioUnitPropertyID inID, AudioUnitScope inScope, AudioUnitElement inElement, const void* inData, UInt32 inDataSize)
{
	//	printf("Automat::SetProperty %i\n", (int)inID);

	ComponentResult result = noErr;

	switch(inID) 
	{
        case kProperty_MidiLearn:
		{
            if (mCCLearning == *(SInt32*)inData)
            {
                for (UInt32 i = 0; i < kNumberOfMidiControls; i++)
                {
                    if (mCCData[i] == mCCLearning)
                        mCCData[i] = -1;
                }

                mCCLearning = -1;
            }
            else
                mCCLearning = *(SInt32*)inData;
			
			break;
		}

		case kProperty_Randomize:
			randomizeParameter(*(UInt32*)inData);
			break;

		case kProperty_File_Sample:
			readSampleData(*(CFStringRef*)inData);
			break;
			
		case kProperty_File_Tuning:
			readTuningData(*(CFStringRef*)inData);
			break;

		case kProperty_TimeLine_0:
		case kProperty_TimeLine_1:
		case kProperty_TimeLine_2:
		case kProperty_TimeLine_3:
		case kProperty_TimeLine_4:
		case kProperty_TimeLine_5:
		case kProperty_TimeLine_6:
		case kProperty_TimeLine_7:
		case kProperty_TimeLine_8:
		case kProperty_TimeLine_9:
		case kProperty_TimeLine_10:
		case kProperty_TimeLine_11:
		case kProperty_TimeLine_12:
		case kProperty_TimeLine_13:
			//	mTimeLineData.pasteData(inID - kProperty_TimeLine_0, (TLData*)inData);
			mTimeLineData.mTLData[inID - kProperty_TimeLine_0] = *(TLData*)inData;			
			break;

		case kProperty_TlPreset_0:
		case kProperty_TlPreset_1:
		case kProperty_TlPreset_2:
		case kProperty_TlPreset_3:
		case kProperty_TlPreset_4:
		case kProperty_TlPreset_5:
		case kProperty_TlPreset_6:
		case kProperty_TlPreset_7:
		case kProperty_TlPreset_8:
		case kProperty_TlPreset_9:
		case kProperty_TlPreset_10:
		case kProperty_TlPreset_11:
		case kProperty_TlPreset_12:
		case kProperty_TlPreset_13:
			mTimeLineData.setPreset(inID - kProperty_TlPreset_0, *(UInt32*)inData);
			PropertyChanged(kProperty_TimeLine_Set + (inID - kProperty_TlPreset_0), kAudioUnitScope_Global, (AudioUnitElement)0);
			break;

		case kProperty_TlEdit_0:
		case kProperty_TlEdit_1:
		case kProperty_TlEdit_2:
		case kProperty_TlEdit_3:
		case kProperty_TlEdit_4:
		case kProperty_TlEdit_5:
		case kProperty_TlEdit_6:
		case kProperty_TlEdit_7:
		case kProperty_TlEdit_8:
		case kProperty_TlEdit_9:
		case kProperty_TlEdit_10:
		case kProperty_TlEdit_11:
		case kProperty_TlEdit_12:
		case kProperty_TlEdit_13:
			mTimeLineData.editData(inID - kProperty_TlEdit_0, *(UInt32*)inData);
			PropertyChanged(kProperty_TimeLine_Set + (inID - kProperty_TlEdit_0), kAudioUnitScope_Global, (AudioUnitElement)0);
			break;
		
		default:
			result = AUMonotimbralInstrumentBase::SetProperty(inID, inScope, inElement, inData, inDataSize);
	}
	
	return result;
}

// -----------------------------------------------------------------------------

ComponentResult Automat::RealTimeStartNote(SynthGroupElement* inGroup, NoteInstanceID inNoteInstanceID, UInt32 inOffsetSampleFrame, const MusicDeviceNoteParams &inParams)
{
	ComponentResult result	= noErr;
	UInt32			mode	= (UInt32)Globals()->GetParameter(kOut_ModeParameter);
	
	switch (mode)
	{
		case kMode_Poly:
			result = AUMonotimbralInstrumentBase::RealTimeStartNote(inGroup, inNoteInstanceID, inOffsetSampleFrame, inParams);
			break;

		case kMode_Mono:
		{
			if (NumActiveNotes() == 1) 
				VoiceStealing(inOffsetSampleFrame, false);
			
			SynthNote *note = GetAFreeNote(inOffsetSampleFrame);
			
			if (!note)
				return -1;
			
			IncNumActiveNotes();
			note->AttackNote(NULL, inGroup, inNoteInstanceID, mAbsoluteSampleFrame + inOffsetSampleFrame, inOffsetSampleFrame, inParams);
					
			inGroup->AddNote(note);
		}
		break;
		
		case kMode_Legato:
		{
			SynthNote *note;
			
			if (NumActiveNotes() == 1)
			{
				note = inGroup->GetNote();

				if (!note)
					return -1;
					
				note->LegatoNote(inNoteInstanceID, inParams);
			}
			else
			{
				note = GetAFreeNote(inOffsetSampleFrame);

				if (!note)
					return -1;
				
				IncNumActiveNotes();
				note->AttackNote(NULL, inGroup, inNoteInstanceID, mAbsoluteSampleFrame + inOffsetSampleFrame, inOffsetSampleFrame, inParams);
						
				inGroup->AddNote(note);
			}
		}
		break;
	}

	return noErr;
}

// -----------------------------------------------------------------------------

OSStatus Automat::HandleAllNotesOff(UInt8 inChannel)
{
	OSStatus result = AUMonotimbralInstrumentBase::HandleAllNotesOff(inChannel);
	
	clear();
	
	return result;
}

// -----------------------------------------------------------------------------

OSStatus Automat::HandleAllSoundOff(UInt8 inChannel)
{
	OSStatus result = AUMonotimbralInstrumentBase::HandleAllSoundOff(inChannel);
	
	clear();
	
	return result;
}

// -----------------------------------------------------------------------------

OSStatus Automat::HandleControlChange(UInt8 inChannel, UInt8 inController, UInt8 inValue, UInt32 inStartFrame)
{
	//	printf("Automat::HandleControlChange inValue %i\n", inValue);

	OSStatus result = AUMonotimbralInstrumentBase::HandleControlChange(inChannel, inController, inValue, inStartFrame);
	
    if (mCCLearning != -1)
    {
        mCCData[inController]	= mCCLearning;
        mCCLearning				= -1;
    }
	
    int id = mCCData[inController];
	
    if (id > -1)
    {
		float	val = inValue / 127.0f;
        float	min = kParameterInfo[id].minValue;
        float	max = kParameterInfo[id].maxValue;
        float 	dif = max - min;
		
        val = min + (val * dif);
		
        struct AudioUnitParameter param = {
            GetComponentInstance(),
            id,
            kAudioUnitScope_Global,
            0
        };
		
		AUParameterSet(0, 0, &param, val, 0);
    }
	
	return result;
}

// -----------------------------------------------------------------------------

void Automat::GetUIComponentDescs(ComponentDescription* inDescArray)
{
    inDescArray->componentType			= kAudioUnitCarbonViewComponentType;
    inDescArray->componentSubType		= kAutomatSubtype;
    inDescArray->componentManufacturer	= kAutomatManufacture;
    inDescArray->componentFlags			= 0;
    inDescArray->componentFlagsMask		= 0;
}

// -----------------------------------------------------------------------------

CFURLRef Automat::CopyIconLocation()
{
	CFBundleRef	bundle	= CFBundleGetBundleWithIdentifier(CFSTR(kAutomatBundleID));
	CFURLRef	icon	= CFBundleCopyResourceURL(bundle, CFSTR("automat"), CFSTR("icns"), NULL);

	return icon;
}

// -----------------------------------------------------------------------------

UInt32 Automat::SupportedNumChannels(const AUChannelInfo** outInfo)
{
    static AUChannelInfo info[] = {{kMaxChannels, kMaxChannels}};
	
    if (outInfo)
        *outInfo = info;
	
    return sizeof(info) / sizeof(*info);
}

// -----------------------------------------------------------------------------

bool Automat::ValidFormat(AudioUnitScope inScope, AudioUnitElement inElement, const CAStreamBasicDescription& inNewFormat)
{
	return AUBase::FormatIsCanonical(inNewFormat) && (inNewFormat.NumberChannels() == kMaxChannels);
}

// -----------------------------------------------------------------------------

ComponentResult Automat::Render(AudioUnitRenderActionFlags &ioActionFlags, const AudioTimeStamp &inTimeStamp, UInt32 inNumberFrames)
{
	OSStatus	result		= noErr;
	Float64		bpm			= 120.0;
	OSStatus	err			= AUMonotimbralInstrumentBase::CallHostBeatAndTempo(NULL, &bpm);
		
	if (err == noErr)
		mBeatsPerMinute = bpm;

	mDelay.setBPM(mBeatsPerMinute);
	mTimeLineData.setBPM(mBeatsPerMinute);

	result = AUMonotimbralInstrumentBase::Render(ioActionFlags, inTimeStamp, inNumberFrames);

	if (result == noErr)
	{
		AudioBufferList	&bufferList = GetOutput(0)->GetBufferList();
		UInt32			numChannels = bufferList.mNumberBuffers;
		
		if (numChannels != kMaxChannels)
			return kAudioUnitErr_FormatNotSupported;

		Float32*	leftOut		= (Float32*)bufferList.mBuffers[0].mData;
		Float32*	rightOut	= (Float32*)bufferList.mBuffers[1].mData;
		
		UInt32		chunkSize;
		Float32*	modulation;
		
		updateParameter();

		while(inNumberFrames > 0)
		{
			chunkSize		 = (inNumberFrames >= kMaxRenderSlice) ? kMaxRenderSlice : inNumberFrames;
			inNumberFrames	-= chunkSize;
			
			modulation		 = mModulator.getModulation(chunkSize);

			mChorus.getEffect(chunkSize, leftOut, rightOut, &modulation[kModEfxModule_Sfx]);
			mDelay.getEffect(chunkSize, leftOut, rightOut, &modulation[kModEfxModule_Dly]);

			mReverb.getEffect(chunkSize, leftOut, rightOut, &modulation[kModEfxModule_Rev]);
			 
			mAmplifier.getAmpWithSpread(chunkSize, leftOut, rightOut, 1.0, NULL);
			
			leftOut		+= chunkSize;
			rightOut	+= chunkSize;
		}
	}
	
	return result;
}

// -----------------------------------------------------------------------------

void Automat::reset()
{
	for (UInt32 i = 0; i < kNumberOfNotes; i++)
		mVoice[i].reset();
	
	mChorus.reset();
	mDelay.reset();
	mReverb.reset();
	mAmplifier.reset();
	
	mModulator.reset();
}

// -----------------------------------------------------------------------------

void Automat::clear()
{
	for (UInt32 i = 0; i < kNumberOfNotes; i++)
		mVoice[i].clear();

	mChorus.clear();
	mDelay.clear();
	mReverb.clear();
}

// -----------------------------------------------------------------------------

void Automat::readSampleData(CFStringRef name)
{
	if (name == NULL)
		return;

	OSStatus	error;
	FSRef		sampleDirectory;
	CFURLRef	bundleURL;
	CFURLRef	sample;

	error		= FSFindFolder(kUserDomain, kAudioSupportFolderType, kDontCreateFolder, &sampleDirectory);
	bundleURL	= CFURLCreateFromFSRef(kCFAllocatorDefault, &sampleDirectory);
	bundleURL	= CFURLCreateCopyAppendingPathComponent(kCFAllocatorDefault, bundleURL, CFSTR(kAutomatPresets), true);
	sample		= CFBundleCopyResourceURLInDirectory(bundleURL, name, NULL, CFSTR("samples"));
		
	CFShow(bundleURL);

	if (bundleURL)
		CFRelease(bundleURL);

	if (sample)
	{
		if (mSample)
			CFRelease(mSample);
		
		mSample = name;
		CFRetain(mSample);
	
		mSampleData.readSampleData(sample);
		CFRelease(sample);
	}
}

// -----------------------------------------------------------------------------

void Automat::readTuningData(CFStringRef name)
{
	if (name == NULL)
		return;
		
	OSStatus	error;
	FSRef		tuningDirectory;
	CFURLRef	bundleURL;
	CFURLRef	tuning;

	error		= FSFindFolder(kUserDomain, kAudioSupportFolderType, kDontCreateFolder, &tuningDirectory);
	bundleURL	= CFURLCreateFromFSRef(kCFAllocatorDefault, &tuningDirectory);
	bundleURL	= CFURLCreateCopyAppendingPathComponent(kCFAllocatorDefault, bundleURL, CFSTR(kAutomatPresets), true);
	tuning		= CFBundleCopyResourceURLInDirectory(bundleURL, name, NULL, CFSTR("tunings"));
		
	if (bundleURL)
		CFRelease(bundleURL);
	
	if (tuning)
	{
		if (mTuning)
			CFRelease(mTuning);
		
		mTuning = name;
		CFRetain(mTuning);
	
		mTuningData.readTuningData(tuning);
		CFRelease(tuning);
	}
}

// -----------------------------------------------------------------------------

void Automat::updateParameter()
{	
	AUElement* global = Globals();

	UInt32		sfx_type			= getIntParameter(global, kSfx_TypeParameter);
	UInt32		sfx_mode			= getIntParameter(global, kSfx_ModeParameter);
	Float32		sfx_speed1			= getFloatParameter(global, kSfx_Speed1Parameter);
	Float32		sfx_speed2			= getFloatParameter(global, kSfx_Speed2Parameter);
	Float32		sfx_feedBack		= getFloatParameter(global, kSfx_FeedBackParameter);
	Float32		sfx_depth			= getFloatParameter(global, kSfx_DepthParameter);
	Float32		sfx_mix				= getFloatParameter(global, kSfx_MixParameter);
	
	UInt32		dly_leftTime		= getIntParameter(global, kDly_LeftTimeParameter);
	UInt32		dly_rightTime		= getIntParameter(global, kDly_RightTimeParameter);
	UInt32		dly_filterType		= getIntParameter(global, kDly_FilterTypeParameter);
	UInt32		dly_filterTarget	= getIntParameter(global, kDly_FilterTargetParameter);
	Float32		dly_cutOff			= getFloatParameter(global, kDly_CutOffParameter);
	Float32		dly_resonance		= getFloatParameter(global, kDly_ResonanceParameter);
	Float32		dly_feedBack		= getFloatParameter(global, kDly_FeedBackParameter);
	Float32		dly_crossFeed		= getFloatParameter(global, kDly_CrossFeedParameter);
	Float32		dly_mix				= getFloatParameter(global, kDly_MixParameter);

	Float32		amp_level			= getFloatParameter(global, kOut_MasterParameter);
	Float32		amp_spread			= getFloatParameter(global, kOut_SpreadParameter);

	UInt32		mod11_target		= getIntParameter(global, kMod11_TargetParameter);
	UInt32		mod11_sync			= getIntParameter(global, kMod11_SyncParameter);
	UInt32		mod11_time			= getIntParameter(global, kMod11_TimeParameter);
	Float32		mod11_speed			= getFloatParameter(global, kMod11_SpeedParameter);
	Float32		mod11_level			= getFloatParameter(global, kMod11_LevelParameter);

	UInt32		mod12_target		= getIntParameter(global, kMod12_TargetParameter);
	UInt32		mod12_sync			= getIntParameter(global, kMod12_SyncParameter);
	UInt32		mod12_time			= getIntParameter(global, kMod12_TimeParameter);
	Float32		mod12_speed			= getFloatParameter(global, kMod12_SpeedParameter);
	Float32		mod12_level			= getFloatParameter(global, kMod12_LevelParameter);

	UInt32		mod13_target		= getIntParameter(global, kMod13_TargetParameter);
	UInt32		mod13_sync			= getIntParameter(global, kMod13_SyncParameter);
	UInt32		mod13_time			= getIntParameter(global, kMod13_TimeParameter);
	Float32		mod13_speed			= getFloatParameter(global, kMod13_SpeedParameter);
	Float32		mod13_level			= getFloatParameter(global, kMod13_LevelParameter);

	UInt32		mod14_target		= getIntParameter(global, kMod14_TargetParameter);
	UInt32		mod14_sync			= getIntParameter(global, kMod14_SyncParameter);
	UInt32		mod14_time			= getIntParameter(global, kMod14_TimeParameter);
	Float32		mod14_speed			= getFloatParameter(global, kMod14_SpeedParameter);
	Float32		mod14_level			= getFloatParameter(global, kMod14_LevelParameter);

	UInt32		rev_mode			= getIntParameter(global, kRev_ModeParameter);
	Float32		rev_size			= getFloatParameter(global, kRev_SizeParameter);
	Float32		rev_hiCut			= getFloatParameter(global, kRev_HiCutParameter);
	Float32		rev_loCut			= getFloatParameter(global, kRev_LoCutParameter);
	Float32		rev_delax			= getFloatParameter(global, kRev_DelayParameter);
	Float32		rev_mix				= getFloatParameter(global, kRev_MixParameter);

	mChorus.setParameter(sfx_type, sfx_mode, sfx_speed1, sfx_speed2, sfx_feedBack, sfx_depth, sfx_mix);
	mDelay.setParameter(dly_leftTime, dly_rightTime, kDlyFilterSet[dly_filterType], dly_filterTarget, dly_cutOff, dly_resonance, dly_feedBack, dly_crossFeed, dly_mix);

	mReverb.setParameter(rev_mode, rev_size, rev_hiCut, rev_loCut, rev_delax, rev_mix);

	mAmplifier.setParameter(amp_level, amp_spread);

	mModulator.setParameter(0, kModEfxTargetSet[mod11_target], mod11_sync, mod11_time, mod11_speed, 0.5, 0.5, mod11_level);
	mModulator.setParameter(1, kModEfxTargetSet[mod12_target], mod12_sync, mod12_time, mod12_speed, 0.5, 0.5, mod12_level);
	mModulator.setParameter(2, kModEfxTargetSet[mod13_target], mod13_sync, mod13_time, mod13_speed, 0.5, 0.5, mod13_level);
	mModulator.setParameter(3, kModEfxTargetSet[mod14_target], mod14_sync, mod14_time, mod14_speed, 0.5, 0.5, mod14_level);
}

// -----------------------------------------------------------------------------

void Automat::randomizeParameter(UInt32 mode)
{
	//	printf("Automat::randomizeParameter %ld\n", mode);

	reset();
	clear();
	
    for (UInt32 i = 0; i < kParameter_Count; i++)
    {
		switch(i)
		{
			case kOut_MasterParameter:
			case kOut_SpreadParameter:
			case kOut_GlideParameter:
			case kOut_ModeParameter:
			case kOut_OctaveParameter:
			case kOut_BendParameter:
				continue;
		}
		
		Float32	val	= 0.0;
		Float32	min	= kParameterInfo[i].minValue;
		Float32	max	= kParameterInfo[i].maxValue;
		Float32	dif	= max - min;
		
		switch (mode)
		{
			case kRandomize_Random:
			{
				val = min + drand48() * dif;
				
				if (kParameterInfo[i].unit == kIndexed)
					val = roundf(val);
			
				break;
			}
				
			case kRandomize_Evolution:
			{
				if (kParameterInfo[i].unit == kIndexed)
					continue;
				
				val = Globals()->GetParameter(i) * (1.2 - drand48() * 0.4);
				break;
			}
		}
		
		switch (i)
		{
			case kDly_ResonanceParameter:
			case kDly_FeedBackParameter:
				val = clamp_f(val, min, max * 0.5);
				break;
		}
		
		Globals()->SetParameter(i, clamp_f(val, min, max));
    }

	AudioUnitParameter param;
	
	param.mAudioUnit	= GetComponentInstance();
	param.mScope 		= kAudioUnitScope_Global;
	param.mParameterID	= kAUParameterListener_AnyParameter;
	
	AUParameterListenerNotify(NULL, NULL, &param);
}

// -----------------------------------------------------------------------------
