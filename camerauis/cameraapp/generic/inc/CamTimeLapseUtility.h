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
* Description:  Setting page for Image/Video quality.*
*/


#ifndef CAMTIMELAPSEUTILITY_H
#define CAMTIMELAPSEUTILITY_H

//  INCLUDES

#include "CamSettingsInternal.hrh"  // TCamTimeLapse

// CLASS DECLARATION

class CamTimeLapseUtility
	{
	
	public:	
  	/**
  	* Convert from enum to the capture mode to be used
  	* @since 3.0
  	* @param aEnum enum value to translate
  	* @return 
  	*/ 
  	static TInt EnumToCommand( TCamTimeLapse aEnum ); 		
  
  	/**
  	* Convert from enum to the microsecond interval it represents
  	* @since 3.0
	  * @param aEnum Enum value to translate
  	*/
  	static TTimeIntervalMicroSeconds EnumToInterval( TCamTimeLapse aEnum );
        
  	/**
  	* Convert from microsecond interval to the enum that represents it
  	* @since 3.0
  	* @param aInterval Interval in microseconds to translate
  	* @param aBurst
  	*/
  	static TCamTimeLapse IntervalToEnum( TTimeIntervalMicroSeconds aInterval, TBool aBurst );      
  
	};
	
#endif // CAMTIMELAPSEUTILITY_H

