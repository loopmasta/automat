/*
 *  Parameter.h
 *  Automat
 *
 *  Created by Stefan Beck on 09.04.06.
 *  Copyright 2006 alphakanal. All rights reserved.
 *
 */

// -----------------------------------------------------------------------------

#pragma once

// -----------------------------------------------------------------------------

#include "AUScopeElement.h"

#include "Inlines.h"

// -----------------------------------------------------------------------------

enum
{
	kGeneric	= kAudioUnitParameterUnit_Generic,
	kIndexed	= kAudioUnitParameterUnit_Indexed,
	
	kReadWrite	= kAudioUnitParameterFlag_IsReadable | kAudioUnitParameterFlag_IsWritable | kAudioUnitParameterFlag_HasCFNameString,
};

#define NAME(X) X, 0, 0, CFSTR(X)

static const AudioUnitParameterInfo kParameterInfo[] =
{
	// Name						Unit		Min		Max		Def		Flags
	{NAME("OSC1_Wave"),			kIndexed,	0.0,	10.0,	1.0,	kReadWrite},
	{NAME("OSC1_Width"),		kGeneric,	0.0,	1.0,	0.5,	kReadWrite},
	{NAME("OSC1_Tune"),			kIndexed,	0.0,	48.0,	24.0,	kReadWrite},
	{NAME("OSC1_Detune"),		kGeneric,	0.0,	1.0,	0.5,	kReadWrite},
	{NAME("OSC1_Sync"),			kIndexed,	0.0,	2.0,	0.0,	kReadWrite},
	{NAME("OSC1_SyncOffset"),	kGeneric,	0.0,	1.0,	0.25,	kReadWrite},
	{NAME("OSC1_Stack"),		kIndexed,	0.0,	4.0,	0.0,	kReadWrite},
	{NAME("OSC1_StackOffset"),	kGeneric,	0.0,	1.0,	0.5,	kReadWrite},
	
	{NAME("SHP1_FilterType"),	kIndexed,	0.0,	17.0,	0.0,	kReadWrite},
	{NAME("SHP1_CutOff"),		kGeneric,	0.0,	1.0,	0.5,	kReadWrite},
	{NAME("SHP1_Resonance"),	kGeneric,	0.0,	1.0,	0.0,	kReadWrite},
	{NAME("SHP1_KeyTrack"),		kGeneric,	0.0,	1.0,	0.5,	kReadWrite},
	{NAME("SHP1_Velocity"),		kGeneric,	0.0,	1.0,	0.5,	kReadWrite},
	{NAME("SHP1_ShapeType"),	kIndexed,	0.0,	5.0,	0.0,	kReadWrite},
	{NAME("SHP1_Bias"),			kGeneric,	0.0,	1.0,	0.5,	kReadWrite},
	{NAME("SHP1_Drive"),		kGeneric,	0.0,	1.0,	0.0,	kReadWrite},
	
	{NAME("Mix1_Panorama"),		kGeneric,	0.0,	1.0,	0.5,	kReadWrite},
	{NAME("Mix1_Level"),		kGeneric,	0.0,	1.0,	0.5,	kReadWrite},

	{NAME("OSC2_Wave"),			kIndexed,	0.0,	10.0,	0.0,	kReadWrite},
	{NAME("OSC2_Width"),		kGeneric,	0.0,	1.0,	0.5,	kReadWrite},
	{NAME("OSC2_Tune"),			kIndexed,	0.0,	48.0,	24.0,	kReadWrite},
	{NAME("OSC2_Detune"),		kGeneric,	0.0,	1.0,	0.5,	kReadWrite},
	{NAME("OSC2_Sync"),			kIndexed,	0.0,	2.0,	0.0,	kReadWrite},
	{NAME("OSC2_SyncOffset"),	kGeneric,	0.0,	1.0,	0.25,	kReadWrite},
	{NAME("OSC2_Stack"),		kIndexed,	0.0,	4.0,	0.0,	kReadWrite},
	{NAME("OSC2_StackOffset"),	kGeneric,	0.0,	1.0,	0.5,	kReadWrite},
	
	{NAME("SHP2_FilterType"),	kIndexed,	0.0,	17.0,	0.0,	kReadWrite},
	{NAME("SHP2_CutOff"),		kGeneric,	0.0,	1.0,	0.5,	kReadWrite},
	{NAME("SHP2_Resonance"),	kGeneric,	0.0,	1.0,	0.0,	kReadWrite},
	{NAME("SHP2_KeyTrack"),		kGeneric,	0.0,	1.0,	0.5,	kReadWrite},
	{NAME("SHP2_Velocity"),		kGeneric,	0.0,	1.0,	0.5,	kReadWrite},
	{NAME("SHP2_ShapeType"),	kIndexed,	0.0,	5.0,	0.0,	kReadWrite},
	{NAME("SHP2_Bias"),			kGeneric,	0.0,	1.0,	0.5,	kReadWrite},
	{NAME("SHP2_Drive"),		kGeneric,	0.0,	1.0,	0.0,	kReadWrite},
	
	{NAME("MIX2_Panorama"),		kGeneric,	0.0,	1.0,	0.5,	kReadWrite},
	{NAME("MIX2_Level"),		kGeneric,	0.0,	1.0,	0.5,	kReadWrite},

	{NAME("OSC3_Wave"),			kIndexed,	0.0,	10.0,	0.0,	kReadWrite},
	{NAME("OSC3_Width"),		kGeneric,	0.0,	1.0,	0.5,	kReadWrite},
	{NAME("OSC3_Tune"),			kIndexed,	0.0,	48.0,	24.0,	kReadWrite},
	{NAME("OSC3_Detune"),		kGeneric,	0.0,	1.0,	0.5,	kReadWrite},
	{NAME("OSC3_Sync"),			kIndexed,	0.0,	2.0,	0.0,	kReadWrite},
	{NAME("OSC3_SyncOffset"),	kGeneric,	0.0,	1.0,	0.25,	kReadWrite},
	{NAME("OSC3_Stack"),		kIndexed,	0.0,	4.0,	0.0,	kReadWrite},
	{NAME("OSC3_StackOffset"),	kGeneric,	0.0,	1.0,	0.5,	kReadWrite},
	
	{NAME("SHP3_FilterType"),	kIndexed,	0.0,	17.0,	0.0,	kReadWrite},
	{NAME("SHP3_CutOff"),		kGeneric,	0.0,	1.0,	0.5,	kReadWrite},
	{NAME("SHP3_Resonance"),	kGeneric,	0.0,	1.0,	0.0,	kReadWrite},
	{NAME("SHP3_KeyTrack"),		kGeneric,	0.0,	1.0,	0.5,	kReadWrite},
	{NAME("SHP3_Velocity"),		kGeneric,	0.0,	1.0,	0.5,	kReadWrite},
	{NAME("SHP3_ShapeType"),	kIndexed,	0.0,	5.0,	0.0,	kReadWrite},
	{NAME("SHP3_Bias"),			kGeneric,	0.0,	1.0,	0.5,	kReadWrite},
	{NAME("SHP3_Drive"),		kGeneric,	0.0,	1.0,	0.0,	kReadWrite},
	
	{NAME("MIX3_Panorama"),		kGeneric,	0.0,	1.0,	0.5,	kReadWrite},
	{NAME("MIX3_Level"),		kGeneric,	0.0,	1.0,	0.5,	kReadWrite},

	{NAME("FLT_Type"),			kIndexed,	0.0,	17.0,	0.0,	kReadWrite},
	{NAME("FLT_DriveType"),		kIndexed,	0.0,	2.0,	0.0,	kReadWrite},
	{NAME("FLT_Drive"),			kGeneric,	0.0,	1.0,	0.0,	kReadWrite},
	{NAME("FLT_CutOff"),		kGeneric,	0.0,	1.0,	0.0,	kReadWrite},
	{NAME("FLT_Resonance"),		kGeneric,	0.0,	1.0,	0.0,	kReadWrite},
	{NAME("FLT_Envelope"),		kGeneric,	0.0,	1.0,	1.0,	kReadWrite},
	{NAME("FLT_KeyTrack"),		kGeneric,	0.0,	1.0,	0.5,	kReadWrite},
	{NAME("FLT_Attack"),		kGeneric,	0.0,	1.0,	0.0,	kReadWrite},
	{NAME("FLT_Hold"),			kGeneric,	0.0,	1.0,	0.0,	kReadWrite},
	{NAME("FLT_Decay"),			kGeneric,	0.0,	1.0,	0.5,	kReadWrite},
	{NAME("FLT_Sustain"),		kGeneric,	0.0,	1.0,	0.5,	kReadWrite},
	{NAME("FLT_Release"),		kGeneric,	0.0,	1.0,	0.0,	kReadWrite},
	{NAME("FLT_Velocity"),		kGeneric,	0.0,	1.0,	0.5,	kReadWrite},

	{NAME("AMP_Attack"),		kGeneric,	0.0,	1.0,	0.1,	kReadWrite},
	{NAME("AMP_Hold"),			kGeneric,	0.0,	1.0,	0.0,	kReadWrite},
	{NAME("AMP_Decay"),			kGeneric,	0.0,	1.0,	0.0,	kReadWrite},
	{NAME("AMP_Sustain"),		kGeneric,	0.0,	1.0,	1.0,	kReadWrite},
	{NAME("AMP_Release"),		kGeneric,	0.0,	1.0,	0.1,	kReadWrite},
	{NAME("AMP_Velocity"),		kGeneric,	0.0,	1.0,	0.5,	kReadWrite},

	{NAME("SFX_Type"),			kIndexed,	0.0,	2.0,	0.0,	kReadWrite},
	{NAME("SFX_Mode"),			kIndexed,	0.0,	1.0,	0.0,	kReadWrite},
	{NAME("SFX_Speed1"),		kGeneric,	0.0,	1.0,	0.3,	kReadWrite},
	{NAME("SFX_Speed2"),		kGeneric,	0.0,	1.0,	0.2,	kReadWrite},
	{NAME("SFX_FeedBack"),		kGeneric,	0.0,	1.0,	0.0,	kReadWrite},
	{NAME("SFX_Depth"),			kGeneric,	0.0,	1.0,	0.5,	kReadWrite},
	{NAME("SFX_Mix"),			kGeneric,	0.0,	1.0,	0.5,	kReadWrite},

	{NAME("DLY_LeftTime"),		kIndexed,	0.0,	16.0,	0.0,	kReadWrite},
	{NAME("DLY_RightTime"),		kIndexed,	0.0,	16.0,	0.0,	kReadWrite},
	{NAME("DLY_FilterType"),	kIndexed,	0.0,	6.0,	0.0,	kReadWrite},
	{NAME("DLY_FilterTarget"),	kIndexed,	0.0,	2.0,	0.0,	kReadWrite},
	{NAME("DLY_CutOff"),		kGeneric,	0.0,	1.0,	0.5,	kReadWrite},
	{NAME("DLY_Resonance"),		kGeneric,	0.0,	1.0,	0.5,	kReadWrite},
	{NAME("DLY_FeedBack"),		kGeneric,	0.0,	1.0,	0.25,	kReadWrite},
	{NAME("DLY_CrossFeed"),		kGeneric,	0.0,	1.0,	0.25,	kReadWrite},
	{NAME("DLY_Mix"),			kGeneric,	0.0,	1.0,	0.5,	kReadWrite},

	{NAME("OUT_Master"),		kGeneric,	0.0,	1.0,	0.75,	kReadWrite},
	{NAME("OUT_Spread"),		kGeneric,	0.0,	1.0,	1.0,	kReadWrite},
	{NAME("OUT_Glide"),			kGeneric,	0.0,	1.0,	0.0,	kReadWrite},
	{NAME("OUT_Mode"),			kIndexed,	0.0,	2.0,	0.0,	kReadWrite},
	{NAME("OUT_Octave"),		kIndexed,	0.0,	2.0,	1.0,	kReadWrite},
	{NAME("OUT_Bend"),			kGeneric,	0.0,	1.0,	0.5,	kReadWrite},

	{NAME("TMP_One"),			kIndexed,	0.0,	0.0,	0.0,	kReadWrite},
	{NAME("TMP_Two"),			kIndexed,	0.0,	0.0,	0.0,	kReadWrite},

	{NAME("MOD01_Target"),		kIndexed,	0.0,	64.0,	0.0,	kReadWrite},
	{NAME("MOD01_Sync"),		kIndexed,	0.0,	4.0,	0.0,	kReadWrite},
	{NAME("MOD01_Time"),		kIndexed,	0.0,	20.0,	0.0,	kReadWrite},
	{NAME("MOD01_Speed"),		kGeneric,	0.0,	1.0,	0.5,	kReadWrite},
	{NAME("MOD01_KeyTrack"),	kGeneric,	0.0,	1.0,	0.5,	kReadWrite},
	{NAME("MOD01_Velocity"),	kGeneric,	0.0,	1.0,	0.5,	kReadWrite},
	{NAME("MOD01_Level"),		kGeneric,	0.0,	1.0,	0.5,	kReadWrite},

	{NAME("MOD02_Target"),		kIndexed,	0.0,	64.0,	0.0,	kReadWrite},
	{NAME("MOD02_Sync"),		kIndexed,	0.0,	4.0,	0.0,	kReadWrite},
	{NAME("MOD02_Time"),		kIndexed,	0.0,	20.0,	0.0,	kReadWrite},
	{NAME("MOD02_Speed"),		kGeneric,	0.0,	1.0,	0.5,	kReadWrite},
	{NAME("MOD02_KeyTrack"),	kGeneric,	0.0,	1.0,	0.5,	kReadWrite},
	{NAME("MOD02_Velocity"),	kGeneric,	0.0,	1.0,	0.5,	kReadWrite},
	{NAME("MOD02_Level"),		kGeneric,	0.0,	1.0,	0.5,	kReadWrite},

	{NAME("MOD03_Target"),		kIndexed,	0.0,	64.0,	0.0,	kReadWrite},
	{NAME("MOD03_Sync"),		kIndexed,	0.0,	4.0,	0.0,	kReadWrite},
	{NAME("MOD03_Time"),		kIndexed,	0.0,	20.0,	0.0,	kReadWrite},
	{NAME("MOD03_Speed"),		kGeneric,	0.0,	1.0,	0.5,	kReadWrite},
	{NAME("MOD03_KeyTrack"),	kGeneric,	0.0,	1.0,	0.5,	kReadWrite},
	{NAME("MOD03_Velocity"),	kGeneric,	0.0,	1.0,	0.5,	kReadWrite},
	{NAME("MOD03_Level"),		kGeneric,	0.0,	1.0,	0.5,	kReadWrite},

	{NAME("MOD04_Target"),		kIndexed,	0.0,	64.0,	0.0,	kReadWrite},
	{NAME("MOD04_Sync"),		kIndexed,	0.0,	4.0,	0.0,	kReadWrite},
	{NAME("MOD04_Time"),		kIndexed,	0.0,	20.0,	0.0,	kReadWrite},
	{NAME("MOD04_Speed"),		kGeneric,	0.0,	1.0,	0.5,	kReadWrite},
	{NAME("MOD04_KeyTrack"),	kGeneric,	0.0,	1.0,	0.5,	kReadWrite},
	{NAME("MOD04_Velocity"),	kGeneric,	0.0,	1.0,	0.5,	kReadWrite},
	{NAME("MOD04_Level"),		kGeneric,	0.0,	1.0,	0.5,	kReadWrite},

	{NAME("MOD05_Target"),		kIndexed,	0.0,	64.0,	0.0,	kReadWrite},
	{NAME("MOD05_Sync"),		kIndexed,	0.0,	4.0,	0.0,	kReadWrite},
	{NAME("MOD05_Time"),		kIndexed,	0.0,	20.0,	0.0,	kReadWrite},
	{NAME("MOD05_Speed"),		kGeneric,	0.0,	1.0,	0.5,	kReadWrite},
	{NAME("MOD05_KeyTrack"),	kGeneric,	0.0,	1.0,	0.5,	kReadWrite},
	{NAME("MOD05_Velocity"),	kGeneric,	0.0,	1.0,	0.5,	kReadWrite},
	{NAME("MOD05_Level"),		kGeneric,	0.0,	1.0,	0.5,	kReadWrite},

	{NAME("MOD06_Target"),		kIndexed,	0.0,	64.0,	0.0,	kReadWrite},
	{NAME("MOD06_Sync"),		kIndexed,	0.0,	4.0,	0.0,	kReadWrite},
	{NAME("MOD06_Time"),		kIndexed,	0.0,	20.0,	0.0,	kReadWrite},
	{NAME("MOD06_Speed"),		kGeneric,	0.0,	1.0,	0.5,	kReadWrite},
	{NAME("MOD06_KeyTrack"),	kGeneric,	0.0,	1.0,	0.5,	kReadWrite},
	{NAME("MOD06_Velocity"),	kGeneric,	0.0,	1.0,	0.5,	kReadWrite},
	{NAME("MOD06_Level"),		kGeneric,	0.0,	1.0,	0.5,	kReadWrite},

	{NAME("MOD07_Target"),		kIndexed,	0.0,	64.0,	0.0,	kReadWrite},
	{NAME("MOD07_Sync"),		kIndexed,	0.0,	4.0,	0.0,	kReadWrite},
	{NAME("MOD07_Time"),		kIndexed,	0.0,	20.0,	0.0,	kReadWrite},
	{NAME("MOD07_Speed"),		kGeneric,	0.0,	1.0,	0.5,	kReadWrite},
	{NAME("MOD07_KeyTrack"),	kGeneric,	0.0,	1.0,	0.5,	kReadWrite},
	{NAME("MOD07_Velocity"),	kGeneric,	0.0,	1.0,	0.5,	kReadWrite},
	{NAME("MOD07_Level"),		kGeneric,	0.0,	1.0,	0.5,	kReadWrite},

	{NAME("MOD08_Target"),		kIndexed,	0.0,	64.0,	0.0,	kReadWrite},
	{NAME("MOD08_Sync"),		kIndexed,	0.0,	4.0,	0.0,	kReadWrite},
	{NAME("MOD08_Time"),		kIndexed,	0.0,	20.0,	0.0,	kReadWrite},
	{NAME("MOD08_Speed"),		kGeneric,	0.0,	1.0,	0.5,	kReadWrite},
	{NAME("MOD08_KeyTrack"),	kGeneric,	0.0,	1.0,	0.5,	kReadWrite},
	{NAME("MOD08_Velocity"),	kGeneric,	0.0,	1.0,	0.5,	kReadWrite},
	{NAME("MOD08_Level"),		kGeneric,	0.0,	1.0,	0.5,	kReadWrite},

	{NAME("MOD09_Target"),		kIndexed,	0.0,	64.0,	0.0,	kReadWrite},
	{NAME("MOD09_Sync"),		kIndexed,	0.0,	4.0,	0.0,	kReadWrite},
	{NAME("MOD09_Time"),		kIndexed,	0.0,	20.0,	0.0,	kReadWrite},
	{NAME("MOD09_Speed"),		kGeneric,	0.0,	1.0,	0.5,	kReadWrite},
	{NAME("MOD09_KeyTrack"),	kGeneric,	0.0,	1.0,	0.5,	kReadWrite},
	{NAME("MOD09_Velocity"),	kGeneric,	0.0,	1.0,	0.5,	kReadWrite},
	{NAME("MOD09_Level"),		kGeneric,	0.0,	1.0,	0.5,	kReadWrite},

	{NAME("MOD10_Target"),		kIndexed,	0.0,	64.0,	0.0,	kReadWrite},
	{NAME("MOD10_Sync"),		kIndexed,	0.0,	4.0,	0.0,	kReadWrite},
	{NAME("MOD10_Time"),		kIndexed,	0.0,	20.0,	0.0,	kReadWrite},
	{NAME("MOD10_Speed"),		kGeneric,	0.0,	1.0,	0.5,	kReadWrite},
	{NAME("MOD10_KeyTrack"),	kGeneric,	0.0,	1.0,	0.5,	kReadWrite},
	{NAME("MOD10_Velocity"),	kGeneric,	0.0,	1.0,	0.5,	kReadWrite},
	{NAME("MOD10_Level"),		kGeneric,	0.0,	1.0,	0.5,	kReadWrite},

	{NAME("MOD11_Target"),		kIndexed,	0.0,	22.0,	0.0,	kReadWrite},
	{NAME("MOD11_Sync"),		kIndexed,	0.0,	0.0,	0.0,	kReadWrite},
	{NAME("MOD11_Time"),		kIndexed,	0.0,	20.0,	0.0,	kReadWrite},
	{NAME("MOD11_Speed"),		kGeneric,	0.0,	1.0,	0.5,	kReadWrite},
	{NAME("MOD11_Level"),		kGeneric,	0.0,	1.0,	0.5,	kReadWrite},

	{NAME("MOD12_Target"),		kIndexed,	0.0,	22.0,	0.0,	kReadWrite},
	{NAME("MOD12_Sync"),		kIndexed,	0.0,	0.0,	0.0,	kReadWrite},
	{NAME("MOD12_Time"),		kIndexed,	0.0,	20.0,	0.0,	kReadWrite},
	{NAME("MOD12_Speed"),		kGeneric,	0.0,	1.0,	0.5,	kReadWrite},
	{NAME("MOD12_Level"),		kGeneric,	0.0,	1.0,	0.5,	kReadWrite},

	{NAME("MOD13_Target"),		kIndexed,	0.0,	22.0,	0.0,	kReadWrite},
	{NAME("MOD13_Sync"),		kIndexed,	0.0,	0.0,	0.0,	kReadWrite},
	{NAME("MOD13_Time"),		kIndexed,	0.0,	20.0,	0.0,	kReadWrite},
	{NAME("MOD13_Speed"),		kGeneric,	0.0,	1.0,	0.5,	kReadWrite},
	{NAME("MOD13_Level"),		kGeneric,	0.0,	1.0,	0.5,	kReadWrite},

	{NAME("MOD14_Target"),		kIndexed,	0.0,	22.0,	0.0,	kReadWrite},
	{NAME("MOD14_Sync"),		kIndexed,	0.0,	0.0,	0.0,	kReadWrite},
	{NAME("MOD14_Time"),		kIndexed,	0.0,	20.0,	0.0,	kReadWrite},
	{NAME("MOD14_Speed"),		kGeneric,	0.0,	1.0,	0.5,	kReadWrite},
	{NAME("MOD14_Level"),		kGeneric,	0.0,	1.0,	0.5,	kReadWrite},

	{NAME("REV_Mode"),			kIndexed,	0.0,	1.0,	0.0,	kReadWrite},
	{NAME("REV_Size"),			kGeneric,	0.0,	1.0,	0.5,	kReadWrite},
	{NAME("REV_Damp"),			kGeneric,	0.0,	1.0,	0.5,	kReadWrite},
	{NAME("REV_Position"),		kGeneric,	0.0,	1.0,	0.5,	kReadWrite},
	{NAME("REV_Delay"),			kGeneric,	0.0,	1.0,	0.5,	kReadWrite},
	{NAME("REV_Mix"),			kGeneric,	0.0,	1.0,	0.5,	kReadWrite},
};

