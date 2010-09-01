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
* Description:  Macros, event definitions and memory logging class header*
*/


#ifndef CAM_PERFORMANCE_H
#define CAM_PERFORMANCE_H

// INCLUDES
#include <e32base.h> // RDebug
#include <coemain.h> // CCoeStatic

// FORWARD DECLARATIONS
class TLogItem;
class RFileWriteStream;

// #define CAMERAAPP_PERFORMANCE_MEASUREMENT

/*
CAMERAAPP PERFORMANCE MEASUREMENT FRAMEWORK

If the flag CAMERAAPP_PERFORMANCE_MEASUREMENT is defined, the following
macros can be used for logging:

LEVEL 1 (High level):
PERF_EVENT_START_L1( EVENT )
PERF_EVENT_END_L1( EVENT )
PERF_MESSAGE_L1( MESSAGE )
PERF_ENGINE_STATE_CHANGE( STATE )
PERF_OPERATION_STATE_CHANGE( STATE )

LEVEL 2 (Intermediate level):  
PERF_EVENT_START_L2( EVENT )
PERF_EVENT_END_L2( EVENT )
PERF_MESSAGE_L2( MESSAGE )

LEVEL 3 (Low level):
PERF_EVENT_START_L3( EVENT )
PERF_EVENT_END_L3( EVENT )
PERF_MESSAGE_L3( MESSAGE )
*/

#ifdef CAMERAAPP_PERFORMANCE_MEASUREMENT
// Log levels
#define CAMERAAPP_PERF_L1 // High level logging enabled
#define CAMERAAPP_PERF_L2 // Intermediate level logging enabled
#define CAMERAAPP_PERF_L3 // Low level logging enabled

// Different methods of logging in use
// Memory log data is written to disk when application is closed
#define CAMERAAPP_PERF_LOG_TRACES // Logging to traces enabled
#define CAMERAAPP_PERF_LOG_MEMORY // Logging to memory enabled

#endif // CAMERAAPP_PERFORMANCE_MEASUREMENT


// Log data to traces using event names, instead of numeric event values
// For example: e_123_1 -> e_EEventName_1
#define CAMERAAPP_PERF_LOG_TRACES_AS_TEXT 

// Log filename for memory logs output
_LIT( KPerfLogFilename, "C:\\CameraPerf.log" );

// Perform analysis of event data after memory logging
#define CAMERAAPP_PERF_LOG_ANALYZE_EVENTS
_LIT( KPerfAnalysisFileName, "C:\\CameraPerfAnalysis.log" );

// Show warnings about invalid start/end events in event analysis log
#define CAMERAAPP_PERF_ANALYSIS_WARN_END_WITHOUT_START
#define CAMERAAPP_PERF_ANALYSIS_WARN_START_WITHOUT_END
#define CAMERAAPP_PERF_ANALYSIS_WARN_MULTIPLE_START

// Event definitions
enum TCamEvent 
	{
	EPerfEventAppFirstStartup = 0,
	EPerfEventApplicationShutdown,
	EPerfEventSwitchToStillMode,
	EPerfEventSwitchToVideoMode,
	EPerfEventKeyToCapture,
	EPerfEventShotToSnapshot,
	EPerfEventShotToStillImageReady,
	EPerfEventShotToSave,
	EPerfEventAutoFocus,
	EPerfEventSequenceCapture,
	EPerfEventStartVideoRecording,
	EPerfEventVideoStopToSave,
	EPerfEventBurstCaptureMomentToViewfinderFrame,
	EPerfEventAvkonUIConstruction,
	EPerfEventCAEConstruction,
	EPerfEventCAEInit,
	EPerfEventPreCaptureViewConstruction,
	EPerfEventActivePaletteConstruction,
	EPerfEventPrepareStill,
	EPerfEventPostCaptureViewConstruction,
	EPerfEventVideoPreCaptureViewActivation,
	EPerfEventStillPreCaptureViewDeactivation,
	EPerfEventPrepareVideo,
	EPerfEventVideoPreCaptureViewDeactivation,
	EPerfEventStillPreCaptureViewActivation,
	EPerfEventStillPostCaptureViewActivation,
	EPerfEventSaveImage,
	EPerfEventBurstThumbnailViewActivation,
	EPerfEventAppSubsequentStartup,
	EPerfEventLastEvent
	};

