/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implemantation for CCamBatteryPaneDrawer class.
*
*/


#include "cambatterypanedrawer.h"

#include <AknIconUtils.h>
#include <AknUtils.h> 
#include <cameraapp.mbg>
#include <aknconsts.h> // KAvkonBitmapFile
#include <fbs.h> // FbsBitmap
#include <AknsUtils.h> 
#include <AknIconUtils.h>
#include <akniconconfig.h>
#include <AknLayoutDef.h> // TAknWindowLineLayout
#include <aknlayoutscalable_avkon.cdl.h>
#include <aknlayoutscalable_apps.cdl.h>
#include <layoutmetadata.cdl.h>

#include <cameraapp.rsg>
#include <vgacamsettings.rsg>
 
#include "CamUtility.h"
#include "cambitmapitem.h"

const TInt KCamDefaultBatteryCellHeight = 3;
const TInt KCamDefaultBatteryCellMargin = 1;

const TInt KBatteryStrengthIcons[] = 
        {
                EMbmCameraappQgn_indi_cam4_battery_strength_0,
                EMbmCameraappQgn_indi_cam4_battery_strength_1,
                EMbmCameraappQgn_indi_cam4_battery_strength_2,
                EMbmCameraappQgn_indi_cam4_battery_strength_3,
                EMbmCameraappQgn_indi_cam4_battery_strength_4,
                EMbmCameraappQgn_indi_cam4_battery_strength_5,
                EMbmCameraappQgn_indi_cam4_battery_strength_6,
                EMbmCameraappQgn_indi_cam4_battery_strength_7
        };

// ---------------------------------------------------------------------------
// CCamBatteryPaneDrawer::CCamBatteryPaneDrawer
// ---------------------------------------------------------------------------
//
CCamBatteryPaneDrawer::CCamBatteryPaneDrawer()
    {
    }

// ---------------------------------------------------------------------------
// CCamBatteryPaneDrawer::~CCamBatteryPaneDrawer
// ---------------------------------------------------------------------------
//
CCamBatteryPaneDrawer::~CCamBatteryPaneDrawer()
  {
  PRINT ( _L("Camera => ~CCamBatteryPaneDrawer") ); 	
  
  DeleteIcons();       
  
  PRINT ( _L("Camera <= ~CCamBatteryPaneDrawer") ); 
  }

// ---------------------------------------------------------------------------
// CCamBatteryPaneDrawer::NewL
// ---------------------------------------------------------------------------
//
CCamBatteryPaneDrawer* CCamBatteryPaneDrawer::NewL()
    {
    PRINT ( _L("Camera => CCamBatteryPaneDrawer::NewL") ); 	    	
    
    CCamBatteryPaneDrawer* self = new (ELeave) CCamBatteryPaneDrawer();

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(); // self
    PRINT ( _L("Camera <= CCamBatteryPaneDrawer::NewL") );    
    return self;
    }

// ---------------------------------------------------------------------------
// CCamBatteryPaneDrawer::ConstructL
// ---------------------------------------------------------------------------
//
void CCamBatteryPaneDrawer::ConstructL()
    {
    PRINT( _L("Camera => CCamBatteryPaneDrawer::ConstructL") );	
    
    // Load icon bitmaps
    LoadIconsL();
    // Load/calculate layout data
    LoadLayoutsL();

    PRINT( _L("Camera <= CCamBatteryPaneDrawer::ConstructL") );	
    }

// ---------------------------------------------------------------------------
// CCamBatteryPaneDrawer::SetBatteryStrength
// ---------------------------------------------------------------------------
//
void CCamBatteryPaneDrawer::SetBatteryStrength( TInt aBatteryStrength )
    {
    iBatteryStrength = aBatteryStrength;
    }
    
// ---------------------------------------------------------------------------
// CCamBatteryPaneDrawer::SetLocation
// ---------------------------------------------------------------------------    
//
void CCamBatteryPaneDrawer::SetLocation( const TPoint& aLocation )
    {
    iRect = TRect( aLocation, iRect.Size() );
    }

// ---------------------------------------------------------------------------
// CCamBatteryPaneDrawer::LoadIconsL
// ---------------------------------------------------------------------------
//
void CCamBatteryPaneDrawer::LoadIconsL()
    {    
    PRINT( _L("Camera => CCamBatteryPaneDrawer::LoadIconsL") );	
    
    // Delete existing icon bitmaps
    DeleteIcons();

    for( TInt index = KMinBatteryStrength; index <= KMaxBatteryStrength; index++ )
        {
        iBatteryIcons.AppendL( CCamBitmapItem::NewL( KBatteryStrengthIcons[index], 
                KBatteryStrengthIcons[index] + 1) );
        }

    PRINT( _L("Camera <= CCamBatteryPaneDrawer::LoadIconsL") );	       
    }
    

// ---------------------------------------------------------------------------
// CCamBatteryPaneDrawer::Rect
// ---------------------------------------------------------------------------    
//
TRect CCamBatteryPaneDrawer::Rect() const
	{		
	return iRect;
	}

