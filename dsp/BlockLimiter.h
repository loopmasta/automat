/*
 *  BlockLimiter.h
 *  Automat
 *
 *  Created by Stefan Beck on 09.04.06.
 *  Copyright 2006 alphakanal. All rights reserved.
 *
 */

// -----------------------------------------------------------------------------

#ifndef BLOCKERLIMITER_H
#define BLOCKERLIMITER_H

#include "Constants.h"

// -----------------------------------------------------------------------------

class BlockLimiter
{
	enum
	{
		kBlockLimiter_Off,
		kBlockLimiter_On
	};

public:
	
	BlockLimiter();
	
	void	initialize(Float32 sampleRate);

	void	setParameter(bool mode);

	void	getDCLimiter(UInt32 bufferSize, Float32* buffer);
	void	getDCLimiter(UInt32 bufferSize, Float32* leftBuffer, Float32* rightBuffer);
		
	void	getDCBlocker(UInt32 bufferSize, Float32* buffer);
	void	getLimiter(UInt32 bufferSize, Float32* buffer);
	
	void	getDCBlocker(UInt32 bufferSize, Float32* leftBuffer, Float32* rightBuffer);
	void	getLimiter(UInt32 bufferSize, Float32* leftBuffer, Float32* rightBuffer);

	void	clear();
	
private:
	
	bool	mMode;
	Float32	mCutOff;
	
	Float32	mIO[4];
	
	Float32	b1[kMaxRenderSlice] __attribute__ ((aligned (16)));	
	Float32	b2[kMaxRenderSlice] __attribute__ ((aligned (16)));	
	Float32	b3[kMaxRenderSlice] __attribute__ ((aligned (16)));	
};

// -----------------------------------------------------------------------------

#endif

// -----------------------------------------------------------------------------

