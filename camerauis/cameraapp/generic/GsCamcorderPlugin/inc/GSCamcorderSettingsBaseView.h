/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Base class for Photo/Video settings view
*/



#ifndef GSCAMCORDERSETTINGSBASEVIEW_H
#define GSCAMCORDERSETTINGSBASEVIEW_H

// INCLUDES
#include <aknview.h>
#include <eiklbo.h>


// CLASS DECLARATION
class CGSCamcorderSettingsContainer;


/**
* Image settings page view class.
* @since
*/
class CGSCamcorderSettingsBaseView : public CAknView,
                                  public MEikListBoxObserver

    {
    public: // Constructors and destructor

        /**
        * Destructor.
        */
        virtual ~CGSCamcorderSettingsBaseView();

        /**
        * From CEikAppUi
        * Handle skin change event.
        * @since 3.1
        */
        void HandleClientRectChange();
        
        /**
        * From CCoeControl
        * Handle skin change event.
        */
        void HandleResourceChange( TInt aType );

    public: // New functions
        /*
        * Check whether this view been lauched from General Settings.
        */
        TBool LaunchedFromGS() const;

    public: // Functions from base classes
        
        /**
        * From CAknView, handle commands
        * @param aCommand command to be handled
        */
        void HandleCommandL( TInt aCommand );

        /**
        * From CAknView Activate this view
        * @param aPrevViewId 
        * @param aCustomMessageId 
        * @param aCustomMessage 
        */
        void DoActivateL( const TVwsViewId& aPrevViewId,
                          TUid aCustomMessageId,
                          const TDesC8& aCustomMessage );                          

        /**
        * From MEikMenuObserver Dynamically customize menu items
        * @param aResourceId Menu pane resource ID
        * @param aMenuPane Menu pane pointer
        */
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

        /**
        * From CAknView Deactivate this view
        */
        void DoDeactivate();       
       
        /**
        * Creates new container.
        * @since 3.1
        */
        void NewContainerL();
        
        /**
        * Get CameraGSPlugin's ccontainer.
        * @since 3.1
        */       
        CGSCamcorderSettingsContainer* Container();
        
        /**
        * Sets Navipane text
        * @since 3.1
        */
        void SetNaviPaneL();
        
        /**
        * Creates new container.
        * @since 3.1
        */   
        void CreateContainerL();
         
    public: // from MEikListBoxObserver

        /**
        * Handles listbox events.
        * @param aListBox Listbox where the event occurred.
        * @param aEventType Event type.
        * 
        */
        void HandleListBoxEventL( CEikListBox* aListBox,
                                  TListBoxEvent aEventType );        

    protected:
        /**
        * C++ constructor.
        * @since 3.1
        */
        CGSCamcorderSettingsBaseView( CCamStaticSettingsModel& aModel,
                                      TBool aPhotoSettings );

    private: // Data
        CCamStaticSettingsModel& iModel;
        TBool iSecondaryCameraSettings;
        TBool iPhotoSettings;
        CGSCamcorderSettingsContainer* iContainer;
    };

#endif

// End of File
