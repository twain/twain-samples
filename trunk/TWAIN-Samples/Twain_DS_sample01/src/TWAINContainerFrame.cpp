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
* @file TWAINContainerFrame.cpp
* Frame Container class for negotiating capabilities.
* @author TWAIN Working Group
* @date April 2007
*/


#include "CommonDS.h"
#include <algorithm>

InternalFrame::InternalFrame()
{
  unLeft   = 0;
  unTop    = 0;
  unRight  = 0;
  unBottom = 0;
}

InternalFrame::InternalFrame(UINT left, UINT top, UINT right, UINT bottom)
{
  unLeft   = left;
  unTop    = top;
  unRight  = right;
  unBottom = bottom;
}

InternalFrame::InternalFrame(float left, float top, float right, float bottom, int fromUnits, float Xresolution, float Yresolution)
{
  unLeft   = int(ConvertUnits(left,   fromUnits, TWUN_1000INCHES, Xresolution));
  unTop    = int(ConvertUnits(top,    fromUnits, TWUN_1000INCHES, Yresolution));
  unRight  = int(ConvertUnits(right,  fromUnits, TWUN_1000INCHES, Xresolution));
  unBottom = int(ConvertUnits(bottom, fromUnits, TWUN_1000INCHES, Yresolution));
}

InternalFrame::InternalFrame(const TW_FRAME twFrame, int fromUnits, float Xresolution, float Yresolution)
{
  unLeft   = int(ConvertUnits(FIX32ToFloat(twFrame.Left),   fromUnits, TWUN_1000INCHES, Xresolution));
  unTop    = int(ConvertUnits(FIX32ToFloat(twFrame.Top),    fromUnits, TWUN_1000INCHES, Yresolution));
  unRight  = int(ConvertUnits(FIX32ToFloat(twFrame.Right),  fromUnits, TWUN_1000INCHES, Xresolution));
  unBottom = int(ConvertUnits(FIX32ToFloat(twFrame.Bottom), fromUnits, TWUN_1000INCHES, Yresolution));
}

TW_FRAME InternalFrame::AsTW_FRAME(int toUnits, float Xresolution, float Yresolution)
{
  TW_FRAME frame;
  frame.Left   = FloatToFIX32(ConvertUnits(float(unLeft),   TWUN_1000INCHES, toUnits, Xresolution));
  frame.Top    = FloatToFIX32(ConvertUnits(float(unTop),    TWUN_1000INCHES, toUnits, Yresolution));
  frame.Right  = FloatToFIX32(ConvertUnits(float(unRight),  TWUN_1000INCHES, toUnits, Xresolution));
  frame.Bottom = FloatToFIX32(ConvertUnits(float(unBottom), TWUN_1000INCHES, toUnits, Yresolution));
  return frame;
}

