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
* @file CTWAINDS_Sample1.cpp
* Reveals the main entry point for the DSM 
* @author TWAIN Working Group
* @date April 2007
*/

#include "CTWAINDS_Sample1.h"
#include <list>
//////////////////////////////////////////////////////////////////////////////
// Globals
/**
* gloabal pointer of the Data Source, for access to the main DS_Entry.
*/
typedef struct _DS_inst
{
  TW_IDENTITY AppId;
  CTWAINDS_Base *pDS; 
}DS_inst;

typedef list<DS_inst> lstDS;
lstDS g_lstDS;

#ifdef TWH_CMP_MSC
  /** 
  * gloadbal Windows Instance handle for the DSM DLL... 
  */
  HINSTANCE   g_hinstance     = 0;
#endif

//////////////////////////////////////////////////////////////////////////////
// This is the main entry point. This function is dlsym'd by the DSM.

#ifdef TWH_CMP_MSC
TW_UINT16 FAR PASCAL
#else
FAR PASCAL TW_UINT16 
#endif
DS_Entry( pTW_IDENTITY _pOrigin,
          TW_UINT32    _DG,
          TW_UINT16    _DAT,
          TW_UINT16    _MSG,
          TW_MEMREF    _pData)
{
  CTWAINDS_Base* pTWAINLayer = 0;

  if(_pOrigin)
  {
    lstDS::iterator llIter=g_lstDS.begin();
    for(;llIter!=g_lstDS.end();llIter++)
    {
      if((*llIter).AppId.Id==_pOrigin->Id)
      {
        pTWAINLayer=(*llIter).pDS;
      }
    }
  }
  // Curently we are not open
  if( 0 == pTWAINLayer )
  {
    // Special case DSM can request to get identity information about 
    // DS before it is open.  In this special case, where the DS is not
    // open, we return this static Idenity.
    if( DG_CONTROL == _DG && DAT_IDENTITY == _DAT && MSG_GET == _MSG )
    {
      // Copy the ID assigned by the DSM eventhough the spec states
      // that the id will not be assigned until MSG_OPENDS
      CTWAINDS_Base::m_TheIdentity.Id = ((pTW_IDENTITY)_pData)->Id;
      memcpy( _pData, &CTWAINDS_Base::m_TheIdentity, sizeof(CTWAINDS_Base::m_TheIdentity) );

      return TWRC_SUCCESS;
    }

    // The DS is not open.  If we get a request to close DS do not open 
    // just to close, instead return that it is success closed.
    if( DG_CONTROL == _DG && DAT_IDENTITY == _DAT && MSG_CLOSEDS == _MSG )
    {
      return TWRC_SUCCESS;
    }

    // Open the DS
    pTWAINLayer = new CTWAINDS_FreeImage(*_pOrigin);
    if( NULL == pTWAINLayer 
     || TWRC_SUCCESS != pTWAINLayer->Initialize())
    {
      // Failed to create the DS 
      //setConditionCode(TWCC_LOWMEMORY);
      if(pTWAINLayer)
      {
        // Created but could not Initialize
        delete pTWAINLayer;
      }
      return TWRC_FAILURE;
    }
    DS_inst _DS;
    _DS.pDS = pTWAINLayer;
    _DS.AppId = *_pOrigin;
    g_lstDS.push_back(_DS);

  }

  // If we were not open before, we are now, so continue with the TWAIN call
  TW_INT16 result = pTWAINLayer->DS_Entry(_pOrigin, _DG, _DAT, _MSG, _pData);

  /**
  * Special case - free memory if closing DS 
  * @todo keep track of what apps are connecting to the ds and only 
  * delete when count goes down to 0
  */
  if( TWRC_SUCCESS == result && 
      DG_CONTROL == _DG && DAT_IDENTITY == _DAT && MSG_CLOSEDS == _MSG &&
      NULL != pTWAINLayer )
  {
    lstDS::iterator llIter=g_lstDS.begin();
    for(;llIter!=g_lstDS.end();)
    {
      if((*llIter).AppId.Id==_pOrigin->Id)
      {
        delete  (*llIter).pDS;
        llIter = g_lstDS.erase(llIter);
        continue;
      }
      llIter++;
    }
  }

  return result;
}

//////////////////////////////////////////////////////////////////////////////


#ifdef TWH_CMP_MSC
/**
* DllMain is only needed for Windows, and it's only needed to collect
* our instance handle, which is also our module handle.  Don't ever
* put anything else in here, not even logging messages.  It just isn't
* safe...
*/
BOOL WINAPI DllMain(HINSTANCE _hmodule,
                    DWORD     _dwReasonCalled,
                    LPVOID)
{
  switch (_dwReasonCalled)
  {
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
      break;
    case DLL_PROCESS_ATTACH:
      g_hinstance = _hmodule;
      break;
    case DLL_PROCESS_DETACH:
      unLoadDSMLib();
      g_hinstance = 0;
      break;
  }
  return(TRUE);
}
#elif (TWNDSM_CMP == TWNDSM_CMP_GNUGPP)
    // Nothing for us to do...
#else
    #error Sorry, we do not recognize this system...
#endif

