/*
 *  AutomatView.cpp
 *  Automat
 *
 *  Created by Stefan Beck on 01.02.06.
 *  Copyright 2006 alphakanal. All rights reserved.
 *
 */

// -----------------------------------------------------------------------------

#include "AutomatView.h"

// -----------------------------------------------------------------------------

COMPONENT_ENTRY(AutomatView);

// -----------------------------------------------------------------------------

AutomatView::AutomatView(AudioUnitCarbonView inInstance):CAUCarbonViewNib(inInstance)
{
	//	printf("AutomatView::AutomatView\n");
	
	mHandlers = NULL;
}

// -----------------------------------------------------------------------------

AutomatView::~AutomatView()
{
	//	printf("AutomatView::~AutomatView\n");

	UnRegisterPropertyChanges(kProperty_File_Sample);
	UnRegisterPropertyChanges(kProperty_File_Tuning);

	for (UInt32 i = 0; i < kNumberOfTimeLines; i++)
		UnRegisterPropertyChanges(kProperty_TimeLine_Set + i);
	
	if (mHandlers != NULL)
	{
        int				count			= CFDictionaryGetCount(mHandlers);
        EventHandlerRef *theHandlers	= (EventHandlerRef*)malloc(count * sizeof(EventHandlerRef));
		
        CFDictionaryGetKeysAndValues(mHandlers, NULL, (const void **)theHandlers);
        
        for (int i = 0; i < count; i++)
            RemoveEventHandler(theHandlers[i]);
			
        CFDictionaryRemoveAllValues(mHandlers);
        CFRelease(mHandlers);
        free(theHandlers);
    }
}

// -----------------------------------------------------------------------------

void AutomatView::InitWindow(CFBundleRef sBundle)
{
	//	printf("AutomatView::InitWindow\n");

	TViewNoCompositingCompatible::RegisterClassForBundleID<TTimeLineControl>(mBundleID);
	TViewNoCompositingCompatible::RegisterClassForBundleID<TTextPopup>(mBundleID);
	TViewNoCompositingCompatible::RegisterClassForBundleID<TSwitch>(mBundleID);
	TViewNoCompositingCompatible::RegisterClassForBundleID<TMultiButton>(mBundleID);
	TViewNoCompositingCompatible::RegisterClassForBundleID<TXYControl>(mBundleID);
	
	RegisterPropertyChanges(kProperty_File_Sample);
	RegisterPropertyChanges(kProperty_File_Tuning);

	for (UInt32 i = 0; i < kNumberOfTimeLines; i++)
		RegisterPropertyChanges(kProperty_TimeLine_Set + i);		
}

// -----------------------------------------------------------------------------

void AutomatView::FinishWindow(CFBundleRef sBundle)
{	
	//	printf("AutomatView::FinishWindow\n");

	HIViewID	auxy	= {'AUxy', 0};
	OSStatus	result	= HIViewFindByID(mRootUserPane, auxy, &mXYView);
	
	if (result == noErr)
	{
		AUCarbonViewControl *auvc;
		CAAUParameter		cutoff(mEditAudioUnit, kDly_CutOffParameter, kAudioUnitScope_Global, 0);
		CAAUParameter		reso(mEditAudioUnit, kDly_ResonanceParameter, kAudioUnitScope_Global, 0);
		
		auvc = new AUCarbonViewEnhancedControl(this, mParameterListener, AUCarbonViewControl::kTypeContinuous, cutoff, mXYView, kControlNoPart, TXYControl::selectorX);
		auvc->Bind();
		AddControl(auvc);
		
		auvc = new AUCarbonViewEnhancedControl(this, mParameterListener, AUCarbonViewControl::kTypeContinuous, reso, mXYView, kControlNoPart, TXYControl::selectorY);
		auvc->Bind();
		AddControl(auvc);
	}

	for (UInt32 i = 0; i < kNumberOfTimeLines; i++)
	{
		HIViewID	autl	= {'AUtl', i};
		OSStatus	result	= HIViewFindByID(mRootUserPane, autl, &mTLViews[i]);
	
		if (result == noErr)
		{
			SetTLData(i);
			SetCallback(mTLViews[i]); 
		}
	}

	HIViewID autp0 = {'AUtp', 0};
	result = HIViewFindByID(mRootUserPane, autp0, &mTPViews[0]);
	
	if (result == noErr)
		BuildTPData(0, CFSTR("aif"), CFSTR("samples"));
	
	HIViewID autp1 = {'AUtp', 1};
	result = HIViewFindByID(mRootUserPane, autp1, &mTPViews[1]);
	
	if (result == noErr)
		BuildTPData(1, CFSTR("tun"), CFSTR("tunings"));
}

