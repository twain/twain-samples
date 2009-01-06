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
* @file CommonTWAIN.cpp
* Utilities functions used by TWAIN Data Sources
* 
* @author TWAIN Working Group
* @date October 2007
*/
#include "CommonTWAIN.h"

extern TW_HANDLE _DSM_Alloc(TW_UINT32 _size);
extern TW_MEMREF _DSM_LockMemory(TW_HANDLE _hMemory);
extern void _DSM_UnlockMemory(TW_HANDLE _hMemory);
extern void _DSM_Free(TW_HANDLE _hMemory);


//////////////////////////////////////////////////////////////////////////////
TW_FIX32 FloatToFIX32 (float floater)
{
  TW_FIX32 Fix32_value;
  TW_INT32 value = (TW_INT32) (floater * 65536.0 + 0.5);
  Fix32_value.Whole = (TW_UINT16)(value >> 16);
  Fix32_value.Frac = (TW_UINT16)(value & 0x0000ffffL);
  return (Fix32_value);
}


//////////////////////////////////////////////////////////////////////////////
float FIX32ToFloat(const TW_FIX32& _fix32)
{
  return float(_fix32.Whole) + float(_fix32.Frac / 65536.0);
}


//////////////////////////////////////////////////////////////////////////////
bool getcurrent(TW_CAPABILITY *pCap, TW_INT16& val)
{
  bool bret = false;

  if(0 != pCap->hContainer)
  {
    if(TWON_ENUMERATION == pCap->ConType)
    {
      pTW_ENUMERATION_INT16 pCapPT = (pTW_ENUMERATION_INT16)_DSM_LockMemory(pCap->hContainer);

      if(TWTY_INT16 == pCapPT->ItemType)
      {
        val = pCapPT->ItemList[pCapPT->CurrentIndex];
        bret = true;
      }
      _DSM_UnlockMemory(pCap->hContainer);
    }
    else if(TWON_ONEVALUE == pCap->ConType)
    {
      pTW_ONEVALUE_INT16 pCapPT = (pTW_ONEVALUE_INT16)_DSM_LockMemory(pCap->hContainer);

      if(TWTY_INT16 == pCapPT->ItemType)
      {
        val = pCapPT->Item;
        bret = true;
      }
      _DSM_UnlockMemory(pCap->hContainer);
    }
  }

  return bret;
}

//////////////////////////////////////////////////////////////////////////////
bool getcurrent(TW_CAPABILITY *pCap, TW_INT32& val)
{
  bool bret = false;

  if(0 != pCap->hContainer)
  {
    if(TWON_ENUMERATION == pCap->ConType)
    {
      pTW_ENUMERATION_INT32 pCapPT = (pTW_ENUMERATION_INT32)_DSM_LockMemory(pCap->hContainer);

      if(TWTY_INT32 == pCapPT->ItemType)
      {
        val = pCapPT->ItemList[pCapPT->CurrentIndex];
        bret = true;
      }
      _DSM_UnlockMemory(pCap->hContainer);
    }
    else if(TWON_ONEVALUE == pCap->ConType)
    {
      pTW_ONEVALUE_INT32 pCapPT = (pTW_ONEVALUE_INT32)_DSM_LockMemory(pCap->hContainer);

      if(TWTY_INT32 == pCapPT->ItemType)
      {
        val = pCapPT->Item;
        bret = true;
      }
      _DSM_UnlockMemory(pCap->hContainer);
    }
  }

  return bret;
}


//////////////////////////////////////////////////////////////////////////////
bool getcurrent(TW_CAPABILITY *pCap, TW_UINT16& val)
{
  bool bret = false;

  if(0 != pCap->hContainer)
  {
    if(TWON_ENUMERATION == pCap->ConType)
    {
      pTW_ENUMERATION_UINT16 pCapPT = (pTW_ENUMERATION_UINT16)_DSM_LockMemory(pCap->hContainer);

      if(TWTY_UINT16 == pCapPT->ItemType)
      {
        val = pCapPT->ItemList[pCapPT->CurrentIndex];
        bret = true;
      }
      _DSM_UnlockMemory(pCap->hContainer);
    }
    else if(TWON_ONEVALUE == pCap->ConType)
    {
      pTW_ONEVALUE pCapPT = (pTW_ONEVALUE)_DSM_LockMemory(pCap->hContainer);
      
      if(TWTY_UINT16 == pCapPT->ItemType)
      {
        val = (TW_UINT16)(pCapPT->Item);
        bret = true;
      }
      _DSM_UnlockMemory(pCap->hContainer);
    }
  }

  return bret;
}