enum 
{
	kOsc1_WaveParameter,			//	0
	kOsc1_WidthParameter,
	kOsc1_TuneParameter,
	kOsc1_DetuneParameter,
	kOsc1_SyncParameter,
	kOsc1_SyncOffsetParameter,
	kOsc1_StackParameter,
	kOsc1_StackOffsetParameter,
	
	kShp1_FilterTypeParameter,		//	8
	kShp1_CutOffParameter,
	kShp1_ResonanceParameter,
	kShp1_KeyTrackParameter,
	kShp1_VelocityParameter,
	kShp1_ShapeTypeParameter,
	kShp1_BiasParameter,
	kShp1_DriveParameter,
	
	kMix1_PanoramaParameter,		//	16
	kMix1_LevelParameter,

	kOsc2_WaveParameter,			//	18
	kOsc2_WidthParameter,
	kOsc2_TuneParameter,
	kOsc2_DetuneParameter,
	kOsc2_SyncParameter,
	kOsc2_SyncOffsetParameter,
	kOsc2_StackParameter,
	kOsc2_StackOffsetParameter,
	
	kShp2_FilterTypeParameter,		//	26
	kShp2_CutOffParameter,
	kShp2_ResonanceParameter,
	kShp2_KeyTrackParameter,
	kShp2_VelocityParameter,
	kShp2_ShapeTypeParameter,
	kShp2_BiasParameter,
	kShp2_DriveParameter,
	
