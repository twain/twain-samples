/***************************************************************************
* Copyright � 2007 TWAIN Working Group:  
*   Adobe Systems Incorporated, AnyDoc Software Inc., Eastman Kodak Company, 
*   Fujitsu Computer Products of America, JFL Peripheral Solutions Inc., 
*   Ricoh Corporation, and Xerox Corporation.
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the TWAIN Working Group nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY TWAIN Working Group ``AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL TWAIN Working Group BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
***************************************************************************/

/**
* @file DSMInterface.cpp
* Common defines and typedefs used for accessing DSM for Twain Data Sources.
* @author TWAIN Working Group
* @date April 2007
*/


#include "DSMInterface.h"

#include <iostream>
using namespace std;

/**
* gloabal pointer to the Data Source Manager
*/
HMODULE gpDSM = 0;

/**
* gloabal pointer to the Data Source Manager's Entry point 
* - cleared & init'd in @see LoadDSMLib
*/
TW_ENTRYPOINT g_DSM_Entry = 
{
  0,// TW_UINT32         Size;
  0,// DSMENTRYPROC      DSM_Entry;
  0,// DSM_MEMALLOCATE   DSM_MemAllocate;
  0,// DSM_MEMFREE       DSM_MemFree;
  0,// DSM_MEMLOCK       DSM_MemLock;
  0 // DSM_MEMUNLOCK     DSM_MemUnlock;
};

#ifdef TWH_CMP_GNU
  #include <dlfcn.h>
#endif

#ifdef TWNDS_OS_APPLE
//  #include "CarbonCore/MacMemory.h"
#include <Carbon/Carbon.h>
#endif

/**
* This is the same as the main DS_Entry function. Routes traffic
* to the proper location.
* @param[in] _pOrigin Identifies the source DS for the message.
* @param[in] _pDest   Identifies the destination application for the message.
* @param[in] _DG      The Data Group. 
* @param[in] _DAT     The Data Attribute Type.
* @param[in] _MSG     The message with respect to the Data Group and the Data Attribute Type.
* @param[in,out] _pData   A pointer to the data structure or variable identified 
*                     by the Data Attribute Type.
* @return a valid TWRC_xxxx return code.
*/
TW_UINT16 _DSM_Entry( pTW_IDENTITY _pOrigin,
                      pTW_IDENTITY _pDest,
                      TW_UINT32    _DG,
                      TW_UINT16    _DAT,
                      TW_UINT16    _MSG,
                      TW_MEMREF    _pData)
{
  TW_UINT16 ret = TWRC_FAILURE;

  // With DSM2 we do not need to load the DSM.  We should have recieved
  // Message with the entry points that we use.
  // On windows with DSM1(twain_32.dll) we will need to first laod the dll
  // then find the entry point First check to see if you have an entry point.
  // On Mac we will use the dynamicaly linked at build time dll.

  if(0 == g_DSM_Entry.DSM_Entry)
  {
     // We do not already have the entry point for regisry callback
     
#ifdef TWNDS_OS_APPLE
     // This should only happen if not being called by the DSM2
     // Other words only on Mac with an older DSM
     // So we use the old dll
     g_DSM_Entry.DSM_Entry = DSM_Entry;
    // ret = DSM_Entry(_pOrigin, _pDest, _DG, _DAT, _MSG, _pData);   
     
#elif defined (TWH_CMP_MSC)
     
      // This should only happen if not being called by the DSM2
      // Other words only on Windows with an older DSM
      // So we load the old dll
	    char DSMName[MAX_PATH];

	    memset(DSMName, 0, MAX_PATH*sizeof(char));

      if(GetWindowsDirectory (DSMName, MAX_PATH)==0)
      {
        DSMName[0]=0;
      }
#if (TWNDS_CMP_VERSION >= 1400)
	    if (DSMName[strlen(DSMName)-1] != '\\')
	    {
		    strcat_s(DSMName,MAX_PATH, "\\");
	    }		
	    strcat_s (DSMName,MAX_PATH, "TWAIN_32.dll");
#else
	    if (DSMName[strlen(DSMName)-1] != '\\')
	    {
		    strcat(DSMName, "\\");
	    }		
	    strcat(DSMName, "TWAIN_32.dll");
#endif

      if((0 == gpDSM) && !LoadDSMLib(DSMName))
      {
        cerr << "Could not load the DSM" << endl;
        return TWRC_FAILURE;
      }
#endif // TWNDS_OS_APPLE & TWH_CMP_MSC
  }
   
  if(0 == g_DSM_Entry.DSM_Entry)
  {
    cerr << "No Entry Point for DSM_Entry" << endl;
    return TWRC_FAILURE;
  }
   
  // If we did not have an enty point before we do now.
  ret = g_DSM_Entry.DSM_Entry(_pOrigin, _pDest, _DG, _DAT, _MSG, _pData);   
   
  return ret;
}

