/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
* 
* Description: CCActivePaletteContainer declaration.
*
*/

#ifndef ACTIVEPALETTECONTAINER_H
#define ACTIVEPALETTECONTAINER_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <coecntrl.h>
#include <coeview.h>
#include <activepalette2observer.h>


// FORWARD DECLARATIONS
class CActivePalette2ApiTest;
class MActivePalette2UI;


// CLASS DECLARATION

/**
 *  CCActivePaletteContainer
 * 
 */
class CActivePaletteContainer : public CCoeControl, 
                                public MCoeView,
                                public MActivePalette2Observer
    {
public:
    
    // Constructors and destructor
    void ConstructL(CActivePalette2ApiTest* aTest);

    /**
     * Destructor.
     */
    ~CActivePaletteContainer();

    // -------------------------------------------------------------------------
    // From CCoeControl
public:
    virtual TInt CountComponentControls() const;
    virtual CCoeControl* ComponentControl(TInt aIndex) const;
    virtual void Draw(const TRect &aRect) const;

    virtual TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);    
protected: 
    virtual void SizeChanged();
    
    // -------------------------------------------------------------------------
    // From MCoeView
public:    
    virtual TVwsViewId ViewId() const;
private: 
    virtual void ViewActivatedL(const TVwsViewId &aPrevViewId, TUid aCustomMessageId, const TDesC8 &aCustomMessage);
    virtual void ViewDeactivated();
    
    // -------------------------------------------------------------------------
    // From MActivePalette2Observer
public:
    /**
     * Called when a new item is focused
     * @param aPrevItem Item ID of previously focused item
     * @param aFocusedItem Item ID of newly focused item
     */
    virtual void NotifyItemFocused(TInt aPrevItem, TInt aFocusedItem);

    /**
     * Called when the user has selected an item from the AP, before the plugin is informed
     * @param aEvent The event data
     */
    virtual void NotifyItemSelected(const TActivePalette2EventData& aEvent);
    
    /**
     * Called when the user has selected an item from the AP, after the relevant plugin
     * has completed processing the event
     * @param aEvent The event data
     * @param aDataDes Data descriptor
     * @param aDataInt Data integer
     */
    virtual void NotifyItemComplete(const TActivePalette2EventData& aEvent,
                                    const TDesC8& aDataDes,
                                    TInt aDataInt);

    /**
     * Called when a plugin generates a message for the client app.
     * @param aEvent The event data
     * @param aMessageID The ID of the message
     * @param aDataDes Data descriptor
     * @param aDataInt Data integer
     */
    virtual void NotifyMessage(const TActivePalette2EventData& aEvent,
                               TInt aMessageID,
                               const TDesC8& aDataDes,
                               TInt aDataInt);
                               
    /**
     * Request for the client app to redraw the given area underneath the AP. Used to support semi-transparency.
     * It is important that this completes quickly. This will only be called if a CCoeControl-derived AP is
     * in use.
     * @param aArea The area to redraw.
     */                                
    virtual void Redraw(const TRect& aArea);
    
    /**
     * Informs the observer that the AP has been updated in some way. Includes animations. 
     * Only called if SetGc() has previously been called on the AP with a non-NULL parameter, and if the AP
     * in use is a CCoeControl.
     */                                
    virtual void ActivePaletteUpdated(void);
    
    // -------------------------------------------------------------------------
    // New methods        
public:
    
    /**
     * Get the Active Palette instance.
     */
    MActivePalette2UI* ActivePalette() const;

private:

    /**
     * Create Active Palette.
     */
    void CreateActivePaletteL();
    
    /**
     * Helper method to trace the Active Palette Event details.
     * @param aEvent The event.
     */
    void CheckActivePaletteEvent( const TActivePalette2EventData& aEvent );

    // -------------------------------------------------------------------------
    // Data 
private:    
    
    CActivePalette2ApiTest* iTest;
    MActivePalette2UI*      iActivePalette;
    TUid                    iAppUid;

    // -------------------------------------------------------------------------
    };

#endif // ACTIVEPALETTECONTAINER_H
