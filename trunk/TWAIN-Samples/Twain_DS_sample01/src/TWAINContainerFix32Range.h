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
* @file TWAINContainerFix32.h
* Fix32 Range Container class for negotiating capabilities.
* @author TWAIN Working Group
* @date April 2007
*/


#ifndef __CTWAINContainerFix32RangeRANGE_H__
#define __CTWAINContainerFix32RangeRANGE_H__

#include "TWAINContainer.h"
typedef struct {
   float  fMinValue;     /* Starting value in the range.           */
   float  fMaxValue;     /* Final value in the range.              */
   float  fStepSize;     /* Increment from MinValue to MaxValue.   */
   float  fCurrentValue; /* The value that is currently in effect. */
} FLOAT_RANGE;
/**
* This class can be used for any Fix32 based TWAIN container. 
*/
class CTWAINContainerFix32Range : public CTWAINContainer
{
  friend class CTWAIN_UI;
public:
  /**
  * Constructor.  
  * @param[in] _unCapID Capability ID
  * @param[in] _unItemType TWAIN Type TWTY_xxxx being stored.
  * @param[in] _unGetType TWON_xxxx container
  * @param[in] _nSupportedQueries the supported querie types TWQC_xxxx
  */
  CTWAINContainerFix32Range(const TW_UINT16 _unCapID, 
                       const FLOAT_RANGE _InitRange, 
                       const TW_INT32  _nSupportedQueries = TWQC_ALL);
  virtual ~CTWAINContainerFix32Range();

  virtual TW_HANDLE GetContainer(const TW_UINT16 _unMsg);
  virtual TW_INT16 Set(pTW_CAPABILITY _pCap, TW_INT16 &Condition);
  virtual bool Reset();

  /**
  * Return the default value through _flVal if set.
  * @param[out] _flVal set the default value on return.
  * @return true if success.
  */
  bool GetDefault(float &_flVal);

  /**
  * Return the current value through _flVal if set.
  * @param[out] _flVal set the current value on return.
  * @return true if success.
  */
  bool GetCurrent(float &_flVal);
  /**
  * Set the current value through _flVal.
  * @param[in] _flVal the current value.
  * @return true if success.
  */
  bool SetCurrent(float _flVal);

  void GetMinMaxStep(float &_flMinVal,float &_flMaxVal,float &_flStepVal);
  int  IsInRange(FLOAT_RANGE _Range, float &_flVal);

protected:
  /**
  * Check to see if type is valid.
  * @param[in] _unTWType type to check
  * @return true if valid
  */
  bool isValidType(const TW_UINT16 _unTWType);

  FLOAT_RANGE m_Cur;
  FLOAT_RANGE m_Def;

};

#endif // __CTWAINContainerFix32RangeRANGE_H__
