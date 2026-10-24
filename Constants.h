/*
 *  Constants.h
 *  Automat
 *
 *  Created by Stefan Beck on 01.02.06.
 *  Copyright 2006 alphakanal. All rights reserved.
 *
 */

// -----------------------------------------------------------------------------

#pragma once

// -----------------------------------------------------------------------------

static const UInt32		kNumberOfNotes			= 10;
static const UInt32		kMaxActiveNotes			= 10;

static const UInt32		kMaxRenderSlice			= 64;
static const UInt32		kMaxChannels			= 2;

static const UInt32		kNumberOfTimeLines		= 14;

static const UInt32		kNumberOfMidiControls	= 128;

static const UInt32		kProperty_MidiLearn		= 100000;
static const UInt32		kProperty_Randomize		= 100001;

static const UInt32		kProperty_File_Sample	= 100005;
static const UInt32		kProperty_File_Tuning	= 100006;

static const UInt32		kProperty_TimeLine_0	= 100010;
static const UInt32		kProperty_TimeLine_1	= 100011;
static const UInt32		kProperty_TimeLine_2	= 100012;
static const UInt32		kProperty_TimeLine_3	= 100013;
static const UInt32		kProperty_TimeLine_4	= 100014;
static const UInt32		kProperty_TimeLine_5	= 100015;
static const UInt32		kProperty_TimeLine_6	= 100016;
static const UInt32		kProperty_TimeLine_7	= 100017;
static const UInt32		kProperty_TimeLine_8	= 100018;
static const UInt32		kProperty_TimeLine_9	= 100019;

static const UInt32		kProperty_TimeLine_10	= 100020;
static const UInt32		kProperty_TimeLine_11	= 100021;
static const UInt32		kProperty_TimeLine_12	= 100022;
static const UInt32		kProperty_TimeLine_13	= 100023;

static const UInt32		kProperty_TimeLine_Set	= 100030;

static const UInt32		kProperty_TlPreset_0	= 100050;
static const UInt32		kProperty_TlPreset_1	= 100051;
static const UInt32		kProperty_TlPreset_2	= 100052;
static const UInt32		kProperty_TlPreset_3	= 100053;
static const UInt32		kProperty_TlPreset_4	= 100054;
static const UInt32		kProperty_TlPreset_5	= 100055;
static const UInt32		kProperty_TlPreset_6	= 100056;
static const UInt32		kProperty_TlPreset_7	= 100057;
static const UInt32		kProperty_TlPreset_8	= 100058;
static const UInt32		kProperty_TlPreset_9	= 100059;

static const UInt32		kProperty_TlPreset_10	= 100060;
static const UInt32		kProperty_TlPreset_11	= 100061;
static const UInt32		kProperty_TlPreset_12	= 100062;
static const UInt32		kProperty_TlPreset_13	= 100063;

static const UInt32		kProperty_TlEdit_0		= 100070;
static const UInt32		kProperty_TlEdit_1		= 100071;
static const UInt32		kProperty_TlEdit_2		= 100072;
static const UInt32		kProperty_TlEdit_3		= 100073;
static const UInt32		kProperty_TlEdit_4		= 100074;
static const UInt32		kProperty_TlEdit_5		= 100075;
static const UInt32		kProperty_TlEdit_6		= 100076;
static const UInt32		kProperty_TlEdit_7		= 100077;
static const UInt32		kProperty_TlEdit_8		= 100078;
static const UInt32		kProperty_TlEdit_9		= 100079;

static const UInt32		kProperty_TlEdit_10		= 100080;
static const UInt32		kProperty_TlEdit_11		= 100081;
static const UInt32		kProperty_TlEdit_12		= 100082;
static const UInt32		kProperty_TlEdit_13		= 100083;

// -----------------------------------------------------------------------------

//	Oscillator

static const UInt32		kOscNumberOfWaves					= 11;
static const void*		kOscWave[kOscNumberOfWaves]			= {CFSTR("OFF"), CFSTR("SINE"), CFSTR("TRI"), CFSTR("RECT"), CFSTR("SAW"), CFSTR("TABLE"), CFSTR("SAMPLE"), CFSTR("GRAIN"), CFSTR("NOISE"), CFSTR("CLICKS"), CFSTR("LINEIN")};

static const UInt32		kOscNumberOfSyncs					= 3;
static const void*		kOscSync[kOscNumberOfSyncs]			= {CFSTR("OFF"), CFSTR("KEY"), CFSTR("OSC")};