	kMix2_PanoramaParameter,		//	34
	kMix2_LevelParameter,

	kOsc3_WaveParameter,			//	36
	kOsc3_WidthParameter,
	kOsc3_TuneParameter,
	kOsc3_DetuneParameter,
	kOsc3_SyncParameter,
	kOsc3_SyncOffsetParameter,
	kOsc3_StackParameter,
	kOsc3_StackOffsetParameter,
	
	kShp3_FilterTypeParameter,		//	44
	kShp3_CutOffParameter,
	kShp3_ResonanceParameter,
	kShp3_KeyTrackParameter,
	kShp3_VelocityParameter,
	kShp3_ShapeTypeParameter,
	kShp3_BiasParameter,
	kShp3_DriveParameter,

	kMix3_PanoramaParameter,		//	52
	kMix3_LevelParameter,

	kFlt_FilterTypeParameter,		//	54
	kFlt_DriveTypeParameter,
	kFlt_DriveParameter,
	kFlt_CutOffParameter,
	kFlt_ResonanceParameter,
	kFlt_EnvelopeParameter,
	kFlt_KeyTrackParameter,
	kFlt_AttackParameter,
	kFlt_HoldParameter,
	kFlt_DecayParameter,
	kFlt_SustainParameter,
	kFlt_ReleaseParameter,
	kFlt_VelocityParameter,

