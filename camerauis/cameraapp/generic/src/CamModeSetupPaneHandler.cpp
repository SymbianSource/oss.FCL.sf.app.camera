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
* Description:  Implementation of the mode setup pane handler class.
*
*/


#include <coemain.h>
#include <barsRead.h>
#include <stringloader.h>
#include "CamModeSetupPaneHandler.h"
#include "CamSetupPaneItemArray.h"
#include "CamSetupPane.h"
#include "CamSetupPaneItem.h"
#include "CamPreCaptureContainerBase.h"
#include "CamAppUi.h"
#include <cameraapp.rsg>
#include <vgacamsettings.rsg>

// -----------------------------------------------------------------------------
// CCamModeSetupPaneHandler::NewLC
// Part of the two-phase class construction.
// -----------------------------------------------------------------------------
//
CCamModeSetupPaneHandler* CCamModeSetupPaneHandler::NewLC( CCamAppController& aController )
	{
	CCamModeSetupPaneHandler* self = new( ELeave ) CCamModeSetupPaneHandler(aController);
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
	}


// -----------------------------------------------------------------------------
// CCamModeSetupPaneHandler::NewL
// Part of the two-phase class construction.
// -----------------------------------------------------------------------------
//
CCamModeSetupPaneHandler* CCamModeSetupPaneHandler::NewL( CCamAppController& aController )
	{
	CCamModeSetupPaneHandler* self = CCamModeSetupPaneHandler::NewLC( aController );
	CleanupStack::Pop(self);
	return self;
	}


// -----------------------------------------------------------------------------
// CCamModeSetupPaneHandler::CCamModeSetupPaneHandler
// C++ class constructor.
// -----------------------------------------------------------------------------
//
CCamModeSetupPaneHandler::CCamModeSetupPaneHandler( CCamAppController& aController )
 : iController( aController ),
   iPaneLevel( EMenuLevel )
	{
	}


// -----------------------------------------------------------------------------
// CCamModeSetupPaneHandler::ConstructL
// Carries out two-phase class construction.
// -----------------------------------------------------------------------------
//
void CCamModeSetupPaneHandler::ConstructL()
	{
    // Setup helper context    
    iLastCaptureHelper = iController.LastCaptureHelper();
    iLastCaptureHelper->SetPaneModel( this ); // register

	iCurrentSettingsArrayIndex = KErrNotFound; // Invalidate.		
	}


// -----------------------------------------------------------------------------
// CCamModeSetupPaneHandler::~CCamModeSetupPaneHandler
// C++ destructor.
// -----------------------------------------------------------------------------
//
CCamModeSetupPaneHandler::~CCamModeSetupPaneHandler()
  {
  PRINT( _L("Camera => ~CCamModeSetupPaneHandler") );
  ResetArray();
  
  iPaneControl = NULL; // Not owned.	
  
  if ( iLastCaptureHelper )
    {
    iLastCaptureHelper->SetPaneModel( NULL ); // de-register
    iLastCaptureHelper = NULL; // Not owned    
    }
  
  delete iLastCapTitle;
  PRINT( _L("Camera <= ~CCamModeSetupPaneHandler") );
  }


// -----------------------------------------------------------------------------
// CCamModeSetupPaneHandler::ResetArray
// Deletes the contents of the settings item array
// -----------------------------------------------------------------------------
//
void CCamModeSetupPaneHandler::ResetArray()
    {
   	TInt i;
	for ( i = 0; i < iNoOfSettingsItems; i++ )
		{
		delete iSettingItems[i];
		iSettingItems[i] = NULL;
		}
    }