// Message definitions
enum TCamMessage
	{
	EPerfMessageTestMessage = 0,
	EPerfMessageStartingViewFinder,
	EPerfMessageActivePaletteAnimationStarting,
	EPerfMessageStoppingViewFinder,
	EPerfMessageBurstSnapshotReady,
	EPerfMessageBurstStillimageReady,
	EPerfMessageStartingRecord,
	EPerfMessagePausingViewFinder,
	EPerfMessageCaptureKeyHalfPressed,
	EPerfMessageLastMessage
	};

enum TCamPerformanceState
	{
	EPerfWaitingForStartup = 0,
	EPerfIdle,
	EPerfWaitingForStillMode,
	EPerfWaitingForVideoMode,
	EPerfWaitingForBurstFrame
	};
		
// Macros that handle the logging to traces
#ifdef CAMERAAPP_PERF_LOG_TRACES_AS_TEXT
	// Log enum arguments with their names, for example: e_EEventApplicationStartup_1
	#define PERF_EVENT_START_TRACE( EVENT ) RDebug::Print( KPerfEventStartText, &(_L( #EVENT )) );
	#define PERF_EVENT_END_TRACE( EVENT ) RDebug::Print( KPerfEventEndText, &(_L( #EVENT )) );
	#define PERF_MESSAGE_TRACE( EVENT ) RDebug::Print( KPerfMessageText, &(_L( #EVENT )) );
	#define PERF_ENGINE_STATE_CHANGE_TRACE( STATE ) RDebug::Print( KPerfEngineStateChange, STATE );
	#define PERF_OPERATION_STATE_CHANGE_TRACE( STATE ) RDebug::Print( KPerfOperationStateChange, STATE );
#else
	// Log enum arguments as their integer values, for example: e_123_1
	#define PERF_EVENT_START_TRACE( EVENT ) RDebug::Print( KPerfEventStart, EVENT );
	#define PERF_EVENT_END_TRACE( EVENT ) RDebug::Print( KPerfEventEnd, EVENT );
	#define PERF_MESSAGE_TRACE( EVENT ) RDebug::Print( KPerfMessage, EVENT );
	#define PERF_ENGINE_STATE_CHANGE_TRACE( STATE ) RDebug::Print( KPerfEngineStateChange, STATE );
	#define PERF_OPERATION_STATE_CHANGE_TRACE( STATE ) RDebug::Print( KPerfOperationStateChange, STATE );
#endif // CAMERAAPP_PERF_LOG_AS_TEXT

// Macros that handle logging to memory
#define PERF_EVENT_START_MEMORY( EVENT ) CCamPerformanceLogger::Logger()->EventStart( EVENT );
#define PERF_EVENT_END_MEMORY( EVENT ) CCamPerformanceLogger::Logger()->EventEnd( EVENT );
#define PERF_MESSAGE_MEMORY( EVENT ) CCamPerformanceLogger::Logger()->Message( EVENT );
#define PERF_ENGINE_STATE_CHANGE_MEMORY( STATE ) CCamPerformanceLogger::Logger()->EngineState( STATE );
#define PERF_OPERATION_STATE_CHANGE_MEMORY( STATE ) CCamPerformanceLogger::Logger()->OperationState( STATE );

