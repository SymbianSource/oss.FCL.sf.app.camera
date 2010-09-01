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
* Description:  Class for managing intercommunication between Camera UI*
*/


// INCLUDE FILES

#include <e32std.h>
#include <e32base.h> // TTime
#include <eikenv.h>
#include <s32file.h> // RFileWriteStream

#include "CamPerformance.h"
#include "CamAppUid.h" // KCameraappUID

#ifdef CAMERAAPP_PERF_LOG_MEMORY
// This file needs only be compiled if logging to memory is enabled

// INTERNAL CONSTANTS, ENUMS AND STRUCTS

// Constants related to logging in memory
const TInt KPerfLogArrayGranularity = 20;
const TInt KPerfMaxLogItemStringLength = 160;

// Constants for converting 64-bit system time to milliseconds
// and seconds
const TInt KDividerSystemToMilliseconds = 1000;
const TInt KDividerMillisecondsToSeconds = 1000;

// Constants for formatting memory log to text
_LIT( KPerfLogItemTab, "\t" );
_LIT8( KPerfLogItemCrLf8, "\n" );
_LIT( KPerfLogItemSecondsFormatSpace, "%6d.%03d" );
_LIT( KPerfLogItemSecondsFormat, "%d.%03d" );

// Constants for writing event analysis log
_LIT( KAnalysisEventType, "Event: %d" );
_LIT( KAnalysisEventStartTime, ", start time: " );
_LIT( KAnalysisEventEndTime, ", end time: " );
_LIT( KAnalysisEventDuration, ", duration: " );
_LIT( KAnalysisEventAlreadyStarted, "Start for event %d, which has already been started, time: " );
_LIT( KAnalysisEndWithoutStart, "End for event %d without start, time: " );
_LIT( KAnalysisStartWithoutEnd, "Start for event %d without end, time: " );

_LIT( KCamPerformanceLogger, "CamPerformanceLogger" );

/**
* Memory log item types
*/
enum TItemType
	{
	EItemEventStart,
	EItemEventEnd,
	EItemMessage,
	EItemEngineStateChange,
	EItemOperationStateChange
	};

/**
* TLogitem struct definition. Used for storing log items in memory
* in an array.
*/
struct TLogItem
	{	
	public:	
	TLogItem( TItemType aItemType, TInt aIntValue, 	TInt64 aTime ):
		iItemType( aItemType ), iIntValue( aIntValue), iTime( aTime ) {}
		
	TItemType iItemType;
	TInt iIntValue;
	TInt64 iTime;
	};	

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCamPerformanceLogger::CCamPerformanceLogger
// C++ default constructor can NOT contain any code, that
// might leave
// -----------------------------------------------------------------------------
//
CCamPerformanceLogger::CCamPerformanceLogger() :
	CCoeStatic( TUid::Uid( KCameraappUID ) ), iLogItems( KPerfLogArrayGranularity )
	{	
	iStartTime = Time64();
	}
	
// -----------------------------------------------------------------------------
// CCamPerformanceLogger::~CCamPerformanceLogger
// Destructor
// -----------------------------------------------------------------------------
//	
CCamPerformanceLogger::~CCamPerformanceLogger()
	{
  PRINT( _L("Camera => ~CCamPerformanceLogger") );
	if( iLogItems.Count() > 0 )
		{
		// Write files only if there are new log items
		// This is to avoid overwriting log already written using SaveAndReset()	
		TRAP_IGNORE( SaveLogDataL() );
		TRAP_IGNORE( SaveAnalysisL() );
		}
	iLogItems.Close();	
  PRINT( _L("Camera <= ~CCamPerformanceLogger") );
	}

