/*
 *  AutomatView.h
 *  Automat
 *
 *  Created by Stefan Beck on 01.02.06.
 *  Copyright 2006 alphakanal. All rights reserved.
 *
 */

// -----------------------------------------------------------------------------

#ifndef AUTOMATVIEW_H
#define AUTOMATVIEW_H

#include "AUCarbonViewNib.h"
#include "AUCarbonViewEnhancedControl.h"

#include "TPngPictControl.h"
#include "TKnobControl.h"
#include "TTimeLineControl.h"
#include "TTextPopup.h"
#include "TKnobArcControl.h"
#include "TSwitch.h"
#include "TMultiButton.h"
#include "TXYControl.h"

#include "Parameter.h"
#include "Constants.h"
#include "AutomatVersion.h"

// -----------------------------------------------------------------------------

class AutomatView: public CAUCarbonViewNib
{
public:

							 AutomatView(AudioUnitCarbonView inInstance);
							~AutomatView();
	
    CFStringRef				GetBundleID(){return CFSTR(kAutomatBundleID);};
	virtual CFStringRef		GetBackgroundFilename(){return CFSTR("background.png");}
	virtual bool			PatchEraseRect(){return false;}
	virtual bool			HasTimer(){return false;}
	virtual bool			SupportEditMode()const{return false;}
	
	virtual void			InitWindow(CFBundleRef sBundle);
	virtual void			FinishWindow(CFBundleRef sBundle);
	
	virtual	bool			HandleCommand(EventRef event, HICommandExtended &cmd);
	virtual	bool			HandleGestureEventForView(EventRef event, HIViewRef view);

    virtual void			PropertyHasChanged(AudioUnitPropertyID inPropertyID, AudioUnitScope inScope, AudioUnitElement inElement);
		
	void					GetTLData(UInt32 id);
	void					GetTPData(UInt32 id);
	
private:

	void					SetTLData(UInt32 id);
	void					SetTPData(UInt32 id);
	
	void					BuildTPData(UInt32 id, CFStringRef type, CFStringRef folder);
	
	void					SetCommandValue(HICommandExtended &cmd, UInt32 property);
	void					SetCallback(HIViewRef view);

    virtual void			AddEventTypes(EventTargetRef target, UInt32 inNumTypes, const EventTypeSpec *inList);
    virtual void			RemoveEventTypes(EventTargetRef target, UInt32 inNumTypes, const EventTypeSpec *inList);
	
	HIViewRef				mXYView;
	HIViewRef				mTLViews[kNumberOfTimeLines];
	HIViewRef				mTPViews[2];
	
    CFMutableDictionaryRef	mHandlers;
};

#endif

// -----------------------------------------------------------------------------