static const UInt32		kOscNumberOfStacks					= 5;
static const void*		kOscStack[kOscNumberOfStacks]		= {CFSTR("OFF"), CFSTR("TWO"), CFSTR("THREE"), CFSTR("FOUR"), CFSTR("FIVE")};
static const Float32	kOscStackOffset[kOscNumberOfStacks]	= {0.0, 0.04, -0.04, 0.02, -0.02};
static const Float32	kOscStackLevel[kOscNumberOfStacks]	= {1.0, 0.8, 0.7, 0.6, 0.5};

//	static const Float32	kOscTuningA							= 440.0;
//	static const Float32	kOscMiddleC							= 261.625565;

// -----------------------------------------------------------------------------

//	Shaper

static const UInt32		kShpNumberOfFilters					= 18;
static const void*		kShpFilter[kShpNumberOfFilters]		= { CFSTR("OFF"),
																CFSTR("LD 06"), CFSTR("LD 12"), CFSTR("LD 18"), CFSTR("LD 24"),
																CFSTR("LP 12"), CFSTR("HP 12"), CFSTR("BP 12"), CFSTR("BR 12"), CFSTR("PK 12"),
																CFSTR("LP 24"), CFSTR("HP 24"), CFSTR("BP 24"), CFSTR("BR 24"), CFSTR("PK 24"),
																CFSTR("AP 04"), CFSTR("AP 08"),
																CFSTR("VOWEL")};
																
static const UInt32		kShpFilterSet[kShpNumberOfFilters]	= { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17};

static const UInt32		kShpNumberOfShapes					= 6;
static const void*		kShpShape[kShpNumberOfShapes]		= { CFSTR("OFF"), 
																CFSTR("TUBE") , CFSTR("SILICON"), CFSTR("DIGITAL"), CFSTR("AM-MOD"), CFSTR("FM-MOD")};

static const UInt32		kShpShapeSet[kShpNumberOfShapes]	= { 0, 1, 2, 3, 4, 5};

// -----------------------------------------------------------------------------

//	Filter

static const UInt32		kFltNumberOfTypes					= 18;
static const void*		kFltType[kFltNumberOfTypes]			= { CFSTR("OFF"),
																CFSTR("LD 06"), CFSTR("LD 12"), CFSTR("LD 18"), CFSTR("LD 24"),
																CFSTR("LP 12"), CFSTR("HP 12"), CFSTR("BP 12"), CFSTR("BR 12"), CFSTR("PK 12"),
																CFSTR("LP 24"), CFSTR("HP 24"), CFSTR("BP 24"), CFSTR("BR 24"), CFSTR("PK 24"),
																CFSTR("AP 04"), CFSTR("AP 08"),
																CFSTR("VOWEL")};
																
static const UInt32		kFltFilterSet[kFltNumberOfTypes]	= { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17};

static const UInt32		kFltNumberOfDrives					= 3;
static const void*		kFltDriveType[kFltNumberOfDrives]	= { CFSTR("OFF"),
																CFSTR("PRE"), CFSTR("POST")};
																
// -----------------------------------------------------------------------------

//	Spatial Effects

static const UInt32		kSfxNumberOfTypes					= 3;
static const void*		kSfxType[kSfxNumberOfTypes]			= {CFSTR("OFF"), CFSTR("CHORUS"), CFSTR("PHASER")};

static const UInt32		kSfxNumberOfModes					= 2;
static const void*		kSfxMode[kSfxNumberOfModes]			= {CFSTR("MOD"), CFSTR("FIX")};

// -----------------------------------------------------------------------------

//	Delay

static const UInt32		kDlyNumberOfTimes					= 17;
static const void*		kDlyTime[kDlyNumberOfTimes]			= {	CFSTR("OFF"),
																CFSTR("4/4"), CFSTR("1/2D"), CFSTR("4/4T"), CFSTR("1/2"), CFSTR("1/4D"), CFSTR("1/2T"),
																CFSTR("1/4"), CFSTR("1/8D"), CFSTR("1/4T"), CFSTR("1/8"), CFSTR("1/16D"), CFSTR("1/8T"),
																CFSTR("1/16"), CFSTR("1/32D"), CFSTR("1/16T"), CFSTR("1/32")};

