/*
 *  Voice.cpp
 *  Automat
 *
 *  Created by Stefan Beck on 09.04.06.
 *  Copyright (c) 2006-2008 Stefan Beck (stiwi@alphakanal, loopmasta). MIT License, see LICENSE.
 *
 */

// -----------------------------------------------------------------------------

#include "Voice.h"

// -----------------------------------------------------------------------------

void Voice::initialize(Float32 sampleRate, SampleData* sampleData, TuningData* tuningData, TimeLineData* timeLineData)
{ 
	mSampleRate	= sampleRate;
	mTuningData	= tuningData;
	
	mOscillator1.initialize(sampleRate, sampleData);
	mOscillator2.initialize(sampleRate, sampleData);
	mOscillator3.initialize(sampleRate, sampleData);

	mFilterShaper1.initialize(sampleRate);
	mFilterShaper2.initialize(sampleRate);
	mFilterShaper3.initialize(sampleRate);

	mMixer.initialize(3);
	mBlockLimiter.initialize(sampleRate);
	
	mFltEnvelope.initialize(sampleRate, 5.0);
	mMainFilter.initialize(sampleRate);

	mAmpEnvelope.initialize(sampleRate, 5.0);
	mAmplifier.setParameter(1.0, 1.0);

	mModulator.initialize(sampleRate, kModTarget_Count, timeLineData, 0, 9, kModModule_Mod_1);
}

// -----------------------------------------------------------------------------

void Voice::Attack(const MusicDeviceNoteParams &inParams)
{ 	
	UInt32	key			= mNoteID + GetGlobalParameter(kOut_OctaveParameter) * 12 - 12;
	Float32	frequency	= mTuningData->getNoteFreq(key);
	Float32	keyTrack	= pow(2.0, (inParams.mPitch - 69.0) / 12.0);
	Float32 velocity	= inParams.mVelocity / 127.0;
	
	//	printf("Voice::Attack note %ld, pitch %f, velocity %f\n", mNoteID, inParams.mPitch, inParams.mVelocity);
	//	printf("Voice::Attack pitch %ld, key %ld, keyTrack %f, frequency %f, velocity %f\n", key, keyTrack, frequency, velocity);

	mOscillator1.start(key, frequency);
	mOscillator2.start(key, frequency);
	mOscillator3.start(key, frequency);	

	mFilterShaper1.start(keyTrack, velocity);
	mFilterShaper2.start(keyTrack, velocity);
	mFilterShaper3.start(keyTrack, velocity);

	mFltEnvelope.start(velocity);
	mMainFilter.start(keyTrack, velocity);

	mAmpEnvelope.start(velocity);
	
	mModulator.start(keyTrack, velocity);
	
	//	reset();
	//	clear();
}

// -----------------------------------------------------------------------------

void Voice::Legato(const MusicDeviceNoteParams &inParams)
{
	UInt32	key			= mNoteID + GetGlobalParameter(kOut_OctaveParameter) * 12 - 12;
	Float32	frequency	= mTuningData->getNoteFreq(key);
	Float32	keyTrack	= pow(2.0, (inParams.mPitch - 69.0) / 12.0);

	mOscillator1.update(key, frequency);
	mOscillator2.update(key, frequency);
	mOscillator3.update(key, frequency);	

	mFilterShaper1.update(keyTrack);
	mFilterShaper2.update(keyTrack);
	mFilterShaper3.update(keyTrack);

	mMainFilter.update(keyTrack);

	mModulator.update(keyTrack);
}

// -----------------------------------------------------------------------------

