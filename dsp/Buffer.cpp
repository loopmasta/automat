/*
 *  Buffer.cpp
 *  Automat
 *
 *  Created by Stefan Beck on 26.01.08.
 *  Copyright 2008 alphakanal. All rights reserved.
 *
 */

// -----------------------------------------------------------------------------

#include "Buffer.h"

// -----------------------------------------------------------------------------

Buffer::Buffer()
{
	mBufferSize			= 0;
	mBuffer				= NULL;
	mAlignedBuffer		= NULL;
}

// -----------------------------------------------------------------------------

Buffer::~Buffer()
{
	deallocate();
}

// -----------------------------------------------------------------------------

void Buffer::allocate(UInt32 size)
{
	if (mBuffer != NULL && mBufferSize == size)
		return;

	mBufferSize		= size;
	mBuffer			= realloc(mBuffer, mBufferSize * sizeof(Float32));
	
	UInt32 misalign	= (uintptr_t)mBuffer & kAlignMask;
	
	if (misalign)
	{
		mBuffer			= realloc(mBuffer, mBufferSize * sizeof(Float32) + kAlignMask);
		mAlignedBuffer	= (Float32*)((char*)mBuffer + kAlignInterval - misalign);
	}
	else
		mAlignedBuffer	= (Float32*)mBuffer;
}

// -----------------------------------------------------------------------------

void Buffer::allocateClear(UInt32 size)
{
	allocate(size);
	clear();
}

// -----------------------------------------------------------------------------

void Buffer::clear()
{
	memset(mAlignedBuffer, 0, mBufferSize * sizeof(Float32));
}

// -----------------------------------------------------------------------------

void Buffer::deallocate()
{
	if (mBuffer == NULL)
		return;
			
	free(mBuffer);
	
	mBufferSize		= 0;
	mBuffer			= NULL;
	mAlignedBuffer	= NULL;	
}

// -----------------------------------------------------------------------------

UInt32 Buffer::getBufferSize()
{
	return mBufferSize;
}

// -----------------------------------------------------------------------------

Float32* Buffer::getAlignedBuffer()
{
	return mAlignedBuffer;
}

// -----------------------------------------------------------------------------