// -----------------------------------------------------------------------------

bool AutomatView::HandleCommand(EventRef event, HICommandExtended &cmd)
{
	//	printf("AutomatView::HandleCommand %i\n", (int)cmd.commandID);

	bool res = true;
	
	switch (cmd.commandID)
	{
		case 'RAND':
			HIViewRef	view	= cmd.source.control;
			UInt32		value	= (UInt32)GetControl32BitValue(view);
			
			AudioUnitSetProperty(mEditAudioUnit, kProperty_Randomize, kAudioUnitScope_Global, 0, &value, sizeof(UInt32));
			break;
		
		case 'WAVE':
			GetTPData(0);
			break;
		
		case 'TUNG':
			GetTPData(1);
			break;
		
		case 'PSET':
			SetCommandValue(cmd, kProperty_TlPreset_0);
			break;

		case 'EDIT':
			SetCommandValue(cmd, kProperty_TlEdit_0);
			break;
		
		default:
			res = false;
	}
	
	return res;
}

// -----------------------------------------------------------------------------

bool AutomatView::HandleGestureEventForView(EventRef event, HIViewRef view)
{
	//	printf("AutomatView::HandleGestureEventForView\n");

	bool			res			= false;
    TCarbonEvent	theEvent	= event;
    UInt32			ekind		= theEvent.GetKind();
		
	if (view == mXYView)
	{
		//	printf("AutomatView::HandleGestureEventForView for TXYControll\n");

		CAAUParameter	cutoff(mEditAudioUnit, kDly_CutOffParameter, kAudioUnitScope_Global, 0);
		CAAUParameter	reso(mEditAudioUnit, kDly_ResonanceParameter, kAudioUnitScope_Global, 0);
	
		if (ekind == kEventControlClick)
		{
			//	printf("AutomatView::HandleGestureEventForView event is kEventControlClick\n");
			
			TellListener(cutoff, kAudioUnitCarbonViewEvent_MouseDownInControl, NULL);
			TellListener(reso, kAudioUnitCarbonViewEvent_MouseDownInControl, NULL);
		}
		else
		{
			//	printf("AutomatView::HandleGestureEventForView event is kEventControlHit\n");

			TellListener(cutoff, kAudioUnitCarbonViewEvent_MouseUpInControl, NULL);
			TellListener(reso, kAudioUnitCarbonViewEvent_MouseUpInControl, NULL);
		}
		
		res = true;
	}

	return res || CAUCarbonViewNib::HandleGestureEventForView(event, view);
}

// -----------------------------------------------------------------------------

void AutomatView::PropertyHasChanged(AudioUnitPropertyID inPropertyID, AudioUnitScope inScope, AudioUnitElement inElement)
{	
	//	printf("AutomatView::PropertyHasChanged %ld\n", inPropertyID);

	switch(inPropertyID) 
	{
		case kProperty_File_Sample:
		case kProperty_File_Tuning:
			SetTPData(inPropertyID - kProperty_File_Sample);
			break;
			
		default:
			SetTLData(inPropertyID - kProperty_TimeLine_Set);
			break;
	}
}