/////////////////////////////////////////////////////////////////////////////
bool LoadDSMLib(char* _pszLibName)
{
  // check if already opened
  if(0 != gpDSM)
  {
    return true;
  }

  memset(&g_DSM_Entry, 0, sizeof(TW_ENTRYPOINT));

#ifdef TWH_CMP_GNU
  char *error;
#endif //TWH_CMP_GNU

  if((gpDSM=LOADLIBRARY(_pszLibName)) != 0)
  {
    if((g_DSM_Entry.DSM_Entry=(DSMENTRYPROC)LOADFUNCTION(gpDSM, "DSM_Entry")) == 0)
    {
#ifdef TWH_CMP_MSC // dlsym returning NULL is not an error on Unix
      cerr << "Error - Could not find DSM_Entry function in DSM: " << _pszLibName << endl;
      return false; 
#endif //TWH_CMP_MSC
    }
#ifdef TWH_CMP_GNU
    if ((error = dlerror()) != 0)
    {
      cerr << "App - dlsym: " << error << endl;
      return false;
    }
#endif //TWH_CMP_GNU
  }
  else
  {
    cerr << "Error - Could not load DSM: " << _pszLibName << endl;
#ifdef TWH_CMP_GNU
    cerr << "App - dlopen: " << dlerror() << endl;
#endif //TWH_CMP_GNU
    return false;
  }


  return true;
}

/////////////////////////////////////////////////////////////////////////////
void unLoadDSMLib()
{
  if(gpDSM)
  {
    memset(&g_DSM_Entry, 0, sizeof(TW_ENTRYPOINT));
    UNLOADLIBRARY(gpDSM);
    gpDSM = 0;
  }
}

/////////////////////////////////////////////////////////////////////////////
void setEntryPoints(pTW_ENTRYPOINT _pEntryPoints)
{
  if(_pEntryPoints)
  {
    g_DSM_Entry = *_pEntryPoints;
  }
  else
  {
    memset(&g_DSM_Entry, 0, sizeof(TW_ENTRYPOINT));
  }
}

//////////////////////////////////////////////////////////////////////////////
// The following functions are defined in the DSM2,
// For backwards compatibiltiy on windows call the default function
TW_HANDLE _DSM_Alloc(TW_UINT32 _size)
{
  if(g_DSM_Entry.DSM_MemAllocate)
  {
    return g_DSM_Entry.DSM_MemAllocate(_size);
  }

#ifdef TWH_CMP_MSC
  return ::GlobalAlloc(GPTR, _size);
#elif defined (TWNDS_OS_APPLE)
  return NewHandle(_size);
#endif

  return 0;
}

//////////////////////////////////////////////////////////////////////////////
void _DSM_Free(TW_HANDLE _hMemory)
{
  if(g_DSM_Entry.DSM_MemFree)
  {
    return g_DSM_Entry.DSM_MemFree(_hMemory);
  }

#ifdef TWH_CMP_MSC
  ::GlobalFree(_hMemory);
#elif defined (TWNDS_OS_APPLE)
  DisposeHandle(_hMemory);
#endif

  return;
}

//////////////////////////////////////////////////////////////////////////////
TW_MEMREF _DSM_LockMemory(TW_HANDLE _hMemory)
{
  if(g_DSM_Entry.DSM_MemLock)
  {
    return g_DSM_Entry.DSM_MemLock(_hMemory);
  }

#ifdef TWH_CMP_MSC
  return (TW_MEMREF)::GlobalLock(_hMemory);
#elif defined (TWNDS_OS_APPLE)
   return (TW_MEMREF)(*_hMemory);
#endif

  return 0;
}

//////////////////////////////////////////////////////////////////////////////
void _DSM_UnlockMemory(TW_HANDLE _hMemory)
{
  if(g_DSM_Entry.DSM_MemUnlock)
  {
    return g_DSM_Entry.DSM_MemUnlock(_hMemory);
  }

#ifdef TWH_CMP_MSC
  ::GlobalUnlock(_hMemory);
#elif defined (TWNDS_OS_APPLE)
  // do nothing
#endif

  return;
}

//////////////////////////////////////////////////////////////////////////////
