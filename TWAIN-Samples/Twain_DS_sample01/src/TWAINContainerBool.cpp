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
* @file TWAINContainerBool.cpp
* bool Container class for negotiating capabilities.
* @author TWAIN Working Group
* @date April 2007
*/

#include "CommonDS.h"
#include <algorithm>

CTWAINContainerBool::CTWAINContainerBool(const TW_UINT16 _unCapID, 
                                         const bool _bAPP2,
                                         const TW_INT32  _nSupportedQueries /*=TWQC_ALL*/)
  : CTWAINContainer(_unCapID, TWTY_BOOL, _bAPP2?TWON_ENUMERATION:TWON_ONEVALUE, _nSupportedQueries)
{
}

CTWAINContainerBool::~CTWAINContainerBool()
{
}

TW_HANDLE CTWAINContainerBool::GetContainer(const TW_UINT16 _unMsg)
{
  TW_HANDLE hContainer = 0;

  if((TWON_ONEVALUE == m_unGetType) ||
     (MSG_GETCURRENT == _unMsg) ||
     (MSG_GETDEFAULT == _unMsg))
  {
    hContainer = _DSM_Alloc(sizeof(TW_ONEVALUE));

    if(0 != hContainer)
    {
      TW_ONEVALUE* pCap = (TW_ONEVALUE*)_DSM_LockMemory(hContainer);

      pCap->ItemType = TWTY_BOOL;
      // If the Cap has been constrained the default may only be in the m_listBoolsDefault.
      const bool bVal = ((MSG_GETDEFAULT == _unMsg)?m_listBoolsDefault[m_nDefault]:m_listBools[m_nCurrent])!=0;
      
      pCap->Item = bVal?1:0;
      _DSM_UnlockMemory(hContainer);
    }
  }
  else if(MSG_GET == _unMsg)
  {
    hContainer = _DSM_Alloc((TW_UINT32)(sizeof(TW_ENUMERATION)-1 + (sizeof(TW_BOOL) * (m_listBools.size())))); // -1 because already contains 1 byte

    if(0 != hContainer)
    {
      TW_ENUMERATION* pCap = (TW_ENUMERATION*)_DSM_LockMemory(hContainer);

      pCap->ItemType = TWTY_BOOL;
      pCap->NumItems = (TW_UINT32)m_listBools.size();
      pCap->CurrentIndex = m_nCurrent;
      //If the CAP has been constrained m_nDefault index might not point 
      // to the correct index and the index may not be valid.  We need to 
      // find the value in the default list and see if we can find a match 
      // in the current list.  If no match found then set to first index.
      // see spec on twain.org Chap4 p73 Advanced Application Implementation | 
      // Capabilities | Constrained Capabilities and Message Responses | MSG_SET
      const bool bVal = m_listBoolsDefault[m_nDefault]!=0;
      int nIndex = getIndexForValue(bVal);
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
        ((TW_BOOL*)pCap->ItemList)[x] = m_listBools[x]?1:0;
      }

      _DSM_UnlockMemory(hContainer);
    }
  }

  return hContainer;
}

bool CTWAINContainerBool::isValidType(const TW_UINT16 _unTWType)
{
  return (TWTY_BOOL == _unTWType);
}

