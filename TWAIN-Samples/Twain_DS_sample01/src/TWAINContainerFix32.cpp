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
* @file TWAINContainerFix32.cpp
* Fix32 Container class for negotiating capabilities.
* @author TWAIN Working Group
* @date April 2007
*/


#include "CommonDS.h"
#include <algorithm>

CTWAINContainerFix32::CTWAINContainerFix32(const TW_UINT16 _unCapID, 
                                           const TW_UINT16 _unItemType, 
                                           const TW_UINT16 _unGetType, 
                                           const TW_INT32  _nSupportedQueries /*=TWQC_ALL*/)
  : CTWAINContainer(_unCapID, _unItemType, _unGetType, _nSupportedQueries)
{
}

CTWAINContainerFix32::~CTWAINContainerFix32()
{
}

TW_HANDLE CTWAINContainerFix32::GetContainer(const TW_UINT16 _unMsg)
{
  TW_HANDLE hContainer = 0;

  if((TWON_ONEVALUE == m_unGetType) ||
     (MSG_GETCURRENT == _unMsg) ||
     (MSG_GETDEFAULT == _unMsg))
  {
    hContainer = _DSM_Alloc(sizeof(TW_ONEVALUE_FIX32));

    if(0 != hContainer)
    {
      pTW_ONEVALUE_FIX32 pCap = (pTW_ONEVALUE_FIX32)_DSM_LockMemory(hContainer);

      pCap->ItemType = TWTY_FIX32;
      // If the Cap has been constrained the default may only be in the m_listFloatsDefault.
      const float flVal = (MSG_GETDEFAULT == _unMsg)?m_listFloatsDefault[m_nDefault]:m_listFloats[m_nCurrent];
      
      pCap->Item = FloatToFIX32(flVal);
      _DSM_UnlockMemory((TW_MEMREF)pCap);
    }
  }
  else if(MSG_GET == _unMsg)
  {
    if(TWON_ENUMERATION == m_unGetType)
    {
      hContainer = _DSM_Alloc(sizeof(TW_ENUMERATION_FIX32) + (sizeof(TW_FIX32) * (m_listFloats.size()-1))); // -1 because already contains 1 element

      if(0 != hContainer)
      {
        pTW_ENUMERATION_FIX32 pCap = (pTW_ENUMERATION_FIX32)_DSM_LockMemory(hContainer);

        pCap->ItemType = TWTY_FIX32;
        pCap->NumItems = (TW_UINT32)m_listFloats.size();
        pCap->CurrentIndex = m_nCurrent;
        //If the CAP has been constrained m_nDefault index might not point 
        // to the correct index and the index may not be valid.  We need to 
        // find the value in the default list and see if we can find a match 
        // in the current list.  If no match found then set to first index.
        // see spec on twain.org Chap4 p73 Advanced Application Implementation | 
        // Capabilities | Constrained Capabilities and Message Responses | MSG_SET
        const float flVal = m_listFloatsDefault[m_nDefault];
        int nIndex = getIndexForValue(flVal);
        if(nIndex != -1)
        {
          pCap->DefaultIndex = nIndex;
        }
        else
        {
          // We use the first index.  We could try transversing through the 
          // list and finding the closest match in value.  But either way
          // the application should not be using this value.
          pCap->DefaultIndex = 0;
        }

        for(TW_UINT32 x = 0; x < pCap->NumItems; ++x)
        {
          pCap->ItemList[x] = FloatToFIX32(m_listFloats[x]);
        }

        _DSM_UnlockMemory((TW_MEMREF)pCap);
      }
    }
    else if(TWON_ARRAY == m_unGetType)
    {
      hContainer = _DSM_Alloc(sizeof(TW_ARRAY_FIX32) + (sizeof(TW_FIX32) * (m_listFloats.size()-1))); // -1 because a TW_ARRAY_FIX32 already includes 1 element

      if(0 != hContainer)
      {
        pTW_ARRAY_FIX32 pCap = (pTW_ARRAY_FIX32)_DSM_LockMemory(hContainer);
        pCap->ItemType = m_unItemType;
        pCap->NumItems = (TW_UINT32)m_listFloats.size();

        for(TW_UINT32 x = 0; x < pCap->NumItems; ++x)
        {
          pCap->ItemList[x] = FloatToFIX32(m_listFloats[x]);
        }

        _DSM_UnlockMemory((TW_MEMREF)pCap);
      }
    }
  }

  return hContainer;
}

