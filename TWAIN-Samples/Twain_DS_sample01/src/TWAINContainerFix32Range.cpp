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

CTWAINContainerFix32Range::CTWAINContainerFix32Range(const TW_UINT16 _unCapID, 
                                            const FLOAT_RANGE _InitRange,
                                            const TW_INT32  _nSupportedQueries /*=TWQC_ALL*/)
  : CTWAINContainer(_unCapID, TWTY_FIX32, TWON_RANGE, _nSupportedQueries)
{
  m_Cur = m_Def = _InitRange;
}

CTWAINContainerFix32Range::~CTWAINContainerFix32Range()
{
}

TW_HANDLE CTWAINContainerFix32Range::GetContainer(const TW_UINT16 _unMsg)
{
  TW_HANDLE hContainer = 0;

  if((MSG_GETCURRENT == _unMsg) ||
     (MSG_GETDEFAULT == _unMsg))
  {
    hContainer = _DSM_Alloc(sizeof(TW_ONEVALUE_FIX32));

    if(0 != hContainer)
    {
      pTW_ONEVALUE_FIX32 pCap = (pTW_ONEVALUE_FIX32)_DSM_LockMemory(hContainer);

      pCap->ItemType = TWTY_FIX32;
      const float flVal = (MSG_GETDEFAULT == _unMsg)?m_Def.fCurrentValue:m_Cur.fCurrentValue;
      
      pCap->Item = FloatToFIX32(flVal);
      _DSM_UnlockMemory(hContainer);
    }
  }
  else if(MSG_GET == _unMsg)
  {
    hContainer = _DSM_Alloc(sizeof(TW_RANGE_FIX32)); 

    if(0 != hContainer)
    {
      pTW_RANGE_FIX32 pCap = (pTW_RANGE_FIX32)_DSM_LockMemory(hContainer);

      pCap->ItemType = TWTY_FIX32;
      pCap->CurrentValue = FloatToFIX32(m_Cur.fCurrentValue);
      pCap->DefaultValue = FloatToFIX32(m_Def.fCurrentValue);
      pCap->MaxValue = FloatToFIX32(m_Cur.fMaxValue);
      pCap->MinValue = FloatToFIX32(m_Cur.fMinValue);
      pCap->StepSize = FloatToFIX32(m_Cur.fStepSize);

      _DSM_UnlockMemory(hContainer);
    }
  }

  return hContainer;
}

bool CTWAINContainerFix32Range::isValidType(const TW_UINT16 _unTWType)
{
  return TWTY_FIX32 == _unTWType;
}