TW_INT16 CTWAINContainerBool::Set(pTW_CAPABILITY _pCap, TW_INT16 &Condition)
{
  TW_INT16 twrc = TWRC_SUCCESS;
  Condition = TWCC_SUCCESS;

  if(TWON_ONEVALUE == _pCap->ConType)
  {
    TW_ONEVALUE* pCap = (TW_ONEVALUE*)_DSM_LockMemory(_pCap->hContainer);

    if(isValidType(pCap->ItemType))
    {
      if(!SetCurrent((pCap->Item&0xFFFF)!=0))
      {
        twrc = TWRC_FAILURE;
        Condition = TWCC_BADVALUE;
      }
    }

    _DSM_UnlockMemory(_pCap->hContainer);
  }
  else if(TWON_ENUMERATION == _pCap->ConType)
  {
    TW_ENUMERATION* pCap = (TW_ENUMERATION*)_DSM_LockMemory(_pCap->hContainer);

    if(isValidType(pCap->ItemType))
    {
      int  nNewCurrentIndex  = pCap->CurrentIndex;
      IntVector::iterator iter;
      bool bListCleared      = false; // We only want to crear the current list if we are passed 
                                      // valid data, and only clear it once through the loop of testing
      bool bVal;

      for(TW_UINT32 x = 0; x < pCap->NumItems; ++x)
      {     
        bVal = ((TW_BOOL*)pCap->ItemList)[x]!=0;

        // only set the value if it exists in m_listBoolsDefault
        iter = find(m_listBoolsDefault.begin(), m_listBoolsDefault.end(), (bVal?1:0));

        if(iter != m_listBoolsDefault.end())
        {
          // We have valid data
          if(!bListCleared)
          {
            // only clear the list if we have not done so already
            m_listBools.clear();
            bListCleared = true;
          }

          // only add it if it was not added already
          iter = find(m_listBools.begin(), m_listBools.end(), (bVal?1:0));
          if(iter == m_listBools.end())
          {
            m_listBools.push_back(bVal);
          }
          else
          {
            if(x < pCap->CurrentIndex)
            {
              nNewCurrentIndex--;
            }
            twrc = TWRC_CHECKSTATUS;
            Condition = TWCC_BADVALUE;
          }
        }
        else
        {
          // if the index is below the current then we need to adjust what is going to be current
          if(x < pCap->CurrentIndex)
          {
            nNewCurrentIndex--;
          }

          twrc = TWRC_CHECKSTATUS;          
          Condition = TWCC_BADVALUE;
        }
      }

      // If the list has been cleared then there was at at least some valid data
      if(bListCleared)
      {
        if(nNewCurrentIndex >= 0 && nNewCurrentIndex < (int)(m_listBools.size()))
        {
          m_nCurrent = nNewCurrentIndex;
        }
        else
        {
          // the new current index is not in range
          m_nCurrent = 0;
          twrc = TWRC_CHECKSTATUS;
          Condition = TWCC_BADVALUE;
        }
      }
      else
      {
        twrc = TWRC_FAILURE;
        Condition = TWCC_BADVALUE;
      }
    }
    else // NOT isValidType(pCap->ItemType))
    {
      twrc = TWRC_FAILURE;
      Condition = TWCC_BADVALUE;
    }

    _DSM_UnlockMemory(_pCap->hContainer);
  }
  else //bad container type
  {
    twrc = TWRC_FAILURE;
    Condition = TWCC_BADVALUE;
  }
  return twrc;
}

bool CTWAINContainerBool::Reset()
{
  m_listBools.clear();

  const int nSize = (int)(m_listBoolsDefault.size());

  for(int x = 0; x < nSize; ++x)
  {
    m_listBools.push_back(m_listBoolsDefault[x]);
  }

  m_nCurrent = m_nDefault;

  return true;
}

bool CTWAINContainerBool::GetCurrent(bool &_bVal)
{
  if((m_nCurrent >= 0) && ((int)(m_listBools.size()) > m_nCurrent))
  {    
    _bVal = m_listBools[m_nCurrent]!=0;
    return true;
  }

  return false;
}

bool CTWAINContainerBool::GetDefault(bool &_bVal)
{
  if((m_nDefault >= 0) && ((int)(m_listBoolsDefault.size()) > m_nDefault))
  {
    _bVal = m_listBoolsDefault[m_nDefault]!=0;
    return true;
  }

  return false;
}

const IntVector &CTWAINContainerBool::GetSupported()
{
  return m_listBools;
}

bool CTWAINContainerBool::Add(const bool _bAdd, bool _bDefault /*= false*/)
{
  m_listBools.push_back(_bAdd);
  m_listBoolsDefault.push_back(_bAdd);
  if(m_nDefault == -1 || _bDefault)
  {
    m_nCurrent = (int)m_listBools.size()-1;
    m_nDefault = (int)m_listBoolsDefault.size()-1;
  }
  return true;
}

bool CTWAINContainerBool::SetCurrent(bool _bCurr)
{
  int nIdx = getIndexForValue(_bCurr);
  if(nIdx < 0)
  {
    return false;
  }

  m_nCurrent = nIdx;
  return true;
}

int CTWAINContainerBool::getIndexForValue(const bool _bVal)
{
  const int nSize = (int)(m_listBools.size());

  for(int x = 0; x < nSize; ++x)
  {
    if(_bVal == (m_listBools[x]!=0))
    {
      return x; 
    }
  }

  return -1;
}