// -----------------------------------------------------------------------------
// CCamModeSetupPaneHandler::HandleEvent
// 
// -----------------------------------------------------------------------------
//
void CCamModeSetupPaneHandler::HandleEvent( TEvent aEvent )
    {
    switch ( aEvent )
        {
        case EEventSelect:
            {
    		// ASSERT(iPaneLevel == EItemLevel); // Should only get this event for item-level selections.
    		// Eventually need to handle commands from captureViewBase to open menu items.
    		// Let OK button open them for now...
    		if ( iPaneLevel == EItemLevel )
    			{
    			HandlePaneItemLevelSelection();
    			}
    		else
    			{
    			if ( iPaneLevel == EMenuLevel &&
    			     IconIsThumbnail( iHighlightedItemIndex ) )
    			    {
    			    HandleLastCaptureSelection();    
    			    }
    			else
    			    {
    			    HandlePaneMenuLevelSelection( iController.IntegerSettingValue( HighlightedItemId() ) );    
    			    }    		
    			}            
            break;            
            }
        case EEventCancel:
            {
            HandlePaneCancel();
            break;
            }
        case EEventLeft:
            {
            MoveToPreviousItem();
            break;
            }
        case EEventRight:
            {
            MoveToNextItem();
            break;
            }
        case EEventEditUser:
            {        
            SwitchToUserSceneView();
            break;
            }            
        default:
            {
            break;
            }
            
        }
    }


// -----------------------------------------------------------------------------
// CCamModeSetupPaneHandler::MoveToPreviousItem
// Changes the highlighted item to the previous one.
// -----------------------------------------------------------------------------
//
void CCamModeSetupPaneHandler::MoveToPreviousItem()
	{
	iPaneControl->ScrollToPreviousItem();
	iHighlightedItemIndex--;
	if ( iHighlightedItemIndex < 0 )
		{
		iHighlightedItemIndex = NumberOfItems() - 1;
		}
	HandlePaneItemChanged();
	}


// -----------------------------------------------------------------------------
// CCamModeSetupPaneHandler::MoveToNextItem
// Changes the highlighted item to the next one.
// -----------------------------------------------------------------------------
//
void CCamModeSetupPaneHandler::MoveToNextItem()
	{
	iPaneControl->ScrollToNextItem();
	iHighlightedItemIndex++;
	if ( iHighlightedItemIndex >= NumberOfItems() )
		{
		iHighlightedItemIndex = 0;
		}
	HandlePaneItemChanged();
	}


// -----------------------------------------------------------------------------
// CCamModeSetupPaneHandler::HighlightedItemId
// Returns the id of the currently highlighted item.
// -----------------------------------------------------------------------------
//
TInt CCamModeSetupPaneHandler::HighlightedItemId() const
	{
	if ( iPaneLevel == EMenuLevel )
		{
		return iMenuItemIds[iHighlightedItemIndex];
		}
	else
		{
		ASSERT( iPaneLevel == EItemLevel );
		ASSERT( iCurrentSettingsArrayIndex != KErrNotFound );
		return iSettingItems[iCurrentSettingsArrayIndex]->At( HighlightedItemIndex() )->Id();
		}
	}


// -----------------------------------------------------------------------------
// CCamModeSetupPaneHandler::NumberOfItems
// Returns the number of items in the current pane model.
// -----------------------------------------------------------------------------
//
TInt CCamModeSetupPaneHandler::NumberOfItems() const
	{
	if ( iPaneLevel == EMenuLevel )
		{		
		TInt items = iNoOfSettingsItems; 
		// Check if there is a "last capture" dynamic item to add
		if ( iLastCaptureHelper->IsThumbValid() )
		    {
		    items ++; // +1 for dynamic 'last picture taken' item
		    }
		return items;		    
		}
	else
		{
		ASSERT( iPaneLevel == EItemLevel );
		ASSERT( iCurrentSettingsArrayIndex != KErrNotFound );
		return iSettingItems[iCurrentSettingsArrayIndex]->Count();
		}
	}


// -----------------------------------------------------------------------------
// CCamModeSetupPaneHandler::TitleText
// Returns the current pane title text (for the highlighted pane item).
// -----------------------------------------------------------------------------
//
const TDesC& CCamModeSetupPaneHandler::TitleText() const
	{
	if ( iPaneLevel == EMenuLevel )
		{
		return iMenuTitles[iHighlightedItemIndex];
		}
	else
		{
		ASSERT( iPaneLevel == EItemLevel );
		ASSERT( iCurrentSettingsArrayIndex != KErrNotFound );
		return iMenuTitles[iCurrentSettingsArrayIndex];
		}
	}


