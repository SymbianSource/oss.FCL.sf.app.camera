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
* Description:  Implements container for the user scene setup list.*
*/


#ifndef CAMUSERSCENESETUPCONTAINER_H
#define CAMUSERSCENESETUPCONTAINER_H

// INCLUDES
#include "CamContainerBase.h"
#include <coecntrl.h>
#include <eiklbo.h>
   
// FORWARD DECLARATIONS
class CAknSettingStyleListBox;
class CCamUserSceneSetupItemArray;
class CEikFormattedCellListBox;

// CLASS DECLARATION

/**
*  CCamUserSceneSetupContainer  container control class.
*  
*/
class CCamUserSceneSetupContainer : public CCamContainerBase, 
                                    public MEikListBoxObserver
                                    //, public MCoeControlObserver
  {    
  /* enum TVfState moved to CCamContainerBase. 
  */
      
  // =======================================================
  // Constructors and destructor
  public:

    /**
    * Symbian two-phased constructor.
    * @param aRect reference to the rectangular area for the container.
    * @param aSettingsModel reference to the settings model.
    * @return pointer to the created CCamUserSceneSetupContainer object.
    * @since 2.8
    */
    static CCamUserSceneSetupContainer* NewLC( const TRect&       aRect,  
                                               TInt               aResourceId, 
                                               CCamAppController& aController, 
                                               CAknView&          aView );
    /**
    * Symbian two-phased constructor.
    * @param aRect reference to the rectangular area for the container.
    * @param aSettingsModel reference to the settings model.
    * @return pointer to the created CCamUserSceneSetupContainer object.
    * @since 2.8
    */
    static CCamUserSceneSetupContainer* NewL( const TRect&       aRect, 
                                              TInt               aResourceId, 
                                              CCamAppController& aController, 
                                              CAknView&          aView );

    /**
    * Destructor.
    * @since 2.8
    */
    virtual ~CCamUserSceneSetupContainer();

  // -------------------------------------------------------
  // From CCoeControl
  private:

    /**
    * @since 2.8
    */
    TInt CountComponentControls() const;
    
    /**
    * @since 2.8
    */
    CCoeControl* ComponentControl( TInt aIndex ) const;

    /**
    * @since 2.8
    * @param aKeyEvent the key event
    * @param aType the type of the event
    * @return TKeyResponse key event was used by this control or not
    */
    TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );

  // -------------------------------------------------------
  // From MEikListBoxObserver
  public: 
    
    /**
    * Handles a list box event.
    * @param aListBox a reference to the list box.
    * @param aEventType the type of event.
    * @since 2.8
    */
    void HandleListBoxEventL( CEikListBox* aListBox, TListBoxEvent aEventType );
    
    /**
    * Handles the user selection of a list item.
    * @since 2.8
    */
    void HandleSelectionL();

  // -------------------------------------------------------
  // New functions
  public: 

    /**
    * Handles a change in the item list visibility.
    * @since 2.8
    */
    void UpdateDisplayL();
    
    /**
    * Returns the resource Id of the title for this container.
    * @return the resource Id
    * @since 2.8
    */
    TInt TitlePaneTextResourceId();
    
    /**
    * Updates the list items with the settings model values.
    * @since 2.8
    */
    void UpdateListItems();
    
    /**
    * Updates the current list item with it's associated
    * settings model value.
    * @since 2.8
    */
    void UpdateCurrentListItem();
        
    /**
    * Returns wheather user scene restore is highlighted
    */
    TBool IsUserSceneResetHighlighted();

  private:
    
    /**
    * Updates a list item with its corresponding settings model value.
    * @param aIndex the index of the list item.
    * @since 2.8
    */
    void UpdateListItem( TInt aIndex );
    
    /**
    * Returns the command id for activating the UI control that is
    * associated with the currently selected list item.
    * @return the command id.
    * @since 2.8
    */
    TInt CommandIdForActivatingCurrentItemControl();

    /**
    * Returns a pointer to the list box.
    * @return the list box pointer.
    * @since 2.8
    */
    CEikFormattedCellListBox* ListBox();

  // -------------------------------------------------------
  // Constructors
  private: 

    /**
    * C++ Constructor
    * @param aController a reference to the controller.
    * @param aView a reference to the view that contains this container.
    * @since 2.8
    */
    CCamUserSceneSetupContainer( CCamAppController& aController, CAknView& aView );
    
    /**
    * 2nd phase constructor.
    * @since 2.8
    * @param aRect reference to the rectangular area for the container.
    * @param aSettingsModel reference to the settings model.
    */
    void ConstructL( const TRect& aRect, TInt aResourceId );
    
  // =======================================================
  // Data
  private:
        
    // User scene setup list owned by this container.
    CAknSettingStyleListBox* iUserSceneSetupList;
    
    // Array of user scene setup list items owned by this container.
    CCamUserSceneSetupItemArray* iUserSceneSetupItemArray;
    
    // The resource if for the title of the scene setup list.
    TInt iTitleResourceId;        

  // =======================================================
  };

#endif

// End of File
