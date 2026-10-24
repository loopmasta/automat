/*
 *  Buffer.h
 *  Automat
 *
 *  Created by Stefan Beck on 26.01.08.
 *  Copyright (c) 2006-2008 Stefan Beck (stiwi@alphakanal, loopmasta). MIT License, see LICENSE.
 *
 */

// -----------------------------------------------------------------------------

#ifndef BUFFER_H
#define BUFFER_H

// -----------------------------------------------------------------------------

class Buffer
{
	enum
	{ 
		kAlignInterval	= 0x10,
		kAlignMask		= kAlignInterval - 1
	};

public:
					Buffer();
				   ~Buffer();

	void			allocate(UInt32 size);
	void			allocateClear(UInt32 size);
	void			clear();
	void			deallocate();
	
	UInt32			getBufferSize();
	Float32*		getAlignedBuffer();
	
private:

	UInt32			mBufferSize;
	void*			mBuffer;
	Float32*		mAlignedBuffer;
};

#endif

// -----------------------------------------------------------------------------