// High level (L1) logging macro definitions
#ifdef CAMERAAPP_PERF_L1

	#if defined (CAMERAAPP_PERF_LOG_TRACES) && defined (CAMERAAPP_PERF_LOG_MEMORY)
		// Log to traces and memory
		#define PERF_EVENT_START_L1( EVENT ) PERF_EVENT_START_TRACE( EVENT ); PERF_EVENT_START_MEMORY( EVENT ); 																						
		#define PERF_EVENT_END_L1( EVENT) PERF_EVENT_END_TRACE( EVENT ); PERF_EVENT_END_MEMORY( EVENT ); 
		#define PERF_MESSAGE_L1( EVENT ) PERF_MESSAGE_TRACE( EVENT ); PERF_MESSAGE_MEMORY( EVENT );
		#define PERF_ENGINE_STATE_CHANGE( STATE ) PERF_ENGINE_STATE_CHANGE_TRACE( STATE ); PERF_ENGINE_STATE_CHANGE_MEMORY( STATE );
		#define PERF_OPERATION_STATE_CHANGE( STATE ) PERF_OPERATION_STATE_CHANGE_TRACE( STATE ); PERF_OPERATION_STATE_CHANGE_MEMORY( STATE );
		
	#elif defined (CAMERAAPP_PERF_LOG_TRACES)
		// Log only to traces
		#define PERF_EVENT_START_L1( EVENT ) PERF_EVENT_START_TRACE( EVENT );
		#define PERF_EVENT_END_L1( EVENT ) PERF_EVENT_END_TRACE( EVENT );
		#define PERF_MESSAGE_L1( EVENT) PERF_MESSAGE_TRACE( EVENT );
		#define PERF_ENGINE_STATE_CHANGE( STATE ) PERF_ENGINE_STATE_CHANGE_TRACE( STATE );
		#define PERF_OPERATION_STATE_CHANGE( STATE ) PERF_OPERATION_STATE_CHANGE_TRACE( STATE );
		
	#elif defined (CAMERAAPP_PERF_LOG_MEMORY)
		// Log only to memory
		#define PERF_EVENT_START_L1( EVENT ) PERF_EVENT_START_MEMORY( EVENT );
		#define PERF_EVENT_END_L1( EVENT ) PERF_EVENT_END_MEMORY( EVENT );
		#define PERF_MESSAGE_L1( EVENT) PERF_MESSAGE_MEMORY( EVENT );
		#define PERF_ENGINE_STATE_CHANGE( STATE ) PERF_ENGINE_STATE_CHANGE_MEMORY( STATE );		
		#define PERF_OPERATION_STATE_CHANGE( STATE ) PERF_OPERATION_STATE_CHANGE_MEMORY( STATE );	
		
	#else
		// Logging enabled, but no logging way specified - empty macro implementations
		#define PERF_EVENT_START_L1( A ) ;
		#define PERF_EVENT_END_L1( A ) ;
		#define PERF_MESSAGE_L1( A ) ;
		#define PERF_ENGINE_STATE_CHANGE( A ) ;
		#define PERF_OPERATION_STATE_CHANGE( A ) ;
	#endif

#else
	// L1 logging not enabled - empty macro implementation
	#define PERF_EVENT_START_L1( A ) ;
	#define PERF_EVENT_END_L1( A ) ;
	#define PERF_MESSAGE_L1( A ) ;
	#define PERF_ENGINE_STATE_CHANGE( A ) ;
	#define PERF_OPERATION_STATE_CHANGE( A ) ;
#endif // CAMERAAPP_PERF_L1