// -----------------------------------------------------------------------------
// CCamModeSetupPaneHandler::DescriptorText
// Returns the current pane descriptor text (for the highlighted pane item).
// -----------------------------------------------------------------------------
//
const TDesC& CCamModeSetupPaneHandler::DescriptorText() const
	{
	if ( iPaneLevel == EMenuLevel )
		{
		if ( iHighlightedItemIndex < iNoOfSettingsItems )
			{
			const TInt settingVal = iController.IntegerSettingValue( HighlightedItemId() );
			const TInt index = iSettingItems[iHighlightedItemIndex]->ItemIndexFromItemId( settingVal );
			if ( index != KErrNotFound )
				{
				return iSettingItems[iHighlightedItemIndex]->At( index )->Text();
				}
			}
		else
			{
			ASSERT( iHighlightedItemIndex == iNoOfSettingsItems );
			
			// Return filename of last taken picture.
			return iLastCapName;
			}
		}
	else
		{
		ASSERT( iPaneLevel == EItemLevel );
		ASSERT( iCurrentSettingsArrayIndex != KErrNotFound );
		return iSettingItems[iCurrentSettingsArrayIndex]->At( iHighlightedItemIndex )->Text();
		}

	return KNullDesC; // All else failed.
	}


// -----------------------------------------------------------------------------
// CCamModeSetupPaneHandler::Icon
// Returns the icon for the given pane item index.
// -----------------------------------------------------------------------------
//
CGulIcon& CCamModeSetupPaneHandler::Icon( TInt aItemIndex )
	{
	ASSERT( aItemIndex >= 0  &&  aItemIndex < NumberOfItems() );

	if ( iPaneLevel == EMenuLevel )
		{
		if ( aItemIndex < iNoOfSettingsItems )
			{
			const TInt settingVal = iController.IntegerSettingValue( iMenuItemIds[aItemIndex] );
			const TInt index = iSettingItems[aItemIndex]->ItemIndexFromItemId( settingVal );
			if ( index != KErrNotFound )
				{
				return iSettingItems[aItemIndex]->At( index )->Icon();
				}			
			}
		}
	else
		{
		ASSERT( iPaneLevel == EItemLevel );
		ASSERT( iCurrentSettingsArrayIndex != KErrNotFound );
		return iSettingItems[iCurrentSettingsArrayIndex]->At( aItemIndex )->Icon();
		}
		
    // Must return something.				
    return iSettingItems[aItemIndex]->At( 0 )->Icon(); 
	}
	
// -----------------------------------------------------------------------------
// CCamModeSetupPaneHandler::Thumbnail
// Get the thumbnail (of specified size) for last captured icon
// -----------------------------------------------------------------------------
//
CGulIcon& CCamModeSetupPaneHandler::Thumbnail( TBool aSmall )
    {
    return *iLastCaptureHelper->Thumbnail( aSmall );
    }
	
	
// -----------------------------------------------------------------------------
// CCamModeSetupPaneHandler::IconIsThumbnail
// Returns whether item indicated by aItemIndex is the last captured thumbnail
// -----------------------------------------------------------------------------
//
TBool CCamModeSetupPaneHandler::IconIsThumbnail( TInt aItemIndex ) const
    {
    // Thumbnail is always the last item so check the specified index is the
    // last item.  Also check that the thumbnail is valid.
    if ( aItemIndex == iNoOfSettingsItems &&
         iPaneLevel == EMenuLevel &&
         iLastCaptureHelper->IsThumbValid() )
        {                
        return ETrue;
        }
    else 
        {
        return EFalse;
        }        
    }