InternalFrame::InternalFrame(const TW_UINT16 ss)	
{
  //see which supported size they want the dimensions of
	switch(ss)	
	{
		case TWSS_USLEDGER:
			InternalFrame( 0, 0, 11.0f,  17.0f, TWUN_INCHES, 100, 100);
      break;
		case TWSS_USLEGAL:
			InternalFrame( 0, 0,  8.5f,  14.0f, TWUN_INCHES, 100, 100);
      break;
		case TWSS_USLETTER:
			InternalFrame( 0, 0,  8.5f,  11.0f, TWUN_INCHES, 100, 100);
      break;
		case TWSS_USEXECUTIVE:
			InternalFrame( 0, 0,  7.25f, 10.5f, TWUN_INCHES, 100, 100);
      break;
		case TWSS_USSTATEMENT:
			InternalFrame( 0, 0,  5.5f,   8.5f, TWUN_INCHES, 100, 100);
      break;
		case TWSS_BUSINESSCARD:
			InternalFrame( 0, 0,  3.5f,   2.0f, TWUN_INCHES, 100, 100);
      break;

    case TWSS_4A0:
			InternalFrame( 0, 0,168.2f, 237.8f, TWUN_CENTIMETERS, 100, 100);
      break;
		case TWSS_2A0:
			InternalFrame( 0, 0,118.9f, 168.2f, TWUN_CENTIMETERS, 100, 100);
      break;
		case TWSS_A0:
			InternalFrame( 0, 0, 84.1f, 118.9f, TWUN_CENTIMETERS, 100, 100);
      break;
		case TWSS_A1:
			InternalFrame( 0, 0, 59.4f,  84.1f, TWUN_CENTIMETERS, 100, 100);
      break;
		case TWSS_A2:
			InternalFrame( 0, 0, 42.0f,  59.4f, TWUN_CENTIMETERS, 100, 100);
      break;
		case TWSS_A3:
			InternalFrame( 0, 0, 29.7f,  42.0f, TWUN_CENTIMETERS, 100, 100);
      break;
		case TWSS_A4:  //	case TWSS_A4LETTER:
			InternalFrame( 0, 0, 21.0f,  29.7f, TWUN_CENTIMETERS, 100, 100);
      break;
		case TWSS_A5:
			InternalFrame( 0, 0, 14.8f,  21.0f, TWUN_CENTIMETERS, 100, 100);
      break;
		case TWSS_A6:
			InternalFrame( 0, 0, 10.5f,  14.8f, TWUN_CENTIMETERS, 100, 100);
      break;
		case TWSS_A7:
			InternalFrame( 0, 0,  7.4f,  10.5f, TWUN_CENTIMETERS, 100, 100);
      break;
		case TWSS_A8:
			InternalFrame( 0, 0,  5.2f,   7.4f, TWUN_CENTIMETERS, 100, 100);
      break;
		case TWSS_A9:
			InternalFrame( 0, 0,  3.7f,   5.2f, TWUN_CENTIMETERS, 100, 100);
      break;
		case TWSS_A10:
			InternalFrame( 0, 0,  2.6f,   3.7f, TWUN_CENTIMETERS, 100, 100);
      break;

		case TWSS_ISOB0:
			InternalFrame( 0, 0,100.0f, 141.4f, TWUN_CENTIMETERS, 100, 100);
      break;
		case TWSS_ISOB1:
			InternalFrame( 0, 0, 70.7f, 100.0f, TWUN_CENTIMETERS, 100, 100);
      break;
		case TWSS_ISOB2:
			InternalFrame( 0, 0, 50.0f,  70.7f, TWUN_CENTIMETERS, 100, 100);
      break;
		case TWSS_ISOB3:  //  case TWSS_B3:
			InternalFrame( 0, 0, 35.3f,  50.0f, TWUN_CENTIMETERS, 100, 100);
      break;
		case TWSS_ISOB4:  //  case TWSS_B4:
			InternalFrame( 0, 0, 25.0f,  35.4f, TWUN_CENTIMETERS, 100, 100);
      break;
		case TWSS_ISOB5:
			InternalFrame( 0, 0, 17.6f,  25.0f, TWUN_CENTIMETERS, 100, 100);
      break;
		case TWSS_ISOB6:  //	case TWSS_B6:
			InternalFrame( 0, 0, 12.5f,  17.6f, TWUN_CENTIMETERS, 100, 100);
      break;
		case TWSS_ISOB7:
			InternalFrame( 0, 0,  8.8f,  12.5f, TWUN_CENTIMETERS, 100, 100);
      break;
		case TWSS_ISOB8:
			InternalFrame( 0, 0,  6.2f,   8.8f, TWUN_CENTIMETERS, 100, 100);
      break;
		case TWSS_ISOB9:
			InternalFrame( 0, 0,  4.4f,   6.2f, TWUN_CENTIMETERS, 100, 100);
      break;
		case TWSS_ISOB10:
			InternalFrame( 0, 0,  3.1f,   4.4f, TWUN_CENTIMETERS, 100, 100);
      break;

		case TWSS_JISB0:
			InternalFrame( 0, 0,103.0f, 145.6f, TWUN_CENTIMETERS, 100, 100);
      break;
		case TWSS_JISB1:
			InternalFrame( 0, 0, 72.8f, 103.0f, TWUN_CENTIMETERS, 100, 100);
      break;
		case TWSS_JISB2:
			InternalFrame( 0, 0, 51.5f,  72.8f, TWUN_CENTIMETERS, 100, 100);
      break;
		case TWSS_JISB3:
			InternalFrame( 0, 0, 36.4f,  51.5f, TWUN_CENTIMETERS, 100, 100);
      break;
		case TWSS_JISB4:
			InternalFrame( 0, 0, 25.7f,  36.4f, TWUN_CENTIMETERS, 100, 100);
      break;
		case TWSS_JISB5:  //	case TWSS_B5LETTER:
			InternalFrame( 0, 0, 18.2f,  25.7f, TWUN_CENTIMETERS, 100, 100);
      break;
		case TWSS_JISB6:
			InternalFrame( 0, 0, 12.8f,  18.2f, TWUN_CENTIMETERS, 100, 100);
      break;
		case TWSS_JISB7:
			InternalFrame( 0, 0,  9.1f,  12.8f, TWUN_CENTIMETERS, 100, 100);
      break;
    case TWSS_JISB8:
			InternalFrame( 0, 0,  6.4f,   9.1f, TWUN_CENTIMETERS, 100, 100);
      break;
		case TWSS_JISB9:
			InternalFrame( 0, 0,  4.5f,   6.4f, TWUN_CENTIMETERS, 100, 100);
      break;
		case TWSS_JISB10:
			InternalFrame( 0, 0,  3.2f,   4.5f, TWUN_CENTIMETERS, 100, 100);
      break;

		case TWSS_C0:
			InternalFrame( 0, 0, 91.7f, 129.7f, TWUN_CENTIMETERS, 100, 100);
      break;
		case TWSS_C1:
			InternalFrame( 0, 0, 64.8f,  91.7f, TWUN_CENTIMETERS, 100, 100);
      break;
		case TWSS_C2:
			InternalFrame( 0, 0, 45.8f,  64.8f, TWUN_CENTIMETERS, 100, 100);
      break;
		case TWSS_C3:
			InternalFrame( 0, 0, 32.4f,  45.8f, TWUN_CENTIMETERS, 100, 100);
      break;
		case TWSS_C4:
			InternalFrame( 0, 0, 22.9f,  32.4f, TWUN_CENTIMETERS, 100, 100);
      break;
		case TWSS_C5:
			InternalFrame( 0, 0, 16.2f,  22.9f, TWUN_CENTIMETERS, 100, 100);
      break;
		case TWSS_C6:
			InternalFrame( 0, 0, 11.4f,  16.2f, TWUN_CENTIMETERS, 100, 100);
      break;
		case TWSS_C7:
			InternalFrame( 0, 0,  8.1f,  11.4f, TWUN_CENTIMETERS, 100, 100);
      break;
		case TWSS_C8:
			InternalFrame( 0, 0,  5.7f,   8.1f, TWUN_CENTIMETERS, 100, 100);
      break;
		case TWSS_C9:
			InternalFrame( 0, 0,  4.0f,   5.7f, TWUN_CENTIMETERS, 100, 100);
      break;
		case TWSS_C10:
			InternalFrame( 0, 0,  2.8f,   4.0f, TWUN_CENTIMETERS, 100, 100);
      break;

    default:
      InternalFrame(0, 0, 0, 0);	//if we don't recognize their size, return a default frame
      break;
	}

	
}