// Intermediate level (L2) logging macro definitions
#ifdef CAMERAAPP_PERF_L2

	#if defined (CAMERAAPP_PERF_LOG_TRACES) && defined (CAMERAAPP_PERF_LOG_MEMORY)
		// Log to traces and memory
		#define PERF_EVENT_START_L2( EVENT ) PERF_EVENT_START_TRACE( EVENT ); PERF_EVENT_START_MEMORY( EVENT ); 																						
		#define PERF_EVENT_END_L2( EVENT) PERF_EVENT_END_TRACE( EVENT ); PERF_EVENT_END_MEMORY( EVENT ); 
		#define PERF_MESSAGE_L2( EVENT ) PERF_MESSAGE_TRACE( EVENT ); PERF_MESSAGE_MEMORY( EVENT );
		#ifndef PERF_ENGINE_STATE_CHANGE
			#define PERF_ENGINE_STATE_CHANGE( STATE ) PERF_ENGINE_STATE_CHANGE_TRACE( STATE ); PERF_ENGINE_STATE_CHANGE_MEMORY( STATE );
			#define PERF_OPERATION_STATE_CHANGE( STATE ) PERF_OPERATION_STATE_CHANGE_TRACE( STATE ); PERF_OPERATION_STATE_CHANGE_MEMORY( STATE );
		#endif
		
	#elif defined (CAMERAAPP_PERF_LOG_TRACES)
		// Log only to traces
		#define PERF_EVENT_START_L2( EVENT ) PERF_EVENT_START_TRACE( EVENT );
		#define PERF_EVENT_END_L2( EVENT ) PERF_EVENT_END_TRACE( EVENT );
		#define PERF_MESSAGE_L2( EVENT) PERF_MESSAGE_TRACE( EVENT );
		#ifndef PERF_ENGINE_STATE_CHANGE 
			#define PERF_ENGINE_STATE_CHANGE( STATE ) PERF_ENGINE_STATE_CHANGE_TRACE( STATE );
			#define PERF_OPERATION_STATE_CHANGE( STATE ) PERF_OPERATION_STATE_CHANGE_TRACE( STATE );
		#endif
		
	#elif defined (CAMERAAPP_PERF_LOG_MEMORY)
		// Log only to memory
		#define PERF_EVENT_START_L2( EVENT ) PERF_EVENT_START_MEMORY( EVENT );
		#define PERF_EVENT_END_L2( EVENT ) PERF_EVENT_END_MEMORY( EVENT );
		#define PERF_MESSAGE_L2( EVENT) PERF_MESSAGE_MEMORY( EVENT );
		#ifndef PERF_ENGINE_STATE_CHANGE
			#define PERF_ENGINE_STATE_CHANGE( STATE ) PERF_ENGINE_STATE_CHANGE_MEMORY( STATE );
			#define PERF_OPERATION_STATE_CHANGE( STATE ) PERF_OPERATION_STATE_CHANGE_MEMORY( STATE );	
		#endif
		
	#else
		// Logging enabled, but no logging way specified - empty macro implementations
		#define PERF_EVENT_START_L2( A ) ;
		#define PERF_EVENT_END_L2( A ) ;
		#define PERF_MESSAGE_L2( A ) ;
		#ifndef PERF_ENGINE_STATE_CHANGE
			#define PERF_ENGINE_STATE_CHANGE( A ) ;
			#define PERF_OPERATION_STATE_CHANGE( A ) ;
		#endif
	#endif

#else
	// L2 logging not enabled - empty macro implementation
	#define PERF_EVENT_START_L2( A ) ;
	#define PERF_EVENT_END_L2( A ) ;
	#define PERF_MESSAGE_L2( A ) ;
		#ifndef PERF_ENGINE_STATE_CHANGE
			#define PERF_ENGINE_STATE_CHANGE( A ) ;
			#define PERF_OPERATION_STATE_CHANGE( A ) ;
		#endif
#endif // CAMERAAPP_PERF_L2