// -----------------------------------------------------------------------------

void AutomatView::GetTLData(UInt32 id)
{
	//	printf("AutomatView::GetTLData %ld\n", id);

	HIViewRef control = mTLViews[id];
	
	if (control)
	{
		TLData			data;
		ComponentResult	result = GetControlData(control, 0, TTimeLineControl::copyData, sizeof(data), &data, NULL);
	
		if (result == noErr)
			AudioUnitSetProperty(mEditAudioUnit, kProperty_TimeLine_0 + id, kAudioUnitScope_Global, 0, &data, sizeof(data));
	}
}

// -----------------------------------------------------------------------------

void AutomatView::SetTLData(UInt32 id)
{
	//	printf("AutomatView::SetTLData %ld\n", id);

	HIViewRef control = mTLViews[id];
	
	if (control)
	{
		TLData			data;
		UInt32			size	= sizeof(TLData);
		ComponentResult result  = AudioUnitGetProperty(mEditAudioUnit, kProperty_TimeLine_0 + id, kAudioUnitScope_Global, 0, &data, &size);
		
		if (result == noErr)
			SetControlData(control, 0, TTimeLineControl::copyData, sizeof(data), &data);
	}
}

// -----------------------------------------------------------------------------

void AutomatView::GetTPData(UInt32 id)
{
	//	printf("AutomatView::GetTPData %ld\n", id);

	HIViewRef control = mTPViews[id];
	
	if (control)
	{
		CFStringRef		data;
		ComponentResult	result = GetControlData(control, 0, kControlEditTextCFStringTag, sizeof(data), &data, NULL);
	
		if (result == noErr)
			AudioUnitSetProperty(mEditAudioUnit, kProperty_File_Sample + id, kAudioUnitScope_Global, 0, &data, sizeof(data));
	}
}

// -----------------------------------------------------------------------------

void AutomatView::SetTPData(UInt32 id)
{
	//	printf("AutomatView::SetTPData %ld\n", id);

	HIViewRef control = mTPViews[id];
	
	if (control)
	{
		CFStringRef		data;
		UInt32			size	= sizeof(CFStringRef);
		ComponentResult result  = AudioUnitGetProperty(mEditAudioUnit, kProperty_File_Sample + id, kAudioUnitScope_Global, 0, &data, &size);
		
		if (result == noErr)
			SetControlData(control, 0, kControlEditTextCFStringTag, sizeof(data), &data);
	}
}

// -----------------------------------------------------------------------------

void AutomatView::BuildTPData(UInt32 id, CFStringRef type, CFStringRef folder)
{
	//	printf("AutomatView::BuildTPData %ld\n", id);

	OSStatus	error;
	FSRef		audioDirectory;
	CFURLRef	bundleURL;
	CFArrayRef	bundleFiles;

	error		= FSFindFolder(kUserDomain, kAudioSupportFolderType, false, &audioDirectory);
	bundleURL	= CFURLCreateFromFSRef(kCFAllocatorDefault, &audioDirectory);
	bundleURL	= CFURLCreateCopyAppendingPathComponent(kCFAllocatorDefault, bundleURL, CFSTR(kAutomatPresets), true);
	bundleFiles	= CFBundleCopyResourceURLsOfTypeInDirectory(bundleURL, type, folder);
	
	if (bundleURL)
		CFRelease(bundleURL);
		
	if (bundleFiles)
	{
		MenuRef menu;
		CreateNewMenu(1, 0, &menu);

		CFIndex		size	= CFArrayGetCount(bundleFiles);
		CFURLRef	url		= NULL;
		CFStringRef	name	= NULL;
		
		for (CFIndex i = 0; i < size; i++)
		{
			url		= reinterpret_cast<CFURLRef>(CFArrayGetValueAtIndex(bundleFiles, i));
			name	= CFURLCopyFileSystemPath(url, kCFURLPOSIXPathStyle);
			
			if (name)
				AppendMenuItemTextWithCFString(menu, name, 0, 0, 0);
		}
		
		if (url)
			CFRelease(url);
			
		if (name)
			CFRelease(name);

		HIViewRef control = mTPViews[id];
	
		if (control)
		{
			SetControlData(control, 0, kControlPopupButtonMenuRefTag, sizeof(menu), &menu);
			SetControl32BitMaximum(control, size);
			
			CFStringRef		data;
			UInt32			size	= sizeof(CFStringRef);
			ComponentResult result  = AudioUnitGetProperty(mEditAudioUnit, kProperty_File_Sample + id, kAudioUnitScope_Global, 0, &data, &size);
			
			if (result == noErr && data)
				SetControlData(control, 0, kControlEditTextCFStringTag, sizeof(data), &data);
			else
				SetControl32BitValue(control, 1);
			
			GetTPData(id);
		}
	}
}

