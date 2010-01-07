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
* Description:  Active Palette Styler*
*/


/**
 * @file ActivePalette2Styler.h
 * Active Palette Styler
 */

#ifndef _ACTIVE_PALETTE_2_STYLER_H
#define _ACTIVE_PALETTE_2_STYLER_H

#include <e32std.h>
#include <e32base.h>

// Forward declarations
class CFbsBitmap;
class CFbsBitmapDevice;
class CFbsBitGc;
class CBitmapContext;
class CFont;

/**
 * Used to apply different rendering styles to the 2D Active Palette.
 * Can not be directly instantiated: used as a base for further sub-classing
 */
class CActivePalette2Styler : public CBase
  {
  public:

    /**
    * Draw the entire background for the item, and the supplied item on top
    * @param aScreenPos The screen index of the item
    * @param aIcon The item's icon
    * @param aMask The item's mask
    * @param aOffsetFrame The offset frame, in case of item-scrolling
    * @param aOffsetTotal Total number of offset frames, in case of item-scrolling
    */         
    virtual void DrawItem(TInt aScreenPos, CFbsBitmap* aIcon, CFbsBitmap* aMask, TInt aOffsetFrame, TInt aOffsetTotal) = 0;
    
    /**
    * Draw the between-item gap following the item at aScreenPos
    * @param aScreenPos The screen index of the item
    * @param aOffsetFrame The offset frame, in case of item-scrolling
    * @param aOffsetTotal Total number of offset frames, in case of item-scrolling
    */         
    virtual void DrawGap(TInt aScreenPos, TInt aOffsetFrame, TInt aOffsetTotal) = 0;
    
    /**
    * Update only the area covered by the item graphic with an animation frame
    * @param aScreenPos The screen index of the item
    * @param aIcon The item's icon
    * @param aMask The item's mask
    */     
    virtual void AnimItem(TInt aScreenPos, CFbsBitmap* aIcon, CFbsBitmap* aMask) = 0;
    
    /**
    * Draws the top section of the AP, with optional arrow
    * @param aShowArrow Whether to display the scroll indicator
    */
    virtual void DrawTopScrollArrowSection(TBool aShowArrow) = 0;
    
    /**
    * Draws the bottom section of the AP, with optional arrow
    * @param aShowArrow Whether to display the scroll indicator
    */
    virtual void DrawBottomScrollArrowSection(TBool aShowArrow) = 0;
    
    /**
    * Draw the focus ring
    * @param aScreenPos The screen index of the item
    * @param aOffsetFrame The offset frame, in case of focus-moving
    * @param aOffsetTotal Total number of offset frames, in case of focus-moving
    */
    void DrawFocusRing(TInt aScreenPos, TInt aOffsetFrame, TInt aOffsetTotal);
    
    /**
    * Render a new tooltip to the buffer
    * @param aText The tooltip text
    * @return The total width of the tooltip, in pixels
    */    
    virtual TInt DrawTooltip(TDesC* aText);
    
    /**
    * Sets the number of items shown on-screen
    * @param aItems The new number of items
    */    
    void SetNumberOfItemsL(TInt aItems);
    
    /**
    * The current size of the entire active palette
    * @return Size of the active palette, in pixels
    */
    TSize Size() const;
    
    /**
    * Give the current "dirty" rect, i.e. the minimal rectangle that contains all updated regions
    * since previous ClearDirtyRect
    * @return Dirty rect
    * @see ClearDirtyRect()
    */        
    TRect DirtyRect();
    
    /**
    * Resets the dirty rect
    * @see DirtyRect()
    */
    void ClearDirtyRect();
    
    /**
    * Blits the current AP buffer to the supplied context
    * @param aGc The context to blit to
    * @param aPos Where to blit to (the top-left corner of the AP)
    * @param aClippingRect The portion of the AP to blit
    */    
    void BlitPalette(CBitmapContext* aGc, TPoint aPos, TRect aClippingRect) const;
    
    /**
    * Blits the current tooltip buffer to the supplied context
    * @param aGc The context to blit to
    * @param aPos Where to blit to (the top-left corner of the tooltip)
    * @param aClippingRect The portion of the tooltip to blit
    */    
    virtual void BlitTooltip(CBitmapContext* aGc, const TPoint& aPos, TRect aClippingRect);
    
    /**
    * Blits the current tooltip to the supplied context. 
    * Phase of the tooltip is given and also the total number of phases.
    * The effect of these depends on the styler, but e.g. the phase could
    * determine the degree of visibility for the tooltip.
    * @param aGc           The context to blit to
    * @param aPos          Where to blit to (the top-left corner of the tooltip)
    * @param aCurrentPhase The phase of the tooltip.
    * @param aTotalPhases  The amount of tooltip phases in total.
    */    
    virtual void BlitTooltip( CBitmapContext* aGc, const TPoint& aPos, TInt aCurrentPhase, TInt aTotalPhases );
    
    /**
    * Where the tooltip should appear, relative to the position of the AP
    * @param aScreenPos The on-screen index of the item the tooltip is for
    * @return The Y-offset of the tooltip
    */
    TInt TootipYOffset(TInt aScreenPos);
    
    /**
    * The total size of the rendered tooltip
    * @return The total size of the rendered tooltip
    */
    TSize TooltipSize();
    
    /**
    * Destructor
    */    
    ~CActivePalette2Styler();
    
  protected:

    /**
    * Constructor
    */
    CActivePalette2Styler();
    
    /**
    * Second-phase leaving constructor
    */
    virtual void ConstructL();
    
    /**
    * Add a rectangle to the current dirty rectangle
    * @param aNewRect The rect to add
    */
    void AddToDirtyRect(TRect aNewRect);
    
    /**
    * General-purpose graphic-blitting method
    * @param aPos Where to blit to
    * @param aGraphicGc The context for the foreground
    * @param aMaskGc The context for the mask
    * @param aGraphic The image to blit
    * @param aGraphicMask The mask for the image
    * @param aMaskGraphic Whether to mask the image when blitting to aGraphicGc.
    * @param aMaskMask Whether to mask the mask when blitting to aMaskGc. Mask is applied to itself.
    */
    void BlitGraphic(TPoint aPos, CBitmapContext* aGraphicGc, CBitmapContext* aMaskGc, CFbsBitmap* aGraphic, CFbsBitmap* aGraphicMask, TBool aMaskGraphic, TBool aMaskMask);
    
    /**
    * Blits a graphic to the centre of a given area
    * @param aArea The area to blit inside
    * @param aGraphicGc The context for the foreground
    * @param aMaskGc The context for the mask
    * @param aGraphic The image to blit
    * @param aGraphicMask The mask for the image
    * @param aMaskGraphic Whether to mask the image when blitting to aGraphicGc.
    * @param aMaskMask Whether to mask the mask when blitting to aMaskGc. Mask is applied to itself.
    */
    void CentreGraphic(TRect aArea, CBitmapContext* aGraphicGc, CBitmapContext* aMaskGc, CFbsBitmap* aGraphic, CFbsBitmap* aGraphicMask, TBool aMaskGraphic, TBool aMaskMask);
    
    /**
    * The mask to use for the tooltip nose
    * @return The mask to use for the tooltip nose
    */
    virtual TInt TooltipNoseMaskId();
    
    /**
    * The mask to use for the tooltip body
    * @return The mask to use for the tooltip body
    */
    virtual TInt TooltipBodyMaskId();
    
    /**
    * The rectangle of the top section, relative to the top-left of the AP
    * @return The top section area
    */    
    inline TRect TopSectionRect();
    
    /**
    * The rectangle of the bottom section, relative to the top-left of the AP
    * @return The bottom section area
    */    
    inline TRect BottomSectionRect();
    
    /**
    * The rectangle of the given item number, relative to the top-left of the AP
    * @param aScreenPos The screen index of the item
    * @param aOffsetFrame The offset frame, in case of item-scrolling
    * @param aOffsetTotal Total number of offset frames, in case of item-scrolling
    * @return The item area
    */    
    inline TRect ItemRect(TInt aScreenPos, TInt aOffsetFrame = 0, TInt aOffsetTotal = 1);
    
    /**
    * The rectangle of the gap following the given item number, relative to the top-left of the AP
    * @param aScreenPos The screen index of the item
    * @param aOffsetFrame The offset frame, in case of item-scrolling
    * @param aOffsetTotal Total number of offset frames, in case of item-scrolling
    * @return The gap area
    */    
    inline TRect GapRect(TInt aScreenPos, TInt aOffsetFrame = 0, TInt aOffsetTotal = 1);
    
    /**
    * Allocate the tooltip draw buffers
    */
    virtual void AllocateTooltipBufL(void);
    
    /**
    * Release the tooltip draw buffers
    */
    virtual void ReleaseTooltipBuf(void);

  private:

    /**
    * Renders the complete tooltip
    * @param aText The tooltip text
    */
    void RenderTooltip(TDesC* aText);
    
    /**
    * Allocate all the AP draw buffers
    */
    void AllocateDrawBufL(void);
    
    /**
    * Release the AP draw buffers
    */
    void ReleaseDrawBuf(void);

  protected:

    /// The draw buffer bitmap. Owned
    CFbsBitmap* iDrawBufBitmap;
    /// The draw buffer device. Owned
    CFbsBitmapDevice* iDrawBufDevice;
    /// The draw buffer context. Owned
    CBitmapContext* iDrawBufContext;
    
    /// The draw buffer mask bitmap. Owned
    CFbsBitmap* iDrawBufMaskBitmap;
    /// The draw buffer mask device. Owned
    CFbsBitmapDevice* iDrawBufMaskDevice;
    /// The draw buffer mask context. Owned
    CBitmapContext* iDrawBufMaskContext;
    
    /// Tooltip buffer bitmap. Owned
    CFbsBitmap* iTooltipBufBitmap;		
    /// Tooltip buffer device. Owned
    CFbsBitmapDevice* iTooltipBufDevice;		
    /// Tooltip buffer context. Owned
    CBitmapContext* iTooltipBufContext;	
    
    /// Tooltip mask buffer bitmap. Owned
    CFbsBitmap* iTooltipBufMaskBitmap;		
    /// Tooltip mask buffer device. Owned
    CFbsBitmapDevice* iTooltipBufMaskDevice;		
    /// Tooltip mask buffer context. Owned
    CBitmapContext* iTooltipBufMaskContext;
	
    /// Number of items on-screen
    TInt iNumItems;
    /// Number of gaps inbetweeen the items
    TInt iNumGaps;
    
    /// Current dirty rectangle
    TRect iDirtyRect;
    /// Whether the dirty rect has yet been written to
    TBool iDirtyRectExists;
    
    /// The font used for the tooltip text. Not owned.
    const CFont* iFont;
    
    /// The width of the tooltip text alone	
    TInt iTextWidth;
    
    /// The up-arrow bitmap. Owned
    CFbsBitmap* 		    iScrollUpIcon;		
    /// The up-arrow bitmap mask. Owned
    CFbsBitmap* 		    iScrollUpIconMask;	
    
    /// The down-arrow bitmap. Owned
    CFbsBitmap* 		    iScrollDownIcon;	
    /// The down-arrow bitmap mask. Owned
    CFbsBitmap* 		    iScrollDownIconMask;
    
    /// The focus-ring bitmap. Owned
    CFbsBitmap* 		    iFocusRing;	
    /// The focus-ring bitmap mask. Owned
    CFbsBitmap* 		    iFocusRingMask;
    
    /// The tooltip-nose bitmap. Owned
    CFbsBitmap* 		    iTooltipNose;	
    /// The tooltip-nose bitmap mask. Owned
    CFbsBitmap* 		    iTooltipNoseMask;
    /// The tooltip-body bitmap. Owned
    CFbsBitmap* 		    iTooltipBody;	
    /// The tooltip-body bitmap mask. Owned
    CFbsBitmap* 		    iTooltipBodyMask;
	
    /// The top-section bitmap. Owned
    CFbsBitmap* iAPTopSection;
    /// The item background bitmap. Owned
    CFbsBitmap* iAPItem;
    /// The gap bitmap. Owned
    CFbsBitmap* iAPGap;
    /// The bottom-section bitmap. Owned
    CFbsBitmap* iAPBottomSection;
    
    /// The current rectangle that encompasses only the items
    TRect iItemsRect;
  };
    
#include "ActivePalette2Styler.inl"

#endif // #ifdef _ACTIVE_PALETTE_2_STYLER_H
