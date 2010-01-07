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
* Description:  Controls the state of the zoom*
*/


// Defining this will reduce the wait period between repeated
// zoom in/out events being sent.  This is just a development
// test feature.

// INCLUDE FILES
#include    "CamZoomModel.h"
#include    "CamZoomPane.h"
#include    "CamPSI.h"
#include    "CamUtility.h"
#include    "CamAppUi.h"

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCamZoomModel::CCamZoomModel
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCamZoomModel::CCamZoomModel( CCamAppController& aController, 
                              CCamZoomPane*      aPane ) 
  : iCurrentResolution( KErrNotReady ),
    iController( aController ),
    iPane( aPane )
  {
  }

// -----------------------------------------------------------------------------
// CCamZoomModel::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCamZoomModel::ConstructL()
    {    
    PRINT( _L( "Camera => CCamZoomModel::ConstructL " ) );
    
    // Retrieve the max digital zoom levels
    TPckgBuf <TCamMaxZoomSteps> pckg;
    CamUtility::GetPsiAnyL( ECamPsiMaxZoomSteps, &pckg );
    iDigZoomSteps = pckg();

    // Retrieve the max EXTENDED zoom levels
    TPckgBuf <TCamMaxZoomSteps> pckg2;
    CamUtility::GetPsiAnyL( ECamPsiMaxExtendedZoomSteps, &pckg2 );
    iExtZoomSteps = pckg2();
        
    // Retrieve the max OPTICAL zoom levels
    TPckgBuf <TCamMaxZoomSteps> pckg3;
    CamUtility::GetPsiAnyL( ECamPsiMaxOpticalZoomSteps, &pckg3 );
    iOptZoomSteps = pckg3();

    // Retrieve the timing and step values for each of the zoom bar 
    // modes for Optical/Digital/Extended    
    TPckgBuf <TCamZoomLAF> pckg4;
    CamUtility::GetPsiAnyL( ECamPsiZoomBarLAF, &pckg4);
    iZoomLAF = pckg4();
    
    // Setting up initial internal values for zoom levels
    iCurZoomStepOptical = 0;
    iCurZoomStepDigital = 0;        

    // Timer used to give smooth zooming    
    iZoomTimer = CPeriodic::NewL( CActive::EPriorityHigh );          
    
    // Register for controller events
    iController.AddControllerObserverL( this );
    
    // Create Zoom Update manager, which is used to 
    // minimize the amount of times we send updates to
    // the camera driver.
    iCamZoomUpdateManager = CCamZoomUpdateManager::NewL( iController );
    
    // This class and zoom pane are created on application start.
    // On app start, we should be setting the zoom to 1x, to 
    // tidy up incase a previous app has not reset to defaults.
    ResetZoomTo1x();
    
    PRINT( _L( "Camera <= CCamZoomModel::ConstructL " ) );
    }

// -----------------------------------------------------------------------------
// CCamZoomModel::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCamZoomModel* CCamZoomModel::NewL( CCamAppController& aController, CCamZoomPane* aPane )
    {
    CCamZoomModel* self = new( ELeave ) CCamZoomModel( aController, aPane );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }


// -----------------------------------------------------------------------------
// CCamZoomModel::~CCamZoomModel
// Destructor.
// -----------------------------------------------------------------------------
//
CCamZoomModel::~CCamZoomModel()
  {
  PRINT( _L("Camera => ~CCamZoomModel" ))  
  
  iController.RemoveControllerObserver( this );
  if ( iZoomTimer && iZoomTimer->IsActive() )
    {    
    iZoomTimer->Cancel();
    }
  delete iZoomTimer;    
  
  if ( iCamZoomUpdateManager && iCamZoomUpdateManager->IsActive() )
    {    
    iCamZoomUpdateManager->Cancel();
    }
  delete iCamZoomUpdateManager;    
    
  iPane = NULL;   // Not owned
  PRINT( _L("Camera <= ~CCamZoomModel" ))  
  }

// -----------------------------------------------------------------------------
// CCamZoomModel::ZoomTo
// Attempts to zoom to a specified value.
// -----------------------------------------------------------------------------
//
void CCamZoomModel::ZoomTo( TInt aValue )
    {    
    PRINT( _L( "Camera => CCamZoomModel::ZoomTo " ) );
    // Note: The following code only supports digital or 
    // digital+extended zoom combinations.  
    // Optical zoom is not taken into consideration.
    
    // avoid mixups between button and touch presses
    if ( ZoomingState() != ECamZoomModelStateZoomNone )
        {
        PRINT( _L( "Camera <> CCamZoomModel::ZoomTo stopping keypress zoom" ) );
        StopZoom();
        }
    
    // Start the zoom state
    iState = ECamZoomModelStateZoomTo;
    
    // We don't support pausing between boundaries.  If the 
    // user experience so requires, it should be implemented here.
     
    // Update the zoom values
    // Note: Both digital and extended zoom are represented by iCurZoomStepDigital.
    // We assume the maximum value is already checked 
    // in CCamZoomPane::StartTouchZoomL
    PRINT1( _L( "Camera <> CCamZoomModel::ZoomTo iMaxZoomStepDig=%d" ), iMaxZoomStepDig ); 
    PRINT1( _L( "Camera <> CCamZoomModel::ZoomTo iMaxZoomStepExt=%d" ), iMaxZoomStepExt ); 
    PRINT1( _L( "Camera <> CCamZoomModel::ZoomTo iCurZoomStepDigital=%d" ), iCurZoomStepDigital ); 
    iCurZoomStepDigital = aValue;
    if ( iCurZoomStepDigital < iMaxZoomStepDig )
        {
        CheckZoomMode( ECamZoomModeExtended );
        }
    else
        {
        CheckZoomMode( ECamZoomModeDigital );
        }
            
    // Notify the camera driver
    PRINT1( _L( "Camera <> CCamZoomModel::ZoomTo to (aValue) %d" ), aValue );
    iCamZoomUpdateManager->SetZoomValue( iCurZoomStepDigital );
    
    // Tell the zoom pane the value to show.
    PRINT1( _L( "CCamZoomModel::ZoomTo set zoom pane %d" ), CurrentZoom() );
    iPane->SetZoomValue( CurrentZoom() );
        
    // Clear the zoom state
    iState = ECamZoomModelStateZoomNone; 
                    
    PRINT( _L( "Camera <= CCamZoomModel::ZoomTo " ) );
    }
    
