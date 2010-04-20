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
* @file TWAINContainer.cpp
* Base Container class for negotiating capabilities.
* @author TWAIN Working Group
* @date April 2007
*/


#include "CommonDS.h"

CTWAINContainer::CTWAINContainer(const TW_UINT16 _unCapID, 
                                 const TW_UINT16 _unItemType, 
                                 const TW_UINT16 _unGetType, 
                                 const TW_INT32  _nSupportedQueries, /*=TWQC_ALL*/
                                 const TW_UINT16 _unGetCurrentType,  /*=TWON_ONEVALUE*/
                                 const TW_UINT16 _unGetDefaultType) /*=TWON_ONEVALUE*/
{
  m_unCapID           = _unCapID;
  m_unItemType        = _unItemType;
  m_unGetType         = _unGetType;
  m_nMSG_QUERYSUPPORT = _nSupportedQueries;
  m_unGetCurrentType = _unGetCurrentType;
  m_unGetDefaultType = _unGetDefaultType;

  m_nCurrent          = -1;
  m_nDefault          = -1;
}

CTWAINContainer::~CTWAINContainer()
{
}

TW_UINT16 CTWAINContainer::GetCapID()
{
  return m_unCapID;
}

TW_UINT16 CTWAINContainer::GetItemType()
{
  return m_unItemType;
}

TW_UINT16 CTWAINContainer::GetGetType(const TW_UINT16 _unMsg)
{
  switch(_unMsg)
  {
    case MSG_GET:
      return m_unGetType;
      break;
    case MSG_GETCURRENT:
      return m_unGetCurrentType;
      break;
    case MSG_GETDEFAULT:
      return m_unGetDefaultType;
      break;
    default:
      break;
  }
  return TWON_ONEVALUE;
}

unsigned int CTWAINContainer::getTWTYSize(TW_UINT16 _TWType)
{
  unsigned int unSize = 0;

  switch(_TWType)
  {
    case TWTY_INT8:
      unSize = sizeof(TW_INT8);
    break;

    case TWTY_INT16:
      unSize = sizeof(TW_INT16);
    break;

    case TWTY_INT32:
      unSize = sizeof(TW_INT32);
    break;

    case TWTY_UINT8:
      unSize = sizeof(TW_UINT8);
    break;

    case TWTY_UINT16:
      unSize = sizeof(TW_UINT16);
    break;

    case TWTY_BOOL:
      unSize = sizeof(TW_BOOL);
    break;

    case TWTY_UINT32:
      unSize = sizeof(TW_UINT32);
    break;

    case TWTY_FIX32:
      unSize = sizeof(TW_FIX32);
    break;

    case TWTY_FRAME:
      unSize = sizeof(TW_FRAME);
    break;

    case TWTY_STR32:
      unSize = sizeof(TW_STR32);
    break;

    case TWTY_STR64:
      unSize = sizeof(TW_STR64);
    break;

    case TWTY_STR128:
      unSize = sizeof(TW_STR128);
    break;

    case TWTY_STR255:
      unSize = sizeof(TW_STR255);
    break;

    case TWTY_STR1024:
      unSize = sizeof(TW_STR1024);
    break;

    case TWTY_UNI512:
      unSize = sizeof(TW_UNI512);
    break;
  }

  return unSize;
}

bool CTWAINContainer::isOperationAllowed(const TW_UINT16 _unMsg)
{
  bool bret = false;

  switch(_unMsg)
  {
    case MSG_RESET:
      if(m_nMSG_QUERYSUPPORT & TWQC_RESET)
      {
        bret = true;
      }
    break;

    case MSG_SET:
      if(m_nMSG_QUERYSUPPORT & TWQC_SET)
      {
        bret = true;
      }      
    break;
      
    case MSG_GETCURRENT:
      if(m_nMSG_QUERYSUPPORT & TWQC_GETCURRENT)
      {
        bret = true;
      }
    break;

    case MSG_GETDEFAULT:
      if(m_nMSG_QUERYSUPPORT & TWQC_GETDEFAULT)
      {
        bret = true;
      }
    break;

    case MSG_GET:
      if(m_nMSG_QUERYSUPPORT & TWQC_GET)
      {
        bret = true;
      }
    break;
  }

  return bret;
}