// -----------------------------------------------------------------------------
// CCamModeSetupPaneHandler::HighlightedItemIndex
// Returns the currently highlighted pane item.
// -----------------------------------------------------------------------------
//
TInt CCamModeSetupPaneHandler::HighlightedItemIndex() const
	{
	return iHighlightedItemIndex;
	}


// -----------------------------------------------------------------------------
// CCamModeSetupPaneHandler::HandlePaneItemChanged
// Handles a change of the currently highlighted pane item.
// -----------------------------------------------------------------------------
//
void CCamModeSetupPaneHandler::HandlePaneItemChanged()
	{
	if ( iPaneLevel == EItemLevel )
		{
		HandlePaneItemLevelItemChanged();
		}
	}


// -----------------------------------------------------------------------------
// CCamModeSetupPaneHandler::HandlePaneItemLevelItemChanged
// Handles a change of the current item at the 'item' level.
// -----------------------------------------------------------------------------
//
void CCamModeSetupPaneHandler::HandlePaneItemLevelItemChanged()
	{
	ASSERT( iPaneLevel == EItemLevel );
	ASSERT( iCurrentSettingsArrayIndex != KErrNotFound );

    // Showing a Scene item requires the Options menu, and setting of the current highlighted item
    if ( ( iMenuItemIds[iCurrentSettingsArrayIndex] == ECamSettingItemDynamicPhotoScene ) ||
         ( iMenuItemIds[iCurrentSettingsArrayIndex] == ECamSettingItemDynamicVideoScene )
         || ( iMenuItemIds[iCurrentSettingsArrayIndex] == ECamSettingItemUserSceneBasedOnScene ) )    
        {
        CCamPreCaptureContainerBase* parent = static_cast<CCamPreCaptureContainerBase*>( iParent );	    
        parent->SetCurrentSetupPaneItem( HighlightedItemId() );
        }	
    
    else
        {        
        // Set the controller to preview the new setting.
        // This will either be comitted by a "Select" or reverted by a "Cancel"
        TRAPD( ignore, iController.PreviewSettingChangeL( iMenuItemIds[iCurrentSettingsArrayIndex], HighlightedItemId() ); )
        if ( ignore )
            { 
            // Do nothing ( removes build warning )
            }      
        }
    

		
	}


// -----------------------------------------------------------------------------
// CCamModeSetupPaneHandler::HandlePaneMenuLevelSelection
// Handles pane selections made at the 'menu' level.
// -----------------------------------------------------------------------------
//
void CCamModeSetupPaneHandler::HandlePaneMenuLevelSelection( TInt aValueToBeHighlighted )
	{
	ASSERT( iPaneLevel == EMenuLevel );

	if ( iHighlightedItemIndex < iNoOfSettingsItems )
		{
		iCurrentSettingsArrayIndex = iHighlightedItemIndex;
		iHighlightedItemIndex = iSettingItems[iCurrentSettingsArrayIndex]->ItemIndexFromItemId( aValueToBeHighlighted );
		iPaneLevel = EItemLevel;
		iPaneControl->HandleModelChanged();

        // Showing a Scene item requires the Options menu, and setting of the current highlighted item
		if ( ( iMenuItemIds[iCurrentSettingsArrayIndex] == ECamSettingItemDynamicPhotoScene ) ||
		     ( iMenuItemIds[iCurrentSettingsArrayIndex] == ECamSettingItemDynamicVideoScene ) ||
	         ( iMenuItemIds[iCurrentSettingsArrayIndex] == ECamSettingItemUserSceneBasedOnScene ) )
		    {		    
		    CCamPreCaptureContainerBase* parent = static_cast<CCamPreCaptureContainerBase*>( iParent );
		    parent->SetUseSetupPaneOptions( ETrue );
		    parent->SetCurrentSetupPaneItem( HighlightedItemId() );   
		    }			
		
		// Check whether the viewfinder should be active but masked to 
		// preview some settings values
		if ( iMenuItemIds[iCurrentSettingsArrayIndex] == ECamSettingItemDynamicPhotoWhiteBalance ||
		     iMenuItemIds[iCurrentSettingsArrayIndex] == ECamSettingItemDynamicPhotoColourFilter ||
		     iMenuItemIds[iCurrentSettingsArrayIndex] == ECamSettingItemDynamicPhotoBrightness ||
		     iMenuItemIds[iCurrentSettingsArrayIndex] == ECamSettingItemDynamicPhotoContrast || 
             iMenuItemIds[iCurrentSettingsArrayIndex] == ECamSettingItemDynamicVideoWhiteBalance ||
		     iMenuItemIds[iCurrentSettingsArrayIndex] == ECamSettingItemDynamicVideoColourFilter ||
		     iMenuItemIds[iCurrentSettingsArrayIndex] == ECamSettingItemDynamicVideoBrightness ||
		     iMenuItemIds[iCurrentSettingsArrayIndex] == ECamSettingItemDynamicVideoContrast ||
		     iMenuItemIds[iCurrentSettingsArrayIndex] == ECamSettingItemUserSceneWhitebalance ||
		     iMenuItemIds[iCurrentSettingsArrayIndex] == ECamSettingItemUserSceneColourFilter
		     
		     )		     
		    {
		    CCamPreCaptureContainerBase* parent = static_cast<CCamPreCaptureContainerBase*>( iParent );	    
            parent->SetViewFinderState( CCamPreCaptureContainerBase::EVfStateActiveMasked );
		    }		    
	    } 
	}
	
	
