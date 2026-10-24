/*
 *  BlockLimiter.cpp
 *  Automat
 *
 *  Created by Stefan Beck on 09.04.06.
 *  Copyright 2006 alphakanal. All rights reserved.
 *
 */

// -----------------------------------------------------------------------------

#include "BlockLimiter.h"

// -----------------------------------------------------------------------------

BlockLimiter::BlockLimiter()
{
	mMode = kBlockLimiter_On;
}

// -----------------------------------------------------------------------------

void BlockLimiter::initialize(Float32 sampleRate)
{ 
	if (sampleRate < 44100.0)
		sampleRate = 44100.0;
	
	mCutOff = 1.0 - 2.0 * (40.0 / sampleRate);
	
	clear();
}

// -----------------------------------------------------------------------------

void BlockLimiter::setParameter(bool mode)
{
	if (mode != mMode)
		clear();
	
	mMode = mode;
}

// -----------------------------------------------------------------------------

void BlockLimiter::getDCLimiter(UInt32 bufferSize, Float32* buffer)
{
	if (mMode == kBlockLimiter_Off)
		return;
	
	getDCBlocker(bufferSize, buffer);
	getLimiter(bufferSize, buffer);
}

// -----------------------------------------------------------------------------

void BlockLimiter::getDCLimiter(UInt32 bufferSize, Float32* leftBuffer, Float32* rightBuffer)
{
	if (mMode == kBlockLimiter_Off)
		return;

	getDCBlocker(bufferSize, leftBuffer, rightBuffer);
	getLimiter(bufferSize, leftBuffer, rightBuffer);
}

// -----------------------------------------------------------------------------

void BlockLimiter::getDCBlocker(UInt32 bufferSize, Float32* buffer)
{	
	Float32	x1	= mIO[0];
	Float32	x2	= mIO[1];
 
	Float32	dc	= mCutOff;
	
	Float32	in;
 
	while(bufferSize--)
	{
		in			= *buffer++;
		x1			= dc * x1 + in - x2;
		x2			= in;
 
		*buffer++	= x1;
	}
 
	mIO[0]	= x1;
	mIO[1]	= x2;
}
 
 // -----------------------------------------------------------------------------

void BlockLimiter::getLimiter(UInt32 bufferSize, Float32* buffer)
{
	const int size		= bufferSize;
	
	Float32 *vTemp1		= (Float32*)&b1;
	Float32 *vTemp2		= (Float32*)&b2;
	Float32 *vTemp3		= (Float32*)&b3;
	
	Float32 v1			= 27.0;
	Float32 v2			= 9.0;
	
	//	x * (27.0 + x * x) / (27.0 + 9.0 * x * x);
	
	vDSP_vmul(buffer, 1, buffer, 1, vTemp1, 1, size);	//	buffer * buffer -> vTemp1	
	vDSP_vsadd(vTemp1, 1, &v1, vTemp2, 1, size);		//	vTemp1 + 27.0 -> vTemp2
	
	vDSP_vfill(&v2, vTemp3, 1, size);					//	fill 9.0 -> vTemp3
	vDSP_vmul(vTemp3, 1, vTemp1, 1, vTemp3, 1, size);	//	vTemp3 * vTemp1 -> vTemp3
	vDSP_vsadd(vTemp3, 1, &v1, vTemp3, 1, size);		//	vTemp3 + 27.0 -> vTemp3
	
	vvdivf(vTemp1, vTemp2, vTemp3, &size);				//	vTemp2 / vTemp3 -> vTemp1
	
	vDSP_vmul(buffer, 1, vTemp1, 1, buffer, 1, size);	//	buffer * vTemp1 -> buffer
}

// -----------------------------------------------------------------------------

void BlockLimiter::getDCBlocker(UInt32 bufferSize, Float32* leftBuffer, Float32* rightBuffer)
{
	Float32	x1	= mIO[0];
	Float32	x2	= mIO[1];
	Float32	x3	= mIO[2];
	Float32	x4	= mIO[3];
	
	Float32	dc	= mCutOff;

	Float32	in;
	
	while(bufferSize--)
	{
		in				= *leftBuffer;
		x1				= dc * x1 + in - x2;
		x2				= in;
		
		*leftBuffer++	= x1;

		in				= *rightBuffer;
		x3				= dc * x3 + in - x4;
		x4				= in;
		
		*rightBuffer++	= x3;
	}
	
	mIO[0]	= x1;
	mIO[1]	= x2;
	mIO[2]	= x3;
	mIO[3]	= x4;
}

// -----------------------------------------------------------------------------

void BlockLimiter::getLimiter(UInt32 bufferSize, Float32* leftBuffer, Float32* rightBuffer)
{
	const int size		= bufferSize;
	
	Float32 *vTemp1		= (Float32*)&b1;
	Float32 *vTemp2		= (Float32*)&b2;
	Float32 *vTemp3		= (Float32*)&b3;
	
	Float32 v1			= 27.0;
	Float32 v2			= 9.0;
	
	//	x * (27.0 + x * x) / (27.0 + 9.0 * x * x);
	
	vDSP_vmul(leftBuffer, 1, leftBuffer, 1, vTemp1, 1, size);	//	leftBuffer * leftBuffer -> vTemp1	
	vDSP_vsadd(vTemp1, 1, &v1, vTemp2, 1, size);				//	vTemp1 + 27.0 -> vTemp2
	
	vDSP_vfill(&v2, vTemp3, 1, size);							//	fill 9.0 -> vTemp3
	vDSP_vmul(vTemp3, 1, vTemp1, 1, vTemp3, 1, size);			//	vTemp3 * vTemp1 -> vTemp3
	vDSP_vsadd(vTemp3, 1, &v1, vTemp3, 1, size);				//	vTemp3 + 27.0 -> vTemp3
	
	vvdivf(vTemp1, vTemp2, vTemp3, &size);						//	vTemp2 / vTemp3 -> vTemp1

	vDSP_vmul(leftBuffer, 1, vTemp1, 1, leftBuffer, 1, size);	//	leftBuffer * vTemp1 -> leftBuffer

	//	-----

	vDSP_vmul(rightBuffer, 1, rightBuffer, 1, vTemp1, 1, size);	//	rightBuffer * rightBuffer -> vTemp1	
	vDSP_vsadd(vTemp1, 1, &v1, vTemp2, 1, size);				//	vTemp1 + 27.0 -> vTemp2
	
	vDSP_vfill(&v2, vTemp3, 1, size);							//	fill 9.0 -> vTemp3
	vDSP_vmul(vTemp3, 1, vTemp1, 1, vTemp3, 1, size);			//	vTemp3 * vTemp1 -> vTemp3
	vDSP_vsadd(vTemp3, 1, &v1, vTemp3, 1, size);				//	vTemp3 + 27.0 -> vTemp3
	
	vvdivf(vTemp1, vTemp2, vTemp3, &size);						//	vTemp2 / vTemp3 -> vTemp1

	vDSP_vmul(rightBuffer, 1, vTemp1, 1, rightBuffer, 1, size);	//	rightBuffer * vTemp1 -> rightBuffer
}

// -----------------------------------------------------------------------------

void BlockLimiter::clear()
{
	memset(&mIO[0], 0, 4 * sizeof(Float32));
}

// -----------------------------------------------------------------------------