	kAmp_AttackParameter,			//	67
	kAmp_HoldParameter,
	kAmp_DecayParameter,
	kAmp_SustainParameter,
	kAmp_ReleaseParameter,
	kAmp_VelocityParameter,

	kSfx_TypeParameter,				//	73
	kSfx_ModeParameter,
	kSfx_Speed1Parameter,
	kSfx_Speed2Parameter,
	kSfx_FeedBackParameter,
	kSfx_DepthParameter,
	kSfx_MixParameter,

	kDly_LeftTimeParameter,			//	80
	kDly_RightTimeParameter,
	kDly_FilterTypeParameter,
	kDly_FilterTargetParameter,
	kDly_CutOffParameter,
	kDly_ResonanceParameter,
	kDly_FeedBackParameter,
	kDly_CrossFeedParameter,
	kDly_MixParameter,
	
	kOut_MasterParameter,			//	89
	kOut_SpreadParameter,
	kOut_GlideParameter,
	kOut_ModeParameter,
	kOut_OctaveParameter,
	kOut_BendParameter,

	kFile_Aif,						//	95
	kFile_Tun,

	kMod01_TargetParameter,			//	97
	kMod01_SyncParameter,
	kMod01_TimeParameter,
	kMod01_SpeedParameter,
	kMod01_KeyTrackParameter,
	kMod01_VelocityParameter,
	kMod01_LevelParameter,
	
