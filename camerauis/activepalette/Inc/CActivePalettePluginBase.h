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
* Description:  Active Palette plugin base class*
*/


/**
 * @file CActivePalettePluginBase.h
 * Active Palette plugin base class
 */

#ifndef CACTIVEPALETTEPLUGINBASE_H
#define CACTIVEPALETTEPLUGINBASE_H

//  INCLUDES
#include <e32std.h>
#include <e32base.h>

// FORWARD DECLARATIONS
class CActivePalettePluginBase;
class CGulIcon;
class CFbsBitmap;
class CFbsBitmapDevice;
class CBitmapContext;

// CLASS DECLARATIONS

/**
 * Plugin observer class.
 * Plugins should use this to communicate with the Active Palette.
 */
class MActivePalettePluginObserver
    {
    public:
        /**
         * Notifies of the creation of the plugin
         * @param aPlugin A pointer to the plugin
         */
    	virtual void NotifyPluginCreated(CActivePalettePluginBase* aPlugin) = 0;

        /**
         * Notifies of the destruction of the plugin
         * @param aPlugin A pointer to the plugin
         */
    	virtual void NotifyPluginDestroyed(CActivePalettePluginBase* aPlugin) = 0;

        /** 
         * Assign icon for plugin obect's item
         * @param aIcon A pointer to the icon
         * @param aOnwershipTransfer Whether to transfer ownership
         */
    	virtual void SetItemIcon(CGulIcon* aIcon, TBool aOnwershipTransfer) = 0;
    	
    	/**
    	 * Assign tool tip for plugin obect's item
    	 * @param aTooltipText A descriptor containing the new tooltip text
    	 */
    	virtual void SetItemTooltipL(const TDesC& aTooltipText) = 0;
    	
    	/**
    	 * Assign icon and tooltip for plugin obect's item
    	 * @param aIcon The new icon
    	 * @param aOnwershipTransfer Whether the AP should take ownership of the icon
    	 * @param aTooltipText The new tooltip text
    	 */
    	virtual void SetItemIconAndTooltipL(CGulIcon* aIcon, 
    										TBool aOnwershipTransfer,
    										const TDesC& aTooltipText) = 0;

    	/**
    	 * Notify comletion of handling of user's item selection
    	 * @param aDataDes Data descriptor
    	 */
    	virtual void NotifyItemComplete(const TDesC8& aDataDes) = 0;

    	/**
    	 * Notify comletion of handling of user's item selection
    	 * @param aDataInt Data integer
    	 */
    	virtual void NotifyItemComplete(TInt aDataInt) = 0;
    	
    	/**
    	 * Send message to application
    	 * @param aMessageID The message ID
    	 * @param aDataDes Data descriptor
    	 */
    	virtual void SendMessage(TInt aMessageID, const TDesC8& aDataDes) = 0;

    	/**
    	 * Send message to application
    	 * @param aMessageID The message ID
    	 * @param aDataInt Data integer
    	 */
    	virtual void SendMessage(TInt aMessageID, TInt aDataInt) = 0;
    };

/**
 * Namespace for the plugin factory
 */
namespace NActivePalettePluginFactory
    {
    /**
     * Definition for data that PLUGIN gets upon its creation
     */
    class TPluginCreationParams
        {
        public:
            /**
             * Return a reference to the observer
             * @return A reference to the observer
             */
        	IMPORT_C MActivePalettePluginObserver& PluginObserver(void) const; 

            /**
             * Return the dimensions
             * @return The dimensions
             */
        	IMPORT_C TSize Dimensions(void) const;
        	
            /**
             * Constructor
             * @param aObserver A reference to the observer
             * @param aDimensions The dimensions
             */
        	IMPORT_C TPluginCreationParams(MActivePalettePluginObserver &aObserver,
        						  const TSize& aDimensions); 

        private:
            /// The observer
        	MActivePalettePluginObserver& 	iObserver;
        	/// Size of the icons used
        	TSize 							iDimensions;
        };

    /**
     * Prototype for plugin DLL factory function
     */
    typedef CActivePalettePluginBase * (* TInstantiatePluginLFunc)(const TUid& aPluginUid, 
    															   const TPluginCreationParams& aCreationParams);

    /// Entry ordinal to use for plugin creation
    const TInt KFactoryEntryOrdinal = 1;
    }


/**
 * Animation helper for plugins.
 * Contains default icon animation (shrinks, then returns to normal)
 */
class CAnimationHelper : public CBase
    {
    private:
        /**
         * Constructor
         */
    	CAnimationHelper();

        /**
         * Symbian leaving constructor
         * @param aSize Size
         * @param aMaskNeeded Indicates if a mask is needed or not
         */
    	void ConstructL(const TSize aSize, const TBool aMaskNeeded);
    	
    public:
        /**
         * Destructor
         */
    	~CAnimationHelper();
    	
        /**
         * Constructor
         * @param aSize Size
         * @param aMaskNeeded Indicates if a mask is needed or not
         */
    	static CAnimationHelper* NewL(const TSize aSize, const TBool aMaskNeeded);

    public:
        /// Current frame bitmap.
    	CFbsBitmap*         iFrame; 
        /// Current frame device.
    	CFbsBitmapDevice*   iFrameDevice;
        /// Current frame context.
    	CBitmapContext*    iFrameGc;
 
        /// Current frame mask bitmap.
    	CFbsBitmap*         iFrameMask; 
        /// Current frame mask device.
    	CFbsBitmapDevice*   iFrameMaskDevice;
        /// Current frame mask context.
    	CBitmapContext*     iFrameMaskGc;
    };