OSStatus Voice::Render(UInt32 inNumFrames, AudioBufferList &inBufferList, bool hasLineIn, AudioBufferList &inputList)
{
	//	printf("Voice::Render %ld\n", inNumFrames);
	
	Float32* lineIn = NULL;
	
	if (hasLineIn)
		lineIn = (Float32*)inputList.mBuffers[0].mData;

	Float32*	leftOut		= (Float32*)inBufferList.mBuffers[0].mData;
	Float32*	rightOut	= (Float32*)inBufferList.mBuffers[1].mData;

	Float32*	chanel1		= mMixer.getChanel(0);
	Float32*	chanel2		= mMixer.getChanel(1);
	Float32*	chanel3		= mMixer.getChanel(2);

	Float32*	mixLeft		= mMixer.getLeftOut();
	Float32*	mixRight	= mMixer.getRightOut();
		
	Float32		flt_env		= 0.0;
	Float32		amp_env		= 0.0;
	Float32*	modulation	= NULL;
	
	UInt32		chunkSize, state;
	
	updateParameter();

    while(inNumFrames > 0)
    {
		chunkSize	 = (inNumFrames >= kMaxRenderSlice) ? kMaxRenderSlice : inNumFrames;
        inNumFrames	-= chunkSize;

		state		= GetState();
		modulation	= mModulator.getModulationWithState(chunkSize, state);
		
		//	printf("Voice::Render frames %ld state %ld\n", inNumFrames, state);
		
		flt_env		= mFltEnvelope.getEnvelope(chunkSize, state);
		amp_env		= mAmpEnvelope.getEnvelope(chunkSize, state);

		//	printf("Voice::Render flt_env %f amp_env %f\n", flt_env, amp_env);

		mMixer.clearAllChanels();

		mOscillator1.getWave(chunkSize, chanel1, lineIn, &modulation[kModModule_Osc_1]);
		mOscillator2.getWave(chunkSize, chanel2, lineIn, &modulation[kModModule_Osc_2]);
		mOscillator3.getWave(chunkSize, chanel3, lineIn, &modulation[kModModule_Osc_3]);

		mFilterShaper1.getEffect(chunkSize, chanel1, &modulation[kModModule_Shp_1]);
		mFilterShaper2.getEffect(chunkSize, chanel2, &modulation[kModModule_Shp_2]);
		mFilterShaper3.getEffect(chunkSize, chanel3, &modulation[kModModule_Shp_3]);

		mMixer.getMix(chunkSize, &modulation[kModModule_Mix]);
			
		mBlockLimiter.getDCLimiter(chunkSize, mixLeft, mixRight);
		
		mMainFilter.getFilter(chunkSize, mixLeft, mixRight, flt_env, &modulation[kModModule_Flt]);			
		mAmplifier.getAmp(chunkSize, mixLeft, mixRight, amp_env, &modulation[kModModule_Amp]);
		
		mMixer.addToOut(chunkSize, leftOut, rightOut);
		
		leftOut		+= chunkSize;
		rightOut	+= chunkSize;
		
		if (hasLineIn)
			lineIn += chunkSize;
	}
	
	if (mAmpEnvelope.isDone())
	{
		//	printf("Voice::Render isDone, endFrame %ld\n", mAmpEnvelope.getEndFrame());

		NoteEnded(mAmpEnvelope.getEndFrame());
		
		mFltEnvelope.reset();
		mAmpEnvelope.reset();
	}
			
	return noErr;
}

// -----------------------------------------------------------------------------

Float32 Voice::Amplitude()
{
	return mAmpEnvelope.getLevel();
}

// -----------------------------------------------------------------------------

void Voice::reset()
{
	mOscillator1.reset();
	mOscillator2.reset();
	mOscillator3.reset();

	mFilterShaper1.reset();
	mFilterShaper2.reset();
	mFilterShaper3.reset();

	mMixer.reset();

	mFltEnvelope.reset();
	mMainFilter.reset();
	
	mAmpEnvelope.reset();
	mAmplifier.reset();
	
	mModulator.reset();
}

// -----------------------------------------------------------------------------

void Voice::clear()
{
	mOscillator1.clear();
	mOscillator2.clear();
	mOscillator3.clear();

	mFilterShaper1.clear();
	mFilterShaper2.clear();
	mFilterShaper3.clear();

	mMixer.clear();
	
	mBlockLimiter.clear();
	
	mMainFilter.clear();
	
	mModulator.clear();
}

// -----------------------------------------------------------------------------
	