	kMod02_TargetParameter,			//	104
	kMod02_SyncParameter,
	kMod02_TimeParameter,
	kMod02_SpeedParameter,
	kMod02_KeyTrackParameter,
	kMod02_VelocityParameter,
	kMod02_LevelParameter,

	kMod03_TargetParameter,			//	111
	kMod03_SyncParameter,
	kMod03_TimeParameter,
	kMod03_SpeedParameter,
	kMod03_KeyTrackParameter,
	kMod03_VelocityParameter,
	kMod03_LevelParameter,

	kMod04_TargetParameter,			//	118
	kMod04_SyncParameter,
	kMod04_TimeParameter,
	kMod04_SpeedParameter,
	kMod04_KeyTrackParameter,
	kMod04_VelocityParameter,
	kMod04_LevelParameter,

	kMod05_TargetParameter,			//	125
	kMod05_SyncParameter,
	kMod05_TimeParameter,
	kMod05_SpeedParameter,
	kMod05_KeyTrackParameter,
	kMod05_VelocityParameter,
	kMod05_LevelParameter,

	kMod06_TargetParameter,			//	132
	kMod06_SyncParameter,
	kMod06_TimeParameter,
	kMod06_SpeedParameter,
	kMod06_KeyTrackParameter,
	kMod06_VelocityParameter,
	kMod06_LevelParameter,

