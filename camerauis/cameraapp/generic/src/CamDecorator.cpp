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
* Description:  Camera UI decorator*
*/


// INCLUDE FILES
#include <gdi.h>
#include <eikenv.h>
#include <barsread.h>
#include <AknsSkinInstance.h>
#include <AknsUtils.h>
#include <AknsDrawUtils.h>
#include <layoutmetadata.cdl.h>
#include <aknlayoutscalable_apps.cdl.h>

#include "Cam.hrh"
#include "CamAppController.h"
#include "CamAppUi.h"
#include "CamObservable.h"
#include "CamRecordingIconDecoration.h"
#include "CamDecorator.h"
#include "camlogging.h"
#include "CameraUiConfigManager.h"
#include "CamUtility.h" 

// -----------------------------------------------------------------------------
// CCamDecorator::CCamDecorator()
// Default constructor
// -----------------------------------------------------------------------------
//
CCamDecorator::CCamDecorator( CCamAppController& aController )
  : iController( aController )
  {
  }

// -----------------------------------------------------------------------------
// CCamDecorator::NewL
// Factory method to safely create a new instance
// -----------------------------------------------------------------------------
//
CCamDecorator* 
CCamDecorator::NewL( CCamAppController& aController,
                     TInt               aResourceId, 
                     MCamObserver*      aObserver /*=NULL*/ )
  {
  CCamDecorator* me = 
      new (ELeave) CCamDecorator( aController );

  CleanupStack::PushL( me );
  me->ConstructL( aResourceId, aObserver );
  CleanupStack::Pop( me );

  return me;
  }

// -----------------------------------------------------------------------------
// CCamDecorator::ConstructL
// Initializes a new instance
// -----------------------------------------------------------------------------
//
void CCamDecorator::ConstructL(TInt aResourceId, MCamObserver* aObserver)
    {
    if ( aResourceId )
        {
        iPossibleObserver = aObserver;
        
        TResourceReader reader;
        CEikonEnv::Static()->CreateResourceReaderLC( reader, aResourceId ); 

        TInt numDecorations = reader.ReadInt16();
        
        TInt currentDecoration = 0;
        TScreenDecorationType decorationType;
        for ( currentDecoration = 0; currentDecoration < numDecorations; currentDecoration++ )
            {
            decorationType = static_cast<TScreenDecorationType>(reader.ReadInt16());
            
            switch(decorationType)
                {
            case ECamScreenDecorationBlendedBackground:
                CreateBlendedBackgroundL(reader);
                break;
                
            case ECamScreenDecorationDVFBlanker:
                CreateDVFBlankerL(reader);
                break;
                
            case ECamScreenDecorationRecordingIcon:
                CreateRecordingIconL(reader);
                break;
                
            default:
                break;
                }
            }
            
        CleanupStack::PopAndDestroy();
        
        }
    else
        {
        iDrawNothing = ETrue;
        }
    }

// -----------------------------------------------------------------------------
// CCamDecorator::~CCamDecorator()
// Destructor
// -----------------------------------------------------------------------------
//
CCamDecorator::~CCamDecorator()
  {
  PRINT( _L("Camera => ~CCamDecorator") );
  iDVFBlankers.Reset();
  iRecordingIcons.ResetAndDestroy();
  PRINT( _L("Camera <= ~CCamDecorator") );
  }

// -----------------------------------------------------------------------------
// CCamDecorator::Draw
// Draws all the decorations
// -----------------------------------------------------------------------------
//
void CCamDecorator::Draw( CBitmapContext& aGc, const TRect& aDrawRect, const TBool aForceBlankers )
    {
    if ( !iDrawNothing )
        {
        TInt element = 0;
        TInt count = 0;

        // Draw the blankers
        count = iDVFBlankers.Count();
        
        if ( count > 0 )
            {
            TRgb blankerColour( KRgbWhite );     
            CCamAppUi* appUi = static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() );
            if ( appUi && appUi->IsDirectViewfinderActive())
                {
                blankerColour = KRgbWhite;
                
                aGc.SetPenStyle( CGraphicsContext::ENullPen );
                aGc.SetBrushColor( blankerColour );
                aGc.SetBrushStyle( CGraphicsContext::ESolidBrush );
                    
                for ( element = 0; element < count; element++ )
                    {
                    aGc.DrawRect( iDVFBlankers[element] );
                    }
                    
                aGc.Reset();                
                }
            }
    
        if ( aForceBlankers )
            {
            count = iDVFBlankers.Count();
            if ( count > 0 )
                {
                MAknsSkinInstance* skin = AknsUtils::SkinInstance();
                CWindowGc& gc = CCoeEnv::Static()->SystemGc();
                for ( element = 0; element < count; element++ )
                    {
                    AknsDrawUtils::Background( skin, NULL, gc, iDVFBlankers[element] );
                    }
                }
            }
           
        // Draw the recording icons
        count = iRecordingIcons.Count();
        for ( element = 0; element < count; element++ )
            {
            iRecordingIcons[element]->Draw( aGc, aDrawRect );
            }
        }
    }
    