// -----------------------------------------------------------------------------
// CCamZoomModel::ZoomIn
// Attempts to zoom in one step (if one more step available).
// -----------------------------------------------------------------------------
//
void CCamZoomModel::ZoomIn( TBool aOneClick )
    {    
    PRINT( _L( "Camera => CCamZoomModel::ZoomIn " ) );
    iState = ECamZoomModelStateZoomIn;
    TInt optZoomJump = 0;
    TInt digZoomJump = 0;    
    TInt extZoomJump = 0;
    
    ZoomStepsToJump( optZoomJump, digZoomJump, extZoomJump );
    PRINT3( _L( "CCamZoomModel::ZoomIn steps opt(%d) dig(%d) ext(%d) " ), optZoomJump, digZoomJump, extZoomJump );
    
    // First of all, check what the current boundary condition is
    // then check if we need to pause at that boundary.
    PRINT( _L( "ZoomIn boundary check start" ) );
    
    TCamZoomBoundary boundary = CheckBoundary();
    if ( PauseAtBoundary( boundary ) )
        {        
        if ( iPauseState == EPauseStateNone )
            {
            PRINT( _L( "ZoomIn at boundary EPauseStateNone return" ) );
            // If this is a new pause, update the state and return
            // (no zoom allowed until release)
            iPauseState = EPauseStatePaused;
            return;
            }
        else if ( iPauseState == EPauseStatePaused )
            {
            PRINT( _L( "ZoomIn at boundary EPauseStatePaused return" ) );
            // If still paused, return (no zoom allowed until release)
            return;
            }
        else // EPauseStateReleased
            {
            PRINT( _L( "ZoomIn at boundary EPauseStateReleased allow" ) );
            // If released, allow the zoom, but set the state
            // back to none.  
            iPauseState = EPauseStateNone;
            }
        }
        
    PRINT( _L( "ZoomIn at boundary EPauseStateNone boundary check end" ) );
        
    if ( optZoomJump )        
        {
        CheckZoomMode( ECamZoomModeOptical );
        iCurZoomStepOptical += optZoomJump;
        PRINT1( _L( "Camera => CCamZoomModel::ZoomIn opt to %d" ), iCurZoomStepOptical );
        //iController.SetZoomValue( iCurZoomStepOptical );            
        iCamZoomUpdateManager->SetZoomValue( iCurZoomStepOptical );
        }
        
    if ( digZoomJump )        
        {
        CheckZoomMode( ECamZoomModeDigital );
        iCurZoomStepDigital += digZoomJump;
        PRINT1( _L( "Camera => CCamZoomModel::ZoomIn dig to %d" ), iCurZoomStepDigital );
        //iController.SetZoomValue( iCurZoomStepDigital );              
        iCamZoomUpdateManager->SetZoomValue( iCurZoomStepDigital );
        }
        
    if ( extZoomJump )
        {
        CheckZoomMode( ECamZoomModeExtended );
        iCurZoomStepDigital += extZoomJump;
        PRINT1( _L( "Camera => CCamZoomModel::ZoomIn ext to %d" ), iCurZoomStepDigital );
        //iController.SetZoomValue( iCurZoomStepDigital );              
        iCamZoomUpdateManager->SetZoomValue( iCurZoomStepDigital );
        }

    // Tell the zoom pane the value to show.
    PRINT1( _L( "CCamZoomModel::ZoomIn set zoom pane %d" ), CurrentZoom() );
    iPane->SetZoomValue( CurrentZoom() );
                    
    if ( aOneClick )
        {
        // Do not start zoom timer
        PRINT( _L( "CCamZoomModel::ZoomIn one click" ) );
        iState = ECamZoomModelStateZoomNone;
        }
    else
        {
        // Start the timer to zoom-in again when timer expires
        PRINT( _L( "CCamZoomModel::ZoomIn start zoom timer" ) );
        StartZoomTimer();
        }
    
    PRINT( _L( "Camera <= CCamZoomModel::ZoomIn " ) );
    }

// ---------------------------------------------------------------------------
// CCamZoomModel::CheckBoundary
// Checks whether the next zoom operation will cross a zoom mode boundary.
// For example from Optical to Digital or from Digital to Extended
// ---------------------------------------------------------------------------
//    
CCamZoomModel::TCamZoomBoundary CCamZoomModel::CheckBoundary() const
    {
    PRINT( _L( "Camera => CCamZoomModel::CheckBoundary " ) );
    
    TCamZoomBoundary retVal = ECamZoomBoundaryNone;        
    
    TCamZoomMode modeBefore = CurrentZoomType();
    TCamZoomMode modeAfter  = CurrentZoomType( ZoomStepsToJump() );
    
    if ( modeBefore != modeAfter )
        {
        if ( modeBefore == ECamZoomModeOptical && modeAfter  == ECamZoomModeDigital ||
             modeBefore == ECamZoomModeDigital && modeAfter  == ECamZoomModeOptical )
            {
            PRINT( _L( "Camera <> CCamZoomModel::CheckBoundary ECamZoomBoundaryOptDig" ) );
            retVal = ECamZoomBoundaryOptDig;
            }
        else if ( modeBefore == ECamZoomModeExtended && modeAfter  == ECamZoomModeDigital ||
                  modeBefore == ECamZoomModeDigital && modeAfter  == ECamZoomModeExtended )
            {
            PRINT( _L( "Camera <> CCamZoomModel::CheckBoundary ECamZoomBoundaryDigExt" ) );
            retVal = ECamZoomBoundaryDigExt;
            }        
        else
            {
            // Undefined boundary, should not allow further zooming
            PRINT( _L( "Camera <> CCamZoomModel::CheckBoundary ECamZoomBoundaryUndefined" ) );
            retVal = ECamZoomBoundaryUndefined;
            }
        }
     else
        {
        PRINT( _L( "Camera <> CCamZoomModel::CheckBoundary ECamZoomBoundaryNone" ) );
        retVal = ECamZoomBoundaryNone;
        }
    
    PRINT( _L( "Camera <= CCamZoomModel::CheckBoundary " ) );
    return retVal;   
    }
    
    
