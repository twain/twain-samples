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
* @file TWAINContainerInt.cpp
* Int Container class for negotiating capabilities.
* @author TWAIN Working Group
* @date April 2007
*/


#include "CommonDS.h"
#include <algorithm>


CTWAINContainerInt::CTWAINContainerInt(const TW_UINT16 _unCapID, 
                                       const TW_UINT16 _unItemType, 
                                       const TW_UINT16 _unGetType, 
                                       const TW_INT32  _nSupportedQueries /*=TWQC_ALL*/)
  : CTWAINContainer(_unCapID, _unItemType, _unGetType, _nSupportedQueries)
{
}

CTWAINContainerInt::~CTWAINContainerInt()
{
}

TW_HANDLE CTWAINContainerInt::GetContainer(const TW_UINT16 _unMsg)
{
  TW_HANDLE hContainer = 0;

  if((TWON_ONEVALUE == m_unGetType) ||
     (MSG_GETCURRENT == _unMsg) ||
     (MSG_GETDEFAULT == _unMsg))
  {
    hContainer = _DSM_Alloc(sizeof(TW_ONEVALUE));

    if(0 != hContainer)
    {
      pTW_ONEVALUE pCap = (pTW_ONEVALUE)_DSM_LockMemory(hContainer);

      pCap->ItemType = m_unItemType;
      // If the Cap has been constrained the default may only be in the defaultlist.
      pCap->Item = (MSG_GETDEFAULT == _unMsg)?m_listIntsDefault[m_nDefault]:m_listInts[m_nCurrent];

      _DSM_UnlockMemory((TW_MEMREF)pCap);
    }
  }
  else if(MSG_GET == _unMsg)
  {
    UINT unSize = getTWTYSize(m_unItemType);

    if(TWON_ENUMERATION == m_unGetType)
    {
      hContainer = _DSM_Alloc(sizeof(TW_ENUMERATION) + (unSize * m_listInts.size()));

      if(0 != hContainer)
      {
        pTW_ENUMERATION pCap = (pTW_ENUMERATION)_DSM_LockMemory(hContainer);

        pCap->ItemType = m_unItemType;
        pCap->NumItems = (TW_UINT32)m_listInts.size();
        pCap->CurrentIndex = m_nCurrent;
        //If the CAP has been constrained m_nDefault index might not point 
        // to the correct index and the index may not be valid.  We need to 
        // find the value in the default list and see if we can find a match 
        // in the current list.  If no match found then set to first index.
        // see spec on twain.org Chap4 p73 Advanced Application Implementation | 
        // Capabilities | Constrained Capabilities and Message Responses | MSG_SET
        const int nVal   = m_listIntsDefault[m_nDefault];
        int nIndex = getIndexForValue(nVal);
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

        fillValues(&pCap->ItemList, pCap->NumItems, m_unItemType);

        _DSM_UnlockMemory((TW_MEMREF)pCap);
      }
    }
    else if(TWON_ARRAY == m_unGetType)
    {
      hContainer = _DSM_Alloc(sizeof(TW_ARRAY) + (unSize * m_listInts.size()));

      if(0 != hContainer)
      {
        pTW_ARRAY pCap = (pTW_ARRAY)_DSM_LockMemory(hContainer);
        pCap->ItemType = m_unItemType;
        pCap->NumItems = (TW_UINT32)m_listInts.size();

        fillValues(&pCap->ItemList, pCap->NumItems, m_unItemType);

        _DSM_UnlockMemory((TW_MEMREF)pCap);
      }
    }
  } // else if(MSG_GET == _unMsg)

  return hContainer;
}

