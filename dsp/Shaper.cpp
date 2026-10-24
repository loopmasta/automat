/*
 *  Shaper.cpp
 *  Automat
 *
 *  Created by Stefan Beck on 09.04.06.
 *  Copyright (c) 2006-2008 Stefan Beck (stiwi@alphakanal, loopmasta). MIT License, see LICENSE.
 *
 */

// -----------------------------------------------------------------------------

#include "Shaper.h"

// -----------------------------------------------------------------------------

Shaper::Shaper()
{
	mType		= kShape_Off;

	mBias		= 0.0;
	mDrive		= 0.0;

	mKeyTrack	= 0.0;
	mVelocity	= 0.0;

	reset();
}

// -----------------------------------------------------------------------------

void Shaper::initialize(Float32 sampleRate)
{ 
	if (sampleRate < 44100.0)
		sampleRate = 44100.0;
		
	mSampleRate = sampleRate;

	reset();
	clear();
}

// -----------------------------------------------------------------------------

void Shaper::setParameter(UInt32 type, Float32 bias, Float32 drive)
{
	if (type != mType)
	{
		reset();
		clear();
	}

	mType	= type;
	mBias	= bias;
	mDrive	= drive;
}

// -----------------------------------------------------------------------------

void Shaper::getShape(UInt32 bufferSize, Float32* buffer, Float32* modulation)
{
	if (mType == kShape_Off)
		return;
	
	Float32	bias	= mBias;
	Float32	drive	= mDrive;

	if (modulation)
	{
		bias	*= modulation[kModulation_Bias];
		drive	*= modulation[kModulation_Drive];
	}
	
	switch (mType)
	{
		case kShape_Tube:
		{
			bias	= 1.0 + bias;
			drive	= 2.0 + drive * 4.0;
			
			getTube(bufferSize, buffer, bias, drive);
			break;
		}

		case kShape_Silicon:
		{
			bias	= 1.0 + bias;
			drive	= drive * 0.9;
			drive	= 2.0 * drive / (1.0 - drive);
			
			getSilicon(bufferSize, buffer, bias, drive);
			break;
		}
		
		case kShape_Digital:
		{
			Float32 minHz = (440.0 / mSampleRate);
			Float32 maxHz = (20000.0 / mSampleRate);

			bias	= minHz + pow3(1.0 - bias) * (maxHz - minHz);
			drive	= 0.001 + pow3(drive) * 0.999;
			
			getDigital(bufferSize, buffer, bias, drive);
			break;
		}
		
		case kShape_AmMod:
		{
			Float32 minHz = (1.0 / mSampleRate);
			Float32 maxHz = (8000.0 / mSampleRate);

			bias = minHz + pow3(bias) * (maxHz - minHz);
			
			getAmMod(bufferSize, buffer, bias * mKeyTrack, drive);
			break;
		}
		
		case kShape_FmMod:
		{
			bias	= bias * M_PI_2;
			drive	= 2.0 + drive * 20.0;
			
			getFmMod(bufferSize, buffer, bias, drive);
			break;
		}
	}
}

// -----------------------------------------------------------------------------

void Shaper::getTube(UInt32 bufferSize, Float32* buffer, Float32 bias, Float32 drive)
{
	const int size		= bufferSize;

	Float32 *vBias		= (Float32*)&b1;
	Float32 *vDrive		= (Float32*)&b2;
	
	Float32 *vTemp1		= (Float32*)&b3;
	Float32 *vTemp2		= (Float32*)&b4;

	Float32 one			= 1.0;
	Float32 two			= 2.0;
	
	Float32	ramp		= 1.0 / size;
	Float32	biasDelta	= (bias - mLastBias) * ramp;
	Float32	driveDelta	= (drive - mLastDrive) * ramp;

    vDSP_vramp(&mLastBias, &biasDelta, vBias, 1, size);
    vDSP_vramp(&mLastDrive, &driveDelta, vDrive, 1, size);
	
	//	1.0 - 2.0 / (exp(in * drive) + 1.0);
	
	vDSP_vmul(vDrive, 1, buffer, 1, vTemp1, 1, size);	//	vDrive * buffer -> vTemp1

	vvexpf(vTemp2, vTemp1, &size);						//	exp(vTemp1) -> vTemp2
	vDSP_vsadd(vTemp2, 1, &one, vTemp1, 1, size);		//	vTemp2 + 1.0 -> vTemp1
	
	vDSP_vfill(&two, vTemp2, 1, size);					//	fill vTemp2 -> 2.0
	vvdivf(vTemp1, vTemp2, vTemp1, &size);				//	vTemp2 / vTemp1 -> vTemp1

	vDSP_vfill(&one, vTemp2, 1, size);					//	fill vTemp2 -> 1.0
	vDSP_vsub(vTemp1, 1, vTemp2, 1, buffer, 1, size);	//	vTemp2 - vTemp1 -> buffer

	mLastBias	= bias;
	mLastDrive	= drive;
}