// ---------------------------------------------------------------------------
// CCamZoomModel::ZoomStepsToJump
// Returns the total number of zoom steps to jump based on the current 
// zoom mode (opt/dig/ext). Does not break these steps into optical/digital
// steps to jump, as may be the case on a boundary.
// ---------------------------------------------------------------------------
//      
TInt CCamZoomModel::ZoomStepsToJump() const
    {      
    PRINT( _L( "Camera => CCamZoomModel::ZoomStepsToJump " ) );
    
    TInt steps = 0;
    
    if ( iController.ActiveCamera() == ECamActiveCameraPrimary )    
        {            
        // Works out the current zoom mode (opt/dig/ext)
        TCamZoomMode mode = CurrentZoomType();    
        PRINT1( _L( "Camera <> CCamZoomModel::ZoomStepsToJump mode = %d" ), mode );
        
        if ( mode == ECamZoomModeOptical )
            {        
            steps = iZoomLAF.iZoomStepsOpt;
            PRINT1( _L( "Camera <> CCamZoomModel::ZoomStepsToJump ECamZoomModeOptical steps = %d" ), steps );
            }
        else if ( mode == ECamZoomModeDigital )
            {
            steps = iZoomLAF.iZoomStepsDig;
            PRINT1( _L( "Camera <> CCamZoomModel::ZoomStepsToJump ECamZoomModeDigital steps = %d" ), steps );
            }
        else if ( mode == ECamZoomModeExtended )    
            {
            steps = iZoomLAF.iZoomStepsExt;
            PRINT1( _L( "Camera <> CCamZoomModel::ZoomStepsToJump ECamZoomModeExtended steps = %d" ), steps );
            }    
        else
            {
            steps = 0;            
            }   
        }
    else if ( iController.ActiveCamera() == ECamActiveCameraSecondary )
        {
        //steps = K2ndCamZoomStepSize;
        steps = iZoomLAF.iSecondCameraZoomSteps;
        PRINT1( _L( "Camera <> CCamZoomModel::ZoomStepsToJump K2ndCamZoomStepSize steps = %d" ), steps );
        }
    else
        {            
        }
        
        
    if ( iState == ECamZoomModelStateZoomOut )        
        {
        steps = -steps;    
        PRINT1( _L( "Camera <> CCamZoomModel::ZoomStepsToJump Inverse, since ZoomOut: steps = %d" ), steps );
        }
        
    PRINT( _L( "Camera <= CCamZoomModel::ZoomStepsToJump " ) );
    return steps;                           
    }

// ---------------------------------------------------------------------------
// CCamZoomModel::ZoomStepsToJump
// Returns the number of Optical, Digital and Extended zoom steps to jump
// ---------------------------------------------------------------------------
//  
void CCamZoomModel::ZoomStepsToJump( TInt& aOpt, TInt& aDig, TInt& aExt ) const
    {  
    PRINT( _L( "Camera => CCamZoomModel::ZoomStepsToJump (by reference)" ) );
    
    TInt steps = ZoomStepsToJump();

    TCamZoomBoundary boundary = CheckBoundary();
    
    // If jumping this number of steps will make us cross the boundary, 
    // we need to split the steps up into the composite opt/dig steps.
    // Depending on if we are pausing or not, we may not want to cross
    // the boundary yet
    if ( boundary == ECamZoomBoundaryOptDig )
        {
        if ( iState == ECamZoomModelStateZoomIn )
            {
            // Zooming in, then assign what we can to Optical, to get to max step
            // any remainder then goes to digital
            TInt optJumpMax = iMaxZoomStepOpt - iCurZoomStepOptical;
            aOpt = optJumpMax;
            aDig = steps - optJumpMax;
            
            // If the zoom in was going to take the digital zoom beyond the max
            // allowed, clip it
            if ( aDig > iMaxZoomStepDig )
                aDig = iMaxZoomStepDig;
            }
        else if ( iState == ECamZoomModelStateZoomOut )        
            {
            // Zooming out, then assign what we can to Digital, to get to min step
            // any remainder then goes to optical            
            aDig = -iCurZoomStepDigital;                
            aOpt = steps + iCurZoomStepDigital; // zooming out, steps is -ve
            }
		else
			{
			// Lint
			}
        }
    else if ( boundary == ECamZoomBoundaryDigExt )
        {
        if ( iState == ECamZoomModelStateZoomIn )
            {
            // Zooming in, then assign what we can to Optical, to get to max step
            // any remainder then goes to digital
            TInt digJumpMax = iMaxZoomStepDig - iCurZoomStepDigital;
            aDig = digJumpMax;
            aExt = steps - digJumpMax;
            
            // If the zoom in was going to take the digital zoom beyond the max
            // allowed, clip it
            if ( aExt > iMaxZoomStepExt )
                aExt = iMaxZoomStepExt;            
            }
        else if ( iState == ECamZoomModelStateZoomOut )        
            {
            // Zooming out, then assign what we can to Digital, to get to min step
            // any remainder then goes to optical            
            //aDig = iCurZoomStepDigital;
            aDig = steps;
            //aExt = steps - iCurZoomStepDigital;                            
            }
        else
        	{
        	// Lint
        	}
        }        
    else if ( boundary == ECamZoomBoundaryNone )
        {
        // No boundary crossed.  Can assign the steps to the current
        // mode        
        TCamZoomMode mode = CurrentZoomType();
        
        if ( iState == ECamZoomModelStateZoomIn )
            {           
            if ( mode == ECamZoomModeOptical && AllowOptZoom() )
                {
                aOpt = steps;
                if ( iCurZoomStepOptical + aOpt > iMaxZoomStepOpt )
                    {
                    aOpt = iMaxZoomStepOpt - iCurZoomStepOptical;                            
                    }                    
                }
            else if ( mode == ECamZoomModeDigital && AllowDigZoom() )
                {
                aDig = steps;    
                if ( iCurZoomStepDigital + aDig > iMaxZoomStepDig )
                    {
                    aDig = iMaxZoomStepDig - iCurZoomStepDigital;                
                    }                    
                }
            else if ( mode == ECamZoomModeExtended && AllowExtZoom() )
                {
                aExt = steps;                        
                if ( iCurZoomStepDigital + aExt > iMaxZoomStepExt )
                    {
                    aExt = iMaxZoomStepExt - iCurZoomStepDigital;    
                    }                    
                }               
            else
                {
                // Do nothing (as invalid state)
                }                                             
            }
        else if ( iState == ECamZoomModelStateZoomOut ) 
            {                
            if ( mode == ECamZoomModeOptical && AllowOptZoom() )
                {
                aOpt = steps;
                if ( iCurZoomStepOptical + aOpt < 0 )
                    {
                    aOpt = -iCurZoomStepOptical;                            
                    }                    
                }
            else if ( mode == ECamZoomModeDigital && AllowDigZoom() )
                {
                aDig = steps;    
                if ( iCurZoomStepDigital + aDig < 0 )
                    {
                    aDig = -iCurZoomStepDigital;                
                    }                    
                }
            else if ( mode == ECamZoomModeExtended && AllowExtZoom() )
                {
                aExt = steps;                        
                if ( iCurZoomStepDigital + aExt < iMaxZoomStepDig )
					{
                    aExt = iMaxZoomStepDig - iCurZoomStepDigital;
					}
                }        
            else
                {
                // Do nothing (as invalid state)
                }                        
            }
		else
			{
			// Lint
			}            
        }       
    else // ECamZoomBoundaryUndefined
        {
        // If undefined boundary, this would result in an invalid
        // zoom valid if we zoom, so set all steps to zero.        
        // This will stop any zoom operation from changing state.
        aOpt = 0;
        aDig = 0;
        aExt = 0;
        }
        
    PRINT( _L( "Camera <= CCamZoomModel::ZoomStepsToJump " ) );
    }
    
    