void CTWAINContainerInt::fillValues(void* _pItemList, const TW_UINT32 _unNumItems, const TW_UINT16 _unItemType)
{
  for(TW_UINT32 x = 0; x < _unNumItems; ++x)
  {
    switch(_unItemType)
    {
      case TWTY_INT8:
        ((TW_INT8*)_pItemList)[x] = (TW_INT8)m_listInts[x];
      break;

      case TWTY_INT16:
        ((TW_INT16*)_pItemList)[x] = (TW_INT16)m_listInts[x];
      break;

      case TWTY_INT32:
        ((TW_INT32*)_pItemList)[x] = (TW_INT32)m_listInts[x];
      break;

      case TWTY_UINT8:
        ((TW_UINT8*)_pItemList)[x] = (TW_UINT8)m_listInts[x];
      break;

      case TWTY_UINT16:
        ((TW_UINT16*)_pItemList)[x] = (TW_UINT16)m_listInts[x];
      break;

      case TWTY_UINT32:
        ((TW_UINT32*)_pItemList)[x] = (TW_UINT32)m_listInts[x];
      break;

      case TWTY_BOOL:
        ((TW_BOOL*)_pItemList)[x] = (TW_BOOL)m_listInts[x];
      break;
    }
  }
}

bool CTWAINContainerInt::isValidType(const TW_UINT16 _unTWType)
{
  bool bret = false;

  switch(_unTWType)
  {
    case TWTY_INT8:
    case TWTY_INT16:
    case TWTY_INT32:
    case TWTY_UINT8:
    case TWTY_UINT16:
    case TWTY_UINT32:
    case TWTY_BOOL:
      bret = true;
    break;
  }

  return bret;
}

int CTWAINContainerInt::getValue(const pTW_ONEVALUE _pVal)
{
  int nRet = 0;
  
  switch(_pVal->ItemType)
  {
    case TWTY_INT8:
      nRet = *((TW_INT8*)&(_pVal->Item));
    break;

    case TWTY_INT16:
      nRet = *((TW_INT16*)&(_pVal->Item));
    break;

    case TWTY_INT32:
      nRet = *((TW_INT32*)&(_pVal->Item));
    break;

    case TWTY_UINT8:
      nRet = *((TW_UINT8*)&(_pVal->Item));
    break;

    case TWTY_UINT16:
      nRet = *((TW_UINT16*)&(_pVal->Item));
    break;

    case TWTY_UINT32:
      nRet = *((TW_UINT32*)&(_pVal->Item));
    break;

    case TWTY_BOOL:
      nRet = *((TW_BOOL*)&(_pVal->Item));
    break;
  }
  
  return nRet;
}