bool CTWAINContainerFix32::isValidType(const TW_UINT16 _unTWType)
{
  bool bret = false;

  if(TWTY_FIX32 == _unTWType)
  {
    bret = true;
  }

  return bret;
}

TW_INT16 CTWAINContainerFix32::Set(pTW_CAPABILITY _pCap, TW_INT16 &Condition)
{
  TW_INT16 twrc = TWRC_SUCCESS;
  Condition = TWCC_SUCCESS;

  if(TWON_ONEVALUE == _pCap->ConType)
  {
    pTW_ONEVALUE_FIX32 pCap = (pTW_ONEVALUE_FIX32)_DSM_LockMemory(_pCap->hContainer);

    int nVal = -1;

    if(isValidType(pCap->ItemType))
    {
      float flVal = FIX32ToFloat(pCap->Item);

      /*if(TWON_RANGE == m_unGetType)
      {
      }
      else */
      if(TWON_ENUMERATION == m_unGetType)
      {
        if((nVal = getIndexForValue(flVal)) >= 0)
        {
          m_nCurrent = nVal;
        }
      }
    }

    _DSM_UnlockMemory((TW_MEMREF)pCap);
  }
  else if(TWON_ENUMERATION == _pCap->ConType)
  {
    pTW_ENUMERATION_FIX32 pCap = (pTW_ENUMERATION_FIX32)_DSM_LockMemory(_pCap->hContainer);

    if(isValidType(pCap->ItemType))
    {
      m_nCurrent = pCap->CurrentIndex;
      // We ignor trying to set the default.  The default can not be changed from outside

      FloatVector::iterator iter;
      m_listFloats.clear();

      float flVal;

      for(TW_UINT32 x = 0; x < pCap->NumItems; ++x)
      {     
        flVal = FIX32ToFloat(pCap->ItemList[x]);

        // only set the value if it exists in m_listFloatsDefault
        iter = find(m_listFloatsDefault.begin(), m_listFloatsDefault.end(), flVal);

        if(iter != m_listFloatsDefault.end())
        {
          m_listFloats.push_back(flVal);          
        }
        else
        {
          // if the index was pointing to this value we have to change it because this value is now invalid
          if(x == (UINT)m_nCurrent) // casting to uint to remove compiler warning
          {
            m_nCurrent = 0;
          }

          twrc = TWRC_CHECKSTATUS;          
          Condition = TWCC_SUCCESS;
        }
      }
    }

    _DSM_UnlockMemory((TW_MEMREF)pCap);
  }

  return twrc;
}

bool CTWAINContainerFix32::Reset()
{
  m_nCurrent = m_nDefault;

  m_listFloats.clear();

  const int nSize = (int)(m_listFloatsDefault.size());

  for(int x = 0; x < nSize; ++x)
  {
    m_listFloats.push_back(m_listFloatsDefault[x]);
  }

  return true;
}

bool CTWAINContainerFix32::GetCurrent(float &_flVal)
{
  bool bret = false;

  if((m_nCurrent >= 0) && ((int)(m_listFloats.size()) > m_nCurrent))
  {    
    _flVal = m_listFloats[m_nCurrent];
    bret = true;
  }

  return bret;
}

bool CTWAINContainerFix32::GetDefault(float &_flVal)
{
  bool bret = false;

  if((m_nDefault >= 0) && ((int)(m_listFloatsDefault.size()) > m_nDefault))
  {
    _flVal = m_listFloatsDefault[m_nDefault];
    bret = true;
  }

  return bret;
}

const FloatVector &CTWAINContainerFix32::GetSupported()
{
  return m_listFloats;
}

bool CTWAINContainerFix32::Add(const float _flAdd, bool _bDefault /*= false*/)
{
  m_listFloats.push_back(_flAdd);
  m_listFloatsDefault.push_back(_flAdd);
  if(m_nDefault == -1 || _bDefault)
  {
    m_nCurrent = m_nDefault = getIndexForValue(_flAdd);
  }
  return true;
}

bool CTWAINContainerFix32::SetCurrent(float _flCurr)
{
  bool bret = true;

  if((m_nCurrent = getIndexForValue(_flCurr)) < 0)
  {
    bret = false;
  }

  return bret;
}

int CTWAINContainerFix32::getIndexForValue(const float _flVal)
{
  int ret = -1;

  const int nSize = (int)(m_listFloats.size());

  for(int x = 0; x < nSize; ++x)
  {
    if(_flVal == m_listFloats[x])
    {
      ret = x; 
      break;
    }
  }

  return ret;
}