// ---------------------------------------------------------------------------
// CCamZoomModel::CurrentZoomType
// Returns the zoom type (opt/dig/ext) based on current zoom levels modified
// by the specified optical/digital valies
// ---------------------------------------------------------------------------
//   
TCamZoomMode CCamZoomModel::CurrentZoomType( TInt aStepModifier ) const
    {
    PRINT( _L( "Camera => CCamZoomModel::CurrentZoomType " ) );
    
    TInt maxOptZoomStep = ( iMaxZoomStepOpt == KErrNotSupported || !AllowOptZoom() )? 0 : iMaxZoomStepOpt;
    TInt maxStdZoomStep = ( iMaxZoomStepDig == KErrNotSupported || !AllowDigZoom() )? maxOptZoomStep : maxOptZoomStep + iMaxZoomStepDig;
    TInt maxExtZoomStep = ( iMaxZoomStepExt == KErrNotSupported || !AllowExtZoom() )? maxStdZoomStep : maxStdZoomStep + iMaxZoomStepExt;   
    
    // 
    TInt step = CurrentZoom() + aStepModifier;    
        
    // Optical runs from step 0 to maxOptZoomStep
    if ( step <= maxOptZoomStep && // Not at max opt zoom level        
         AllowOptZoom() )          // ... and optical zoom is allowed        
        {
        PRINT( _L( "Camera <= CCamZoomModel::CurrentZoomType ECamZoomModeOptical" ) );
        return ECamZoomModeOptical;
        }
    // Digital runs from maxOptZoomStep to maxOptZoomStep+maxStdZoomStep
    else if ( step >= maxOptZoomStep &&
              step <= maxStdZoomStep && // Inside std zoom step limits
              AllowDigZoom() )          // ...and is allowed by user setting
        {
        PRINT( _L( "Camera <= CCamZoomModel::CurrentZoomType ECamZoomModeDigital" ) );
        return ECamZoomModeDigital;
        }
    // Extended runs from maxOptZoomStep+maxStdZoomStep to maxOptZoomStep+maxStdZoomStep        
    else if ( step >= maxStdZoomStep &&
              step <= maxExtZoomStep &&   // Inside ext zoom step limits
              AllowExtZoom() )            // ...and ext zoom is allowed by user
        {
        PRINT( _L( "Camera <= CCamZoomModel::CurrentZoomType ECamZoomModeExtended" ) );
        return ECamZoomModeExtended;
        }
    else
        {
        PRINT( _L( "Camera <= CCamZoomModel::CurrentZoomType ECamZoomModeUndefined" ) );
        return ECamZoomModeUndefined;
        }                   
    }

// -----------------------------------------------------------------------------
// CCamZoomModel::ZoomOut
// Attempts to zoom out one step (if one more step available).
// -----------------------------------------------------------------------------
//
void CCamZoomModel::ZoomOut( TBool aOneClick )
    {        
    PRINT( _L( "Camera => CCamZoomModel::ZoomOut " ) );
    iState = ECamZoomModelStateZoomOut;
    TInt optZoomJump = 0;
    TInt digZoomJump = 0;    
    TInt extZoomJump = 0; 
    
    ZoomStepsToJump( optZoomJump, digZoomJump, extZoomJump );
    PRINT3( _L( "CCamZoomModel::ZoomOut steps opt(%d) dig(%d) ext(%d) " ), optZoomJump, digZoomJump, extZoomJump );
    
    if ( optZoomJump )        
        {
        CheckZoomMode( ECamZoomModeOptical );
        iCurZoomStepOptical += optZoomJump;
        PRINT1( _L( "Camera => CCamZoomModel::ZoomOut opt to %d" ), iCurZoomStepOptical );
        //iController.SetZoomValue( iCurZoomStepOptical );            
        iCamZoomUpdateManager->SetZoomValue( iCurZoomStepOptical );
        }
        
    if ( digZoomJump )        
        {
        CheckZoomMode( ECamZoomModeDigital );
        iCurZoomStepDigital += digZoomJump;
        PRINT1( _L( "Camera => CCamZoomModel::ZoomOut dig to %d" ), iCurZoomStepDigital );
        //iController.SetZoomValue( iCurZoomStepDigital );              
        iCamZoomUpdateManager->SetZoomValue( iCurZoomStepDigital );
        }
        
    if ( extZoomJump )
        {
        CheckZoomMode( ECamZoomModeExtended );
        iCurZoomStepDigital += extZoomJump;
        PRINT1( _L( "Camera => CCamZoomModel::ZoomOut ext to %d" ), iCurZoomStepDigital );
        //iController.SetZoomValue( iCurZoomStepDigital );           
        iCamZoomUpdateManager->SetZoomValue( iCurZoomStepDigital );   
        }    

    // Tell the zoom pane the value to show.
    PRINT1( _L( "ZoomOut set zoom pane %d" ), CurrentZoom() );
    iPane->SetZoomValue( CurrentZoom() );    

    if ( aOneClick )
        {
        // Do not start zoom timer
        PRINT( _L( "CCamZoomModel::ZoomIn one click" ) );
        iState = ECamZoomModelStateZoomNone;
        }
    else
        {
        // Start the timer to zoom-in again when timer expires
        PRINT( _L( "CCamZoomModel::ZoomIn start zoom timer" ) );
        StartZoomTimer();
        }
    
    PRINT( _L( "Camera <= CCamZoomModel::ZoomOut " ) );
    }

// -----------------------------------------------------------------------------
// CCamZoomModel::CheckZoomMode
// Checks to see if the zoom mode needs to be changed.
// The purpose is to avoid unneccessary calls to SetZoomMode where possible
// -----------------------------------------------------------------------------
//
void CCamZoomModel::CheckZoomMode( TCamZoomMode aMode )
  {
  PRINT( _L( "Camera => CCamZoomModel::CheckZoomMode " ) );
  if ( iZoomMode             != aMode 
    && ECamZoomModeUndefined != aMode )
    {
    //iController.SetZoomMode( aMode );
    PRINT2( _L( "Camera <> CCamZoomModel::CheckZoomMode iZoomMode changed (%d) -> (%d) " ), aMode, iZoomMode );
    iZoomMode = aMode;
    }
  PRINT( _L( "Camera <= CCamZoomModel::CheckZoomMode " ) );  
  }