// ---------------------------------------------------------------------------
// CCamPerformanceLogger::Logger
// Static function, which returns a pointer to the currently active 
// CCamPerformanceLogger object or instantiates a new one
// ---------------------------------------------------------------------------
//	
CCamPerformanceLogger* CCamPerformanceLogger::Logger()
	{	
	// Get pointer to the current CCamPerformanceLogger object using CCoeEnv.FindStatic()	
	CCamPerformanceLogger* self = static_cast<CCamPerformanceLogger*>( CCoeEnv::Static()->FindStatic( TUid::Uid( KCameraappUID ) ) );
	if( self )
		{
		return self;
		}
	else
		{
		// FindStatic returned null, create a new instance
		self = new CCamPerformanceLogger();
		if( !self )
			{
			// Not enough memory to instantiate CCamPerfomranceLogger
			User::Panic( KCamPerformanceLogger, KErrNoMemory );			
			}
		return self;
		}
	}

// ---------------------------------------------------------------------------
// CCamPerformanceLogger::SaveAndReset
// Static function, which saves the recoded log data and clears the log
// ---------------------------------------------------------------------------
//	
void CCamPerformanceLogger::SaveAndReset()
	{
	// Get pointer to the current CCamPerformanceLogger object using CCoeEnv.FindStatic()	
	CCamPerformanceLogger* self = static_cast<CCamPerformanceLogger*>( CCoeEnv::Static()->FindStatic( TUid::Uid( KCameraappUID ) ) );
	
	if( self && self->iLogItems.Count() > 0 )
		{
		TRAP_IGNORE( self->SaveLogDataL() );
		TRAP_IGNORE( self->SaveAnalysisL() );
		
		// Clear the logitems array
		self->iLogItems.Reset();
		}		
	}

// ---------------------------------------------------------------------------
// CCamPerformanceLogger::EventStart
// Appends an event start item to the memory log
// ---------------------------------------------------------------------------
//		
void CCamPerformanceLogger::EventStart( TCamEvent aEvent )
	{
	TLogItem item( EItemEventStart, aEvent, Time64() - iStartTime );
	iLogItems.Append( item ); // Ignore return value
	}

// ---------------------------------------------------------------------------
// CCamPerformanceLogger::EventEnd
// Appends an event end item to the memory log
// ---------------------------------------------------------------------------
//		
void CCamPerformanceLogger::EventEnd( TCamEvent aEvent )
	{
	TLogItem item( EItemEventEnd, aEvent, Time64() - iStartTime );
	iLogItems.Append( item );	// Ignore return value
	}

// ---------------------------------------------------------------------------
// CCamPerformanceLogger::Message
// Appends a message to the memory log
// ---------------------------------------------------------------------------
//		
void CCamPerformanceLogger::Message( TCamMessage aMessage )
	{
	TLogItem item( EItemMessage, aMessage, Time64() - iStartTime );
	iLogItems.Append( item );	// Ignore return value
	}

// ---------------------------------------------------------------------------
// CCamPerformanceLogger::EngineState
// Appends an engine state change to the memory log
// ---------------------------------------------------------------------------
//	
void CCamPerformanceLogger::EngineState( TInt aState )
	{
	TLogItem item( EItemEngineStateChange, aState, Time64() - iStartTime );
	iLogItems.Append( item );	// Ignore return value
	}
	
// ---------------------------------------------------------------------------
// CCamPerformanceLogger::OperationState
// Appends an operation state change to the memory log
// ---------------------------------------------------------------------------
//	
void CCamPerformanceLogger::OperationState( TInt aState )
	{
	TLogItem item( EItemOperationStateChange, aState, Time64() - iStartTime );
	iLogItems.Append( item );	// Ignore return value
	}	