//////////////////////////////////////////////////////////////////////////////
bool getcurrent(TW_CAPABILITY *pCap, TW_UINT32& val)
{
  bool bret = false;

  if(0 != pCap->hContainer)
  {
    if(TWON_ENUMERATION == pCap->ConType)
    {
      pTW_ENUMERATION_UINT32 pCapPT = (pTW_ENUMERATION_UINT32)_DSM_LockMemory(pCap->hContainer);

      if(TWTY_UINT32 == pCapPT->ItemType)
      {
        val = pCapPT->ItemList[pCapPT->CurrentIndex];
        bret = true;
      }
      _DSM_UnlockMemory(pCap->hContainer);
    }
    else if(TWON_ONEVALUE == pCap->ConType)
    {
      pTW_ONEVALUE_UINT32 pCapPT = (pTW_ONEVALUE_UINT32)_DSM_LockMemory(pCap->hContainer);

      if(TWTY_UINT32 == pCapPT->ItemType)
      {
        val = pCapPT->Item;
        bret = true;
      }
      _DSM_UnlockMemory(pCap->hContainer);
    }
  }

  return bret;
}

//////////////////////////////////////////////////////////////////////////////
bool getcurrent(TW_CAPABILITY *pCap, TW_STR32& val)
{
  bool bret = false;

  if(0 != pCap->hContainer)
  {
    if(TWON_ENUMERATION == pCap->ConType)
    {
      pTW_ENUMERATION_STR32 pCapPT = (pTW_ENUMERATION_STR32)_DSM_LockMemory(pCap->hContainer);

      if(TWTY_STR32 == pCapPT->ItemType)
      {
        SSTRCPY(val, sizeof(val),pCapPT->ItemList[pCapPT->CurrentIndex]);
        bret = true;
      }
      _DSM_UnlockMemory(pCap->hContainer);
    }
    else if(TWON_ONEVALUE == pCap->ConType)
    {
      pTW_ONEVALUE_STR32 pCapPT = (pTW_ONEVALUE_STR32)_DSM_LockMemory(pCap->hContainer);

      if(TWTY_STR32 == pCapPT->ItemType)
      {
        SSTRCPY(val, sizeof(val), pCapPT->Item);
        bret = true;
      }
      _DSM_UnlockMemory(pCap->hContainer);
    }
  }

  return bret;
}

//////////////////////////////////////////////////////////////////////////////
bool getcurrent(TW_CAPABILITY *pCap, TW_STR64& val)
{
  bool bret = false;

  if(0 != pCap->hContainer)
  {
    if(TWON_ENUMERATION == pCap->ConType)
    {
      pTW_ENUMERATION_STR64 pCapPT = (pTW_ENUMERATION_STR64)_DSM_LockMemory(pCap->hContainer);

      if(TWTY_STR64 == pCapPT->ItemType)
      {
        SSTRCPY(val, sizeof(val),pCapPT->ItemList[pCapPT->CurrentIndex]);
        bret = true;
      }
      _DSM_UnlockMemory(pCap->hContainer);
    }
    else if(TWON_ONEVALUE == pCap->ConType)
    {
      pTW_ONEVALUE_STR64 pCapPT = (pTW_ONEVALUE_STR64)_DSM_LockMemory(pCap->hContainer);

      if(TWTY_STR64 == pCapPT->ItemType)
      {
        SSTRCPY(val, sizeof(val), pCapPT->Item);
        bret = true;
      }
      _DSM_UnlockMemory(pCap->hContainer);
    }
  }

  return bret;
}

