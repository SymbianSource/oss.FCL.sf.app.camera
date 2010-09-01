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
* Description:  Document class for Camera Application*
*/

#include "camlogging.h"
#include "CamLogger.h"

#ifdef _DEBUG
#include <e32debug.h>


//#define _AO_TRACKING
#ifdef _AO_TRACKING

static void ReplaceCurrentASL();
static void RestoreCurrentAS();

class CFilterScheduler;

class CFilterInfo : public CBase
    {
    friend class CFilterScheduler;

public:
    static CFilterInfo* NewL();

protected:
    struct CFilterStruct
        {
        CFilterStruct(CActive* aActiveAddPointer, char* aActiveLogInfo,
                CFilterStruct* aLink);
        CActive *iActiveAddPointer;
        char *iActiveLogInfo;
        CFilterStruct *iLink;
        };

protected:
    CFilterInfo();
    void ConstructL();
    void AddInfoL(CActive* aActiveAddPointer, char* aActiveLogInfo,
            CFilterStruct * &aLink);
    char* FindInfo(CActive *);
    ~CFilterInfo();
protected:
    struct CFilterStruct *iFiSHead;

    };

class CFilterScheduler : public CActiveScheduler
    {
    friend void ReplaceCurrentASL();
    friend void RestoreCurrentAS();

public:
    static CFilterScheduler* NewL();
private:
    static CActiveScheduler* iSavedFS;
public:
    using CActiveScheduler::Start;
    // It is used as a hook, which makes possible to collect meaningful
    // information about every active object
    virtual TInt Extension_(TUint aUid, TAny*& a0, TAny* a1);
protected:
    void ConstructL();
    CFilterScheduler();
    ~CFilterScheduler();

    void DebugPrint();
    char* FindInfo(CActive *);
    // it has to default to CActiveScheduler implementations  
    virtual void WaitForAnyRequest();
    // it has to default to CActiveScheduler implementations
    virtual void Error(TInt aError) const;
    // It has to be implemented since this is a concrete class
    virtual void OnStarting()
        {
        }
    virtual void OnStopping()
        {
        }
    virtual void Reserved_1()
        {
        }
    virtual void Reserved_2()
        {
        }
private:
    CFilterInfo *iFilterInfo;
    };

CActiveScheduler* CFilterScheduler::iSavedFS= NULL;