// -----------------------------------------------------------------------------
// CCamZoomModel::RefreshSettings
// Checks the current state of the Camera application, and adjusts the min/max
// range of the zoom pane accordingly.
// -----------------------------------------------------------------------------
//
void CCamZoomModel::RefreshSettings()
  {
  PRINT( _L("Camera => CCamZoomModel::RefreshSettings") );
  // If there is no active camera yet, or engine is not 
  // in a prepared state yet, do not try to refresh settings
  // as we can't work out what to base it on.

  // <CAMERAAPP_CAPI_V2_MIGRATION/>
  if( iController.ActiveCamera() == ECamActiveCameraNone 
   || !( iCameraState & (ECamImageOn|ECamVideoOn) ) 
    ) 
    {
    return;
    }        

  // Cache the current resolution to be used when 
  // determining zoom limits and steps.
  ReadCurrentResolution();      

  // Read settings to update internal state        
  ReadOpticalZoomSetting();
  ReadDigitalZoomSetting();

  iMaxZoomStepOpt = MaxZoomStep( iOptZoomSteps );
  iMaxZoomStepDig = MaxZoomStep( iDigZoomSteps );
  iMaxZoomStepExt = MaxZoomStep( iExtZoomSteps );
      
  iMaxZoomStep = 0;

  PRINT3( _L("Camera <> CCamZoomModel: max steps opt:%d dig:%d ext:%d"), iMaxZoomStepOpt, iMaxZoomStepDig, iMaxZoomStepExt );

  if ( AllowOptZoom() )
    {        
    iMaxZoomStep += iMaxZoomStepOpt;        
    }
    
  // Check if Digital zoom is allowed
  if ( AllowDigZoom() )
    {                
    // Check if EXTENDED zoom is allowed by user setting
    // and the value is valid.  For secondary camera, it may
    // be that it is not supported.
    if ( AllowExtZoom() ) // done in AllowExtZoom: && iMaxZoomStepExt != KErrNotSupported)
      {                                        
      iMaxZoomStep += iMaxZoomStepExt;        
      }
    else
      {            
      iMaxZoomStep += iMaxZoomStepDig;        
      }
    }

  TInt optSteps = ( AllowOptZoom() ) ? iMaxZoomStepOpt : 0; 
  TInt stdSteps = ( AllowDigZoom() ) ? iMaxZoomStepDig : 0;
  TInt extSteps = ( AllowExtZoom() ) ? Max( (iMaxZoomStepExt - iMaxZoomStepDig), 0 ) : 0;
          
  // Check that the max zoom is within (possibly new) limits
  CheckZoomLimit();
  
  if ( iPane )
    {
    // Inform the zoom pane of the new range of zoom steps
    iPane->SetZoomRange( 0, iMaxZoomStep ); 
    iPane->SetZoomSteps( optSteps, stdSteps, extSteps );        
    iPane->SetZoomValue( CurrentZoom() );
    }

  PRINT( _L("Camera <= CCamZoomModel::RefreshSettings") );
  }
    
    
// -----------------------------------------------------------------------------
// CCamZoomModel::ReadDigitalZoomSetting
// Updates the setting for digital/extended zoom or not depending if user 
// setting for it is on AND we have a valid setting for it.
// -----------------------------------------------------------------------------
//    
void CCamZoomModel::ReadDigitalZoomSetting()
    {        
    iZoomSetting = static_cast<TCamSettingsDigitalZoom>(
        iController.IntegerSettingValue( ECamSettingItemPhotoDigitalZoom ) );    
    }



// -----------------------------------------------------------------------------
// CCamZoomModel::ReadOpticalZoomSetting
// Reads whether optical zoom is allowed
// -----------------------------------------------------------------------------
//    
void CCamZoomModel::ReadOpticalZoomSetting()
    {     
    }    
    
    
// -----------------------------------------------------------------------------
// CCamZoomModel::CheckZoomLimit
// Checks whether the current zoom level is too great for the limits.
// This can happen if the resolution is changed, as this may result in new limits
// The zoom level should be cropped to where appropriate
// -----------------------------------------------------------------------------
//      
void CCamZoomModel::CheckZoomLimit()
    {    
    TInt maxOptZoomStep = MaxZoomStep( iOptZoomSteps );
    TInt maxStdZoomStep = MaxZoomStep( iDigZoomSteps );
    TInt maxExtZoomStep = MaxZoomStep( iExtZoomSteps );

    if ( AllowOptZoom() &&                      // If settings allow optical zoom, 
         iCurZoomStepOptical > maxOptZoomStep ) // ...and value now too high
        {
        iCurZoomStepOptical = maxOptZoomStep;
        // iController.SetZoomMode ( ECamZoomModeOptical );
        iController.SetZoomValue( iCurZoomStepOptical );                                         
        }

    if ( AllowDigZoom()                        // If digital zoom is allowed
      && iCurZoomStepDigital > maxStdZoomStep  // and value now too high
      && !AllowExtZoom() )                     // and extended not allowed
        {
        // If we get here, digital zoom IS allowed, but extended isn't
        // and the digital zoom is too high.
        iCurZoomStepDigital = maxStdZoomStep;
        //iController.SetZoomMode ( ECamZoomModeDigital );
        iController.SetZoomValue( iCurZoomStepDigital );                                                        
        }
    else if ( AllowExtZoom()                         // If extended digitial zoom is allowed
           && iCurZoomStepDigital > maxExtZoomStep ) // but over the max extended range
        {        
        iCurZoomStepDigital = maxExtZoomStep;
        // iController.SetZoomMode ( ECamZoomModeDigital );
        iController.SetZoomValue( iCurZoomStepDigital );                                                                    
        }
    // otherwise, do nothing
    else
        {
        // empty statement to remove Lint error.
        }
        
    // If we have had to manually set the mode and value of zoom, 
    // make sure that the zoom mode is still set to what internal state
    // dictates it should be.    

    /*
    if ( iZoomMode == ECamZoomModeOptical )
        {
        iController.SetZoomMode( ECamZoomModeOptical );
        }
    else if ( iZoomMode == ECamZoomModeDigital ||
              iZoomMode == ECamZoomModeExtended )
        {
        iController.SetZoomMode( ECamZoomModeDigital );
        }            
    // otherwise, do nothing
    else
        {
        // empty statement to remove Lint error.
    */
    }
 
// -----------------------------------------------------------------------------
// CCamZoomModel::ResetZoomTo1x
// Resets the zoom level to 1x.
// -----------------------------------------------------------------------------
//    
void CCamZoomModel::ResetZoomTo1x()
  {
  PRINT( _L( "Camera => CCamZoomModel::ResetZoomTo1x " ) );
  // Recalculates the min/max values for the zoom pane and
  // forwards them to the control.
  RefreshSettings();        
  
  // Reset the actual values used by the engine
  iCurZoomStepOptical = 0;
  iCurZoomStepDigital = 0;
  
  iZoomMode = ECamZoomModeUndefined;
  
  // Only digital mode supported for now.        
  //iController.SetZoomMode( ECamZoomModeDigital );
  iController.SetZoomValue( iCurZoomStepDigital );                                
  if ( iPane )
    {
    iPane->SetZoomValue( 0 );
    iPane->MakeVisible( EFalse, EFalse );
    }
    
  PRINT( _L( "Camera <= CCamZoomModel::ResetZoomTo1x " ) );
  }        
        