static const Float32	kDlySlices[kDlyNumberOfTimes]		= { 0.0,
																4.0, 3.0, 2.6666, 2.0, 1.5, 1.3333,
																1.0, 0.75, 0.6666, 0.5, 0.375, 0.3333,
																0.25, 0.1875, 0.1666, 0.125};

static const UInt32		kDlyNumberOfFilters					= 7;
static const void*		kDlyFilter[kDlyNumberOfFilters]		= {	CFSTR("OFF"),
																CFSTR("LP 12"), CFSTR("HP 12"), CFSTR("BP 12"), CFSTR("BR 12"), CFSTR("PK 12"),
																CFSTR("AP 4")};

static const UInt32		kDlyFilterSet[kDlyNumberOfFilters]	= { 0, 5, 6, 7, 8, 9, 15};
																
static const UInt32		kDlyNumberOfTargets					= 3;
static const void*		kDlyTarget[kDlyNumberOfTargets]		= {CFSTR("DELAY"), CFSTR("F-BACK"), CFSTR("BOTH")};

// -----------------------------------------------------------------------------

//	Reverb

static const UInt32		kRevNumberOfModes					= 2;
static const void*		kRevMode[kRevNumberOfModes]			= {CFSTR("OFF"), CFSTR("ON")};

// -----------------------------------------------------------------------------

//	Out

static const UInt32		kOutNumberOfModes					= 3;
static const void*		kOutMode[kOutNumberOfModes]			= {CFSTR("POLY"), CFSTR("MONO"), CFSTR("LEGATO")};

static const UInt32		kOutNumberOfOctaves					= 3;
static const void*		kOutOctave[kOutNumberOfOctaves]		= {CFSTR("C1"), CFSTR("C2"), CFSTR("C3")};

// -----------------------------------------------------------------------------

//	Main Modulator

enum
{
	kModTarget_Off,
	
	kModTarget_Osc_Width,
	kModTarget_Osc_Tune,
	kModTarget_Osc_Phase,
	kModTarget_Osc_Spread,

	kModTarget_Osc1_Width,
	kModTarget_Osc1_Tune,
	kModTarget_Osc1_Phase,
	kModTarget_Osc1_Spread,

	kModTarget_Osc2_Width,
	kModTarget_Osc2_Tune,
	kModTarget_Osc2_Phase,
	kModTarget_Osc2_Spread,

	kModTarget_Osc3_Width,
	kModTarget_Osc3_Tune,
	kModTarget_Osc3_Phase,
	kModTarget_Osc3_Spread,

	kModTarget_Shp_CutOff,
	kModTarget_Shp_Resonance,
	kModTarget_Shp_Bias,
	kModTarget_Shp_Drive,

	kModTarget_Shp1_CutOff,
	kModTarget_Shp1_Resonance,
	kModTarget_Shp1_Bias,
	kModTarget_Shp1_Drive,

	kModTarget_Shp2_CutOff,
	kModTarget_Shp2_Resonance,
	kModTarget_Shp2_Bias,
	kModTarget_Shp2_Drive,

	kModTarget_Shp3_CutOff,
	kModTarget_Shp3_Resonance,
	kModTarget_Shp3_Bias,
	kModTarget_Shp3_Drive,
	
	kModTarget_Mix_Panorama,
	kModTarget_Mix_Level,

	kModTarget_Mix1_Panorama,
	kModTarget_Mix1_Level,
	
	kModTarget_Mix2_Panorama,
	kModTarget_Mix2_Level,

	kModTarget_Mix3_Panorama,
	kModTarget_Mix3_Level,

	kModTarget_Flt_Drive,
	kModTarget_Flt_CutOff,
	kModTarget_Flt_Resonance,

	kModTarget_Amp_Level,
	/*kModTarget_Amp_Spread,*/

	kModTarget_Mod_Speed1,
	kModTarget_Mod_Level1,

	kModTarget_Mod_Speed2,
	kModTarget_Mod_Level2,

	kModTarget_Mod_Speed3,
	kModTarget_Mod_Level3,

	kModTarget_Mod_Speed4,
	kModTarget_Mod_Level4,

	kModTarget_Mod_Speed5,
	kModTarget_Mod_Level5,

	kModTarget_Mod_Speed6,
	kModTarget_Mod_Level6,

	kModTarget_Mod_Speed7,
	kModTarget_Mod_Level7,

	kModTarget_Mod_Speed8,
	kModTarget_Mod_Level8,

	kModTarget_Mod_Speed9,
	kModTarget_Mod_Level9,

	kModTarget_Mod_Speed10,
	kModTarget_Mod_Level10,