// -----------------------------------------------------------------------------
// CCamModeSetupPaneHandler::HandleLastCaptureSelection
// Handles user selection of the "LastCapture" thumbnail where available
// -----------------------------------------------------------------------------
//
void CCamModeSetupPaneHandler::HandleLastCaptureSelection()	
    {
    // Check we have a valid thumbnail
    if ( IconIsThumbnail( iHighlightedItemIndex ) )
	    {
	    // Need to fire off post capture view with the snapshot.
        TRAPD( err, iLastCaptureHelper->ShowPostCaptureViewL() );            
        if ( err == KErrNoMemory )
            {
            TRAPD( ignore, static_cast<CCamAppUi*>( CEikonEnv::Static()->AppUi() )->ShowOOMNoteL() );
            if ( ignore )
                {       
                // Do nothing ( removes build warning )
                }
            }
        }    
    }

// -----------------------------------------------------------------------------
// CCamModeSetupPaneHandler::ActivateMenuItem
// Activates a particular sub-menu.
// -----------------------------------------------------------------------------
//
void CCamModeSetupPaneHandler::ActivateMenuItem( TInt aItemId )
{
    // Set the current item in the main menu to the item that is to be activated.
    iHighlightedItemIndex = iNoOfSettingsItems;
    TInt i;
    for ( i = 0; i < iNoOfSettingsItems; ++i  )
        {
        if ( iMenuItemIds[i] == aItemId )  
            {
            iHighlightedItemIndex = i; 
            break;
            }
        }       
    ASSERT( iHighlightedItemIndex != iNoOfSettingsItems );
    
    // If the user has just exited the User scene setup, and entered scene pane,
    // ensure that the user scene is the highlighted scene.
    if ( aItemId == ECamSettingItemDynamicPhotoScene )
        {
        HandlePaneMenuLevelSelection( ECamSceneUser );
        }
    // Otherwise, ensure that the current setting value is highlighted.
    else
        {
        HandlePaneMenuLevelSelection( iController.IntegerSettingValue( HighlightedItemId() ) );             
        }
}

