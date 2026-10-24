/*
 *  wave_data.h
 *  Automat
 *
 *  Created by Stefan Beck on 09.04.06.
 *  Copyright 2006 alphakanal. All rights reserved.
 *
 */

// -----------------------------------------------------------------------------

#ifndef WAVE_DATA_H
#define WAVE_DATA_H

// -----------------------------------------------------------------------------

#define WAVETABLE_SIZE				1032
#define WAVETABLE_POINTS			1024	//	WAVETABLE_SIZE - (2 * 4) start and end bounds
#define WAVETABLE_COUNT				10
#define WAVETABLE_MAX_WAVES			12

typedef struct WaveData
{
    UInt32			key;
	Float32			level;
    signed short*	data;
}
WaveData;

typedef struct WaveSet
{
	char*		name;
	WaveData	wave[WAVETABLE_MAX_WAVES];
}
WaveSet;

extern	WaveSet	wavetable[];

// -----------------------------------------------------------------------------

static inline void printWaveSet(UInt32 set, UInt32 key)
{
	WaveSet*		waveSet		= &wavetable[set];
	UInt32			dataIndex	= key < 47 ? 0 : key > 127 ? 11 : ((key + 1) / 8) - 5;
	signed short*	data		= waveSet->wave[dataIndex].data;
	UInt32			i			= 0;
	
	printf("wave_data::printWaveSet set %ld key %ld name %s\n", set, key, waveSet->name);
	
	for (i = 0; i < WAVETABLE_POINTS; i++)
	{
		short value = data[i];
		
		if (i == 0)
			printf("%04x", value);
		else
			printf(",%04x", value);
	}
	
	printf("\n");
}

// -----------------------------------------------------------------------------

static inline void writeWaveSet(UInt32 set, UInt32 key)
{
	//	open with "Wave Editor -> Import"  16 bits, little endian, signed, 1 channel, 44100Hz
	 
	WaveSet*		waveSet		= &wavetable[set];
	UInt32			dataIndex	= key < 47 ? 0 : key > 127 ? 11 : ((key + 1) / 8) - 5;
	signed short*	data		= waveSet->wave[dataIndex].data;
	//	UInt32			i			= 0;
	
	static char name[1024];
	sprintf (name, "/Users/stiwi/Desktop/waveSet_%s_%ld.raw", waveSet->name, key);
		
	printf("wave_data::writeWaveSet opening file %s\n", name);

	FILE	*file = fopen(name, "wb");
	int		numWrites;

	numWrites = fwrite(data, sizeof(signed short), WAVETABLE_POINTS, file);

	if (numWrites != WAVETABLE_POINTS)
		printf("wave_data::writeWaveSet error writing waveSet\n");
	
	fclose(file);
}

// -----------------------------------------------------------------------------

#endif

// -----------------------------------------------------------------------------
