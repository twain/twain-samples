/***************************************************************************
* Copyright © 2007 TWAIN Working Group:  
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
 * User interface for Twain Application
 * @author JFL Peripheral Solutions Inc.
 * @date October 2007
 */

#ifdef _WINDOWS
  #include "stdafx.h"
#endif
#include "Common.h"
#include "DSMInterface.h"

#include <iostream>
using namespace std;

#ifdef TWH_CMP_MSC
HMODULE
#else
void*
#endif
  gpDSM = 0;                    /**< global pointer to the DSM library */

DSMENTRYPROC gpDSM_Entry = 0;   /**< global pointer to the DSM entry point */

TW_ENTRYPOINT g_DSM_Entry   = {0};


// windows has builtin functions to alloc/free/lock/unlock but on Unix they
// are built into the DSM so they will have to be dlsym'd.
#ifdef TWH_CMP_GNU
  #include <dlfcn.h>
#endif

TW_UINT16 _DSM_Entry( pTW_IDENTITY _pOrigin,
                      pTW_IDENTITY _pDest,
                      TW_UINT32    _DG,
                      TW_UINT16    _DAT,
                      TW_UINT16    _MSG,
                      TW_MEMREF    _pData)
{
  TW_UINT16 ret = TWRC_FAILURE;

  if((0 == gpDSM) && !LoadDSMLib(kTWAIN_DS_DIR kTWAIN_DSM_DLL_NAME))
  {
    cerr << "Could not load the DSM: " << kTWAIN_DS_DIR kTWAIN_DSM_DLL_NAME << endl;
    return 0;   /*< return 0.  @todo are we sure of this return? */
  }

  if(0 != gpDSM_Entry)
  {
    ret = gpDSM_Entry(_pOrigin, _pDest, _DG, _DAT, _MSG, _pData);
  }

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
#ifdef TWH_CMP_GNU
  char *error;
#endif //TWH_CMP_MSC

  if((gpDSM=LOADLIBRARY(_pszLibName)) != 0)
  {
    if((gpDSM_Entry=(DSMENTRYPROC)LOADFUNCTION(gpDSM, "DSM_Entry")) == 0)
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
#endif //TWH_CMP_MSC
  }
  else
  {
    cerr << "Error - Could not load DSM: " << _pszLibName << endl;
#ifdef TWH_CMP_GNU
    cerr << "App - dlopen: " << dlerror() << endl;
#endif //TWH_CMP_MSC
    return false;
  }

#ifdef TWH_CMP_GNU

#endif

  return true;
}

/////////////////////////////////////////////////////////////////////////////
void unLoadDSMLib()
{
  if(gpDSM)
  {
    UNLOADLIBRARY(gpDSM);
    gpDSM = 0;
    gpDSM_Entry = 0;
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
#endif

  return 0;
}

//////////////////////////////////////////////////////////////////////////////
void _DSM_Free(TW_HANDLE _pPtr)
{
  if(g_DSM_Entry.DSM_MemFree)
  {
    return g_DSM_Entry.DSM_MemFree(_pPtr);
  }

#ifdef TWH_CMP_MSC
  ::GlobalFree(_pPtr);
#endif

  return;
}

//////////////////////////////////////////////////////////////////////////////
TW_MEMREF _DSM_LockMemory(TW_HANDLE _pMemory)
{
  if(g_DSM_Entry.DSM_MemLock)
  {
    return g_DSM_Entry.DSM_MemLock(_pMemory);
  }

#ifdef TWH_CMP_MSC
  return (TW_MEMREF)::GlobalLock(_pMemory);
#endif

  return 0;
}

//////////////////////////////////////////////////////////////////////////////
void _DSM_UnlockMemory(TW_MEMREF _pMemory)
{
  if(g_DSM_Entry.DSM_MemUnlock)
  {
    return g_DSM_Entry.DSM_MemUnlock(_pMemory);
  }

#ifdef TWH_CMP_MSC
  ::GlobalUnlock(_pMemory);
#endif

  return;
}

//////////////////////////////////////////////////////////////////////////////