// -----------------------------------------------------------------------------
// CCamModeSetupPaneHandler::HandlePaneItemLevelSelection
// Handles pane selections made at the 'item' level.
// -----------------------------------------------------------------------------
//
void CCamModeSetupPaneHandler::HandlePaneItemLevelSelection()
	{
	ASSERT( iPaneLevel == EItemLevel ); // Should only receive 'item' level IDs, not top level 'menu' IDs.
	ASSERT( iCurrentSettingsArrayIndex != KErrNotFound );
   
    // If current Item was a photo-scene item, the new (cancelled) state will not
    // need the "options" menu or the current Item information, and the viewfinder
    // can be activated.
	if ( ( iMenuItemIds[iCurrentSettingsArrayIndex] == ECamSettingItemDynamicPhotoScene ) ||
	     ( iMenuItemIds[iCurrentSettingsArrayIndex] == ECamSettingItemDynamicVideoScene ) )
	
	    {
        TRAPD( ignore, iController.SetIntegerSettingValueL( iMenuItemIds[iCurrentSettingsArrayIndex], HighlightedItemId() ); )
        if ( ignore )
            { 
            // Do nothing ( removes build warning )
            }             		    
        CCamPreCaptureContainerBase* parent = static_cast<CCamPreCaptureContainerBase*>( iParent );	    
	    parent->SetUseSetupPaneOptions( EFalse );
	    parent->SetCurrentSetupPaneItem( KErrNotFound );    // Invalidate
	    parent->SetViewFinderState( CCamPreCaptureContainerBase::EVfStateActive );       // Ensure reverted viewfinder
	    }			           
	// else if current item is user scene base on scene item, the new (cancelled) state will not
    // need the "options" menu or the current Item information. 
	else if ( iMenuItemIds[iCurrentSettingsArrayIndex] == ECamSettingItemUserSceneBasedOnScene )		        
		{
        TRAPD( ignore, iController.SetIntegerSettingValueL( iMenuItemIds[iCurrentSettingsArrayIndex], HighlightedItemId() ); )
        if ( ignore )
            { 
            // Do nothing ( removes build warning )
            }         		
	    CCamPreCaptureContainerBase* parent = static_cast<CCamPreCaptureContainerBase*>( iParent );
	    parent->SetUseSetupPaneOptions( EFalse );
	    parent->SetCurrentSetupPaneItem( KErrNotFound );    // Invalidate    		        		    
		}
	else
	    {
        // User has selected the highlighted item, so tell controller to commit this setting
        iController.CommitPreviewChanges();	    
	    }

	iHighlightedItemIndex = iCurrentSettingsArrayIndex;
	iCurrentSettingsArrayIndex = KErrNotFound; // Invalidate.

    // If in user scene mode revert back to the user scene setup list.
    if ( iUserSceneSetupMode )
        {
        SwitchToUserSceneView();         
        }               

    // else revert back to menu level, and remove focus from setup pane.
    else
        {
    	iPaneLevel = EMenuLevel;
    	iPaneControl->HandleModelChanged();
    	  
    	// Select-key focus goes back to viewfinder
    	CCamPreCaptureContainerBase* parent = static_cast<CCamPreCaptureContainerBase*>( iParent );
    	parent->FocusToSetupPane( EFalse );        
        }
	} 


// -----------------------------------------------------------------------------
// CCamModeSetupPaneHandler::SwitchToUserSceneView
// Switches view to the user scene view.
// -----------------------------------------------------------------------------
//
void CCamModeSetupPaneHandler::SwitchToUserSceneView()
    {
    CCamAppUiBase* appUi = static_cast<CCamAppUiBase*>( CEikonEnv::Static()->AppUi() );
    TRAPD( ignore, appUi->HandleCommandL( ECamCmdUserScene ) )   
    if ( ignore )
        { 
        // Do nothing ( removes build warning )
        }                        
    }
   

