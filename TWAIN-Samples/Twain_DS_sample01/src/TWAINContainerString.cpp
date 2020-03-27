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
* @file TWAINContainerInt.cpp
* Int Container class for negotiating capabilities.
* @author TWAIN Working Group
* @date April 2007
*/


#include "CommonDS.h"
#include <algorithm>


CTWAINContainerString::CTWAINContainerString(const TW_UINT16 _unCapID, 
                                         const TW_UINT16 _unItemType, 
                                         const TW_UINT16 _unGetType, 
                                         const TW_INT32  _nSupportedQueries/* = TWQC_ALL*/,
                                         const TW_UINT16 _unGetCurrentType/* = TWON_ONEVALUE*/,
                                         const TW_UINT16 _unGetDefaultType/* = TWON_ONEVALUE*/)
  : CTWAINContainer(_unCapID, _unItemType, _unGetType, _nSupportedQueries,_unGetCurrentType,_unGetDefaultType)
{
  m_unItemSize = getTWTYSize(_unItemType);
}

CTWAINContainerString::~CTWAINContainerString()
{
}

TW_HANDLE CTWAINContainerString::GetContainer(const TW_UINT16 _unMsg)
{
  TW_HANDLE hContainer = 0;
  TW_UINT16 unGetType=TWON_ONEVALUE;
  switch(_unMsg)
  {
    case MSG_GET:
      unGetType = m_unGetType;
      break;
    case MSG_GETCURRENT:
      unGetType = m_unGetCurrentType;
      break;
    case MSG_GETDEFAULT:
      unGetType = m_unGetDefaultType;
      break;
  }

  switch(unGetType)
  {
    default:
    case TWON_ONEVALUE:
    {
      hContainer = _DSM_Alloc(sizeof(TW_ONEVALUE)+m_unItemSize-sizeof(TW_UINT32));
      if(0 != hContainer)
      {
        pTW_ONEVALUE pCap = (pTW_ONEVALUE)_DSM_LockMemory(hContainer);

        pCap->ItemType = m_unItemType;
        // If the Cap has been constrained the default may only be in the defaultlist.
        string strTemp = ((MSG_GETDEFAULT == _unMsg)?m_listStrsDefault[m_nDefault]:m_listStrs[m_nCurrent]);
        memset((char*)&pCap->Item,0,m_unItemSize);
#ifdef __APPLE__ 
		  memcpy(((char*)&pCap->Item)+1, strTemp.c_str(),MIN(m_unItemSize, strTemp.length()));
		  ((char*)&pCap->Item)[0]=strTemp.length();
#else
		  memcpy((char*)&pCap->Item, strTemp.c_str(),MIN(m_unItemSize, strTemp.length()));
#endif
        _DSM_UnlockMemory(hContainer);
      }
    }
    break;

    case TWON_ENUMERATION:
    {
      hContainer = _DSM_Alloc((TW_UINT32)(sizeof(TW_ENUMERATION) -1 + (m_unItemSize * m_listStrs.size())));

      if(0 != hContainer)
      {
        pTW_ENUMERATION pCap = (pTW_ENUMERATION)_DSM_LockMemory(hContainer);

        pCap->ItemType = m_unItemType;
        pCap->NumItems = (TW_UINT32)m_listStrs.size();
        pCap->CurrentIndex = m_nCurrent;
        //If the CAP has been constrained m_nDefault index might not point 
        // to the correct index and the index may not be valid.  We need to 
        // find the value in the default list and see if we can find a match 
        // in the current list.  If no match found then set to first index.
        // see spec on twain.org Chap4 p73 Advanced Application Implementation | 
        // Capabilities | Constrained Capabilities and Message Responses | MSG_SET
        const string strVal   = m_listStrsDefault[m_nDefault];
        int nIndex = getIndexForValue(strVal);
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

        fillValues(&pCap->ItemList, pCap->NumItems);

        _DSM_UnlockMemory(hContainer);
      }
    }
    break;

    case TWON_ARRAY:
    {
      hContainer = _DSM_Alloc((TW_UINT32)(sizeof(TW_ARRAY)-1 + (m_unItemSize * m_listStrs.size())));

      if(0 != hContainer)
      {
        pTW_ARRAY pCap = (pTW_ARRAY)_DSM_LockMemory(hContainer);
        pCap->ItemType = m_unItemType;
        pCap->NumItems = (TW_UINT32)m_listStrs.size();

        fillValues(&pCap->ItemList, pCap->NumItems);

        _DSM_UnlockMemory(hContainer);
      }
    }
  } // switch(unGetType)

  return hContainer;
}

void CTWAINContainerString::fillValues(void* _pItemList, const TW_UINT32 _unNumItems)
{
  for(TW_UINT32 x = 0; x < _unNumItems; ++x)
  {
    string strTemp = m_listStrs[x];
    char *pItem = (char*)_pItemList + m_unItemSize*x;
    memset(pItem,0,m_unItemSize);
#ifdef __APPLE__ 
	  memcpy(((char*)pItem)+1, strTemp.c_str(),MIN(m_unItemSize, strTemp.length()));
	  ((char*)pItem)[0]=strTemp.length();
#else
	  memcpy(pItem, strTemp.c_str(),MIN(m_unItemSize, strTemp.length()));
#endif
  }
}