// Low level (L3) logging macro definitions
#ifdef CAMERAAPP_PERF_L3

	#if defined (CAMERAAPP_PERF_LOG_TRACES) && defined (CAMERAAPP_PERF_LOG_MEMORY)
		// Log to traces and memory
		#define PERF_EVENT_START_L3( EVENT ) PERF_EVENT_START_TRACE( EVENT ); PERF_EVENT_START_MEMORY( EVENT ); 																						
		#define PERF_EVENT_END_L3( EVENT) PERF_EVENT_END_TRACE( EVENT ); PERF_EVENT_END_MEMORY( EVENT ); 
		#define PERF_MESSAGE_L3( EVENT ) PERF_MESSAGE_TRACE( EVENT ); PERF_MESSAGE_MEMORY( EVENT );
		#ifndef PERF_ENGINE_STATE_CHANGE
			#define PERF_ENGINE_STATE_CHANGE( STATE ) PERF_ENGINE_STATE_CHANGE_TRACE( STATE ); PERF_ENGINE_STATE_CHANGE_MEMORY( STATE );
			#define PERF_OPERATION_STATE_CHANGE( STATE ) PERF_OPERATION_STATE_CHANGE_TRACE( STATE ); PERF_OPERATION_STATE_CHANGE_MEMORY( STATE );
		#endif
		
	#elif defined (CAMERAAPP_PERF_LOG_TRACES)
		// Log only to traces
		#define PERF_EVENT_START_L3( EVENT ) PERF_EVENT_START_TRACE( EVENT );
		#define PERF_EVENT_END_L3( EVENT ) PERF_EVENT_END_TRACE( EVENT );
		#define PERF_MESSAGE_L3( EVENT) PERF_MESSAGE_TRACE( EVENT );
		#ifndef PERF_ENGINE_STATE_CHANGE 
			#define PERF_ENGINE_STATE_CHANGE( STATE ) PERF_ENGINE_STATE_CHANGE_TRACE( STATE );
			#define PERF_OPERATION_STATE_CHANGE( STATE ) PERF_OPERATION_STATE_CHANGE_TRACE( STATE );
		#endif
		
	#elif defined (CAMERAAPP_PERF_LOG_MEMORY)
		// Log only to memory
		#define PERF_EVENT_START_L3( EVENT ) PERF_EVENT_START_MEMORY( EVENT );
		#define PERF_EVENT_END_L3( EVENT ) PERF_EVENT_END_MEMORY( EVENT );
		#define PERF_MESSAGE_L3( EVENT) PERF_MESSAGE_MEMORY( EVENT );
		#ifndef PERF_ENGINE_STATE_CHANGE
			#define PERF_ENGINE_STATE_CHANGE( STATE ) PERF_ENGINE_STATE_CHANGE_MEMORY( STATE );
			#define PERF_OPERATION_STATE_CHANGE( STATE ) PERF_OPERATION_STATE_CHANGE_MEMORY( STATE );	
		#endif
		
	#else
		// Logging enabled, but no logging way specified - empty macro implementations
		#define PERF_EVENT_START_L3( A ) ;
		#define PERF_EVENT_END_L3( A ) ;
		#define PERF_MESSAGE_L3( A ) ;
		#ifndef PERF_ENGINE_STATE_CHANGE
			#define PERF_ENGINE_STATE_CHANGE( A ) ;
			#define PERF_OPERATION_STATE_CHANGE( A ) ;
		#endif
		
	#endif

#else
	// L3 logging not enabled - empty macro implementation
	#define PERF_EVENT_START_L3( A ) ;
	#define PERF_EVENT_END_L3( A ) ;
	#define PERF_MESSAGE_L3( A ) ;
	#ifndef PERF_ENGINE_STATE_CHANGE
		#define PERF_ENGINE_STATE_CHANGE( A ) ;
		#define PERF_OPERATION_STATE_CHANGE( A ) ;
	#endif
#endif // CAMERAAPP_PERF_L3

#ifdef CAMERAAPP_PERF_LOG_MEMORY