// -----------------------------------------------------------------------------
// CCamDecorator::CreateBlendedBackgroundL
// Create a new blended background decoration based on resource
// -----------------------------------------------------------------------------
//
void CCamDecorator::CreateBlendedBackgroundL(TResourceReader& aResourceReader)
    {
    PRINT( _L("Camera => CCamDecorator::CreateBlendedBackgroundL") );	
    aResourceReader.ReadInt16(); // tl_x
    aResourceReader.ReadInt16(); // tl_y
    aResourceReader.ReadInt16(); // width

    /*
    CBlendedBackground* blendedBackground = CBlendedBackground::NewL( TPoint(tl_x, tl_y), width, KBlendedBackgroundDisplayMode );

    CleanupStack::PushL( blendedBackground );
    
    User::LeaveIfError(iBlendedBackgrounds.Append(blendedBackground));

    CleanupStack::Pop( blendedBackground );
    */

    PRINT( _L("Camera <= CCamDecorator::CreateBlendedBackgroundL") );	
    }

// -----------------------------------------------------------------------------
// CCamDecorator::CreateDVFBlankerL
// Create a new dvf blanker decoration based on resource
// -----------------------------------------------------------------------------
//
void CCamDecorator::CreateDVFBlankerL(TResourceReader& aResourceReader)
    {
    PRINT( _L("Camera => CCamDecorator::CreateDVFBlankerL") );		
    TInt tl_x = aResourceReader.ReadInt16();
    TInt tl_y = aResourceReader.ReadInt16();
    TInt width = aResourceReader.ReadInt16();
    TInt height = aResourceReader.ReadInt16();

    TRect blanker(TPoint(tl_x, tl_y), TSize(width, height));
    
    User::LeaveIfError(iDVFBlankers.Append(blanker));
    PRINT( _L("Camera <= CCamDecorator::CreateDVFBlankerL") );		
    }

// -----------------------------------------------------------------------------
// CCamDecorator::CreateRecordingIconL
// Create a new recording icon decoration based on resource
// -----------------------------------------------------------------------------
//
void CCamDecorator::CreateRecordingIconL(TResourceReader& /* aResourceReader */)  
    {
    PRINT( _L("Camera => CCamDecorator::CreateRecordingIconL") );	
    
    TRect screenRect;
    if ( Layout_Meta_Data::IsLandscapeOrientation() || CamUtility::IsNhdDevice() )
        {
        AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EScreen, screenRect );
        }
    else
        {
        AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, screenRect );
        }
    
    TAknLayoutRect progressPaneLayout;
    TAknLayoutRect recIconLayout;
    TInt vari = Layout_Meta_Data::IsLandscapeOrientation();
    if ( CamUtility::IsNhdDevice() )
        {
         progressPaneLayout.LayoutRect( screenRect,
             AknLayoutScalable_Apps::vid4_progress_pane( vari ) );
            
         recIconLayout.LayoutRect( progressPaneLayout.Rect(),
             AknLayoutScalable_Apps::vid4_progress_pane_g3( !vari ) ); 
        }
    else
        {
        if ( Layout_Meta_Data::IsLandscapeOrientation() )
            {
            progressPaneLayout.LayoutRect( screenRect,
                AknLayoutScalable_Apps::vid6_indi_pane( !vari ) );
            recIconLayout.LayoutRect( progressPaneLayout.Rect(),
                AknLayoutScalable_Apps::vid6_indi_pane_g5( vari ) );

            }
        else
            {
            progressPaneLayout.LayoutRect( screenRect,
                AknLayoutScalable_Apps::vid6_indi_pane( 3 ) );
            recIconLayout.LayoutRect( progressPaneLayout.Rect(),
                AknLayoutScalable_Apps::vid6_indi_pane_g5( 2 ) );
            }
        }
        
    TRect location = recIconLayout.Rect();  
    
    CCamRecordingIconDecoration* recIcon = 
        CCamRecordingIconDecoration::NewL( location, iController );
    
    CleanupStack::PushL( recIcon );

    if ( iPossibleObserver )
        {
        recIcon->RegisterObserverL(iPossibleObserver);
        }
        
    User::LeaveIfError(iRecordingIcons.Append(recIcon));

    CleanupStack::Pop( recIcon );
    PRINT( _L("Camera <= CCamDecorator::CreateRecordingIconL") );			
    }

// End of File