// -----------------------------------------------------------------------------

void Shaper::getSilicon(UInt32 bufferSize, Float32* buffer, Float32 bias, Float32 drive)
{
	const int size		= bufferSize;

	Float32 *vBias		= (Float32*)&b1;
	Float32 *vDrive		= (Float32*)&b2;

	Float32 *vTemp1		= (Float32*)&b3;
	Float32 *vTemp2		= (Float32*)&b4;
	
	Float32 *vInput		= (Float32*)&b5;

	Float32 one			= 1.0;
	Float32	ramp		= 1.0 / size;
	Float32	biasDelta	= (bias - mLastBias) * ramp;
	Float32	driveDelta	= (drive - mLastDrive) * ramp;

    vDSP_vramp(&mLastBias, &biasDelta, vBias, 1, size);
    vDSP_vramp(&mLastDrive, &driveDelta, vDrive, 1, size);
	
	//	in = in * bias;
	//	(1.0 + drive) * in / (1.0 + drive * fabsf(in));

	vDSP_vmul(vBias, 1, buffer, 1, vInput, 1, size);	//	vBias * buffer -> vInput
	
	vDSP_vabs(vInput, 1, vTemp1, 1, size);				//	abs(vInput) -> vTemp1
	vDSP_vmul(vDrive, 1, vTemp1, 1, vTemp1, 1, size);	//	vDrive * vTemp1 -> vTemp1
	vDSP_vsadd(vTemp1, 1, &one, vTemp1, 1, size);		//	vTemp1 + 1.0 -> vTemp1
	vvdivf(vTemp1, vInput, vTemp1, &size);				//	vInput / vTemp1 -> vTemp1

	vDSP_vsadd(vDrive, 1, &one, vTemp2, 1, size);		//	vDrive + 1.0 -> vTemp2
	vDSP_vmul(vTemp2, 1, vTemp1, 1, buffer, 1, size);	//	vTemp2 * vTemp1 -> buffer

	mLastBias	= bias;
	mLastDrive	= drive;
}

// -----------------------------------------------------------------------------

void Shaper::getDigital(UInt32 bufferSize, Float32* buffer, Float32 bias, Float32 drive)
{
	Float32	ramp		= 1.0 / bufferSize;

	Float32	l_bias		= mLastBias;
	Float32	biasDelta	= (bias - l_bias) * ramp;
	
	Float32	l_drive		= mLastDrive;
	Float32	driveDelta	= (drive - l_drive) * ramp;
	
	Float32	l_phase		= mSample[0];
	Float32	l_out		= mSample[1];

	Float32	in, step, delta, dummy;
	
	while(bufferSize--)
	{
		l_bias		+= biasDelta;   
		l_drive		+= driveDelta;
		l_phase 	+= l_bias;
				
		if (l_phase >= 1.0)
		{
			l_phase	   -= 1.0;
			step		= 1.0 / l_drive;
			
			in			= *buffer;
			delta		= modff((in + (in < 0.0 ? -0.5 : 0.5) * l_drive) * step, &dummy) * l_drive;
			l_out		= in - delta;
		}
			  
		*buffer++ = l_out;
	}
	
	mLastBias	= l_bias;
	mLastDrive	= l_drive;
	
	mSample[0]	= l_phase;
	mSample[1]	= l_out;
}

// -----------------------------------------------------------------------------