/**
* Handles memory logging of events, messages and state changes, performing 
* simple event analysis and writing the resulting log files to disk.
*
*  @since 2.8
*/
class CCamPerformanceLogger: public CCoeStatic
	{
	public:
							
	  /**
    * Static function, which returns a pointer to the currently
    * active CCamPerformanceLogger object or instantiates a new one
    * @since 2.8
    * @return pointer to CCamPerformanceLogger object
    */    
		static CCamPerformanceLogger* Logger();		

	  /**
    * Static function, which saves the currently recorded log 
    * data and clears the log.
    * @since 2.8
    */ 		
		static void SaveAndReset();		
								
	  /**
    * Destructor.
    * @since 2.8
    */
		~CCamPerformanceLogger();
						
		/**
    * Appends an event start item to the memory log
    * @since 2.8
    * @param aEvent Event type
    */
		void EventStart( TCamEvent aEvent );
		
		/**
    * Appends an event end item to the memory log
    * @since 2.8
    * @param aEvent Event type
    */				
		void EventEnd( TCamEvent aEvent );
		
		/**
    * Appends a message to the memory log
    * @since 2.8
    * @param aMessage Message type
    */
		void Message( TCamMessage aMessage );
		
		/**
    * Appends a state change to the memory log
    * @since 2.8
    * @param aState New state
    */		
		void EngineState( TInt aState );
		
		/**
    * Appends a state change to the memory log
    * @since 2.8
    * @param aState New state
    */		
		void OperationState( TInt aState );
		
		/**
    * Saves all data from memory log to file KPerfLogFilename
    * @since 2.8
    */
		void SaveLogDataL() const;
		
		/**
    * Performs simple analysis to event data from memory log and writes
    * the result to file KPerfAnalysisFilename
    * @since 2.8
    */
		void SaveAnalysisL() const;
						
	private:
		/**
    * Converts log item data into LogicAnalyzer compatible string, and stores the result in aDes
    * @param aItem Log item
    * @param aDes Descriptor
    * @since 2.8
    */
		static void LogItemToDes( const TLogItem& aItem, TDes& aDes ); 		
		
		/**
    * Appends time represented by aTime to aDes with format seconds.milliseconds
    * @param aDes Destination descriptor
    * @param aTime Time in system 64-bit format
    * @param aSpace Reserve 6 characters for seconds field
    * @since 2.8
    */
		static void AppendTime( TDes& aDes, TInt64 aTime, TBool aSpace=EFalse );
		
		/**
    * Writes the contents of descriptor aDes followed by '\n' to aStream
    * @param aStream RFileWriteStream
    * @param aDes Descriptor
    * @since 2.8
    */
		static void WriteLineL( RFileWriteStream& aStream, TDes& aDes );
		
		/**
    * Returns system 64-bit representation of the current time
    * @since 2.8
    * @return Time
    */		
		static TInt64 Time64();
		
	private:
		CCamPerformanceLogger();
		TInt64 iStartTime;
		RArray<TLogItem> iLogItems;			
	};
#endif // CAMERAAPP_PERF_LOG_MEMORY
	
// Log item formatting literals
_LIT( KPerfEventStart, "e_%d_1" );
//_LIT( KPerfEventStartText, "e_%S_1" );
_LIT( KPerfEventStartText, "e_%S 1" );
_LIT( KPerfEventEnd, "e_%d_0" );
//_LIT( KPerfEventEndText, "e_%S_0" );
_LIT( KPerfEventEndText, "e_%S 0" );
_LIT( KPerfMessage, "m_%d" );
//_LIT( KPerfMessageText, "m_%S" );
_LIT( KPerfMessageText, "m_%S;CamMsg;CamMsg" );
//_LIT( KPerfEngineStateChange, "sm_CamEngine_%d" );
_LIT( KPerfEngineStateChange, "sm_CamEngine;%d" );
_LIT( KPerfEngineStateChangeText, "sm_CamEngine_%S" );
//_LIT( KPerfOperationStateChange, "sm_CamOperation_%d" );
_LIT( KPerfOperationStateChange, "sm_CamOperation;%d" );
_LIT( KPerfOperationStateChangeText, "sm_CamOperation_%S" );
_LIT( KPerfUnknown, "unknown_%d" );
	
#endif // CAM_PERFORMANCE_H