// -----------------------------------------------------------------------------

void AutomatView::SetCommandValue(HICommandExtended &cmd, UInt32 property)
{
	HIViewID	viewId;
	HIViewRef	view	= cmd.source.control;
	
	GetControlID(view, &viewId);

	UInt32		value	= (UInt32)GetControl32BitValue(view);
	UInt32		id		= property + viewId.id;
	
	AudioUnitSetProperty(mEditAudioUnit, id, kAudioUnitScope_Global, 0, &value, sizeof(UInt32));
}

// -----------------------------------------------------------------------------

void AutomatView::SetCallback(HIViewRef view)
{
    if (view)
	{
        EventTypeSpec paneEvents[] = {{kEventClassControl, kEventControlDraw}};
        AddEventTypes(GetControlEventTarget(view), GetEventTypeCount(paneEvents), paneEvents);
	}
}

// -----------------------------------------------------------------------------

static pascal OSStatus HandleCallback(EventHandlerCallRef inHandlerRef, EventRef inEvent, void *inUserData)
{
    AutomatView		*handler	= (AutomatView*)inUserData;
    TCarbonEvent	event		= inEvent;
	HIViewRef		view		= NULL;
	
	event.GetParameter<HIViewRef>(kEventParamDirectObject, typeControlRef, &view);

	HIViewID		viewId;
	ComponentResult result = GetControlID(view, &viewId);

	if (result == noErr)
	{
		//	printf("AutomatView::HandleCallback %ld\n", viewId.id);

		if (viewId.signature == 'AUtl')
			handler->GetTLData(viewId.id);
		else if (viewId.signature == 'AUtp')
			handler->GetTPData(viewId.id);
	}
	
	return eventNotHandledErr;
}

// -----------------------------------------------------------------------------

void AutomatView::AddEventTypes(EventTargetRef target, UInt32 inNumTypes, const EventTypeSpec *inList)
{
    if (mHandlers == NULL)
        mHandlers = CFDictionaryCreateMutable(NULL, 0, NULL, NULL);
    
    EventHandlerRef handler;
    
    if (CFDictionaryGetValueIfPresent(mHandlers, target, (const void **)&handler))
        verify_noerr(AddEventTypesToHandler(handler, inNumTypes, inList));
    else
	{
        verify_noerr(InstallEventHandler(target, HandleCallback, inNumTypes, inList, this, &handler));
        CFDictionaryAddValue(mHandlers, target, handler);
    }
}

// -----------------------------------------------------------------------------

void AutomatView::RemoveEventTypes(EventTargetRef target, UInt32 inNumTypes, const EventTypeSpec *inList)
{
	if (mHandlers)
	{
		EventHandlerRef handler;
    
		if (CFDictionaryGetValueIfPresent(mHandlers, target, (const void **)&handler))
			verify_noerr(RemoveEventTypesFromHandler(handler, inNumTypes, inList));
	}
}

// -----------------------------------------------------------------------------