// -----------------------------------------------------------------------------
// CCamModeSetupPaneHandler::HandlePaneCancel
// Handles the user Cancelling from the setup pane.  Will revert to last selected
// menu level and force a redraw
// -----------------------------------------------------------------------------
//
void CCamModeSetupPaneHandler::HandlePaneCancel()
    {	    
    if ( iPaneLevel == EItemLevel )
        {
        ASSERT( iCurrentSettingsArrayIndex != KErrNotFound );    
        // If current Item was a photo-scene item, the new (cancelled) state will not
        // need the "options" menu or the current Item information, and the viewfinder
        // can be activated.
		if ( ( iMenuItemIds[iCurrentSettingsArrayIndex] == ECamSettingItemDynamicPhotoScene ) ||
		     ( iMenuItemIds[iCurrentSettingsArrayIndex] == ECamSettingItemDynamicVideoScene ) )
		    {		    
		    CCamPreCaptureContainerBase* parent = static_cast<CCamPreCaptureContainerBase*>( iParent );
		    parent->SetUseSetupPaneOptions( EFalse );
		    parent->SetCurrentSetupPaneItem( KErrNotFound );    // Invalidate
		    parent->SetViewFinderState( CCamPreCaptureContainerBase::EVfStateActive );       // Ensure reverted viewfinder
		    }	
		    
		// else if current item is user scene base on scene item, the new (cancelled) state will not
        // need the "options" menu or the current Item information. 
		else if ( iMenuItemIds[iCurrentSettingsArrayIndex] == ECamSettingItemUserSceneBasedOnScene )		        
    		{
		    CCamPreCaptureContainerBase* parent = static_cast<CCamPreCaptureContainerBase*>( iParent );
		    parent->SetUseSetupPaneOptions( EFalse );
		    parent->SetCurrentSetupPaneItem( KErrNotFound );    // Invalidate    		        		    
    		}
        // otherwise, do nothing
        else
            {
            // empty statement to remove Lint error.
            }    		
    		
        iHighlightedItemIndex = iCurrentSettingsArrayIndex;
        iCurrentSettingsArrayIndex = KErrNotFound; // Invalidate.

        // User has cancelled the highlighted item, so tell controller to cancel this change
        TRAPD( ignore, iController.CancelPreviewChangesL() );               
        if ( ignore )
            { 
            // Do nothing ( removes build warning )
            }             
        }	

    // If in user scene mode revert back to the user scene setup list.
    if ( iUserSceneSetupMode )
        {
        SwitchToUserSceneView();         
        }               

    // else revert back to menu level, and remove focus from setup pane.
    else
        {
    	iPaneLevel = EMenuLevel;
    	iPaneControl->HandleModelChanged();
    	
    	// Select-key focus goes back to viewfinder
    	CCamPreCaptureContainerBase* parent = static_cast<CCamPreCaptureContainerBase*>( iParent );
    	parent->FocusToSetupPane( EFalse );       
        }
    }

// -----------------------------------------------------------------------------
// CCamModeSetupPaneHandler::CreatePaneControlL
// Creates and returns a pointer to a new pane control.  Used by the control container.
// Keeps a pointer to the pane control, but gives ownership to caller.
// -----------------------------------------------------------------------------
//
CCoeControl* CCamModeSetupPaneHandler::CreatePaneControlL( CCoeControl* aParent, const TRect& aRect )
	{
	iPaneControl = CCamSetupPane::NewL( *aParent, aRect, *this, *this );
	iParent = aParent;
	return iPaneControl;
	}


// -----------------------------------------------------------------------------
// CCamModeSetupPaneHandler::UpdateFromResourceL
// Updates the contents of the setup pane based on the supplied resource
// -----------------------------------------------------------------------------
//
void CCamModeSetupPaneHandler::UpdateFromResourceL( TInt aResource )
    {        
    TResourceReader reader;
    
    ResetArray();
    
    CCoeEnv::Static()->CreateResourceReaderLC( reader, aResource );

	const TInt menuItemCount = reader.ReadInt16();

	TInt i;
	for ( i = 0 ; i < menuItemCount ; i++ )
		{
		iMenuItemIds[i] = reader.ReadInt16();
		iMenuTitles[i] = reader.ReadTPtrC().Left( KMaxTitleCharLength );
		TInt itemsId = reader.ReadInt32();
		iSettingItems[i] = new( ELeave ) CCamSetupPaneItemArray( KItemArrayGranularity );
		iSettingItems[i]->ConstructFromResourceL( itemsId );
		ASSERT( iSettingItems[i]->Count() > 0 );
		}
		
	iNoOfSettingsItems = menuItemCount;

	CleanupStack::PopAndDestroy(); // reader

    if ( iLastCapTitle ) 
        {
    	iMenuItemIds[i] = 0;
    	iMenuTitles[i] = iLastCapTitle->Des();        
        }            

	iCurrentSettingsArrayIndex = KErrNotFound; // Invalidate.    
	
	// Given mode change, set highlight back to start of items list
	iHighlightedItemIndex = 0;       
	iPaneControl->HandleModelChanged();
    }