bool CTWAINContainerString::isValidType(const TW_UINT16 _unTWType)
{
  return m_unItemType == _unTWType;
}

string CTWAINContainerString::getValue(const char* _pchVal)
{
  string strRet;
  char *pstrTemp = new char[m_unItemSize+1];
  memcpy(pstrTemp,_pchVal,m_unItemSize);
  pstrTemp[m_unItemSize]=0;
  strRet = pstrTemp;
  delete [] pstrTemp;
  return strRet;
}

TW_INT16 CTWAINContainerString::Set(pTW_CAPABILITY _pCap, TW_INT16 &Condition)
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
          m_listStrs.clear();
          m_listStrs.push_back(getValue((char*)&(pCap->Item)));
          m_nCurrent = 0;
        }
        break;
        case TWON_ENUMERATION:
        {
          int nVal = -1;

          if( (nVal = getIndexForValue(getValue((char*)&(pCap->Item)))) >= 0 )
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
    _DSM_UnlockMemory(_pCap->hContainer);
  }
  else if(TWON_ENUMERATION == _pCap->ConType)
  {
    pTW_ENUMERATION pCap = (pTW_ENUMERATION)_DSM_LockMemory(_pCap->hContainer);

    if(isValidType(pCap->ItemType))
    {
      int  nNewCurrentIndex  = pCap->CurrentIndex;
      StringVector::iterator iter;
      bool bListCleared      = false; // We only want to crear the current list if we are passed 
                                      // valid data, and only clear it once through the loop of testing
      
      for(TW_UINT32 x = 0; x < pCap->NumItems; ++x)
      {
        // only set the value if it exists in m_listStrsDefault
        string strValue = getValue((char*)&(pCap->ItemList)+x*m_unItemSize);

        iter = find(m_listStrsDefault.begin(), m_listStrsDefault.end(), strValue);

        if(iter != m_listStrsDefault.end())
        {
          // We have valid data
          if(!bListCleared)
          {
            // only clear the list if we have not done so already
            m_listStrs.clear();
            bListCleared = true;
          }

          // only add it if it was not added already
          iter = find(m_listStrs.begin(), m_listStrs.end(), strValue);
          if(iter == m_listStrs.end())
          {
            m_listStrs.push_back(strValue);
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
        if(nNewCurrentIndex >= 0 && nNewCurrentIndex < (int)(m_listStrs.size()))
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

    _DSM_UnlockMemory(_pCap->hContainer);
  }
  else //bad container type
  {
    twrc = TWRC_FAILURE;
    Condition = TWCC_BADVALUE;
  }

  return twrc;
}

bool CTWAINContainerString::Reset()
{
  m_nCurrent = m_nDefault;

  m_listStrs.clear();

  const int nSize = (int)(m_listStrsDefault.size());

  for(int x = 0; x < nSize; ++x)
  {
    m_listStrs.push_back(m_listStrsDefault[x]);
  }

  return true;
}

// For int vals
bool CTWAINContainerString::GetCurrent(string &_strVal)
{
  bool bret = false;

  if(!(m_nMSG_QUERYSUPPORT & TWQC_GETCURRENT))
  {
    return false;
  }

  if((m_nCurrent >= 0) && ((int)(m_listStrs.size()) > m_nCurrent))
  {    
    _strVal = m_listStrs[m_nCurrent];
    bret = true;
  }

  return bret;
}

bool CTWAINContainerString::GetDefault(string &_strVal)
{
  bool bret = false;

  if(!(m_nMSG_QUERYSUPPORT & TWQC_GETDEFAULT))
  {
    return TWRC_FAILURE;
  }

  if((m_nDefault >= 0) && ((int)(m_listStrsDefault.size()) > m_nDefault))
  {
    _strVal = m_listStrsDefault[m_nDefault];
    bret = true;
  }

  return bret;
}

const StringVector &CTWAINContainerString::GetSupported()
{
  return m_listStrs;
}

bool CTWAINContainerString::Add(string _strAdd, bool _bDefault /*= false*/)
{
  m_listStrs.push_back(_strAdd);
  m_listStrsDefault.push_back(_strAdd);
  if(m_nDefault == -1 || _bDefault)
  {
	  m_nCurrent = m_nDefault = (int)(m_listStrsDefault.size() - 1);
  }
  return true;
}

bool CTWAINContainerString::SetCurrent(string _strCurr)
{
  if(TWON_ONEVALUE == m_unGetType)//check before call
  {
    m_listStrs.clear();
    m_listStrs.push_back(_strCurr);
    m_nCurrent = 0;
  }
  else
  {
    int nIdx = getIndexForValue(_strCurr);
    if(nIdx < 0)
    {
      return false;
    }

    m_nCurrent = nIdx;
  }
  return true;
}

int CTWAINContainerString::getIndexForValue(const string _strVal)
{
  int ret = -1;

  const int nSize = (int)(m_listStrs.size());

  for(int x = 0; x < nSize; ++x)
  {
    if(_strVal == m_listStrs[x])
    {
      ret = x; 
      break;
    }
  }

  return ret;
}