	kModTarget_Count,
};

enum
{
	kModModule_Osc_1	= kModTarget_Osc1_Width,
	kModModule_Osc_2	= kModTarget_Osc2_Width,
	kModModule_Osc_3	= kModTarget_Osc3_Width,

	kModModule_Shp_1	= kModTarget_Shp1_CutOff,
	kModModule_Shp_2	= kModTarget_Shp2_CutOff,
	kModModule_Shp_3	= kModTarget_Shp3_CutOff,

	kModModule_Mix		= kModTarget_Mix1_Panorama,
	
	kModModule_Flt		= kModTarget_Flt_Drive,
	
	kModModule_Amp		= kModTarget_Amp_Level,

	kModModule_Mod_1	= kModTarget_Mod_Speed1,
	kModModule_Mod_2	= kModTarget_Mod_Speed2,
	kModModule_Mod_3	= kModTarget_Mod_Speed3,
	kModModule_Mod_4	= kModTarget_Mod_Speed4,
	kModModule_Mod_5	= kModTarget_Mod_Speed5,
	kModModule_Mod_6	= kModTarget_Mod_Speed6,
	kModModule_Mod_7	= kModTarget_Mod_Speed7,
	kModModule_Mod_8	= kModTarget_Mod_Speed8,
	kModModule_Mod_9	= kModTarget_Mod_Speed9,
	kModModule_Mod_10	= kModTarget_Mod_Speed10,

	kModModule_Count,
};

static const UInt32		kModNumberOfTargets							= 65;
static const void*		kModTarget[kModNumberOfTargets]				= {	CFSTR("OFF"),
																		CFSTR("OSC - WIDTH"), CFSTR("OSC - TUNE"), CFSTR("OSC - PHASE"), CFSTR("OSC - SPREAD"),
																		CFSTR("OSC 1 WIDTH"), CFSTR("OSC 1 TUNE"), CFSTR("OSC 1 PHASE"), CFSTR("OSC 1 SPREAD"), 
																		CFSTR("OSC 2 WIDTH"), CFSTR("OSC 2 TUNE"), CFSTR("OSC 2 PHASE"), CFSTR("OSC 2 SPREAD"), 
																		CFSTR("OSC 3 WIDTH"), CFSTR("OSC 3 TUNE"), CFSTR("OSC 3 PHASE"), CFSTR("OSC 3 SPREAD"),
																		CFSTR("SHP - CUTOFF"), CFSTR("SHP - RESO"), CFSTR("SHP - BIAS"), CFSTR("SHP - DRIVE"),
																		CFSTR("SHP 1 CUTOFF"), CFSTR("SHP 1 RESO"), CFSTR("SHP 1 BIAS"), CFSTR("SHP 1 DRIVE"),
																		CFSTR("SHP 2 CUTOFF"), CFSTR("SHP 2 RESO"), CFSTR("SHP 2 BIAS"), CFSTR("SHP 2 DRIVE"), 
																		CFSTR("SHP 3 CUTOFF"), CFSTR("SHP 3 RESO"), CFSTR("SHP 3 BIAS"), CFSTR("SHP 3 DRIVE"),
																		CFSTR("MIX - PANO"), CFSTR("MIX - LEVEL"),
																		CFSTR("MIX 1 PANO"), CFSTR("MIX 1 LEVEL"),
																		CFSTR("MIX 2 PANO"), CFSTR("MIX 2 LEVEL"),
																		CFSTR("MIX 3 PANO"), CFSTR("MIX 3 LEVEL"),
																		CFSTR("FLT - DRIVE"), CFSTR("FLT - CUTOFF"), CFSTR("FLT - RESO"),
																		CFSTR("AMP - LEVEL"),
																		CFSTR("MOD 1 SPEED"), CFSTR("MOD 1 LEVEL"),
																		CFSTR("MOD 2 SPEED"), CFSTR("MOD 2 LEVEL"),
																		CFSTR("MOD 3 SPEED"), CFSTR("MOD 3 LEVEL"),
																		CFSTR("MOD 4 SPEED"), CFSTR("MOD 4 LEVEL"),
																		CFSTR("MOD 5 SPEED"), CFSTR("MOD 5 LEVEL"),
																		CFSTR("MOD 6 SPEED"), CFSTR("MOD 6 LEVEL"),
																		CFSTR("MOD 7 SPEED"), CFSTR("MOD 7 LEVEL"),
																		CFSTR("MOD 8 SPEED"), CFSTR("MOD 8 LEVEL"),
																		CFSTR("MOD 9 SPEED"), CFSTR("MOD 9 LEVEL"),
																		CFSTR("MOD 10 SPEED"), CFSTR("MOD 10 LEVEL")};