/**
 * Plugin base class
 */
class CActivePalettePluginBase : public CBase
    {
    public:
        /**
         * Reserved for internal use in this version
         */
    	class TPluginAnimationInfo
        	{
        	public:
        	    /// Number of frames in animation
        		TInt 	iNofFrames;
        		/// Interval between frames
        		TInt	iFrameTimeGapMs;
        		/// Dimensions of icons
        		TSize   iFrameSize;
        	};

        /**
         * The type of animation to perform
         */
    	enum TPluginAnimationType
        	{
    		EAniInvalid,     ///< Invalid animation
    		EAniFocused = 2  ///< Focus animation
        	};

    protected:

        /**
         * Constructor
         */
    	IMPORT_C CActivePalettePluginBase(const NActivePalettePluginFactory::TPluginCreationParams& aCreationParams);

        /**
         * Returns a reference to the plugin observer
         * @return A reference to the plugin observer
         */
    	inline MActivePalettePluginObserver& PluginObserver(void) const;

    public:

        /**
         * Destructor
         */
    	IMPORT_C virtual ~CActivePalettePluginBase(void);
    	
    	/** 
    	 * Handling of user's item selection
    	 * @warning MUST BE IMPLEMENTED BY PLUGIN
    	 */
    	IMPORT_C virtual void HandleItemSelectedL(void) = 0;
    	
    	/**
    	 * Handling of message from application
    	 * @warning SHOULD BE IMPLEMENTED BY PLUGIN
    	 * @param aMessageID The message ID
    	 * @param aDataDes Data descriptor
    	 * @param aDataInt Data integer
    	 */
    	IMPORT_C virtual void HandleMessageL(TInt aMessageID,
    	                                     const TDesC8& aDataDes,
    	                                     TInt aDataInt);
    	
    	/**
    	 * 2nd phase constructor
    	 * @warning MUST BE IMPLEMENTED BY PLUGIN
    	 * @param aCreationParams A reference to the construction parameter class to use
    	 * @param aCustomDataDes Data descriptor
    	 * @param aCustomDataInt Data integer
    	 */
    	IMPORT_C virtual void ConstructL(const NActivePalettePluginFactory::TPluginCreationParams& aCreationParams, 
                                         const TDesC8& aCustomDataDes,
                                         TInt aCustomDataInt);
    	
    	/**
    	 * Resource destruction 
    	 * @warning MAY BE IMPLEMENTED BY PLUGIN
    	 * (possible to use AP services)
    	 */
    	IMPORT_C virtual void Shutdown(void);

    	/** 
    	 * Reserved for internal use in this version
    	 * @param aAniType
    	 * @param aAniInfo
    	 * @param aDefaultAniInfo
    	 * @param aItemIcon
    	 * @param aItemIconMask
    	 */
    	IMPORT_C virtual void PrepareAniFramesL(TInt aAniType,				
                                                TPluginAnimationInfo& aAniInfo,
                                                const TPluginAnimationInfo& aDefaultAniInfo,
                                                const CFbsBitmap & aItemIcon,
                                                const CFbsBitmap * aItemIconMask);
    								  
    	/** 
    	 * Release the frames used in the animation
    	 * @param aAniType
    	 */
    	IMPORT_C virtual void ReleaseAniFrames(TInt aAniType);
    	
    	/**
    	 * Produces an animation frame
    	 * @param aAniType
    	 * @param aFrameNo
    	 * @param aAniInfo
    	 * @param aItemIcon
    	 * @param aItemIconMask
    	 * @param aFrame
    	 * @param aFrameMask
    	 * @param aOwnershipTransferForFrame
    	 * @param aOwnershipTransferForMask
         */
    	IMPORT_C virtual void ProduceAniFrame(TInt aAniType,
                                              TInt aFrameNo,
                                              const TPluginAnimationInfo& aAniInfo,
                                              const CFbsBitmap & aItemIcon,
                                              const CFbsBitmap * aItemIconMask,
                                              CFbsBitmap** aFrame,
                                              CFbsBitmap** aFrameMask,
                                              TBool& aOwnershipTransferForFrame,
                                              TBool& aOwnershipTransferForMask);
    				      		  				      		  						      		  		
    private:
        /// The plugin observer
        MActivePalettePluginObserver& iPluginObserver;
        
        /// The animation helper
        CBase*						  iAnimationHelper;
    }; 

// -----------------------------------------------------------------------------
// CActivePalettePluginBase::PluginObserver()
// -----------------------------------------------------------------------------
//
inline MActivePalettePluginObserver & CActivePalettePluginBase::PluginObserver(void) const
    {
	return iPluginObserver;
    };

#endif // CACTIVEPALETTEPLUGINBASE_H

// End of File