//////////////////////////////////////////////////////////////////////////////
bool getcurrent(TW_CAPABILITY *pCap, TW_STR128& val)
{
  bool bret = false;

  if(0 != pCap->hContainer)
  {
    if(TWON_ENUMERATION == pCap->ConType)
    {
      pTW_ENUMERATION_STR128 pCapPT = (pTW_ENUMERATION_STR128)_DSM_LockMemory(pCap->hContainer);

      if(TWTY_STR128 == pCapPT->ItemType)
      {
        SSTRCPY(val, sizeof(val),pCapPT->ItemList[pCapPT->CurrentIndex]);
        bret = true;
      }
      _DSM_UnlockMemory(pCap->hContainer);
    }
    else if(TWON_ONEVALUE == pCap->ConType)
    {
      pTW_ONEVALUE_STR128 pCapPT = (pTW_ONEVALUE_STR128)_DSM_LockMemory(pCap->hContainer);

      if(TWTY_STR128 == pCapPT->ItemType)
      {
        SSTRCPY(val, sizeof(val), pCapPT->Item);
        bret = true;
      }
      _DSM_UnlockMemory(pCap->hContainer);
    }
  }

  return bret;
}

//////////////////////////////////////////////////////////////////////////////
bool getcurrent(TW_CAPABILITY *pCap, TW_STR255& val)
{
  bool bret = false;

  if(0 != pCap->hContainer)
  {
    if(TWON_ENUMERATION == pCap->ConType)
    {
      pTW_ENUMERATION_STR255 pCapPT = (pTW_ENUMERATION_STR255)_DSM_LockMemory(pCap->hContainer);

      if(TWTY_STR255 == pCapPT->ItemType)
      {
        SSTRCPY(val, sizeof(val),pCapPT->ItemList[pCapPT->CurrentIndex]);
        bret = true;
      }
      _DSM_UnlockMemory(pCap->hContainer);
    }
    else if(TWON_ONEVALUE == pCap->ConType)
    {
      pTW_ONEVALUE_STR255 pCapPT = (pTW_ONEVALUE_STR255)_DSM_LockMemory(pCap->hContainer);

      if(TWTY_STR255 == pCapPT->ItemType)
      {
        SSTRCPY(val, sizeof(val), pCapPT->Item);
        bret = true;
      }
      _DSM_UnlockMemory(pCap->hContainer);
    }
  }

  return bret;
}

//////////////////////////////////////////////////////////////////////////////
bool getcurrent(TW_CAPABILITY *pCap, TW_FIX32& val)
{
  bool bret = false;

  if(0 != pCap->hContainer)
  {
    if(TWON_ENUMERATION == pCap->ConType)
    {
      pTW_ENUMERATION_FIX32 pCapPT = (pTW_ENUMERATION_FIX32)_DSM_LockMemory(pCap->hContainer);

      if(TWTY_FIX32 == pCapPT->ItemType)
      {
        val = pCapPT->ItemList[pCapPT->CurrentIndex];
        bret = true;
      }
      _DSM_UnlockMemory(pCap->hContainer);
    }
    else if(TWON_ONEVALUE == pCap->ConType)
    {
      pTW_ONEVALUE_FIX32 pCapPT = (pTW_ONEVALUE_FIX32)_DSM_LockMemory(pCap->hContainer);
      
      if(TWTY_FIX32 == pCapPT->ItemType)
      {
        val = pCapPT->Item;
        bret = true;
      }
      _DSM_UnlockMemory(pCap->hContainer);
    }
  }
  
  return bret;
}

//////////////////////////////////////////////////////////////////////////////
bool getcurrent(TW_CAPABILITY *pCap, TW_FRAME& frame)
{
  bool bret = false;

  if(0 != pCap->hContainer)
  {
    if(TWON_ENUMERATION == pCap->ConType)
    {
      pTW_ENUMERATION_FRAME pCapPT = (pTW_ENUMERATION_FRAME)_DSM_LockMemory(pCap->hContainer);

      if(TWTY_FRAME == pCapPT->ItemType)
      {
        frame = pCapPT->ItemList[pCapPT->CurrentIndex];
        bret = true;
      }
      _DSM_UnlockMemory(pCap->hContainer);
    }
    else if(TWON_ONEVALUE == pCap->ConType)
    {
      pTW_ONEVALUE_FRAME pCapPT = (pTW_ONEVALUE_FRAME)_DSM_LockMemory(pCap->hContainer);
      
      if(TWTY_FRAME == pCapPT->ItemType)
      {
        frame = pCapPT->Item;
        bret = true;
      }
      _DSM_UnlockMemory(pCap->hContainer);
    }
  }
  
  return bret;
}

