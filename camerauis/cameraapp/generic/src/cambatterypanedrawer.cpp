/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
*  Copyright © 2007-2008 Nokia.  All rights reserved.
*  This material, including documentation and any related computer
*  programs, is protected by copyright controlled by Nokia.  All
*  rights are reserved.  Copying, including reproducing, storing,
*  adapting or translating, any or all of this material requires the
*  prior written consent of Nokia.  This material also contains
*  confidential information which may not be disclosed to others
*  without the prior written consent of Nokia.

*
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

    // Load and resize battery icon and mask
    delete iBatteryIcon;
    iBatteryIcon = NULL;
    iBatteryIcon = CCamBitmapItem::NewL(
        EMbmCameraappQgn_prop_cam_battery_icon,
        EMbmCameraappQgn_prop_cam_battery_icon_mask );
    
    // Load and resize battery strength icon and mask    
    delete iBatteryStrengthIcon;
    iBatteryStrengthIcon = NULL;
    iBatteryStrengthIcon = CCamBitmapItem::NewL(
        EMbmCameraappQgn_indi_cam_battery_strength,
        EMbmCameraappQgn_indi_cam_battery_strength_mask );                            

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
    // Make sure that no brush is being used
    aGc.SetBrushStyle( CGraphicsContext::ENullBrush );    

    if( iBatteryStrengthIcon )
        {
        TSize iconSize = iBatteryStrengthIcon->BitmapSize();

        TRect strengthIconCropRect(
            0,
            iconSize.iHeight - BatteryStrengthIconHeight( iBatteryStrength ),
            iconSize.iWidth,
            iconSize.iHeight );

        // Draw the icon, with correct battery strength
        iBatteryStrengthIcon->DrawPartial( 
            aGc,
            iBatteryStrengthIcon->LayoutRect(),
            strengthIconCropRect );
        }

    if( iBatteryIcon )
        {
        iBatteryIcon->Draw( aGc );
        }    
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
    if ( CamUtility::IsNhdDevice() )
        {
        TouchLayoutL();
        }
    else
        {
        NonTouchLayoutL();
        }       
    }


// ---------------------------------------------------------------------------
// CCamBatteryPaneDrawer::BatteryStrengthIconHeight
// ---------------------------------------------------------------------------    
//
TInt CCamBatteryPaneDrawer::BatteryStrengthIconHeight( TInt aLevel ) const
    {
    TInt cellHeight = KCamDefaultBatteryCellHeight;
    if ( iBatteryStrengthIcon )
        {
        cellHeight = iBatteryStrengthIcon->BitmapSize().iHeight /
            KMaxBatteryStrength;
        }
    return ( aLevel * cellHeight ) + KCamDefaultBatteryCellMargin;
    }

// ---------------------------------------------------------------------------
// CCamBatteryPaneDrawer::DeleteIcons
// ---------------------------------------------------------------------------
//
void CCamBatteryPaneDrawer::DeleteIcons()
    {
    delete iBatteryIcon;
    iBatteryIcon = NULL;
    delete iBatteryStrengthIcon;
    iBatteryStrengthIcon = NULL;
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

    // Battery icon
    if ( iBatteryIcon )
        {
        iBatteryIcon->SetLayoutL( iRect,
            AknLayoutScalable_Apps::cam6_battery_pane_g1( cba ));
        }

    // Battery strength icon    
    if ( iBatteryStrengthIcon )
        {
        iBatteryStrengthIcon->SetLayoutL( iRect,
            AknLayoutScalable_Apps::cam6_battery_pane_g2( cba ));
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

    // Battery icon
    if ( iBatteryIcon )
        {
        iBatteryIcon->SetLayoutL( iRect,
            AknLayoutScalable_Apps::cam4_battery_pane_g2( var ).LayoutLine() );
        }

    // Battery strength icon    
    if ( iBatteryStrengthIcon )
        {
        iBatteryStrengthIcon->SetLayoutL( iRect,
            AknLayoutScalable_Apps::cam4_battery_pane_g1( var ).LayoutLine() );
        }
    }

// End of file