bool operator== (const InternalFrame& _frame1, const InternalFrame& _frame2)
{
  if((2> abs((int)_frame1.unLeft   - (int)_frame2.unLeft)  ) &&
     (2> abs((int)_frame1.unTop    - (int)_frame2.unTop)   ) &&
     (2> abs((int)_frame1.unRight  - (int)_frame2.unRight) ) &&
     (2> abs((int)_frame1.unBottom - (int)_frame2.unBottom)) )
  {
    return true;
  }

  return false;
}

 
//////////////////////////////////////////////////////////////////////////////


CTWAINContainerFrame::CTWAINContainerFrame(const TW_UINT16 _unCapID, 
                                           const TW_UINT16 _unItemType, 
                                           const TW_UINT16 _unGetType, 
                                           const TW_INT32  _nSupportedQueries /*=TWQC_ALL*/) 
  : CTWAINContainer(_unCapID, _unItemType, _unGetType, _nSupportedQueries)
{
  m_Unit = TWUN_PIXELS;
  m_Xres = 100;
  m_Yres = 100;
}

CTWAINContainerFrame::~CTWAINContainerFrame()
{
}

TW_HANDLE CTWAINContainerFrame::GetContainer(const TW_UINT16 _unMsg)
{
  TW_HANDLE hContainer = 0;

  if((TWON_ONEVALUE == m_unGetType) ||
     (MSG_GETCURRENT == _unMsg) ||
     (MSG_GETDEFAULT == _unMsg))
  {
    hContainer = _DSM_Alloc(sizeof(TW_ONEVALUE_FRAME));

    if(0 != hContainer)
    {
      pTW_ONEVALUE_FRAME pCap = (pTW_ONEVALUE_FRAME)_DSM_LockMemory(hContainer);
      // If the Cap has been constrained the default may only be in the defaultlist.
      InternalFrame frame = (MSG_GETDEFAULT == _unMsg)?m_listFramesDefault[m_nDefault]:m_listFrames[m_nCurrent];

      pCap->Item = frame.AsTW_FRAME(m_Unit, m_Xres, m_Yres);
      pCap->ItemType = m_unItemType;

      _DSM_UnlockMemory((TW_MEMREF)pCap);
    }
  }
  else if(MSG_GET == _unMsg)
  {
    if(TWON_ENUMERATION == m_unGetType)
    {
      hContainer = _DSM_Alloc(sizeof(TW_ENUMERATION_FRAME) + (sizeof(TW_FRAME) * (m_listFrames.size()-1))); // -1 because already contains 1 element

      if(0 != hContainer)
      {
        pTW_ENUMERATION_FRAME pCap = (pTW_ENUMERATION_FRAME)_DSM_LockMemory(hContainer);

        pCap->ItemType = m_unItemType;
        pCap->NumItems = (TW_UINT32)m_listFrames.size();
        pCap->CurrentIndex = m_nCurrent;
                //If the CAP has been constrained m_nDefault index might not point 
        // to the correct index and the index may not be valid.  We need to 
        // find the value in the default list and see if we can find a match 
        // in the current list.  If no match found then set to first index.
        // see spec on twain.org Chap4 p73 Advanced Application Implementation | 
        // Capabilities | Constrained Capabilities and Message Responses | MSG_SET
        const InternalFrame frame = m_listFramesDefault[m_nDefault];
        int nIndex = getIndexForValue(frame);
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
          pCap->ItemList[x] = m_listFrames[x].AsTW_FRAME(m_Unit, m_Xres, m_Yres);
        }

        _DSM_UnlockMemory((TW_MEMREF)pCap);
      }
    }
    else if(TWON_ARRAY == m_unGetType)
    {
      hContainer = _DSM_Alloc(sizeof(TW_ARRAY_FRAME) + (sizeof(TW_FRAME) * (m_listFrames.size()-1))); // -1 because a TW_ARRAY_FIX32 already includes 1 element

      if(0 != hContainer)
      {
        pTW_ARRAY_FRAME pCap = (pTW_ARRAY_FRAME)_DSM_LockMemory(hContainer);
        pCap->ItemType = m_unItemType;
        pCap->NumItems = (TW_UINT32)m_listFrames.size();

        for(TW_UINT32 x = 0; x < pCap->NumItems; ++x)
        {
          pCap->ItemList[x] = m_listFrames[x].AsTW_FRAME(m_Unit, m_Xres, m_Yres);
        }

        _DSM_UnlockMemory((TW_MEMREF)pCap);
      }
    }
  }

  return hContainer;
}