void Shaper::getAmMod(UInt32 bufferSize, Float32* buffer, Float32 bias, Float32 drive)
{
	const int size		= bufferSize;
	
	Float32 *vPhase		= (Float32*)&b1;
	Float32 *vSine		= (Float32*)&b2;

	Float32 *vTemp1		= (Float32*)&b3;
	Float32 *vTemp2		= (Float32*)&b4;
	Float32 *vTemp3		= (Float32*)&b5;

	Float32 one			= 1.0;
	Float32 scale		= 0.5;

	Float32	ramp		= 1.0 / bufferSize;

	Float32	l_bias		= mLastBias;
	Float32	biasDelta	= (bias - l_bias) * ramp;
	
	Float32	l_phase		= mSample[0];
	Float32 *tempPhase	= vPhase;

	while(bufferSize--)
	{
		l_bias	+= biasDelta;   
		l_phase	+= l_bias;
				
		if (l_phase >= 1.0)
			l_phase -= 1.0;
		
		*tempPhase++ = (l_phase * 2.0 - 1.0) * M_PI;
	}
	
	mSample[0]	= l_phase;
	mLastBias	= l_bias;

	//	in * (1.0 + ((0.5 + sin(phase) * 0.5) - 1.0) * drive);

	vvsinf(vSine, vPhase, &size);							//	sin(vPhase) -> vSine
	vDSP_vsmul(vSine, 1, &scale, vSine, 1, size);			//	vSine * scale -> vSine
	vDSP_vsadd(vSine, 1, &scale, vSine, 1, size);			//	vSine + scale -> vSine
			
	vDSP_vfill(&one, vTemp1, 1, size);						//	fill vTemp1 -> 1.0
	vDSP_vsub(vTemp1, 1, vSine, 1, vTemp2, 1, size);		//	vSine - vTemp1 -> vTemp2

	Float32	driveDelta	= (drive - mLastDrive) * ramp;
    vDSP_vramp(&mLastDrive, &driveDelta, vTemp3, 1, size);	//	ramp drive -> vTemp3

	vDSP_vmul(vTemp3, 1, vTemp2, 1, vTemp2, 1, size);		//	vTemp2 * vTemp3 -> vTemp2
	vDSP_vadd(vTemp2, 1, vTemp1, 1, vTemp2, 1, size);		//	vTemp2 + vTemp1 -> vTemp2
	
	vDSP_vmul(buffer, 1, vTemp2, 1, buffer, 1, size);		//	buffer * vTemp2 -> buffer

	mLastDrive	= drive;
}

// -----------------------------------------------------------------------------

void Shaper::getFmMod(UInt32 bufferSize, Float32* buffer, Float32 bias, Float32 drive)
{
	const int size		= bufferSize;
	
	Float32 *vBias		= (Float32*)&b1;
	Float32 *vDrive		= (Float32*)&b2;
	
	Float32 *vTemp1		= (Float32*)&b3;
	Float32 *vTemp2		= (Float32*)&b4;

	Float32 scale		= 0.5;
		
	Float32	ramp		= 1.0 / size;
	Float32	biasDelta	= (bias - mLastBias) * ramp;
	Float32	driveDelta	= (drive - mLastDrive) * ramp;

    vDSP_vramp(&mLastBias, &biasDelta, vBias, 1, size);
    vDSP_vramp(&mLastDrive, &driveDelta, vDrive, 1, size);

	//	sin(bias + drive * in) * scale;

	vDSP_vmul(buffer, 1, vDrive, 1, vTemp1, 1, size);	//	vDrive * buffer -> vTemp1
	vDSP_vadd(vBias, 1, vTemp1, 1, vTemp1, 1, size);	//	vBias + vTemp1 -> vTemp1	

	vvsinf(vTemp2, vTemp1, &size);						//	sin(vTemp1) -> vTemp2
	vDSP_vsmul(vTemp2, 1, &scale, buffer, 1, size);		//	vTemp2 * scale -> buffer
		
	mLastBias	= bias;
	mLastDrive	= drive;
}

// -----------------------------------------------------------------------------

void Shaper::start(Float32 keyTrack, Float32 velocity)
{
	mKeyTrack	= keyTrack;
	mVelocity	= velocity;
	
	//	clear();
}

// -----------------------------------------------------------------------------

void Shaper::update(Float32 keyTrack)
{
	mKeyTrack = keyTrack;
}

// -----------------------------------------------------------------------------

void Shaper::reset()
{
	mLastBias	= 0.0;
	mLastDrive	= 0.0;
}

// -----------------------------------------------------------------------------

void Shaper::clear()
{
	memset(&mSample[0], 0, kSample_Size * sizeof(Float32));
}

// -----------------------------------------------------------------------------