// ---------------------------------------------------------------------------
// CCamPerformanceLogger::LogItemToDes
// Converts log item data into LogicAnalyzer compatible string, and stores 
// it in aDes
// ---------------------------------------------------------------------------
//		
void CCamPerformanceLogger::LogItemToDes( const TLogItem& aItem, TDes& aDes )
	{
	// Clear the descriptor contents
	aDes.Zero();
	
	// Append time of the log item and space
	TInt64 time = aItem.iTime;
	AppendTime( aDes, time, ETrue );
	aDes.Append( KPerfLogItemTab );
		
	// Append item type specific formatted string
	switch( aItem.iItemType ) 
		{
		case EItemEventStart:
			{
			aDes.AppendFormat( KPerfEventStart, aItem.iIntValue );
			}
			break;
		case EItemEventEnd:
			{
			aDes.AppendFormat( KPerfEventEnd, aItem.iIntValue );	
			}	
			break;
		case EItemMessage:
			{
			aDes.AppendFormat( KPerfMessage, aItem.iIntValue );	
			}
			break;
		case EItemEngineStateChange:
			{
			aDes.AppendFormat( KPerfEngineStateChange, aItem.iIntValue );	
			}
			break;
		case EItemOperationStateChange:
			{
			aDes.AppendFormat( KPerfOperationStateChange, aItem.iIntValue );	
			}
			break;			
		default:
			{
			aDes.AppendFormat( KPerfUnknown, aItem.iIntValue );	
			}
			break;
		}	
	}


// ---------------------------------------------------------------------------
// CCamPerformanceLogger::AppendTime
// Appends time represendted by aTime to aDes with format seconds.milliseconds
// ---------------------------------------------------------------------------
//	
void CCamPerformanceLogger::AppendTime( TDes& aDes, TInt64 aTime, TBool aSpace )
	{
	// Convert system time to milliseconds
	TInt64 timeInMillis = aTime / KDividerSystemToMilliseconds;
	
	// Get seconds and remainder (milliseconds)
	TInt seconds = timeInMillis / KDividerMillisecondsToSeconds;
	TInt milliseconds = timeInMillis % KDividerMillisecondsToSeconds;
	
	// Append seconds to the log item, with or without trailing space
	if( aSpace )
		{
		aDes.AppendFormat( KPerfLogItemSecondsFormatSpace, seconds, milliseconds );
		}
	else
		{
		aDes.AppendFormat( KPerfLogItemSecondsFormat, seconds, milliseconds );
		}
	}

// ---------------------------------------------------------------------------
// CCamPerformanceLogger::SaveLogDataL
// Saves all data from memory log to file KPerfLogFilename
// ---------------------------------------------------------------------------
//
void CCamPerformanceLogger::SaveLogDataL() const
	{ 	
	TBuf<KPerfMaxLogItemStringLength> itemDes;	
	
	// Connect to file server and create the output stream	
	RFs fs;
	User::LeaveIfError( fs.Connect() );
	CleanupClosePushL( fs );
	
	RFileWriteStream writeStream;
	User::LeaveIfError( writeStream.Replace( fs, KPerfLogFilename, EFileWrite ) );
	writeStream.PushL();
	
	// Convert each item to Des8 and write to the stream
	TInt n = iLogItems.Count();
	for( int i=0; i<n; i++ )
		{
		const TLogItem& item = iLogItems[i];
		LogItemToDes( item, itemDes );
		WriteLineL( writeStream, itemDes );
		}	
	
	// Commit and release the stream		
	writeStream.CommitL();
	writeStream.Pop();
	writeStream.Release();	
			
	CleanupStack::PopAndDestroy(); // fs
	}