bool CTWAINContainerFrame::isValidType(const TW_UINT16 _unTWType)
{
  bool bret = false;

  if(TWTY_FRAME == _unTWType)
  {
    bret = true;
  }

  return bret;
}

TW_INT16 CTWAINContainerFrame::Set(pTW_CAPABILITY _pCap, TW_INT16 &Condition)
{
  // TW_FRAME can override the current and available list regardless if they
  // are available in the default list.
  TW_INT16 twrc = TWRC_SUCCESS;
  Condition = TWCC_SUCCESS;

  if(TWON_ONEVALUE == _pCap->ConType)
  {
    pTW_ONEVALUE_FRAME pCap = (pTW_ONEVALUE_FRAME)_DSM_LockMemory(_pCap->hContainer);

    if(isValidType(pCap->ItemType))
    {
      InternalFrame frame(pCap->Item, m_Unit, m_Xres, m_Yres);

      switch(m_unGetType)
      {
        case TWON_ONEVALUE:
        case TWON_ENUMERATION:
        {
          /// @todo check to see if frame is in range
          m_listFrames.clear();
          m_listFrames.push_back(frame);
          m_nCurrent = 0;
        }
        break;
        /*
        case TWON_ENUMERATION:
        {
          int nVal = -1;
          if((nVal = getIndexForValue(frame)) >= 0)
          {
            m_nCurrent = nVal;
          }
          else
          {
            twrc = TWRC_CHECKSTATUS;
            Condition = TWCC_BADVALUE;
          }
        }
        break;
        */
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

    _DSM_UnlockMemory((TW_MEMREF)pCap);
  }
  else if(TWON_ENUMERATION == _pCap->ConType)
  {
    pTW_ENUMERATION_FRAME pCap = (pTW_ENUMERATION_FRAME)_DSM_LockMemory(_pCap->hContainer);

    if(isValidType(pCap->ItemType))
    {
      m_nCurrent = pCap->CurrentIndex;
      // We ignor trying to set the default.  The default can not be changed from outside

      FrameVector::iterator iter;
      m_listFrames.clear();

      for(TW_UINT32 x = 0; x < pCap->NumItems; ++x)
      {
        InternalFrame frame(pCap->ItemList[x], m_Unit, m_Xres, m_Yres);

        // Only add items that are within the range of the frame regardless what is in our m_listFramesDefault
        // iter = find(m_listFramesDefault.begin(), m_listFramesDefault.end(), frame);

        /// @todo check if value is within range
        if(1)
        {
          m_listFrames.push_back(frame);
          m_nCurrent = 0;
        }
        else
        {
          twrc = TWRC_CHECKSTATUS;          
          Condition = TWCC_SUCCESS;
        }
      }
    }

    _DSM_UnlockMemory((TW_MEMREF)pCap);
  }

  return twrc;
}

bool CTWAINContainerFrame::Reset()
{
  m_nCurrent = m_nDefault;

  m_listFrames.clear();

  const int nSize = (int)(m_listFramesDefault.size());

  for(int x = 0; x < nSize; ++x)
  {
    m_listFrames.push_back(m_listFramesDefault[x]);
  }

  return true;
}

bool CTWAINContainerFrame::GetCurrent(InternalFrame& _frame)
{
  bool bret = false;

  if(!(m_nMSG_QUERYSUPPORT & TWQC_GETCURRENT))
  {
    return false;
  }

  if((m_nCurrent >= 0) && ((int)(m_listFrames.size()) > m_nCurrent))
  {    
    _frame = m_listFrames[m_nCurrent];
    bret = true;
  }

  return bret;
}

bool CTWAINContainerFrame::GetDefault(InternalFrame& _frame)
{
  bool bret = false;

  if(!(m_nMSG_QUERYSUPPORT & TWQC_GETDEFAULT))
  {
    return false;
  }

  if((m_nDefault >= 0) && ((int)(m_listFramesDefault.size()) > m_nDefault))
  {
    _frame = m_listFramesDefault[m_nDefault];
    bret = true;
  }

  return bret;
}

const FrameVector &CTWAINContainerFrame::GetSupported()
{
  return m_listFrames;
}

bool CTWAINContainerFrame::Add(const UINT _unLeft, const UINT _unTop, const UINT _unRight, const UINT _unBottom, bool _bDefault /*= false*/)
{
  InternalFrame frame(_unLeft, _unTop, _unRight, _unBottom);
  
  return Add(frame, _bDefault);
}

bool CTWAINContainerFrame::Add(const InternalFrame& _frame, bool _bDefault /*= false*/)
{
  m_listFrames.push_back(_frame);
  m_listFramesDefault.push_back(_frame);
  if(m_nDefault == -1 || _bDefault)
  {
    m_nCurrent = m_nDefault = getIndexForValue(_frame);
  }
  return true;
}

bool CTWAINContainerFrame::SetCurrent(const UINT _unLeft, const UINT _unTop, const UINT _unRight, const UINT _unBottom)
{
  InternalFrame frame(_unLeft, _unTop, _unRight, _unBottom);
  
  return SetCurrent(frame);
}

bool CTWAINContainerFrame::SetCurrent(const InternalFrame& _frame)
{
  bool bret = true;

  if((m_nCurrent = getIndexForValue(_frame)) < 0)
  {
    bret = false;
  }

  return bret;
}

int CTWAINContainerFrame::getIndexForValue(const InternalFrame& _frame)
{
  int ret = -1;

  const int nSize = (int)(m_listFrames.size());

  for(int x = 0; x < nSize; ++x)
  {
    if(_frame == m_listFrames[x])
    {
      ret = x; 
      break;
    }
  }

  return ret;
}


void CTWAINContainerFrame::setCurrentUnits(int Unit, float Xres, float Yres)
{
 m_Unit = Unit;
 m_Xres = Xres;
 m_Yres = Yres;
}