// ---------------------------------------------------------------------------
// CCamBatteryPaneDrawer::Draw
// ---------------------------------------------------------------------------
//
void CCamBatteryPaneDrawer::Draw( CBitmapContext& aGc ) const
    {
    iBatteryIcons[ iBatteryStrength ]->Draw( aGc );
    }       

// ---------------------------------------------------------------------------
// CCamBatteryPaneDrawer::ClearBattery
// ---------------------------------------------------------------------------
//
void CCamBatteryPaneDrawer::ClearBattery( CBitmapContext& aGc ) const
    {   
    PRINT( _L("Camera => CCamBatteryPaneDrawer::ClearBattery") ); 
    
    // Get the battery pane rectangle   
    const TRect rect = Rect();                            
    aGc.SetPenStyle( CGraphicsContext::ENullPen );
    aGc.SetBrushStyle( CGraphicsContext::ESolidBrush );
    aGc.SetDrawMode( CGraphicsContext::EDrawModeWriteAlpha );
    aGc.SetBrushColor( TRgb::Color16MA( 0 ) );
    aGc.Clear( rect ); //DrawRect( rect );
    PRINT( _L("Camera <= CCamBatteryPaneDrawer::ClearBattery") );  
    }
    
// ---------------------------------------------------------------------------
// CCamBatteryPaneDrawer::LoadLayoutsL
// ---------------------------------------------------------------------------    
//
void CCamBatteryPaneDrawer::LoadLayoutsL()
    {    
    PRINT( _L("Camera => CCamBatteryPaneDrawer::LoadLayoutsL") );	    	
    if ( AknLayoutUtils::PenEnabled() )
        {
        TouchLayoutL();
        }
    else
        {
        NonTouchLayoutL();
        }       
    }


// ---------------------------------------------------------------------------
// CCamBatteryPaneDrawer::DeleteIcons
// ---------------------------------------------------------------------------
//
void CCamBatteryPaneDrawer::DeleteIcons()
    {
    iBatteryIcons.ResetAndDestroy();
    }

// ---------------------------------------------------------------------------
// CCamBatteryPaneDrawer::HandleResourceChange
// ---------------------------------------------------------------------------    
//   
void CCamBatteryPaneDrawer::HandleResourceChange( TInt /* aType */ )
    {
    PRINT( _L("Camera => CCamBatteryPaneDrawer::HandleResourceChange") );	    	
    	
    // Called when either skin or color scheme has changed.
    // We need to reload icons and re-create the battery color bitmap.
    
        
    // Reload icons
    TRAP_IGNORE(LoadIconsL());
	TRAP_IGNORE(LoadLayoutsL());	
      
    PRINT( _L("Camera <= CCamBatteryPaneDrawer::HandleResourceChange") );        
    }

// ---------------------------------------------------------------------------
// CCamBatteryPaneDrawer::NonTouchLayout
// ---------------------------------------------------------------------------    
void CCamBatteryPaneDrawer::NonTouchLayoutL() 
    {
    TRect screen;
    TAknLayoutRect indicatorsPane;
    TAknLayoutRect batteryPane;
    TInt cba =  AknLayoutUtils::CbaLocation() == 
                AknLayoutUtils::EAknCbaLocationLeft? 
                AknLayoutUtils::EAknCbaLocationLeft : 0;
 
    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EScreen, screen );
    indicatorsPane.LayoutRect( screen,
        AknLayoutScalable_Apps::cam6_indi_pane( cba ));
    batteryPane.LayoutRect( indicatorsPane.Rect(),
        AknLayoutScalable_Apps::cam6_battery_pane( cba ) );
    iRect = batteryPane.Rect();
	
    TInt batteryIconsCount = iBatteryIcons.Count();
    for( TInt index = 0; index < batteryIconsCount; index++ )
        {
        iBatteryIcons[index]->SetLayoutL( iRect );
        }
    }

// ---------------------------------------------------------------------------
// CCamBatteryPaneDrawer::TouchLayoutL
// ---------------------------------------------------------------------------    
void CCamBatteryPaneDrawer::TouchLayoutL()
    {
    TRect screen;
    TAknLayoutRect indicatorsPane;
    TAknLayoutRect batteryPane;
    TInt var = Layout_Meta_Data::IsLandscapeOrientation();

    AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EScreen, screen );
    indicatorsPane.LayoutRect( screen,
        AknLayoutScalable_Apps::cam4_indicators_pane( var ).LayoutLine() );
    batteryPane.LayoutRect( indicatorsPane.Rect(),
        AknLayoutScalable_Apps::cam4_battery_pane( var ).LayoutLine() );
    iRect = batteryPane.Rect();

    TInt batteryIconsCount = iBatteryIcons.Count();
    for( TInt index = 0; index < batteryIconsCount; index++ )
        {
        iBatteryIcons[index]->SetLayoutL( iRect );
        }
    
    }

// End of file