	kMod07_TargetParameter,			//	139
	kMod07_SyncParameter,
	kMod07_TimeParameter,
	kMod07_SpeedParameter,
	kMod07_KeyTrackParameter,
	kMod07_VelocityParameter,
	kMod07_LevelParameter,

	kMod08_TargetParameter,			//	146
	kMod08_SyncParameter,
	kMod08_TimeParameter,
	kMod08_SpeedParameter,
	kMod08_KeyTrackParameter,
	kMod08_VelocityParameter,
	kMod08_LevelParameter,

	kMod09_TargetParameter,			//	153
	kMod09_SyncParameter,
	kMod09_TimeParameter,
	kMod09_SpeedParameter,
	kMod09_KeyTrackParameter,
	kMod09_VelocityParameter,
	kMod09_LevelParameter,

	kMod10_TargetParameter,			//	160
	kMod10_SyncParameter,
	kMod10_TimeParameter,
	kMod10_SpeedParameter,
	kMod10_KeyTrackParameter,
	kMod10_VelocityParameter,
	kMod10_LevelParameter,

	kMod11_TargetParameter,			//	167
	kMod11_SyncParameter,
	kMod11_TimeParameter,
	kMod11_SpeedParameter,
	kMod11_LevelParameter,

	kMod12_TargetParameter,			//	172
	kMod12_SyncParameter,
	kMod12_TimeParameter,
	kMod12_SpeedParameter,
	kMod12_LevelParameter,

	kMod13_TargetParameter,			//	177
	kMod13_SyncParameter,
	kMod13_TimeParameter,
	kMod13_SpeedParameter,
	kMod13_LevelParameter,

	kMod14_TargetParameter,			//	182
	kMod14_SyncParameter,
	kMod14_TimeParameter,
	kMod14_SpeedParameter,
	kMod14_LevelParameter,

	kRev_ModeParameter,				//	187
	kRev_SizeParameter,
	kRev_HiCutParameter,
	kRev_LoCutParameter,
	kRev_DelayParameter,
	kRev_MixParameter,

	kParameter_Count,				//	193
};

// -----------------------------------------------------------------------------

static inline Float32 getFloatParameter(AUElement* global, UInt32 id)
{
	return global->GetParameter(id);
}

static inline UInt32 getIntParameter(AUElement* global, UInt32 id)
{
	return (UInt32)global->GetParameter(id);
}

static inline bool getBoolParameter(AUElement* global, UInt32 id)
{
	return global->GetParameter(id);
}

// -----------------------------------------------------------------------------


