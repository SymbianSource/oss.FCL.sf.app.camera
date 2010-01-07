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
* Description:  
*
*/



#ifndef CAM_PERFORMANCELOGGER_H
#define CAM_PERFORMANCELOGGER_H


#include "camcameracontrollerflags.hrh"


#ifdef CAMERAAPP_PERFORMANCE_CONTROLLER
// ===========================================================================
_LIT( KCamPerformanceItemFormat, "%s duration: %d \n");

class TCamPerformanceItem
  {
  public:
    TCamPerformanceItem( TCamCameraRequestId aId )
      : iId( aId ), iStart( Time64() ), iStop( iStart ) {};

    TCamPerformanceItem( const TCamPerformanceItem& aOther )
      : iId( aOther.iId ), iStart( aOther.iStart ), iStop( aOther.iStop ) {};

    void Start() { iStart = Time64(); iStop = iStart; };
    void Stop()  { iStop  = Time64();                 };
  
    TInt64 Time64() const
      {
      TTime time;
      time.HomeTime();
      return time.Int64();
      };

    TInt Duration() const
      {
      return (TInt)(iStop - iStart);
      };

    const TCamCameraRequestId iId;
    TInt64                    iStart;
    TInt64                    iStop;
  };




class CCamPerformanceLogger : public CBase
  {
  public:
    CCamPerformanceLogger() {};
    ~CCamPerformanceLogger() 
      { 
      iData.Reset();
      iData.Close(); 
      };
  
    void Requested( const TCamCameraRequestId& aRequest )
      {
      TInt error = iData.Append( TCamPerformanceItem( aRequest ) );

      if( KErrNone != error )
        {
        // Ignored
        }
      };

    void Completed( const TCamCameraRequestId& aRequest )
      {
      // Set the end time for last performance item
      // found with this id.
      for( TInt i = iData.Count() - 1; i >= 0; i-- )
        {
        if( aRequest == iData[i].iId )
          {
          iData[i].Stop();
          return;
          }          
        }
      /// Ignore error if not found
      };

    TInt InfoSizeEstimate() const
      {
      return 50 * iData.Count();
      }

    void InfoText( TDes& aDes ) const
      {
      TInt count = iData.Count();
      for( TInt i = 0; i < count; i++ )
        {
        const TCamPerformanceItem& item = iData[i];
        // ECamRequestNone used to mark sequence start and end.
        if( item.iId >= ECamRequestNone && item.iId < ECamRequestLast )
          {
          aDes.AppendFormat( KCamPerformanceItemFormat, KCamRequestNames[item.iId], item.Duration() );
          }
        } 
      }

  private:
    RArray<TCamPerformanceItem> iData;
  };
// ===========================================================================
#endif // CAMERAAPP_PERFORMANCE_CONTROLLER

#endif // CAM_PERFORMANCELOGGER_H
