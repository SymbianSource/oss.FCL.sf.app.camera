/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Base container for Photo/video settings views
*
*  Copyright (c) 2008 Nokia Corporation.
*  This material, including documentation and any related 
*  computer programs, is protected by copyright controlled by 
*  Nokia Corporation. All rights are reserved. Copying, 
*  including reproducing, storing, adapting or translating, any 
*  or all of this material requires the prior written consent of 
*  Nokia Corporation. This material also contains confidential 
*  information which may not be disclosed to others without the 
*  prior written consent of Nokia Corporation.
*
*
*/


#ifndef GSCAMCORDERSETTINGSCONTAINER_H
#define GSCAMCORDERSETTINGSCONTAINER_H


// FORWARD DECLARATIONS
class CCamStaticSettingsModel;
class CAknSettingItemList;


// CLASS DECLARATION

/**
* Container for the image settings page.
* @since
*/
class CGSCamcorderSettingsContainer: public CCoeControl
    {
    public: // Constructors and destructor
      
        /**
        * Symbian OS default constructor.
        * @since 3.1
        */
        void ConstructL( CCamStaticSettingsModel& aModel, 
                         TBool aLauchedFromGS,
                         TBool aSecondaryCameraSettings,
                         TBool aImageSettings );

        /**
        * Destructor.
        */
        ~CGSCamcorderSettingsContainer();       
      
         
    public: // New functions
        /**
        * Open setting page for currently selected setting item.
        * @since 3.1
        */
        void EditCurrentL( TBool aCalledFromMenu = ETrue );
        
        /**
        * Save all settings.
        * @since 3.1
        */
        void SaveSettingsL();        

        /**
        * Get a pointer to the settingslist.
        * @since 9.1
        */
        CAknSettingItemList* SettingsList() const;

    protected: // Functions from base classes  
          
        /**
        * From CCoeControl
        * Gives the help context to be displayed
        * @param aContext help context related to current view
        */
        void GetHelpContext( TCoeHelpContext& aContext ) const;

    private: //data 
    
        /**
        * From CCoeControl set the size and position of its component controls.
        */
        void SizeChanged();  
        
        /**
        * From CCoeControl changes the size of the list box
        */
        void HandleResourceChange( TInt aType );
           
        /**
        * From CCoeControl return the number of controls owned
        * @return TInt number of controls
        */
        TInt CountComponentControls() const;
        
        /**
        * From CCoeControl returns a control
        * @param aIndex index of a control
        * @return CCoeControl* pointer on a control
        */
        CCoeControl* ComponentControl( TInt aIndex ) const;

        /**
        * From CCoeControl event handling section
        * @param aKeyEvent the key event
        * @param aType the type of the event
        * @return TKeyResponse key event was used by this control or not
        */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, 
                                     TEventCode aType );
        /**
        * From CCoeControl changes the size of the list box
        */                                     
        void FocusChanged( TDrawNow aDrawNow );
        
        /**
        * Draw the counter to the bitmap used in the navi pane
        * @since 3.0
        * @param aBmpGc The graphics context for the bitmap
        * @param aBmpMaskGc The graphics context for the bitmap mask
        */
	    void CreateNaviBitmapsL();

    private:
        //CGSCamPhotoSettingsList* iImageSettingsList;  // owned
        CAknSettingItemList* iSettingsList;  // owned
        TBool iLauchedFromGS;
        TBool iImageSettings;
        TInt iProductCaptureKey;
    };

#endif

// End of File