static const UInt32		kModTargetSet[kModNumberOfTargets]			= { kModTarget_Off,
																		kModTarget_Osc_Width, kModTarget_Osc_Tune, kModTarget_Osc_Phase, kModTarget_Osc_Spread,
																		kModTarget_Osc1_Width, kModTarget_Osc1_Tune, kModTarget_Osc1_Phase, kModTarget_Osc1_Spread,
																		kModTarget_Osc2_Width, kModTarget_Osc2_Tune, kModTarget_Osc2_Phase, kModTarget_Osc2_Spread,
																		kModTarget_Osc3_Width, kModTarget_Osc3_Tune, kModTarget_Osc3_Phase, kModTarget_Osc3_Spread,
																		kModTarget_Shp_CutOff, kModTarget_Shp_Resonance, kModTarget_Shp_Bias, kModTarget_Shp_Drive,
																		kModTarget_Shp1_CutOff, kModTarget_Shp1_Resonance, kModTarget_Shp1_Bias, kModTarget_Shp1_Drive,
																		kModTarget_Shp2_CutOff, kModTarget_Shp2_Resonance, kModTarget_Shp2_Bias, kModTarget_Shp2_Drive,
																		kModTarget_Shp3_CutOff, kModTarget_Shp3_Resonance, kModTarget_Shp3_Bias, kModTarget_Shp3_Drive,
																		kModTarget_Mix_Panorama, kModTarget_Mix_Level,
																		kModTarget_Mix1_Panorama, kModTarget_Mix1_Level,
																		kModTarget_Mix2_Panorama, kModTarget_Mix2_Level,
																		kModTarget_Mix3_Panorama, kModTarget_Mix3_Level,
																		kModTarget_Flt_Drive, kModTarget_Flt_CutOff, kModTarget_Flt_Resonance,
																		kModTarget_Amp_Level,
																		kModTarget_Mod_Speed1, kModTarget_Mod_Level1,
																		kModTarget_Mod_Speed2, kModTarget_Mod_Level2,
																		kModTarget_Mod_Speed3, kModTarget_Mod_Level3,
																		kModTarget_Mod_Speed4, kModTarget_Mod_Level4,
																		kModTarget_Mod_Speed5, kModTarget_Mod_Level5,
																		kModTarget_Mod_Speed6, kModTarget_Mod_Level6,
																		kModTarget_Mod_Speed7, kModTarget_Mod_Level7,
																		kModTarget_Mod_Speed8, kModTarget_Mod_Level8,
																		kModTarget_Mod_Speed9, kModTarget_Mod_Level9,
																		kModTarget_Mod_Speed10, kModTarget_Mod_Level10};
	
static const UInt32		kModNumberOfSyncs							= 5;
static const void*		kModSync[kModNumberOfSyncs]					= {CFSTR("OFF"), CFSTR("LOOP"), CFSTR("PPONG"), CFSTR("SHOT"), CFSTR("ENV")};

static const UInt32		kModNumberOfTimes							= 21;
static const void*		kModTime[kModNumberOfTimes]					= { CFSTR("1S"), CFSTR("1/10S"),
																		CFSTR("4B"), CFSTR("3B"), CFSTR("2B"),
																		CFSTR("4/4"), CFSTR("1/2D"), CFSTR("4/4T"), CFSTR("1/2"), CFSTR("1/4D"), CFSTR("1/2T"),
																		CFSTR("1/4"), CFSTR("1/8D"), CFSTR("1/4T"), CFSTR("1/8"), CFSTR("1/16D"), CFSTR("1/8T"),
																		CFSTR("1/16"), CFSTR("1/32D"), CFSTR("1/16T"), CFSTR("1/32")};

static const Float32	kModSlices[kModNumberOfTimes]				= { 1.0, 10.0,
																		0.0625, 0.0833, 0.125,
																		0.25, 0.3333, 0.375, 0.5, 0.6666, 0.75,
																		1.0, 1.3333, 1.75, 2.0, 2.6666, 3.5,
																		4.0, 5.3333, 6.0, 8.0};

// -----------------------------------------------------------------------------

//	Efx Modulator