TW_INT16 CTWAINContainerInt::Set(pTW_CAPABILITY _pCap, TW_INT16 &Condition)
{
  TW_INT16 twrc = TWRC_SUCCESS;
  Condition = TWCC_SUCCESS;

  if(TWON_ONEVALUE == _pCap->ConType)
  {
    pTW_ONEVALUE pCap = (pTW_ONEVALUE)_DSM_LockMemory(_pCap->hContainer);
    if(isValidType(pCap->ItemType))
    {
      switch(m_unGetType)
      {
        case TWON_ONEVALUE:
        {
          m_listInts.clear();
          m_listInts.push_back(getValue(pCap));
          m_nCurrent = 0;
        }
        break;
        case TWON_ENUMERATION:
        {
          int nVal = -1;

          if( (nVal = getIndexForValue(getValue(pCap))) >= 0 )
          {
            m_nCurrent = nVal;
          }
          else
          {
            twrc = TWRC_FAILURE;
            Condition = TWCC_BADVALUE;
          }
        }
        break;
        //case TWON_ARRAY:
        //break;
        //case TWON_RANGE:
        //break;
        default:
          twrc = TWRC_FAILURE;
          Condition = TWCC_CAPBADOPERATION;
        break;
      }
    }
    else // NOT isValidType(pCap->ItemType))
    {
      twrc = TWRC_FAILURE;
      Condition = TWCC_CAPBADOPERATION;
    }
    _DSM_UnlockMemory((TW_MEMREF)pCap);
  }
  else if(TWON_ENUMERATION == _pCap->ConType)
  {
    pTW_ENUMERATION pCap = (pTW_ENUMERATION)_DSM_LockMemory(_pCap->hContainer);

    if(isValidType(pCap->ItemType))
    {
      int  nNewCurrentIndex  = pCap->CurrentIndex;
      IntVector::iterator iter;
      bool bListCleared      = false; // We only want to crear the current list if we are passed 
                                      // valid data, and only clear it once through the loop of testing
      
      for(TW_UINT32 x = 0; x < pCap->NumItems; ++x)
      {
        // only set the value if it exists in m_listIntsDefault
        int nValue = GetIndexIntValue(pCap, x);
        iter = find(m_listIntsDefault.begin(), m_listIntsDefault.end(), nValue);

        if(iter != m_listIntsDefault.end())
        {
          // We have valid data
          if(!bListCleared)
          {
            // only clear the list if we have not done so already
            m_listInts.clear();
            bListCleared = true;
          }

          // only add it if it was not added already
          iter = find(m_listInts.begin(), m_listInts.end(), nValue);
          if(iter == m_listInts.end())
          {
            m_listInts.push_back(nValue);
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
        if(nNewCurrentIndex >= 0 && nNewCurrentIndex < (int)(m_listInts.size()))
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
      Condition = TWCC_CAPBADOPERATION;
    }

    _DSM_UnlockMemory((TW_MEMREF)pCap);
  }

  return twrc;
}

bool CTWAINContainerInt::Reset()
{
  m_nCurrent = m_nDefault;

  m_listInts.clear();

  const int nSize = (int)(m_listIntsDefault.size());

  for(int x = 0; x < nSize; ++x)
  {
    m_listInts.push_back(m_listIntsDefault[x]);
  }

  return true;
}

// For int vals
bool CTWAINContainerInt::GetCurrent(int &_nVal)
{
  bool bret = false;

  if(!(m_nMSG_QUERYSUPPORT & TWQC_GETCURRENT))
  {
    return false;
  }

  if((m_nCurrent >= 0) && ((int)(m_listInts.size()) > m_nCurrent))
  {    
    _nVal = m_listInts[m_nCurrent];
    bret = true;
  }

  return bret;
}

bool CTWAINContainerInt::GetDefault(int &_nVal)
{
  bool bret = false;

  if(!(m_nMSG_QUERYSUPPORT & TWQC_GETDEFAULT))
  {
    return TWRC_FAILURE;
  }

  if((m_nDefault >= 0) && ((int)(m_listIntsDefault.size()) > m_nDefault))
  {
    _nVal = m_listIntsDefault[m_nDefault];
    bret = true;
  }

  return bret;
}

const IntVector &CTWAINContainerInt::GetSupported()
{
  return m_listInts;
}

bool CTWAINContainerInt::Add(int _nAdd, bool _bDefault /*= false*/)
{
  m_listInts.push_back(_nAdd);
  m_listIntsDefault.push_back(_nAdd);
  if(m_nDefault == -1 || _bDefault)
  {
    m_nCurrent = m_nDefault = getIndexForValue(_nAdd);
  }
  return true;
}

bool CTWAINContainerInt::SetCurrent(int _nCurr)
{
  bool bret = true;

  if((m_nCurrent = getIndexForValue(_nCurr)) < 0)
  {
    bret = false;
  }

  return bret;
}

int CTWAINContainerInt::getIndexForValue(const int _nVal)
{
  int ret = -1;

  const int nSize = (int)(m_listInts.size());

  for(int x = 0; x < nSize; ++x)
  {
    if(_nVal == m_listInts[x])
    {
      ret = x; 
      break;
    }
  }

  return ret;
}

int GetIndexIntValue(pTW_ENUMERATION pCap, TW_UINT32 index)
{
  int rtn = 0;
  if(pCap && pCap->NumItems > index)
  {
    switch(pCap->ItemType)
    {
      case TWTY_INT8:
        rtn = ((TW_INT8*)&pCap->ItemList)[index];
        break;

      case TWTY_INT16:
        rtn = ((TW_INT16*)&pCap->ItemList)[index];
        break;

      case TWTY_INT32:
        rtn = ((TW_INT32*)&pCap->ItemList)[index];
        break;

      case TWTY_UINT8:
        rtn = ((TW_UINT8*)&pCap->ItemList)[index];
        break;

      case TWTY_UINT16:
        rtn = ((TW_UINT16*)&pCap->ItemList)[index];
        break;

      case TWTY_UINT32:
        rtn = ((TW_UINT32*)&pCap->ItemList)[index];
        break;
      
      case TWTY_BOOL:
        rtn = ((TW_BOOL*)&pCap->ItemList)[index];
        break;
    }
  }
  return rtn;
}