// -----------------------------------------------------------------------------
// CCamZoomModel::MaxZoomStep
// Returns the maximum zoom step for the supplied step array 
// (optical/digital/extended), based on current camera state.
// -----------------------------------------------------------------------------
//
TInt CCamZoomModel::MaxZoomStep( const TCamMaxZoomSteps& aStepArray ) const
    {        
    PRINT( _L( "Camera => CCamZoomModel::MaxZoomStep " ) );
    
    TInt maxStep = KErrNotSupported;
    if ( iCameraState & ECamImageOn )
        {                
        // TCamPhotoSizeId  sizeId = iController.GetCurrentImageResolution();
        TCamActiveCamera camera = iController.ActiveCamera();
        if ( ECamActiveCameraPrimary == camera )
            {
            // switch ( sizeId )
            switch ( iCurrentResolution )
                {
                case ECamPhotoSizeVGA:       // 640 x 480
                    maxStep = aStepArray.iMaxPhotoStepVGA;
                    break;
                case ECamPhotoSizeXGA: 	// 0.8MegaPixel (1024 x 768 )
                    maxStep = aStepArray.iMaxPhotoStep0_8MP;
                    break;
                case ECamPhotoSize1MP:
                    maxStep = aStepArray.iMaxPhotoStep1MP;
                    break;
                case ECamPhotoSize1_3MP:     // 1280x960
                    maxStep = aStepArray.iMaxPhotoStep1_3MP;
                    break;
                case ECamPhotoSize2MP:       // 1600x1200
                    maxStep = aStepArray.iMaxPhotoStep2MP;
                    break;
                case ECamPhotoSize3MP:       // 
                    maxStep = aStepArray.iMaxPhotoStep3MP;
                    break;  
                case ECamPhotoSize5MP:
                    maxStep = aStepArray.iMaxPhotoStep5MP;
                    break;
                case ECamPhotoSize8MP:
                    maxStep = aStepArray.iMaxPhotoStep8MP;
                    break;                    
                case ECamPhotoSize12MP:
                    maxStep = aStepArray.iMaxPhotoStep12MP;
                    break;
                case ECamPhotoSizeW6MP: // 3264x1832
                    maxStep = aStepArray.iMaxPhotoStepW6MP;
                    break;                    
                case ECamPhotoSizeW9MP: // 4000x2248
                    maxStep = aStepArray.iMaxPhotoStepW9MP;
                    break;                    
                default: 
                    break;
                }                
            }
        else if ( ECamActiveCameraSecondary == camera  )
            {
            maxStep = aStepArray.iMax2ndCamPhotoStep;
            }  
        // otherwise, do nothing
        else
            {
            // empty statement to remove Lint error.
            }      
        }
    else if ( iCameraState & ECamVideoOn )
        {
        // TCamVideoResolution resId = iController.GetCurrentVideoResolution();        

        if ( iController.ActiveCamera() == ECamActiveCameraPrimary )
            {        
            // switch ( resId )
            switch ( iCurrentResolution )            
                {
                case ECamVideoResolutionSubQCIF:     // Small (128 x 96)
                    maxStep = aStepArray.iMaxVideoStepSQCIF;
                    break;
                case ECamVideoResolutionQCIF:        // Medium (176 x 144)
                    maxStep = aStepArray.iMaxVideoStepQCIF;
                    break;
                case ECamVideoResolutionCIF:         // Large (352 x 288) (Default)
                    maxStep = aStepArray.iMaxVideoStepCIF;
                    break;            
                case ECamVideoResolutionVGA:         // VGA (640 x 480)
                    maxStep = aStepArray.iMaxVideoStepVGA;            
                    break;            
                case ECamVideoResolutionQVGA:         // QVGA ( 320 x 240 )
                    maxStep = aStepArray.iMaxVideoStepQVGA;
                    break;                                        
                case ECamVideoResolutionNHD:         // NHD ( 640 x 352 )
                    maxStep = aStepArray.iMaxVideoStepNHD;
                    break;                                            
                case ECamVideoResolutionWVGA:         // WVGA ( 864 x 480 )
                    maxStep = aStepArray.iMaxVideoStepWVGA;
                    break;                                                
                case ECamVideoResolutionHD:           // HD ( 1280 x 720 )
                    maxStep = aStepArray.iMaxVideoStepHD;
                    break;                                                
                default: 
                    break;
                }
            }
        else if ( iController.ActiveCamera() == ECamActiveCameraSecondary )
            {
            switch ( iCurrentResolution )
                {
                case ECamVideoResolutionCIF:         // Large (352 x 288) (Default)
                    maxStep = aStepArray.iMax2ndCamVideoStepCIF;
                    break;
                case ECamVideoResolutionQCIF:        // Medium (176 x 144)
                    maxStep = aStepArray.iMax2ndCamVideoStepQCIF;
                    break;
                case ECamVideoResolutionSubQCIF:     // Small (128 x 96)
                    maxStep = aStepArray.iMax2ndCamVideoStepSQCIF;
                    break;
                default:
                    break;
                }                        
            }  
        // otherwise, do nothing
        else
            {
            // empty statement to remove Lint error.
            }      
        }                
    // otherwise, do nothing
    else
        {
        // empty statement to remove Lint error.
        }

    PRINT1( _L( "Camera <= CCamZoomModel::MaxZoomStep maxStep=%d" ), maxStep );
    return maxStep;
    }


    

// -----------------------------------------------------------------------------
// CCamZoomModel::StopZoom
// Called when the user releases the zoom key to stop an ongoing zoom operation.
// -----------------------------------------------------------------------------
//
void CCamZoomModel::StopZoom()
    {    
    PRINT( _L( "Camera => CCamZoomModel::StopZoom " ) );
    
    if ( iZoomTimer->IsActive() )
        {
        iZoomTimer->Cancel();
        }
        
    // Clear the zoom state
    iState = ECamZoomModelStateZoomNone;     
    
    if ( iPauseState == EPauseStatePaused )
        {
        iPauseState = EPauseStateReleased;
        }
    else
        {
        iPauseState = EPauseStateNone;
        }
        
    PRINT( _L( "Camera <= CCamZoomModel::StopZoom " ) );
    }

