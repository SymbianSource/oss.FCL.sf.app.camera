/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Video settings view
*
*/



#ifndef GSCAMVIDEOSETTINGSVIEW_H
#define GSCAMVIDEOSETTINGSVIEW_H

// INCLUDES
#include <aknview.h>
#include <eiklbo.h>
#include <gstabbedview.h>
#include "GSCamcorderSettingsBaseView.h"

// CONSTANTS

// FORWARD DECLARATIONS
class CGSTabHelper;
class MGSTabbedView;

// CLASS DECLARATION

/**
* Image settings page view class.
* @since 2.1
*/
class CGSCamVideoSettingsView : public CGSCamcorderSettingsBaseView,
                                public MGSTabbedView

    {
    public: // Constructors and destructor
        /**
        * Symbian two-phased constructor.
        * @since 3.1
        * @param aTabViewArray reference to tab viewarray
        */
        static CGSCamVideoSettingsView* NewLC( 
            CCamStaticSettingsModel& aModel,
            CArrayPtrFlat<MGSTabbedView>* aTabViewArray );

        /**
        * Destructor.
        */
        virtual ~CGSCamVideoSettingsView();

        /**
        * Creates new icon for tab. Ownership is transferred to client.
        * @since 3.1
        */       
        CGulIcon* CreateTabIconL();
        
    private: // Functions from base classes
        
        /**
        * From CAknView returns Uid of View
        * @return TUid uid of the view
        */
        TUid Id() const;

        
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
        * From CAknView Deactivate this view
        */
        void DoDeactivate();       
       
    private:
        /**
        * C++ constructor.
        * @since 3.1
        */
        CGSCamVideoSettingsView( CCamStaticSettingsModel& aModel );

        /**
        * Symbian 2nd phase constructor.
        * @since 3.1
        * @param aTabViewArray reference to tab viewarray
        */
        void ConstructL(CArrayPtrFlat<MGSTabbedView>* aTabViewArray);

    private: // Data
        CGSTabHelper* iTabHelper;
    };

#endif

// End of File
