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
* Description:  Interface for General Setting plug-in.
*
*/


#include <e32std.h>
#include <aknview.h>

class CCamGSInterface : public CAknView
    {
    public: // New functions

        /**
        * Wraps ECom object instantitation.
        * @param aUid Specifies the concrete implementation.
        */
        static CCamGSInterface* NewL( const TUid aUid );       
        
        
        /**
        * Notifies framwork that this instance is being destroyed and resources
        * can be released.
        */
        void DestroyPlugin();
        
	public:
	    /** iDtor_ID_Key Instance identifier key. When instance of an
	     *               implementation is created by ECOM framework, the
	     *               framework will assign UID for it. The UID is used in
	     *               destructor to notify framework that this instance is
	     *               being destroyed and resources can be released.
         */	
	    TUid iDtor_ID_Key;     
    };
    
    
// ----------------------------------------------------
// CCamGSInterface::NewL
// Creates General Settings plugin.
// ----------------------------------------------------
//    
inline CCamGSInterface* CCamGSInterface::NewL( const TUid aUid )
    {
	TAny* ext = REComSession::CreateImplementationL(
		aUid, _FOFF( CCamGSInterface, iDtor_ID_Key ) );

    CCamGSInterface* result = 
        reinterpret_cast< CCamGSInterface* >( ext );
	return result;
    }
    
    
// ----------------------------------------------------
// CCamGSInterface::DestroyPlugin
// Destroy Ecom plugin.
// ----------------------------------------------------
//    
inline void CCamGSInterface::DestroyPlugin()
    {
    REComSession::DestroyedImplementation( iDtor_ID_Key );    
    }    