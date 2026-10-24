/*
 *  TuningData.cpp
 *  Automat
 *
 *  Created by Stefan Beck on 09.04.06.
 *  Copyright 2006 alphakanal. All rights reserved.
 *
 */

// -----------------------------------------------------------------------------

#include "TuningData.h"

// -----------------------------------------------------------------------------

TuningData::TuningData()
{
	reset();
}

// -----------------------------------------------------------------------------

void TuningData::readTuningData(CFURLRef url)
{
	//	reset();

	char		line[256];	
	eSection	currentSection		= SEC_None;
	bool		tuningFound			= false;
	bool		exactTuningFound	= false;
	
	UInt8		maxChars			= 254;
	UInt8		lastNoteFound		= 0xFF;
	UInt32		lineCount			= 0;
								
	CFStringRef			string		= CFURLCopyFileSystemPath(url, kCFURLPOSIXPathStyle);
	CFStringEncoding	encoding	= CFStringGetSystemEncoding();
	const char*			filePath	= CFStringGetCStringPtr(string, encoding);
	
	if (filePath == NULL)
	{
		printf("TuningData::readTuningData error opening file\n");
		return;
	}
	
	std::ifstream file;
    file.open(filePath, std::ios::in | std::ios::binary);
	
	if (!file)
	{
		printf("TuningData::readTuningData error opening file %s\n", filePath);
		return;
	}
	
	while (file)
	{
		++lineCount;
		UInt8 currentPosition = 0;

		while ((file) && (currentPosition < maxChars))
		{
			char c = '\0';
			file.read(&c, 1);
			
			if ((c == '\0') || (c == '\r') || (c == '\n'))
				break;
			
			line[currentPosition++] = (char)tolower(c);
		}
		
		if (currentPosition >= maxChars)
		{
			printf("TuningData::readTuningData line %ld too long\n", lineCount);
			return;
		}
		
		line[currentPosition] = '\0';
		
		const char* currentValue = line;
		
		while ((*currentValue != '\0') && ((*currentValue == ' ') || (*currentValue == '\t')))
			currentValue++;
		
		if (currentValue[0] == '\0')
			continue;
		
		if (currentValue[0] == ';')
			continue;
		
		char* lastValue = (char*)&currentValue[strlen(currentValue) - 1];
		
		while ((lastValue > currentValue) && ((*lastValue == ' ') || (*lastValue == '\t')))
			--lastValue;
		
		*(lastValue + 1) = '\0';
		
		if (currentValue[0] == '[')
		{
			if (currentValue[strlen(currentValue) - 1] != ']')
			{
				printf("TuningData::readTuningData section-tag must be the only string in line %ld\n", lineCount);
				return;
			}
			
			currentSection = SEC_Unknown;
			
			if (strcmp(&currentValue[1], "tuning]") == 0)
			{
				currentSection	= SEC_Tuning;
				tuningFound		= true;
			}
			else if (strcmp(&currentValue[1], "exact tuning]") == 0)
			{
				currentSection		= SEC_ExactTuning;
				exactTuningFound	= true;
			}

			continue;
		}
		
		if ((currentSection == SEC_None) || (currentSection == SEC_Unknown))
			continue;
		
		const char* param = currentValue;
		const char* value = strchr(currentValue, '=');
		
		if (value == NULL)
		{
			printf("TuningData::readTuningData missing '=' in line %ld\n", lineCount);
			return;
		}
		
		value++;
		
		lastValue = (char*)&value[-2];
		
		while ((lastValue > param) && ((*lastValue == ' ') || (*lastValue == '\t')))
			lastValue--;
		
		*(lastValue + 1) = '\0';
		
		while ((*value != '\0') && ((*value == ' ') || (*value == '\t')))
			value++;
		
		switch (currentSection)
		{
			case SEC_Tuning:
			{
				if (memcmp(param, "note", 4) != 0)
					continue;
				
				int noteIndex = atoi(&param[4]);

				if (!noteInRange(noteIndex))
					continue;
				
				mTunes[noteIndex] = atof(value);
			}
			break;
				
			case SEC_ExactTuning:
			{
				if (memcmp(param, "note", 4) == 0)
				{
					int noteIndex = atoi(&param[4]);
					
					if (!noteInRange(noteIndex))
						continue;
					
					mTunes[noteIndex] = atof(value);
					
					if (lastNoteFound == 0xFF || lastNoteFound < noteIndex)
						lastNoteFound = noteIndex;
					
					continue;
				}
				
				if (strcmp(param, "basefreq") == 0)
				{
					mBaseFreq = atof(value);
					continue;
				}
				
				continue;
			}
			break;
				
			default:
				assert(false);
				printf("TuningData::readTuningData compilation error in line %ld\n", lineCount);
				return;
		}
	}
	
	//	debug();

	if ((!tuningFound) && (!exactTuningFound))
	{
		printf("TuningData::readTuningData no tuning data found\n");
		return;
	}
	
	if (noteInRange(lastNoteFound))
	{
		printf("TuningData::readTuningData last note found %d\n", lastNoteFound);

		for (UInt8 i = lastNoteFound ; i < 128 ; i++)
			mTunes[i] = mTunes[i - lastNoteFound] + mTunes[lastNoteFound];
	}
}

// -----------------------------------------------------------------------------

Float32 TuningData::getBaseFreq()
{
	return mBaseFreq;
}

// -----------------------------------------------------------------------------

Float32 TuningData::getNoteFreq(UInt8 noteIndex)
{
	return mBaseFreq * pow(2, mTunes[noteIndex] / 1200.0);
}

// -----------------------------------------------------------------------------

Float32 TuningData::getRelativeTune(UInt8 noteIndex)
{
	return mTunes[noteIndex];
}

// -----------------------------------------------------------------------------

void TuningData::reset()
{
	mBaseFreq = 8.1757989156437073336; // Means A = 440Hz
	
	for (UInt8 i = 0 ; i < 128 ; i++)
		mTunes[i] = 100 * i;
}

// -----------------------------------------------------------------------------

bool TuningData::noteInRange(int noteIndex)
{
	return (noteIndex >= 0) && (noteIndex <= 127);
}

// -----------------------------------------------------------------------------

void TuningData::debug()
{
	printf("basefreq=%f\n", mBaseFreq);
	
	for (UInt8 i = 0 ; i < 128 ; i++)
		printf("note %i=%f\n", i, mTunes[i]);
}

// -----------------------------------------------------------------------------
