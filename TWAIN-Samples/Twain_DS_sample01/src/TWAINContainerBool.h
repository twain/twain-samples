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
* @file TWAINContainerBool.h
* Fix32 Container class for negotiating capabilities.
* @author TWAIN Working Group
* @date April 2007
*/


#ifndef __CTWAINCONTAINERBOOL_H__
#define __CTWAINCONTAINERBOOL_H__

#include "TWAINContainer.h"

/**
* This class can be used for any Bool based TWAIN container. 
*/
class CTWAINContainerBool : public CTWAINContainer
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
  CTWAINContainerBool(const TW_UINT16 _unCapID, 
                      const bool _bAPP2,
                      const TW_INT32  _nSupportedQueries = TWQC_ALL);
  virtual ~CTWAINContainerBool();
  /**
  * Return the TWON_xxxx containor type.
  * @param[in] _MSG the MSG_GETxxxx get message.
  * @return the containor type
  */
  virtual TW_UINT16 GetGetType(const TW_UINT16 _unMsg);
  virtual TW_HANDLE GetContainer(const TW_UINT16 _unMsg);
  virtual TW_INT16 Set(pTW_CAPABILITY _pCap, TW_INT16 &Condition);
  virtual bool Reset();

  // For float vals
  /**
  * Try to add a value for container.  The first value added to a capabiltiy is set as the default and current value.
  * @param[in] _flAdd the value to be added.
  * @param[in] _bDefault if true explisitly sets this value to be the default and current.  
  * @return true if success.
  */
  bool Add(const bool _blAdd, bool _bDefault = false);

  /**
  * Try to set the current value for container.
  * The value must already be part of the container.
  * @param[in] _flAdd the value to be set as current.
  * @return true if success.
  */
  bool SetCurrent(bool _blCurr);

  /**
  * Return the default value through _flVal if set.
  * @param[out] _flVal set the default value on return.
  * @return true if success.
  */
  bool GetDefault(bool &_blVal);

  /**
  * Return the current value through _flVal if set.
  * @param[out] _flVal set the current value on return.
  * @return true if success.
  */
  bool GetCurrent(bool &_bVal);

  /**
  * Return a vector of supported values.
  * @return supported values.
  */
  const IntVector &GetSupported();

  /**
  * Return the weather or not the value is supported by this capability.
  * @param[in] _flVal the value to check to see if it is supported
  * @return true is the _flVal is supported.
  */
  bool isValueSupported(const bool _bVal) {return -1 != getIndexForValue(_bVal);}

  /**
  * Return the index in vector list for value.
  * @param[in] _flVal value to search for.
  * @return the index of value, or -1 if does not exist.
  */
  int getIndexForValue(const bool _bVal);


protected:
  /**
  * Check to see if type is valid.
  * @param[in] _unTWType type to check
  * @return true if valid
  */
  bool isValidType(const TW_UINT16 _unTWType);

  IntVector m_listBools;                   /**< vector of valid container values. */
  IntVector m_listBoolsDefault;            /**< vector of valid container default values. */
  bool      m_bApp2;
};

#endif // __CTWAINCONTAINERBOOL_H__