// -----------------------------------------------------------------------------
// CCamZoomModel::HandleControllerEventL
// Called when a controller event occurs
// -----------------------------------------------------------------------------
//
void CCamZoomModel::HandleControllerEventL( TCamControllerEvent aEvent, 
                                            TInt /*aError*/ )
  {
  PRINT( _L( "Camera => CCamZoomModel::HandleControllerEventL " ) );
  switch( aEvent )
    {
    // -----------------------------------------
    case ECamEventEngineStateChanged:
      {
      iCameraState = iController.CameraControllerState();  
      
      // If a reset is waiting. This happens if the application 'pretends to exit' 
      // but this occurred when the engine was not prepared e.g. in standby.
      // The zoom needs to be set back to default when prepared, and the engine has
      // now entered a prepared state
      if ( iResetPending 
        && ( iCameraState & (ECamImageOn|ECamVideoOn) ) )
          {
          // The zoom pane will not be redrawn while the iResetPending flag is on.
          // This calls RefreshSettings()
          ResetZoomTo1x();
          // In this situation the zoom pane should not be shown.
          /*if ( iPane->IsVisible() )
              {
              iPane->MakeVisible( EFalse, ETrue );
              }*/
          iResetPending = EFalse;                
          }     
      else
          {
          // If state changes (video <-> photo) refresh settings
          RefreshSettings();
          }
      }
      break;
    // -----------------------------------------
    case ECamEventCameraChanged:
      {        
      // If state changes (camera change) refresh settings
      RefreshSettings();
      
      ResetZoomTo1x();
      }       
      break;
    // -----------------------------------------
    // Zoom settings now involve basic digital zoom too.
    // Event name changed ECamEventExtZoomStateChanged => ECamEventZoomSetupChanged
    case ECamEventImageQualityChanged:
    case ECamEventZoomSetupChanged:    // Digital/Extended zoom turned on/off/paused/continuous
      {
      // If image quality has changed, this may update zoom levels.
      RefreshSettings();
      }
      break;
    // -----------------------------------------
    case ECamEventOperationStateChanged:
      {
      // As the iRecordingVideo state has changed, need to update
      // the settings, as this may disable the optical zoom 
      RefreshSettings();                                            
      }
      break;
    // -----------------------------------------
    // otherwise, do nothing
    default:
      break;
    } // switch
    
  PRINT( _L( "Camera <= CCamZoomModel::HandleControllerEventL " ) );
  }