TW_INT16 CTWAINContainerFix32Range::Set(pTW_CAPABILITY _pCap, TW_INT16 &Condition)
{
  TW_INT16 twrc = TWRC_SUCCESS;
  Condition = TWCC_SUCCESS;

  if(TWON_ONEVALUE == _pCap->ConType)
  {
    pTW_ONEVALUE_FIX32 pCap = (pTW_ONEVALUE_FIX32)_DSM_LockMemory(_pCap->hContainer);

    if(isValidType(pCap->ItemType))
    {
      float flVal = FIX32ToFloat(pCap->Item);
      int nRes = IsInRange(m_Cur,flVal);

      if(nRes!=0)
      {
        float flTemp = flVal;
        nRes = IsInRange(m_Def,flTemp);
        if(nRes<0)
        {
          twrc = TWRC_FAILURE;
          Condition = TWCC_BADVALUE;
        }
        else if(nRes==0)
        {
          m_Cur = m_Def;
          m_Cur.fCurrentValue = flTemp;
        }
        else if(nRes>0)
        {
          if(flTemp != flVal)
          {
            m_Cur = m_Def;
          }
          m_Cur.fCurrentValue = flTemp;
        }
        twrc = TWRC_CHECKSTATUS;
      }
      else
      {
        m_Cur.fCurrentValue = flVal;
      }
    }
    else
    {
      twrc = TWRC_FAILURE;
      Condition = TWCC_CAPBADOPERATION;
    }
    _DSM_UnlockMemory(_pCap->hContainer);
  }
  else if(TWON_RANGE == _pCap->ConType)
  {
    pTW_RANGE_FIX32 pCap = (pTW_RANGE_FIX32)_DSM_LockMemory(_pCap->hContainer);
    if(isValidType(pCap->ItemType))
    {
      FLOAT_RANGE fNewRange;
      fNewRange.fCurrentValue = FIX32ToFloat(pCap->CurrentValue);
      fNewRange.fMaxValue = FIX32ToFloat(pCap->MaxValue);
      fNewRange.fMinValue = FIX32ToFloat(pCap->MinValue);
      fNewRange.fStepSize = FIX32ToFloat(pCap->StepSize);
      int nRes = 0;
      if(fNewRange.fMinValue>fNewRange.fMaxValue || 
        fNewRange.fCurrentValue<fNewRange.fMinValue || fNewRange.fCurrentValue>fNewRange.fMaxValue ||
        fNewRange.fStepSize <=0 || 
        (fNewRange.fMinValue!=fNewRange.fMaxValue && 
        (((int)((fNewRange.fMaxValue-fNewRange.fMinValue)/fNewRange.fStepSize))*fNewRange.fStepSize+fNewRange.fMinValue)!=fNewRange.fMaxValue) ||
        m_Def.fStepSize>fNewRange.fStepSize ||
        (fNewRange.fMinValue!=fNewRange.fMaxValue && fNewRange.fMinValue+fNewRange.fStepSize>fNewRange.fMaxValue))
      {
        twrc = TWRC_FAILURE;
        Condition = TWCC_BADVALUE;
      }
      if(twrc != TWRC_FAILURE)
      {
        nRes = IsInRange(m_Def,fNewRange.fCurrentValue);
        if(nRes<0)
        {
          twrc = TWRC_FAILURE;
          Condition = TWCC_BADVALUE;
        }    
        else if(nRes>0)
        {
          twrc = TWRC_CHECKSTATUS;
        }
      }
      if(twrc != TWRC_FAILURE)
      {
        nRes = IsInRange(m_Def,fNewRange.fMinValue);
        if(nRes<0)
        {
          twrc = TWRC_FAILURE;
          Condition = TWCC_BADVALUE;
        }    
        else if(nRes>0)
        {
          twrc = TWRC_CHECKSTATUS;
        }
      }
      if(twrc != TWRC_FAILURE)
      {
        nRes = IsInRange(m_Def,fNewRange.fMaxValue);
        if(nRes<0)
        {
          twrc = TWRC_FAILURE;
          Condition = TWCC_BADVALUE;
        }    
        else if(nRes>0)
        {
          twrc = TWRC_CHECKSTATUS;
        }
      }
      if(twrc != TWRC_FAILURE)
      {
        int nIdx  = (int)(fNewRange.fStepSize/m_Def.fStepSize);
        if(nIdx*m_Def.fStepSize != fNewRange.fStepSize)
        {
          twrc = TWRC_CHECKSTATUS;
        }

        if(((nIdx+0.5)*m_Def.fStepSize)>fNewRange.fStepSize)
        {
          fNewRange.fStepSize = nIdx*m_Def.fStepSize;
        }
        else
        {
          fNewRange.fStepSize = (nIdx+1)*m_Def.fStepSize;
        }
      }
      if(twrc != TWRC_FAILURE)
      {
        if(IsInRange(fNewRange,fNewRange.fCurrentValue)<0)
        {
          twrc = TWRC_FAILURE;
        }
      }
      if(twrc != TWRC_FAILURE)
      {
        fNewRange.fMaxValue = ((int)((fNewRange.fMaxValue - fNewRange.fMinValue)/fNewRange.fStepSize))*fNewRange.fStepSize + fNewRange.fMinValue;
        m_Cur = fNewRange;
      }
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

bool CTWAINContainerFix32Range::Reset()
{
  m_Cur = m_Def;
  return true;
}

bool CTWAINContainerFix32Range::GetCurrent(float &_flVal)
{
  _flVal = m_Cur.fCurrentValue;

  return true;
}

bool CTWAINContainerFix32Range::SetCurrent(float _flVal)
{
  int nRes = IsInRange(m_Cur,_flVal);

  if(nRes!=0)
  {
    float flTemp = _flVal;
    nRes = IsInRange(m_Def,flTemp);
    if(nRes<0)
    {
      return false;
    }
    else if(nRes==0)
    {
      m_Cur = m_Def;
      m_Cur.fCurrentValue = _flVal;
    }
    else if(nRes>0)
    {
      if(flTemp != _flVal)
      {
        m_Cur = m_Def;
      }
      m_Cur.fCurrentValue = flTemp;
    }
  }
  else
  {
    m_Cur.fCurrentValue = _flVal;
  }
  return true;
}

bool CTWAINContainerFix32Range::GetDefault(float &_flVal)
{
  _flVal = m_Def.fCurrentValue;

  return true;
}

void CTWAINContainerFix32Range::GetMinMaxStep(float &_flMinVal,float &_flMaxVal,float &_flStepVal)
{
  _flMinVal=m_Cur.fMinValue;
  _flMaxVal=m_Cur.fMaxValue;
  _flStepVal=m_Cur.fStepSize;
}

int CTWAINContainerFix32Range::IsInRange(FLOAT_RANGE _Range, float &_flVal)
{
  if(_flVal<_Range.fMinValue || _flVal>_Range.fMaxValue)
  {
    return -1;
  }
  if(_Range.fMinValue == _Range.fMaxValue && _flVal!=_Range.fMinValue)//@TODO remove flaot comparision
  {
    return -1;
  }

  int nIdx = (int)((_flVal-_Range.fMinValue)/_Range.fStepSize);
  float fTemp = nIdx*_Range.fStepSize+_Range.fMinValue;
  if(fTemp==_flVal)//@TODO remove float comparision
  {
    return 0;
  }

  if((_flVal+_Range.fStepSize/2)>fTemp)
  {
    _flVal = fTemp;
  }
  else
  {
    _flVal = fTemp-_Range.fStepSize;
  }
  return 1;
}

