/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Container for custom camera capture button (touch) 
*
*/

#ifndef CAMCAPTUREBUTTONCONTAINER_H_
#define CAMCAPTUREBUTTONCONTAINER_H_


#include "CamPreCaptureContainerBase.h"
#include "CamSettingsInternal.hrh"


// ===========================================================================
// FORWARD DECLARATIONS

class MTouchFeedback;


// ===========================================================================
// CLASS DECLARATION

class CCamCaptureButtonContainer : public CCoeControl
    {
private:
    CCamCaptureButtonContainer();
    
protected: 

    /**
    * C++ constructor
    * @since 5.2
    * @param aController reference to CCamAppController instance
    * @param aView reference to the view containing this container
    * @param aContainer reference to the parent container
    * @param aCameraMode mode of the camera (image/video) 
    */
    CCamCaptureButtonContainer( CCamAppController& aController,
                                CAknView& aView,
                                CCamPreCaptureContainerBase& aContainer,
                                TCamCameraMode aCameraMode );
    
    /**
    * Perform the second phase construction of a CCamCaptureButtonContainer object.
    * @since 5.2
    * @param aRect rectangular region bounding this container
    */
    void ConstructL( const TRect& aRect );

public:
    /**
    * Destructor.
    */
    virtual ~CCamCaptureButtonContainer();
    
    /**
    * Two-phased factory constructor
    * @since 5.2
    * @param aController reference to CCamAppController instance
    * @param aView reference to the view containing this container
    * @param aContainer Parent container
    * @return pointer to the created CCamCaptureButtonContainer object already pushed on the Cleanupstack
    */
    static CCamCaptureButtonContainer* NewLC( CCamAppController& aController,
                                              CAknView& aView,            
                                              CCamPreCaptureContainerBase& aContainer,
                                              const TRect& aRect,
                                              TCamCameraMode aCameraMode );
    
    /**
    * Two-phased factory constructor.
    * @since 5.2
    * @param aController reference to CCamAppController instance
    * @param aView reference to the view containing this container
    * @param aContainer Parent container
    * @return pointer to the created CCamCaptureButtonContainer object
    */
    static CCamCaptureButtonContainer* NewL( CCamAppController& aController, 
                                             CAknView& aView,
                                             CCamPreCaptureContainerBase& aContainer,
                                             const TRect& aRect,
                                             TCamCameraMode aCameraMode );
    
public:
    /**
    * Performs the actual draw on the container 
    * @since 5.2
    * @param aRect area where to draw
    */
    virtual void Draw( const TRect& aRect ) const;
    
    /**
     * From CCoeControl
     * Handle pointer events. 
     * @since S60 5.2
     * @param aPointerEvent information about the pointerevent
     */
    virtual void HandlePointerEventL( const TPointerEvent& aPointerEvent );

    /**
     * Used to control visibilty of capture button
     * @since S60 5.2
     * @param aShown button is shown if ETrue, else not shown 
     */
    void SetCaptureButtonShown( TBool aShown );

private:
    /**
     * Draws the capture button
     * @since S60 5.2
     * @param aGc graphics context to draw to 
     */
    void DrawCaptureButton( CBitmapContext& aGc ) const;
    
private:

    CCamAppController& iController; 
    CAknView& iView;
    CCamPreCaptureContainerBase& iParentContainer;

    TCamCameraMode iCameraMode;
    MTouchFeedback* iFeedback;

    // Icons and rect for capturing image
    CFbsBitmap *iCaptureIcon;
    CFbsBitmap *iCaptureIconMask;
    CFbsBitmap *iCaptureIconPressed;
    CFbsBitmap *iCaptureIconPressedMask;
    TRect iCaptureRect;
    TBool iCaptureButtonShown;
    TBool iCaptureButtonPressed;
    };

#endif /* CAMCAPTUREBUTTONCONTAINER_H_ */