CFilterInfo* CFilterInfo::NewL()
    {
    CFilterInfo* self = new( ELeave ) CFilterInfo();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

void CFilterInfo::AddInfoL(CActive* aActiveAddPointer, char* aActiveLogInfo,
        CFilterInfo::CFilterStruct* &aLink)
    {
    PRINT( _L("Camera => CFilterInfo::AddInfoL") );
    CFilterStruct *pFiS = new( ELeave ) CFilterStruct( aActiveAddPointer,
            aActiveLogInfo, aLink );
    pFiS->iLink = aLink;
    aLink = pFiS;
    }

char* CFilterInfo::FindInfo(CActive *aActive)
    {
    char *pInfo = (char *)NULL;
    CFilterStruct *pFiS = iFiSHead;
    while (pFiS != NULL)
        {
        if (pFiS->iActiveAddPointer == aActive)
            {
            pInfo = pFiS->iActiveLogInfo;
            break;
            }
        pFiS = pFiS->iLink;
        }
    return pInfo;
    }

CFilterInfo::CFilterStruct::CFilterStruct(CActive* aActiveAddPointer,
        char* aActiveLogInfo, CFilterInfo::CFilterStruct* aLink)
    {
    iActiveAddPointer = aActiveAddPointer;
    iActiveLogInfo = aActiveLogInfo;
    iLink = aLink;
    }

void CFilterInfo::ConstructL()
    {
    }

CFilterInfo::CFilterInfo()
    {
    iFiSHead = NULL;
    }

CFilterInfo::~CFilterInfo()
    {
    }

CFilterScheduler* CFilterScheduler::NewL()
    {
    CFilterScheduler* self = new( ELeave ) CFilterScheduler();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

void ReplaceCurrentASL()
    {
    CFilterScheduler *fSH = CFilterScheduler::NewL();
    //if (fSH != NULL)
    CFilterScheduler::iSavedFS = CActiveScheduler::Replace(fSH);
    }

void RestoreCurrentAS()
    {

    if (CFilterScheduler::iSavedFS != NULL)
        {
        CActiveScheduler *iFS =
                CActiveScheduler::Replace(CFilterScheduler::iSavedFS);
        delete iFS;
        }

    }

TInt CFilterScheduler::Extension_(TUint aUid, TAny*& a0, TAny* a1)
    {
    if (aUid == 0)
        {
        TRAPD( rr, iFilterInfo->AddInfoL( (CActive *)a0, (char *)a1,
                        iFilterInfo->iFiSHead ) );
        return rr;
        }
    return KErrExtensionNotSupported;;
    }

void CFilterScheduler::WaitForAnyRequest()
    {
    // Here is the printout at every wait
    DebugPrint();
    if (iSavedFS)
        iSavedFS->WaitForAnyRequest();
    else
        User::WaitForAnyRequest();
    }

void CFilterScheduler::Error(TInt aError) const
    {
    if (iSavedFS)
        iSavedFS->Error(aError);
    }

CFilterScheduler::CFilterScheduler()
    {
    }

CFilterScheduler::~CFilterScheduler()
    {
    iSavedFS = NULL;
    }

void CFilterScheduler::ConstructL()
    {
    /* iActive = CFilterActive::NewL( this ); */
    iFilterInfo = CFilterInfo::NewL();
    }

char* CFilterScheduler::FindInfo(CActive * aA)
    {
    char *pFI= NULL;
    if (iFilterInfo)
        pFI = iFilterInfo->FindInfo(aA);
    return pFI;
    }

void CFilterScheduler::DebugPrint()
    {
    RDebug::Print( _L( " Camera => CFilterScheduler::DebugPrint" ));
    TDblQueIter<CActive> q(iActiveQ);
    q.SetToFirst();
    while (q != NULL)
        {
        CActive *pR = q++;
        /* TInt pri = pR->iLink.iPriority; */
        TInt pri = pR->Priority();
        TInt stat = pR->iStatus.Int(); // TRequestStatus
        // find active objects identification string otherwise
        // print it's address
        char *pText = FindInfo(pR);

        if (pR->IsActive() )
            {
            if (stat == KRequestPending)
                {
                // active, pending
                if (pText)
                    RDebug::Printf(
                            "Camera: CFilterScheduler::DebugPrint: %s - active/pending - priority: %d - req status: %d",
                            pText, pri, stat);
                else
                    RDebug::Printf(
                            "Camera: CFilterScheduler::DebugPrint: %x - active/pending - priority: %d - req status: %d",
                            (TInt)pR, pri, stat);
                }
            else
                {
                // active, not pending
                if (pText)
                    RDebug::Printf(
                            "Camera: CFilterScheduler::DebugPrint: %s - active/not pending - priority: %d - req status: %d",
                            pText, pri, stat);
                else
                    RDebug::Printf(
                            "Camera: CFilterScheduler::DebugPrint: %x - active/not pending - priority: %d - req status: %d",
                            (TInt)pR, pri, stat);
                }
            }
        else
            {
            // not active
            if (pText)
                RDebug::Printf(
                        "Camera: CFilterScheduler::DebugPrint: %s - not active: priority: %d - req status: %x",
                        pText, pri, stat);
            else
                RDebug::Printf(
                        "Camera: CFilterScheduler::DebugPrint: %x - not active: priority: %d - req status: %x",
                        (TInt)pR, pri, stat);
            }
        }
    RDebug::Print( _L( "Camera <= CFilterScheduler::DebugPrint" ));
    }
#endif // _AO_TRACKING
#endif // _DEBUG
// INCLUDE FILES
#include "CamAppUi.h"
#include "CamDocument.h"
#include "CamPerformance.h"
#include "CamAppController.h"

#ifdef _DEBUG
// These are used for memory allocation tracing.
#undef _MEMORY_TRACKING
//#define _MEMORY_TRACKING
#ifdef _MEMORY_TRACKING
#include <e32panic.h>


// stores the last RDebug::Print or RDebug::Printf calls
// string from cameraapp code if SInUse was ETrue
static char *sLastHeapContext = NULL;
// controls the context taking if it is EFalse sLastHeapContext is not updated
// used also by RTraceAllocator::Alloc, Free, Realloc etc.
static TBool sInUse = EFalse;

// forward declaration for friend statements.
class PtrAssocArray;
class RTraceAllocator;


// class which stores heap context info
// per allocation
// every allocation will have one entry in the hash table
class AllocHeapContext
    {
    // added as friend for writing less set/get methods
    friend class PtrAssocArray;
public:
    AllocHeapContext(TAny* aPtr, TInt aSize, char* aText1,
            AllocHeapContext *aNext = NULL);
    ~AllocHeapContext();
protected:
    // stored pointer of the allocation  
    TAny* iPtr;
    // stored size of the allocation
    TInt iSize;
    // context string of the allocation, collected from
    // RDebug::Print, RDebug::Printf calls
    char* iText1;
    // link field to the next context if they are more with same hash value
    // in PtrAssocArray then they can be linked into a single linked list
    AllocHeapContext *iNext;
    };

// class which stores the hash table
class PtrAssocArray
    {
public:
    // constructor takes a Heap to use for allocations and deallocations done by
    // this class,
    // Note: For not perturbing normal allocation/deallocation a separate heap is
    // used for internal needs. 
    PtrAssocArray(RHeap *aHeap);
    ~PtrAssocArray();
    // insert an element into the list
    // return KErrArgument if aAHC == NULL
    // otherwise KErrNone
    TInt Insert(AllocHeapContext* aAHC);
    // find an element in the list
    // return NULL pointer if cannot find, this means
    // also the case if aPtr == NULL  
    AllocHeapContext *Find(TAny *aPtr);
    // removes an element from the list
    // if aPtr == NULL does nothing and returns KErrArgument.
    // Returns KErrNotFound and does nothnig if cannot find it.
    // In normal case return KErrNone.
    TInt Remove(TAny *aPtr);
    // Prints the elements in the list
    void PrintLeaks();
    // cleans the list. It could be used if implemented for restarting
    // leak search from a certain point onward.
    // Note: only the list elements are cleaned, for real deallocation purposes
    // it cannot be used as this system doesn't offers a reference counting of
    // the pointer. It still can be used even that idea for debugging.  
    void CleanTables();
protected:
    // the hash function itself,
    // simply takes the middle 2 bytes of a 4 byte unsigned,
    // to which the pointer is converted  
    TUint Hash(TAny *aPtr);
    // internal structures for has array entries
    // iInfo points
    struct AssocHashTbl
        {
        AllocHeapContext* iInfo;
        };
    // the hash array
    AssocHashTbl iAHT[65536];
    // pointer to iOwnHeap of
    RHeap *iHeap;
    };

// the allocator class
class RTraceAllocator : public RAllocator
    {
public:
    // return current heap, just utility  
    static RTraceAllocator* OwnAllocator()
        {
        RTraceAllocator& currentHeap = (RTraceAllocator &)User::Heap();
        return &currentHeap;
        }
    // Simple utility function for getting a size of a char*
    // User::StringLength looks similar,
    // but this return 0 if aStr == NULL.
    static TUint StrSize(char * aStr);
    // stores the current heap for calls forwarding to that ones
    // implementation
    RTraceAllocator(RHeap *aCurrentHeap);
    // destructor sets sInUse to EFalse as iOwnHeap is not available any
    // more
    ~RTraceAllocator()
        {
        iOwnHeap->Close();
        sInUse = EFalse;
        }

    // These virtuals needs to be overwritten
    // there functionality is documented by Symbian
    virtual TAny* Alloc(TInt aSize);
    virtual void Free(TAny* aPtr);
    virtual TAny* ReAlloc(TAny* aPtr, TInt aSize, TInt aMode = 0);
    virtual TInt AllocLen(const TAny* aCell) const;
    virtual TInt Compress();
    virtual void Reset();
    virtual TInt AllocSize(TInt& aTotalAllocSize) const;
    virtual TInt Available(TInt& aBiggestBlock) const;
    virtual TInt DebugFunction(TInt aFunc, TAny* a1=NULL, TAny* a2=NULL);
    // print entries in hash table, it is just a proxy method 
    void PrintLeaks()
        {
        iAHM->PrintLeaks();
        }
    // clean the tables it is just a proxy
    void CleanTables()
        {
        iAHM->CleanTables();
        }
    // frees memory from own heap
    void FreeOwn(TAny* aPtr)
        {
        iOwnHeap->Free(aPtr);
        }
    // allocates from own heap and fills with 0
    TAny* AllocZOwn(TInt aSize)
        {
        return iOwnHeap->AllocZ(aSize);
        }
    // returns the saved heap
    RHeap* SavedHeap()
        {
        return iHeap;
        }

protected:
    // the original heap of the application, which does the real
    // allocation. All allocations are deferred to it  
    RHeap* iHeap;
    // A heap for own allocations, different than the one used by
    // cameraapp process 
    RHeap* iOwnHeap;
    // pointer to the hash table
    PtrAssocArray* iAHM;
    };


#if defined( _UNICODE )

// just, because they are needed
class TestOverflowTruncate : public TDes16Overflow
    {
public:
    virtual void Overflow(TDes16& /* aDes */)
        {
        }
    ;
    };

#else

class TestOverflowTruncate : public TDes8Overflow
    {
public:
    virtual void Overflow(TDes8 &aDes)
        {};
    };

#endif

// just, because they are needed
class TestOverflowTruncate8 : public TDes8Overflow
    {
public:
    virtual void Overflow(TDes8& /*aDes*/)
        {
        }
    };


// RDebug::Print is overwritten, because it is used
EXPORT_C TInt RDebug::Print( TRefByValue<const TDesC> aFmt,... )
    {
    TestOverflowTruncate overflow;
    VA_LIST list;
    VA_START(list,aFmt);
    TBuf<0x100> buf;
    buf.AppendFormatList( aFmt, list, &overflow );
#ifdef _UNICODE
    TPtr8 p( buf.Collapse() );

    // remember it
    char *context = ( char *)p.Ptr();
    if( sInUse )
        {
        if( sLastHeapContext )
            {
            //free it  
            RTraceAllocator::OwnAllocator()->FreeOwn( sLastHeapContext );
            sLastHeapContext = NULL;
            }

        sLastHeapContext = (char *)RTraceAllocator::OwnAllocator()->AllocZOwn( 0x100 );
        memcpy( sLastHeapContext, context, 0x100 );
        }
    // instead of Exec::DebugPrint((TAny*)&p, 0);
    // RawPrint is used
    // it could have been this hacked to, but it is not seen necessary
    // as recording context from Printf/Print seen as enough
    RDebug::RawPrint( p );
#else
    TPtr8 p( buf );
    // rember it
    sLastHeapContext = ( char *)p;
    RDebug::RawPrint( p );
#endif
    return 0;
    }

EXPORT_C void RDebug::Printf( const char* aFmt, ... )
    {
    TestOverflowTruncate8 overflow;
    VA_LIST list;
    VA_START( list,aFmt );
    TPtrC8 fmt( (const TText8*)aFmt );
    TBuf8<0x100> buf;
    buf.AppendFormatList(fmt, list, &overflow );
    TPtrC8 p( buf /* (TUint8 *)(buf.Ptr()), 0x100 */);

    // rember it
    char *context = ( char *)p.Ptr();
    if( sInUse )
        {
        if( sLastHeapContext )
            {
            //free it
            RTraceAllocator::OwnAllocator()->FreeOwn( sLastHeapContext );
            sLastHeapContext = NULL;
            }
        sLastHeapContext = (char *)RTraceAllocator::OwnAllocator()->AllocZOwn( 0x100 );
        memcpy( sLastHeapContext, context, 0x100 );
        }

    RDebug::RawPrint( p );
    }

// If some custom print wanted RawPrint could be hacked,
// just added for completness
#if 0
EXPORT_C void RDebug::RawPrint( const TDesC8& aDes )
    {
    // Exec::DebugPrint( ( TAny* )&aDes, 1 );
    // above should be replaced by custom print
    }

EXPORT_C void RDebug::RawPrint( const TDesC16& aDes )
    {
    TBuf8<0x100> aDes8;
    if( aDes.Length()>0x100 )
        {
        TPtrC ptr( aDes.Ptr(), 0x100 );
        aDes8.Copy( ptr );
        }
    else
    aDes8.Copy(aDes);
    // Exec::DebugPrint((TAny*)&aDes8, 1);
    // above should be replaced by custom print
    }
#endif

AllocHeapContext::AllocHeapContext(TAny* aPtr, TInt aSize, char* aText1,
        AllocHeapContext *aNext)
    {
    iPtr = aPtr;
    iSize = aSize;

    // but we need to copy here  
    if (aText1 == NULL)
        {
        iText1 = NULL;
        }
    else
        {
        iText1 = (char *)RTraceAllocator::OwnAllocator()->AllocZOwn( 0x100);
        if (iText1 != NULL)
            memcpy(iText1, aText1, 0x100);
        else
            {
            // just for testing
            // in this case 
            // there is no space to store additional info
            iText1 = iText1;
            }
        }

    iNext = aNext;
    }

AllocHeapContext::~AllocHeapContext()
    {
    if (iText1)
        {
        RTraceAllocator::OwnAllocator()->FreeOwn(iText1);
        iText1 = NULL;
        }
    }

PtrAssocArray::PtrAssocArray(RHeap *aHeap)
    {
    iHeap = aHeap; //points to in Traceallocator iOwnHeap
    // not necessary to fill with NULL pointers, as RHeap::Allocz 
    // does the job already 
    for (TUint i = 0; i < 65535; i++)
        {
        iAHT[i].iInfo = NULL;
        }
    }

PtrAssocArray::~PtrAssocArray()
    {
    }

// insert an element into the list
TInt PtrAssocArray::Insert(AllocHeapContext* aAHC)
    {
    if ( !aAHC)
        {
        return KErrArgument;
        }

    TUint hashVal = Hash(aAHC->iPtr);
    // no collision, add to the table
    if (iAHT[hashVal].iInfo == NULL)
        {
        iAHT[hashVal].iInfo = aAHC;
        }
    // collision, insert at the beginning of linked list.
    else
        {
        aAHC->iNext = iAHT[hashVal].iInfo;
        iAHT[hashVal].iInfo = aAHC;
        }
    return KErrNone;
    }

TUint PtrAssocArray::Hash(TAny *aPtr)
    {
    TUint val = (TUint)aPtr;
    // take the middle part as hash value 65536 
    // different values
    val = (val & 0x00FFFF00 ) >> 8;
    return val;
    }

// find an element in the list
AllocHeapContext *PtrAssocArray::Find(TAny *aPtr)
    {
    if ( !aPtr)
        return NULL;
    TUint hashVal = Hash(aPtr);
    AllocHeapContext *ptr = iAHT[hashVal].iInfo;
    if (ptr != NULL)
        {
        do
            {
            if (ptr->iPtr != aPtr)
                {
                ptr = ptr->iNext;
                continue;
                }
            else
                {
                return ptr;
                }
            }
        while (ptr);
        }
    return NULL;
    }

// remove an element from the list
TInt PtrAssocArray::Remove(TAny *aPtr)
    {
    if ( !aPtr)
        return KErrArgument;
    TUint hashVal = Hash(aPtr);
    AllocHeapContext *ptr = iAHT[hashVal].iInfo;
    if (ptr != NULL)
        {
        AllocHeapContext *prevPtr= NULL;
        do
            {
            if (ptr->iPtr != aPtr)
                {
                prevPtr = ptr;
                ptr = ptr->iNext;
                continue;
                }
            else
                {
                // found, we should unlink and free memory          
                if (prevPtr)
                    {
                    prevPtr->iNext = ptr->iNext;
                    }
                else
                    {
                    //This is first element in linked list.
                    if (ptr->iNext==NULL)
                        {
                        //List should cleared only if this is last element
                        iAHT[hashVal].iInfo = NULL;
                        }
                    else
                        {
                        iAHT[hashVal].iInfo = ptr->iNext;
                        }
                    }
                // call destructor
                ptr->AllocHeapContext::~AllocHeapContext();
                // deallocate
                iHeap->Free(ptr);

                return KErrNone;
                }
            }
        while (ptr);
        }
    return KErrNotFound;
    }

void PtrAssocArray::PrintLeaks()
    {
    for (TUint i = 0; i < 65536; i++)
        {
        AllocHeapContext *ptr = iAHT[i].iInfo;
        if ( !ptr)
            continue;
        do
            {
            if (ptr->iText1 != NULL)
                RDebug::Printf(
                        "Camera:PrintLeaks ptr: %x, size: %d, context: %s",
                        ptr->iPtr, ptr->iSize, ptr->iText1);
            else
                RDebug::Printf(
                        "Camera:PrintLeaks ptr: %x, size: %d, context: <<NO CONTEXT>>",
                        ptr->iPtr, ptr->iSize);
            ptr = ptr->iNext;
            }
        while (ptr);
        }
    }

void PtrAssocArray::CleanTables()
    {
    // Not really necessary to implement

    // disable code below
    // by replacing #if 1 by #if 0
#if 1
    for (TUint i = 0; i < 65536; i++)
        {
        AllocHeapContext *ptr = iAHT[i].iInfo;
        if ( !ptr)
            {
            //Don't continue...
            continue;
            }
        // deallocate the list    
        AllocHeapContext *nextPtr= NULL;
        while (ptr) //Is this ever true? 
            {
            nextPtr = ptr->iNext;
            // call destructor
            ptr->AllocHeapContext::~AllocHeapContext();
            // deallocate
            iHeap->Free(ptr);
            ptr = nextPtr;
            }
        }
#endif    
    }



RTraceAllocator::RTraceAllocator(RHeap *aCurrentHeap)
    {
    iHeap = aCurrentHeap;
    iOwnHeap = UserHeap::ChunkHeap( NULL, 1024 * 64, 1024*1024 * 2);
    iAHM = (PtrAssocArray *)iOwnHeap->AllocZ(sizeof(PtrAssocArray));
    ::new(iAHM) PtrAssocArray( iOwnHeap );
    sInUse = ETrue;
    }

TAny* RTraceAllocator::Alloc(TInt aSize)
    {
    TAny *tmp = iHeap->Alloc(aSize);

    sInUse = EFalse;
    RDebug::Printf("Camera: RTraceAllocator::Alloc %x, %d", ( TUint32 )tmp,
            aSize);
    sInUse = ETrue;
  
    if (tmp)
        {
  
    AllocHeapContext* ahc = (AllocHeapContext*) ( iOwnHeap->AllocZ(sizeof(AllocHeapContext)) ); 
        // text from last debug print is added
        // it can be extended to add more. Timing information for example.
        ::new(ahc) AllocHeapContext( tmp, aSize, sLastHeapContext );
        iAHM->Insert(ahc);
        }
    else
        {
        // can be commented out this, if failed allocations are not interesting
        // RDebug::Printf("M:Alloc %x, %d", (TUint32) tmp, aSize);
        }
    return tmp;
    }


void RTraceAllocator::Free(TAny* aPtr)
    {
    if (aPtr)
        {
        iAHM->Remove(aPtr);
        sInUse = EFalse;
        RDebug::Printf("RTraceAllocator::Free %x", ( TUint32 ) aPtr );
        sInUse = ETrue;
        iHeap->Free(aPtr);
        }
    }


TAny* RTraceAllocator::ReAlloc(TAny* aPtr, TInt aSize, TInt aMode)
    {
    TAny *tmp= NULL;

    if (aPtr == NULL)
        {
        if (aMode != ENeverMove)
            {
            tmp = RTraceAllocator::Alloc(aSize);

            sInUse = EFalse;
            RDebug::Printf("M:Realloc %x %x %d %x", (TUint32) tmp, aPtr,
                    aSize, aMode);
            sInUse = ETrue;
            return tmp;
            }
        else
            {
            // Now we are allocating a block which should never move...
            // Call below could panic USER:42
            tmp = iHeap->ReAlloc(aPtr, aSize, aMode);

            sInUse = EFalse;
            RDebug::Printf("M:Realloc %x %x %d %x", (TUint32) tmp, aPtr,
                    aSize, aMode);
            sInUse = ETrue;

            if (tmp)
                {
            AllocHeapContext *ahc = (AllocHeapContext*) ( iOwnHeap->AllocZ(sizeof(AllocHeapContext)) );
                // not yet handled if this kind of allocation above fails !
                // Add last debug print...
                ::new(ahc) AllocHeapContext( tmp, aSize, sLastHeapContext );
                iAHM->Insert(ahc);
                }
            return tmp;
            }
        }

    //aPtr is not NULL. So it is real reallocation or accidentally
    //pointing a cell -> PANIC
    tmp = iHeap->ReAlloc(aPtr, aSize, aMode);

    sInUse = EFalse;
    RDebug::Printf("M:Realloc %x %x %d %x", (TUint32) tmp, aPtr, aSize, aMode);
    sInUse = ETrue;

    if (tmp)
        {
        // Check if information of old allocation has been stored.
        if (iAHM->Find(aPtr) != NULL)
            {
            // remove the info
            iAHM->Remove(aPtr);
            }
        // store new information
      AllocHeapContext *ahc = (AllocHeapContext*) ( iOwnHeap->AllocZ(sizeof(AllocHeapContext)) );
        // not yet handled if this kind of allocation above fails !
        ::new(ahc) AllocHeapContext( tmp, aSize, sLastHeapContext );
        iAHM->Insert(ahc);
        }

    return tmp;
    }

TInt RTraceAllocator::AllocLen(const TAny* aCell) const
    {
    // Printing commented out because it cause big delay
    // when exiting application.
    /*
     sInUse = EFalse;
     RDebug::Printf( "Camera: RTraceAllocator::AllocLen" );
     sInUse = ETrue;
     */
    return iHeap->AllocLen(aCell);
    }

TInt RTraceAllocator::Compress()
    {
    sInUse = EFalse;
    RDebug::Printf("Camera: RTraceAllocator::Compress");
    sInUse = ETrue;
    //This doesn't require AllocHeapContext or iAHM because
    //Compress doesn't affect sizes of allocated area.
    return iHeap->Compress();
    }

void RTraceAllocator::Reset()
    {
    sInUse = EFalse;
    RDebug::Printf("Camera: RTraceAllocator::Reset");
    sInUse = ETrue;
    //This should also sync iAHM because otherwise iAHM is showing
    //freed cells as allocated. Currently not implemented.
    return iHeap->Reset();
    }

TInt RTraceAllocator::AllocSize(TInt& aTotalAllocSize) const
    {
    sInUse = EFalse;
    RDebug::Printf("Camera: RTraceAllocator::AllocSize %d", aTotalAllocSize);
    sInUse = ETrue;
    return iHeap->AllocSize(aTotalAllocSize);
    }

TInt RTraceAllocator::Available(TInt& aBiggestBlock) const
    {
    TInt t = iHeap->Available(aBiggestBlock);
    sInUse = EFalse;
    RDebug::Printf("Camera: RTraceAllocator::Available %d %d", aBiggestBlock,
            t);
    sInUse = ETrue;
    return t;
    }

TInt RTraceAllocator::DebugFunction(TInt /* aFunc */, TAny* /* a1 */, TAny* /* a2 */)
    {
    sInUse = EFalse;
    RDebug::Printf("Camera: RTraceAllocator::DebugFunction");
    sInUse = ETrue;
    return 0;
    }

#endif //_MEMORY_TRACKING
#endif //_DEBUG

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCamDocument::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCamDocument* CCamDocument::NewL( CEikApplication& aApp )
    {
    CCamDocument* self = NewLC( aApp );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CCamDocument::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCamDocument* CCamDocument::NewLC( CEikApplication& aApp )
    {
    CCamDocument* self = new( ELeave ) CCamDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

    
// Destructor
CCamDocument::~CCamDocument()
    {
    PRINT( _L("Camera => ~CCamDocument") );
#ifdef CAMERAAPP_PERF_LOG_TRACES	
    // Shutdown end event cannot be logged to memory logger
    PERF_EVENT_END_TRACE( EPerfEventApplicationShutdown );
#endif	

    delete iController;

    // print leaks info

#ifdef _DEBUG
  
#ifdef _MEMORY_TRACKING
    RTraceAllocator& alloc = (RTraceAllocator &)User::Heap();
    alloc.PrintLeaks();
    alloc.CleanTables();
    User::SwitchHeap(alloc.SavedHeap() );
    alloc.Close();
    delete &alloc;
#endif //_MEMORY_TRACKING
#ifdef _AO_TRACKING
    // restoring the original scheduler
    RestoreCurrentAS();
#endif // _AO_TRACKING
#endif //_DEBUG
    PRINT( _L("Camera <= ~CCamDocument") );
    }

// -----------------------------------------------------------------------------
// CCamDocument::CreateAppUiL
// Create the application user interface, and return a pointer to it
// the framework takes ownership of this object
// -----------------------------------------------------------------------------
//
CEikAppUi* CCamDocument::CreateAppUiL()
    {
#ifdef _DEBUG
#ifdef _AO_TRACKING
    // replacing the scheduler
    ReplaceCurrentASL();
#endif // _AO_TRACKING
#endif // _DEBUG
    return new( ELeave ) CCamAppUi( *iController ); 
    }

// -----------------------------------------------------------------------------
// CCamDocument::CCamDocument
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCamDocument::CCamDocument( CEikApplication& aApp )  
    : CAknDocument( aApp ) 
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CCamDocument::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCamDocument::ConstructL()
    {
#ifdef _DEBUG
#ifdef _MEMORY_TRACKING
    // New heap for memory leak information.
     RHeap& currentHeap = User::Heap();
     RTraceAllocator *alloc = new RTraceAllocator( &currentHeap );
     if( alloc )
       User::SwitchHeap( alloc );
     //End of memoryleak heap replacer.    
#endif //_MEMORY_TRACKING
#endif //_DEBUG
    
    PERF_EVENT_START_L1( EPerfEventAppFirstStartup );	
    // Create the controller.
    iController = CCamAppController::NewL();
    }

//  End of File  