void Voice::updateParameter()
{
	AUElement* global = GetAudioUnit()->Globals();
	
	UInt32	osc1_wave			= getIntParameter(global, kOsc1_WaveParameter);
	Float32	osc1_width			= getFloatParameter(global, kOsc1_WidthParameter);
	Float32	osc1_tune			= getFloatParameter(global, kOsc1_TuneParameter);
	Float32	osc1_detune			= getFloatParameter(global, kOsc1_DetuneParameter);
	UInt32	osc1_sync			= getIntParameter(global, kOsc1_SyncParameter);
	Float32	osc1_syncOffset		= getFloatParameter(global, kOsc1_SyncOffsetParameter);
	UInt32	osc1_stack			= getIntParameter(global, kOsc1_StackParameter);
	Float32	osc1_stackOffset	= getFloatParameter(global, kOsc1_StackOffsetParameter);
	
	UInt32	shp1_filtertype		= getIntParameter(global, kShp1_FilterTypeParameter);
	Float32	shp1_cutOff			= getFloatParameter(global, kShp1_CutOffParameter);
	Float32	shp1_resonance		= getFloatParameter(global, kShp1_ResonanceParameter);
	Float32	shp1_keyTrack		= getFloatParameter(global, kShp1_KeyTrackParameter);
	Float32	shp1_velocity		= getFloatParameter(global, kShp1_VelocityParameter);
	UInt32	shp1_shapetype		= getIntParameter(global, kShp1_ShapeTypeParameter);
	Float32	shp1_bias			= getFloatParameter(global, kShp1_BiasParameter);
	Float32	shp1_drive			= getFloatParameter(global, kShp1_DriveParameter);
	
	Float32	mix1_level			= getFloatParameter(global, kMix1_LevelParameter);
	Float32	mix1_panorama		= getFloatParameter(global, kMix1_PanoramaParameter);

	UInt32	osc2_wave			= getIntParameter(global, kOsc2_WaveParameter);
	Float32	osc2_width			= getFloatParameter(global, kOsc2_WidthParameter);
	Float32	osc2_tune			= getFloatParameter(global, kOsc2_TuneParameter);
	Float32	osc2_detune			= getFloatParameter(global, kOsc2_DetuneParameter);
	UInt32	osc2_sync			= getIntParameter(global, kOsc2_SyncParameter);
	Float32	osc2_syncOffset		= getFloatParameter(global, kOsc2_SyncOffsetParameter);
	UInt32	osc2_stack			= getIntParameter(global, kOsc2_StackParameter);
	Float32	osc2_stackOffset	= getFloatParameter(global, kOsc2_StackOffsetParameter);
	
	UInt32	shp2_filtertype		= getIntParameter(global, kShp2_FilterTypeParameter);
	Float32	shp2_cutOff			= getFloatParameter(global, kShp2_CutOffParameter);
	Float32	shp2_resonance		= getFloatParameter(global, kShp2_ResonanceParameter);
	Float32	shp2_keyTrack		= getFloatParameter(global, kShp2_KeyTrackParameter);
	Float32	shp2_velocity		= getFloatParameter(global, kShp2_VelocityParameter);
	UInt32	shp2_shapetype		= getIntParameter(global, kShp2_ShapeTypeParameter);
	Float32	shp2_bias			= getFloatParameter(global, kShp2_BiasParameter);
	Float32	shp2_drive			= getFloatParameter(global, kShp2_DriveParameter);
	
	Float32	mix2_level			= getFloatParameter(global, kMix2_LevelParameter);
	Float32	mix2_panorama		= getFloatParameter(global, kMix2_PanoramaParameter);

	UInt32	osc3_wave			= getIntParameter(global, kOsc3_WaveParameter);
	Float32	osc3_width			= getFloatParameter(global, kOsc3_WidthParameter);
	Float32	osc3_tune			= getFloatParameter(global, kOsc3_TuneParameter);
	Float32	osc3_detune			= getFloatParameter(global, kOsc3_DetuneParameter);
	UInt32	osc3_sync			= getIntParameter(global, kOsc3_SyncParameter);
	Float32	osc3_syncOffset		= getFloatParameter(global, kOsc3_SyncOffsetParameter);
	UInt32	osc3_stack			= getIntParameter(global, kOsc3_StackParameter);
	Float32	osc3_stackOffset	= getFloatParameter(global, kOsc3_StackOffsetParameter);
	
	UInt32	shp3_filtertype		= getIntParameter(global, kShp3_FilterTypeParameter);
	Float32	shp3_cutOff			= getFloatParameter(global, kShp3_CutOffParameter);
	Float32	shp3_resonance		= getFloatParameter(global, kShp3_ResonanceParameter);
	Float32	shp3_keyTrack		= getFloatParameter(global, kShp3_KeyTrackParameter);
	Float32	shp3_velocity		= getFloatParameter(global, kShp3_VelocityParameter);
	UInt32	shp3_shapetype		= getIntParameter(global, kShp3_ShapeTypeParameter);
	Float32	shp3_bias			= getFloatParameter(global, kShp3_BiasParameter);
	Float32	shp3_drive			= getFloatParameter(global, kShp3_DriveParameter);
	
	Float32	mix3_level			= getFloatParameter(global, kMix3_LevelParameter);
	Float32	mix3_panorama		= getFloatParameter(global, kMix3_PanoramaParameter);

	UInt32	flt_filterType		= getIntParameter(global, kFlt_FilterTypeParameter);
	UInt32	flt_driveType		= getIntParameter(global, kFlt_DriveTypeParameter);
	Float32	flt_drive			= getFloatParameter(global, kFlt_DriveParameter);
	Float32	flt_cutOff			= getFloatParameter(global, kFlt_CutOffParameter);
	Float32	flt_resonance		= getFloatParameter(global, kFlt_ResonanceParameter);
	Float32	flt_envelope		= getFloatParameter(global, kFlt_EnvelopeParameter);
	Float32	flt_keyTrack		= getFloatParameter(global, kFlt_KeyTrackParameter);
	Float32	flt_attack			= getFloatParameter(global, kFlt_AttackParameter);
	Float32	flt_hold			= getFloatParameter(global, kFlt_HoldParameter);
	Float32	flt_decay			= getFloatParameter(global, kFlt_DecayParameter);
	Float32	flt_sustain			= getFloatParameter(global, kFlt_SustainParameter);
	Float32	flt_release			= getFloatParameter(global, kFlt_ReleaseParameter);
	Float32	flt_velocity		= getFloatParameter(global, kFlt_VelocityParameter);

	Float32	amp_attack			= getFloatParameter(global, kAmp_AttackParameter);
	Float32	amp_hold			= getFloatParameter(global, kAmp_HoldParameter);
	Float32	amp_decay			= getFloatParameter(global, kAmp_DecayParameter);
	Float32	amp_sustain			= getFloatParameter(global, kAmp_SustainParameter);
	Float32	amp_release			= getFloatParameter(global, kAmp_ReleaseParameter);
	Float32	amp_velocity		= getFloatParameter(global, kAmp_VelocityParameter);
	
	Float32	osc_glide			= getFloatParameter(global, kOut_GlideParameter);
	Float32	osc_bend			= getFloatParameter(global, kOut_BendParameter);
	
	UInt32	mod01_target		= getIntParameter(global, kMod01_TargetParameter);
	UInt32	mod01_sync			= getIntParameter(global, kMod01_SyncParameter);
	UInt32	mod01_time			= getIntParameter(global, kMod01_TimeParameter);
	Float32	mod01_speed			= getFloatParameter(global, kMod01_SpeedParameter);
	Float32	mod01_keyTrack		= getFloatParameter(global, kMod01_KeyTrackParameter);
	Float32	mod01_velocity		= getFloatParameter(global, kMod01_VelocityParameter);
	Float32	mod01_level			= getFloatParameter(global, kMod01_LevelParameter);

	UInt32	mod02_target		= getIntParameter(global, kMod02_TargetParameter);
	UInt32	mod02_sync			= getIntParameter(global, kMod02_SyncParameter);
	UInt32	mod02_time			= getIntParameter(global, kMod02_TimeParameter);
	Float32	mod02_speed			= getFloatParameter(global, kMod02_SpeedParameter);
	Float32	mod02_keyTrack		= getFloatParameter(global, kMod02_KeyTrackParameter);
	Float32	mod02_velocity		= getFloatParameter(global, kMod02_VelocityParameter);
	Float32	mod02_level			= getFloatParameter(global, kMod02_LevelParameter);

	UInt32	mod03_target		= getIntParameter(global, kMod03_TargetParameter);
	UInt32	mod03_sync			= getIntParameter(global, kMod03_SyncParameter);
	UInt32	mod03_time			= getIntParameter(global, kMod03_TimeParameter);
	Float32	mod03_speed			= getFloatParameter(global, kMod03_SpeedParameter);
	Float32	mod03_keyTrack		= getFloatParameter(global, kMod03_KeyTrackParameter);
	Float32	mod03_velocity		= getFloatParameter(global, kMod03_VelocityParameter);
	Float32	mod03_level			= getFloatParameter(global, kMod03_LevelParameter);

	UInt32	mod04_target		= getIntParameter(global, kMod04_TargetParameter);
	UInt32	mod04_sync			= getIntParameter(global, kMod04_SyncParameter);
	UInt32	mod04_time			= getIntParameter(global, kMod04_TimeParameter);
	Float32	mod04_speed			= getFloatParameter(global, kMod04_SpeedParameter);
	Float32	mod04_keyTrack		= getFloatParameter(global, kMod04_KeyTrackParameter);
	Float32	mod04_velocity		= getFloatParameter(global, kMod04_VelocityParameter);
	Float32	mod04_level			= getFloatParameter(global, kMod04_LevelParameter);

	UInt32	mod05_target		= getIntParameter(global, kMod05_TargetParameter);
	UInt32	mod05_sync			= getIntParameter(global, kMod05_SyncParameter);
	UInt32	mod05_time			= getIntParameter(global, kMod05_TimeParameter);
	Float32	mod05_speed			= getFloatParameter(global, kMod05_SpeedParameter);
	Float32	mod05_keyTrack		= getFloatParameter(global, kMod05_KeyTrackParameter);
	Float32	mod05_velocity		= getFloatParameter(global, kMod05_VelocityParameter);
	Float32	mod05_level			= getFloatParameter(global, kMod05_LevelParameter);

	UInt32	mod06_target		= getIntParameter(global, kMod06_TargetParameter);
	UInt32	mod06_sync			= getIntParameter(global, kMod06_SyncParameter);
	UInt32	mod06_time			= getIntParameter(global, kMod06_TimeParameter);
	Float32	mod06_speed			= getFloatParameter(global, kMod06_SpeedParameter);
	Float32	mod06_keyTrack		= getFloatParameter(global, kMod06_KeyTrackParameter);
	Float32	mod06_velocity		= getFloatParameter(global, kMod06_VelocityParameter);
	Float32	mod06_level			= getFloatParameter(global, kMod06_LevelParameter);

	UInt32	mod07_target		= getIntParameter(global, kMod07_TargetParameter);
	UInt32	mod07_sync			= getIntParameter(global, kMod07_SyncParameter);
	UInt32	mod07_time			= getIntParameter(global, kMod07_TimeParameter);
	Float32	mod07_speed			= getFloatParameter(global, kMod07_SpeedParameter);
	Float32	mod07_keyTrack		= getFloatParameter(global, kMod07_KeyTrackParameter);
	Float32	mod07_velocity		= getFloatParameter(global, kMod07_VelocityParameter);
	Float32	mod07_level			= getFloatParameter(global, kMod07_LevelParameter);

	UInt32	mod08_target		= getIntParameter(global, kMod08_TargetParameter);
	UInt32	mod08_sync			= getIntParameter(global, kMod08_SyncParameter);
	UInt32	mod08_time			= getIntParameter(global, kMod08_TimeParameter);
	Float32	mod08_speed			= getFloatParameter(global, kMod08_SpeedParameter);
	Float32	mod08_keyTrack		= getFloatParameter(global, kMod08_KeyTrackParameter);
	Float32	mod08_velocity		= getFloatParameter(global, kMod08_VelocityParameter);
	Float32	mod08_level			= getFloatParameter(global, kMod08_LevelParameter);

	UInt32	mod09_target		= getIntParameter(global, kMod09_TargetParameter);
	UInt32	mod09_sync			= getIntParameter(global, kMod09_SyncParameter);
	UInt32	mod09_time			= getIntParameter(global, kMod09_TimeParameter);
	Float32	mod09_speed			= getFloatParameter(global, kMod09_SpeedParameter);
	Float32	mod09_keyTrack		= getFloatParameter(global, kMod09_KeyTrackParameter);
	Float32	mod09_velocity		= getFloatParameter(global, kMod09_VelocityParameter);
	Float32	mod09_level			= getFloatParameter(global, kMod09_LevelParameter);

	UInt32	mod10_target		= getIntParameter(global, kMod10_TargetParameter);
	UInt32	mod10_sync			= getIntParameter(global, kMod10_SyncParameter);
	UInt32	mod10_time			= getIntParameter(global, kMod10_TimeParameter);
	Float32	mod10_speed			= getFloatParameter(global, kMod10_SpeedParameter);
	Float32	mod10_keyTrack		= getFloatParameter(global, kMod10_KeyTrackParameter);
	Float32	mod10_velocity		= getFloatParameter(global, kMod10_VelocityParameter);
	Float32	mod10_level			= getFloatParameter(global, kMod10_LevelParameter);

	Float32	pitchBend			= PitchBend() * osc_bend;

	mOscillator1.setParameter(pitchBend, osc1_wave, osc1_width, osc1_tune, osc1_detune, osc_glide, osc1_sync, osc1_syncOffset, osc1_stack, osc1_stackOffset);
	mOscillator2.setParameter(pitchBend, osc2_wave, osc2_width, osc2_tune, osc2_detune, osc_glide, osc2_sync, osc2_syncOffset, osc2_stack, osc2_stackOffset);
	mOscillator3.setParameter(pitchBend, osc3_wave, osc3_width, osc3_tune, osc3_detune, osc_glide, osc3_sync, osc3_syncOffset, osc3_stack, osc3_stackOffset);

	mFilterShaper1.setParameter(1, kShpFilterSet[shp1_filtertype], kShpShapeSet[shp1_shapetype], shp1_cutOff, shp1_resonance, shp1_bias, shp1_drive, shp1_keyTrack, shp1_velocity);
	mFilterShaper2.setParameter(1, kShpFilterSet[shp2_filtertype], kShpShapeSet[shp2_shapetype], shp2_cutOff, shp2_resonance, shp2_bias, shp2_drive, shp2_keyTrack, shp2_velocity);
	mFilterShaper3.setParameter(1, kShpFilterSet[shp3_filtertype], kShpShapeSet[shp3_shapetype], shp3_cutOff, shp3_resonance, shp3_bias, shp3_drive, shp3_keyTrack, shp3_velocity);
	
	mMixer.setParameter(0, mix1_panorama, mix1_level);
	mMixer.setParameter(1, mix2_panorama, mix2_level);
	mMixer.setParameter(2, mix3_panorama, mix3_level);
	
	mBlockLimiter.setParameter(true);
	
	mFltEnvelope.setParameter(flt_attack, flt_hold, flt_decay, flt_sustain, flt_release, flt_velocity);

	mMainFilter.setParameter(flt_driveType, kFltFilterSet[flt_filterType], flt_drive, flt_cutOff, flt_resonance, flt_envelope, flt_keyTrack);
	
	mAmpEnvelope.setParameter(amp_attack, amp_hold, amp_decay, amp_sustain, amp_release, amp_velocity);
	
	mModulator.setParameter(0, kModTargetSet[mod01_target], mod01_sync, mod01_time, mod01_speed, mod01_keyTrack, mod01_velocity, mod01_level);
	mModulator.setParameter(1, kModTargetSet[mod02_target], mod02_sync, mod02_time, mod02_speed, mod02_keyTrack, mod02_velocity, mod02_level);
	mModulator.setParameter(2, kModTargetSet[mod03_target], mod03_sync, mod03_time, mod03_speed, mod03_keyTrack, mod03_velocity, mod03_level);
	mModulator.setParameter(3, kModTargetSet[mod04_target], mod04_sync, mod04_time, mod04_speed, mod04_keyTrack, mod04_velocity, mod04_level);
	mModulator.setParameter(4, kModTargetSet[mod05_target], mod05_sync, mod05_time, mod05_speed, mod05_keyTrack, mod05_velocity, mod05_level);
	mModulator.setParameter(5, kModTargetSet[mod06_target], mod06_sync, mod06_time, mod06_speed, mod06_keyTrack, mod06_velocity, mod06_level);
	mModulator.setParameter(6, kModTargetSet[mod07_target], mod07_sync, mod07_time, mod07_speed, mod07_keyTrack, mod07_velocity, mod07_level);
	mModulator.setParameter(7, kModTargetSet[mod08_target], mod08_sync, mod08_time, mod08_speed, mod08_keyTrack, mod08_velocity, mod08_level);
	mModulator.setParameter(8, kModTargetSet[mod09_target], mod09_sync, mod09_time, mod09_speed, mod09_keyTrack, mod09_velocity, mod09_level);
	mModulator.setParameter(9, kModTargetSet[mod10_target], mod10_sync, mod10_time, mod10_speed, mod10_keyTrack, mod10_velocity, mod10_level);
}

// -----------------------------------------------------------------------------
