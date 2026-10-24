#include <AudioUnit/AudioUnit.r>
#include "AutomatVersion.h"

// -----------------------------------------------------------------------------

#define kAudioUnitResID_Automat		1000
#define kAudioUnitResID_AutomatView	2000

// -----------------------------------------------------------------------------

#define RES_ID			kAudioUnitResID_Automat
#define COMP_TYPE		kAudioUnitType_MusicDevice
#define COMP_SUBTYPE	kAutomatSubtype
#define COMP_MANUF		kAutomatManufacture
#define VERSION			kAutomatVersion
#define NAME			kAutomatName
#define DESCRIPTION		"Automat1 AudioUnit"
#define ENTRY_POINT		"AutomatEntry"

#include "AUResources.r"

// -----------------------------------------------------------------------------

#define RES_ID			kAudioUnitResID_AutomatView
#define COMP_TYPE		kAudioUnitCarbonViewComponentType
#define COMP_SUBTYPE	kAutomatSubtype
#define COMP_MANUF		kAutomatManufacture	
#define VERSION			kAutomatVersion
#define NAME			kAutomatName
#define DESCRIPTION		"Automat1 View"
#define ENTRY_POINT		"AutomatViewEntry"

#include "AUResources.r"

// -----------------------------------------------------------------------------