// -----------------------------------------------------------------------------
// CCamModeSetupPaneHandler::Refresh
// Forces a refresh of the SetupPane control
// -----------------------------------------------------------------------------
//
void CCamModeSetupPaneHandler::Refresh()
    {
    TParse parse;
    parse.Set( iLastCaptureHelper->FileName(), NULL, NULL );    
    iLastCapName = parse.Name();			
            
    // Only refresh the control if we are in the foreground        
    if ( iForeground )     
        {
        iPaneControl->DrawNow();
        }       
    }
    
// -----------------------------------------------------------------------------
// CCamModeSetupPaneHandler::SetForeground
// Sets whether SetupPane is in foreground or background
// -----------------------------------------------------------------------------
// 
void CCamModeSetupPaneHandler::SetForeground( TBool aForeground )
    {
    iForeground = aForeground;
    }
    
// -----------------------------------------------------------------------------
// CCamModeSetupPaneHandler::SetMenuContextL
// Used to change the contents of the Setup Pane depending on which camera and
// mode is to be used.
// -----------------------------------------------------------------------------
//
void CCamModeSetupPaneHandler::SetMenuContextL( TBool aFirstCamera, TBool aPhoto, TBool aUserSceneSetup )
    {
    TInt resource = KErrNotFound; 
    iPhotoMode = aPhoto;
   
    // Identify which resource to use
    
    // ...User scene setup mode
    if ( aUserSceneSetup )
        {
        resource = ROID(R_CAM_USER_SCENE_SETUP_PANE_MENU_ITEM_ARRAY_ID);
        iUserSceneSetupMode = ETrue; 
        }
    // ...Non-user scene setup mode.    
    else 
        {
        iUserSceneSetupMode = EFalse; 
        
        if ( aFirstCamera )
            {
            if ( aPhoto )
                {
                resource = ROID(R_CAM_IMAGE_MODE_SETUP_PANE_MENU_ITEM_ARRAY_ID);
                }
            else
                {
                resource = ROID(R_CAM_VIDEO_MODE_SETUP_PANE_MENU_ITEM_ARRAY_ID);
                }
            }
        else
            {
            if ( aPhoto )
                {
                resource = ROID(R_CAM_SECONDARY_IMAGE_MODE_SETUP_PANE_MENU_ITEM_ARRAY_ID);
                }
            else
                {
                resource = ROID(R_CAM_SECONDARY_VIDEO_MODE_SETUP_PANE_MENU_ITEM_ARRAY_ID);
                }        
            }
        }
           
    iLastCaptureHelper->SetPhotoMode( aPhoto );
    
    // Load the string for the last captured item title
    if ( iLastCapTitle )
        {
        delete iLastCapTitle;
        iLastCapTitle = NULL;       
        }
    TInt lastCapRes = 0;        
    if ( aPhoto )
        {        
        lastCapRes = ROID(R_CAM_SETUP_LASTCAP_PHOTO_TITLE_ID);   
        }
    else
        {
        lastCapRes = ROID(R_CAM_SETUP_LASTCAP_VIDEO_TITLE_ID);    
        }
    iLastCapTitle = StringLoader::LoadLC( lastCapRes );            
    CleanupStack::Pop( iLastCapTitle );    
    
    TParse parse;
    parse.Set( iLastCaptureHelper->FileName(), NULL, NULL );    
    iLastCapName = parse.Name();			
    
    
    // Make the change
    if ( resource != KErrNotFound )
        {
        UpdateFromResourceL( resource );
        }
    }