// ---------------------------------------------------------------------------
// CCamZoomModel::ZoomTimerCallback
// Callback for Zoom Pane timer, used to give smooth zooming
// ---------------------------------------------------------------------------
//
TInt CCamZoomModel::ZoomTimerCallback( TAny* aObject )
    {    
    CCamZoomModel* model = static_cast< CCamZoomModel* >( aObject );
    model->ZoomTimerTick();
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CCamZoomModel::ZoomTimerTick
// Called each timer period while the zoom in/out key is held down.
// ---------------------------------------------------------------------------
//
void CCamZoomModel::ZoomTimerTick()
    {        
    PRINT( _L("Camera => CCamZoomModel::ZoomTimerTick") );
    TCamZoomMode modeBefore = iZoomMode;
                   
    // Continue the ongoing zoom operation
    if ( iState == ECamZoomModelStateZoomIn )
        {
        ZoomIn();
        }
    else if ( iState == ECamZoomModelStateZoomOut )
        {
        ZoomOut();
        }    
    else
        {
        // Do nothing
        }               
    
    // Check if we're currently on a boundary, in which case we need 
    // to stop and re-initialise the timer for the (potentially) new
    // interval.                            
    TCamZoomMode modeAfter = iZoomMode;
    
    if ( modeBefore != modeAfter )
        {
        // Restart zoom;
        if ( iZoomTimer->IsActive() )
            {
            iZoomTimer->Cancel();
            }
        StartZoomTimer();
        }   
    PRINT( _L("Camera <= CCamZoomModel::ZoomTimerTick") );
    }        



// ---------------------------------------------------------------------------
// CCamZoomModel::StartZoomTimer
// Called each timer period while the zoom in/out key is held down.
// ---------------------------------------------------------------------------
//
void CCamZoomModel::StartZoomTimer()
    {
    PRINT( _L( "Camera => CCamZoomModel::StartZoomTimer " ) );
    
    if ( iZoomTimer->IsActive() )
        {
        return; // If only one speed, we're already running so just return.        
        }
          
    TCamZoomMode mode = CurrentZoomType();                
    
    TInt stepPeriod = 0;
    
    switch ( mode )
        {            
        case ECamZoomModeOptical:
            {
            stepPeriod = iZoomLAF.iZoomSpeedOpt * 1000;
            break;        
            }
        
        case ECamZoomModeDigital:
            {
            stepPeriod = iZoomLAF.iZoomSpeedDig * 1000;
            break;        
            }
        
        case ECamZoomModeExtended:
            {
            stepPeriod = iZoomLAF.iZoomSpeedExt * 1000;
            break;        
            }
                    
        case ECamZoomModeUndefined:
        default:
            {
            stepPeriod = 1000000;
            break;        
            }        
        }
    
    
    iZoomTimer->Start( stepPeriod, stepPeriod, TCallBack( ZoomTimerCallback , this) );
    iZoomMode = mode;
    
    PRINT( _L( "Camera <= CCamZoomModel::StartZoomTimer " ) );
    }

// ---------------------------------------------------------------------------
// CCamZoomModel::AllowExtZoom
// Returns whether extended zoom is allowed or not, based on current settings
// ---------------------------------------------------------------------------
//
TBool CCamZoomModel::AllowExtZoom() const
    {
    PRINT( _L( "Camera => CCamZoomModel::AllowExtZoom " ) );
    
    TInt extendedStep = MaxZoomStep( iExtZoomSteps );    
    
    // Check there is a valid extended zoom step to allow to first, and that
    // we are not in embedded mode (extended zoom not allowed in embedded)
    if (  KErrNotSupported != extendedStep )
        {
        if ( iZoomSetting == ECamSettDigZoomExtendPause
          || iZoomSetting == ECamSettDigZoomExtendCont )
            {
            // If setting allows extended zoom, return true
            PRINT( _L( "Camera <= CCamZoomModel::AllowExtZoom ETrue" ) );
            return ETrue;
            }
        else
            {
            // else user setting disallows it, so return false
            PRINT( _L( "Camera <= CCamZoomModel::AllowExtZoom EFalse" ) );
            return EFalse;
            }
        }
    else
        {
        // No valid extended zoom, so not allowed even if user said so.
        PRINT( _L( "Camera <= CCamZoomModel::AllowExtZoom EFalse" ) );
        return EFalse;
        }
    }
    
// ---------------------------------------------------------------------------
// CCamZoomModel::AllowOptZoom
// Returns whether optical zoom is allowed
// ---------------------------------------------------------------------------
// 
TBool CCamZoomModel::AllowOptZoom() const
    {        
    PRINT( _L( "Camera => CCamZoomModel::AllowOptZoom " ) ); 
    TInt maxOptZoomStep = MaxZoomStep( iOptZoomSteps );
    if ( KErrNotSupported == maxOptZoomStep // Zoom is NOT supported or
      || ( !iAllowOptZoom                   // Settings dont allow zoom during recording 
        && iRecordingVideo ) )              // ...and we're recording now
        {
        PRINT( _L( "Camera <= CCamZoomModel::AllowOptZoom EFalse" ) );
        return EFalse;   
        }
    else
        {
        PRINT( _L( "Camera <= CCamZoomModel::AllowOptZoom ETrue" ) );
        return ETrue;                 
        }      
    }

// ---------------------------------------------------------------------------
// CCamZoomModel::AllowDigZoom
// Returns whether digital zoom is allowed
// ---------------------------------------------------------------------------
//
TBool CCamZoomModel::AllowDigZoom() const
    {         
    PRINT( _L( "Camera => CCamZoomModel::AllowDigZoom " ) );
    TInt maxStdZoomStep = MaxZoomStep( iDigZoomSteps );

    if ( KErrNotSupported    != maxStdZoomStep   // Zoom is supported in current state         
      && ECamSettDigZoomNone != iZoomSetting   ) // ...and is allowed by user setting
        {
        PRINT( _L( "Camera <= CCamZoomModel::AllowDigZoom ETrue" ) );
        return ETrue;         
        }
    else
        {
        PRINT( _L( "Camera <= CCamZoomModel::AllowDigZoom ETrue" ) );
        return EFalse;   
        }      
    }


// ---------------------------------------------------------------------------
// CCamZoomModel::CurrentZoom
// Returns the current zoom level, taking into account optical (if allowed)
// *and* digital zooms.
// ---------------------------------------------------------------------------
//
TInt CCamZoomModel::CurrentZoom() const
    {
    PRINT( _L( "Camera => CCamZoomModel::CurrentZoom" ) );
    // If we are recording video, and optical isn't allowed while recording
    // then we only need to represent the Digital zoom in the zoom pane.    
    if ( iRecordingVideo && !iAllowOptZoom )
        {
        // Only use digital value if optical not allowed
        PRINT( _L( "Camera <= CCamZoomModel::CurrentZoom digital" ) );
        return iCurZoomStepDigital;
        }
    else
        {
        // Return the combined zoom value
        PRINT( _L( "Camera <= CCamZoomModel::CurrentZoom optical+digital" ) );
        return (iCurZoomStepOptical + iCurZoomStepDigital);
        }        
    }


// ---------------------------------------------------------------------------
// CCamZoomModel::PauseAtBoundary
// Returns whether to pause at the specified boundary
// ---------------------------------------------------------------------------
//
TBool CCamZoomModel::PauseAtBoundary( TCamZoomBoundary aBoundary ) const
  {
  PRINT( _L( "Camera => CCamZoomModel::PauseAtBoundary" ) );
  if ( iCameraState & ECamVideoOn )
  	{
    // Always return false for video, as we do not want to 
    // force the zoom operation to pause at any boundary
    // in video mode. 
    PRINT( _L( "Camera <= CCamZoomModel::PauseAtBoundary EFalse" ) );  
    return EFalse;
    }
  else
  	{
  	if ( ECamZoomBoundaryOptDig == aBoundary )
      {
      // Reason to *not* pause at the optical/digital boundary is if
      // user set the "On (continuous)" or "Extended on (continuous) setting
      if ( ECamSettDigZoomNormalCont == iZoomSetting
        || ECamSettDigZoomExtendCont == iZoomSetting )
        {
        PRINT( _L( "Camera <= CCamZoomModel::PauseAtBoundary EFalse" ) );  
        return EFalse;            
        }
      else 
        {
        PRINT( _L( "Camera <= CCamZoomModel::PauseAtBoundary ETrue" ) );  
        return ETrue;
        }
      }
    else if ( ECamZoomBoundaryDigExt == aBoundary ) 
      {
      // Only reason *not* to pause in digital/extended boundary is if
      // user zoom setting states "extended on without pauses".
      if ( ECamSettDigZoomExtendCont == iZoomSetting )
        {
        PRINT( _L( "Camera <= CCamZoomModel::PauseAtBoundary EFalse" ) );  
        return EFalse;
        }
      else
        {
        PRINT( _L( "Camera <= CCamZoomModel::PauseAtBoundary ETrue" ) );  
        return ETrue;    
        }
      }
    else // No other boundaries known, return "no pause".
      {
      PRINT( _L( "Camera <= CCamZoomModel::PauseAtBoundary EFalse" ) );  
      return EFalse;
      }     
    }	
  }
  
    
// -----------------------------------------------------------------------------
// CCamZoomPane::IsCurrentlyZooming
// Returns ETrue if the zoom model is currently zooming in/out,
// else returns EFalse
// -----------------------------------------------------------------------------
//
TBool CCamZoomModel::IsCurrentlyZooming() const
    {    
    PRINT( _L( "Camera => CCamZoomModel::IsCurrentlyZooming" ) );
    if ( ECamZoomModelStateZoomIn  == iState 
      || ECamZoomModelStateZoomOut == iState )
        {
        PRINT( _L( "Camera <= CCamZoomModel::IsCurrentlyZooming ETrue" ) );
        return ETrue;
        }
    else
        {
        PRINT( _L( "Camera <= CCamZoomModel::IsCurrentlyZooming EFalse" ) );
        return EFalse;
        }
    }
    
// -----------------------------------------------------------------------------
// CCamZoomModel::ZoomingState
// -----------------------------------------------------------------------------
//
CCamZoomModel::TCamZoomModelState CCamZoomModel::ZoomingState()
    {    
    return iState;  
    }
 

// -----------------------------------------------------------------------------
// CCamZoomPane::ResetToDefaultAfterPrepare
// Sets the zoompane to reset the zoom level to default values
// next time the engine is prepared
// -----------------------------------------------------------------------------
//
void CCamZoomModel::ResetToDefaultAfterPrepare( TBool aReset )
    {
    iResetPending = aReset;
    }     

// -----------------------------------------------------------------------------
// CCamZoomPane::IsResetPending
// Whether or not the zoom level is waiting to be reset to default
// -----------------------------------------------------------------------------
//
TBool CCamZoomModel::IsResetPending() const
    {
    return iResetPending;
    }

// -----------------------------------------------------------------------------
// ReadCurrentResolution
// -----------------------------------------------------------------------------
//
void 
CCamZoomModel::ReadCurrentResolution()
  {
  PRINT1( _L("Camera => CCamZoomModel::ReadCurrentResolution, now:%d"), iCurrentResolution );
  if ( iCameraState & ECamImageOn )
    {                
    iCurrentResolution = iController.GetCurrentImageResolution();
    }
  else if( iCameraState & ECamVideoOn )
    {
    iCurrentResolution = iController.GetCurrentVideoResolution();
    }
  else
    {
    iCurrentResolution = KErrNotReady;
    }
  PRINT1( _L("Camera <= CCamZoomModel::ReadCurrentResolution, got:%d"), iCurrentResolution );
  }

//  End of File  