// ---------------------------------------------------------------------------
// CCamPerformanceLogger::SaveAnalysisL
// Performs simple analysis to event data from memory log and writes the
// result to file KPerfAnalysisFilenam
// ---------------------------------------------------------------------------
//	
void CCamPerformanceLogger::SaveAnalysisL() const
	{ 	
	#ifdef CAMERAAPP_PERF_LOG_ANALYZE_EVENTS
		
	TBuf<KPerfMaxLogItemStringLength> itemDes;	

	TBool eventStatus[ EPerfEventLastEvent ];
	TInt64 startTimes[ EPerfEventLastEvent ];
	
	for( int i=0; i<EPerfEventLastEvent; i++)
		{
		eventStatus[i] = EFalse;
		}
		
	// Connect to file server and create the output stream	
	RFs fs;
	User::LeaveIfError( fs.Connect() );
	CleanupClosePushL( fs );
	
	RFileWriteStream writeStream;
	User::LeaveIfError( writeStream.Replace( fs, KPerfAnalysisFileName, EFileWrite ) );
	writeStream.PushL();	
	
	TInt n = iLogItems.Count();
	
	// Go through each item in the memory log
	for( TInt i=0; i<n; i++ )
	{
		const TLogItem& item = iLogItems[i];
		
		TInt intValue = item.iIntValue;
		TInt64 time = item.iTime;
		TBool status = eventStatus[ intValue ];
				
		if( EItemEventStart == item.iItemType )
			{
			if( !status )
				{
				// Start for an event that has not yet been start
				eventStatus[ intValue ] = ETrue;
				startTimes[ intValue ] = time;
				}
			else
				{
				// Start for an event that has already been started
				// Replace old start time with the new one
				startTimes[ intValue ] = time;
				#ifdef CAMERAAPP_PERF_ANALYSIS_WARN_MULTIPLE_START				
				itemDes.Format( KAnalysisEventAlreadyStarted, intValue );
				AppendTime( itemDes, time );
				WriteLineL( writeStream, itemDes );
				#endif	
				}
			}	
		else if( EItemEventEnd == item.iItemType )
			{
			if( status )	
				{
				// End for an event that has been started
				itemDes.Format( KAnalysisEventType, intValue );
				itemDes.Append( KAnalysisEventStartTime );
				AppendTime( itemDes, startTimes[ intValue ] );
				itemDes.Append( KAnalysisEventEndTime );
				AppendTime( itemDes, time );
				itemDes.Append( KAnalysisEventDuration );
				AppendTime( itemDes, time - startTimes[ intValue ] );
				WriteLineL( writeStream, itemDes );
					
				eventStatus[ intValue ] = EFalse;
				}
			else
				{	
				// End for an event that has not been started
				#ifdef CAMERAAPP_PERF_ANALYSIS_WARN_END_WITHOUT_START
					itemDes.Format( KAnalysisEndWithoutStart, intValue );
					AppendTime( itemDes, time );
					WriteLineL( writeStream, itemDes );
				#endif
				}				
			}
		else
			{
			// Ignore other event types
			}	
	}
	
	#ifdef CAMERAAPP_PERF_ANALYSIS_WARN_START_WITHOUT_END
	for( int i=0; i<EPerfEventLastEvent; i++ )
		{
		if( eventStatus[ i ] )
			{
			itemDes.Format( KAnalysisStartWithoutEnd, i );
			AppendTime( itemDes, startTimes[ i ] );
			WriteLineL( writeStream, itemDes );
			}
		}
	#endif
	
	// Commit and release the stream		
	writeStream.CommitL();
	writeStream.Pop();
	writeStream.Release();	
			
	CleanupStack::PopAndDestroy(); // fs
	
	#endif // CAMERAAPP_PERF_LOG_ANALYZE_EVENTS
	}


// ---------------------------------------------------------------------------
// CCamPerformanceLogger::WriteLineL
// Writes the contents of descriptor aDes followed by '\n' to aStream
// ---------------------------------------------------------------------------
//
void CCamPerformanceLogger::WriteLineL( RFileWriteStream& aStream, TDes& aDes )
	{
	TBuf8<KPerfMaxLogItemStringLength> des8;
	des8.Copy( aDes );
	aStream.WriteL( des8 );
	aStream.WriteL( KPerfLogItemCrLf8 );
	}	

// ---------------------------------------------------------------------------
// CCamPerformanceLogger::Time64
// Returns system 64-bit representation of the current time
// ---------------------------------------------------------------------------
//	
TInt64 CCamPerformanceLogger::Time64()
	{
	TTime time;
	time.HomeTime();
	return time.Int64();
	}

#endif // CAMERAAPP_PERF_LOG_MEMORY
		
//  End of File  