enum
{
	kModEfxTarget_Off,

	kModEfxTarget_Sfx_Speed1,
	kModEfxTarget_Sfx_Speed2,
	kModEfxTarget_Sfx_FeedBack,
	kModEfxTarget_Sfx_Depth,
	kModEfxTarget_Sfx_Mix,
	
	kModEfxTarget_Dly_CutOff,
	kModEfxTarget_Dly_Resonance,
	kModEfxTarget_Dly_FeedBack,
	kModEfxTarget_Dly_CrossFeed,
	kModEfxTarget_Dly_Mix,

	kModEfxTarget_Rev_Size,
	kModEfxTarget_Rev_HiCut,
	kModEfxTarget_Rev_LoCut,
	kModEfxTarget_Rev_Mix,

	kModEfxTarget_Mod_Speed1,
	kModEfxTarget_Mod_Level1,

	kModEfxTarget_Mod_Speed2,
	kModEfxTarget_Mod_Level2,

	kModEfxTarget_Mod_Speed3,
	kModEfxTarget_Mod_Level3,

	kModEfxTarget_Mod_Speed4,
	kModEfxTarget_Mod_Level4,

	kModEfxTarget_Count,
};

enum
{
	kModEfxModule_Sfx	= kModEfxTarget_Sfx_Speed1,
	kModEfxModule_Dly	= kModEfxTarget_Dly_CutOff,
	kModEfxModule_Rev	= kModEfxTarget_Rev_Size,

	kModEfxModule_Mod_1	= kModEfxTarget_Mod_Speed1,
	kModEfxModule_Mod_2	= kModEfxTarget_Mod_Speed2,
	kModEfxModule_Mod_3	= kModEfxTarget_Mod_Speed3,
	kModEfxModule_Mod_4	= kModEfxTarget_Mod_Speed4,

	kModEfxModule_Count,
};

static const UInt32		kModNumberOfEfxTargets						= 23;
static const void*		kModEfxTarget[kModNumberOfEfxTargets]		= {	CFSTR("OFF"),
																		CFSTR("SFX SPEED1"), CFSTR("SFX SPEED2"), CFSTR("SFX F-BACK"), CFSTR("SFX DEPTH"), CFSTR("SFX MIX"),
																		CFSTR("DLY CUTOFF"), CFSTR("DLY RESO"), CFSTR("DLY F-BACK"), CFSTR("DLY X-FEED"), CFSTR("DLY MIX"),
																		CFSTR("REV SIZE"), CFSTR("REV HICUT"), CFSTR("REV LOCUT"), CFSTR("REV MIX"),
																		CFSTR("MOD 11 SPEED"), CFSTR("MOD 11 LEVEL"),
																		CFSTR("MOD 12 SPEED"), CFSTR("MOD 12 LEVEL"),
																		CFSTR("MOD 13 SPEED"), CFSTR("MOD 13 LEVEL"),
																		CFSTR("MOD 14 SPEED"), CFSTR("MOD 14 LEVEL")};

static const UInt32		kModEfxTargetSet[kModNumberOfEfxTargets]	= { kModEfxTarget_Off,
																		kModEfxTarget_Sfx_Speed1, kModEfxTarget_Sfx_Speed2, kModEfxTarget_Sfx_FeedBack, kModEfxTarget_Sfx_Depth, kModEfxTarget_Sfx_Mix,
																		kModEfxTarget_Dly_CutOff, kModEfxTarget_Dly_Resonance, kModEfxTarget_Dly_FeedBack, kModEfxTarget_Dly_CrossFeed, kModEfxTarget_Dly_Mix,
																		kModEfxTarget_Rev_Size, kModEfxTarget_Rev_HiCut, kModEfxTarget_Rev_LoCut, kModEfxTarget_Rev_Mix,
																		kModEfxTarget_Mod_Speed1, kModEfxTarget_Mod_Level1,
																		kModEfxTarget_Mod_Speed2, kModEfxTarget_Mod_Level2,
																		kModEfxTarget_Mod_Speed3, kModEfxTarget_Mod_Level3,
																		kModEfxTarget_Mod_Speed4, kModEfxTarget_Mod_Level4};

static const UInt32		kModNumberOfEfxSyncs						= 1;
static const void*		kModEfxSync[kModNumberOfEfxSyncs]			= {CFSTR("OFF")};

static const UInt32		kModNumberOfPresets							= 16;


// -----------------------------------------------------------------------------
