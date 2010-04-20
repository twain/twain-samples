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
* @file TWAINContainer.h
* Base Container class for negotiating capabilities.
* @author TWAIN Working Group
* @date April 2007
*/


#ifndef __CTWAINCONTAINER_H__
#define __CTWAINCONTAINER_H__

/**
  @TODO  TW_RANGE not implimented.  Need to be able to Set with one type and return with another.  
  @TODO  TWTY_STRnnn not implimented for any containers
*/

#ifdef TWH_CMP_GNU
  #include <stdlib.h>
#endif // TWH_CMP_GNU

#include "twain.h"

#include <string>
#include <vector>
using namespace std;


/**
* String Vector
*/
typedef vector<string> StringVector;

/**
* Float Vector
*/
typedef vector<float> FloatVector;

/**
* Int Vector
*/
typedef vector<int> IntVector; 


/**
* All possible TWQC_xxxxx messages
*/
#define TWQC_ALL (TWQC_GET | TWQC_SET | TWQC_GETDEFAULT | TWQC_GETCURRENT | TWQC_RESET)

/**
* All get TWQC_xxxxx messages
*/
#define TWQC_GETS (TWQC_GET | TWQC_GETDEFAULT | TWQC_GETCURRENT)

/**
* TWAIN Container Base class.
* All values are stored internally as 1/1000th of an inch. They are converted
* as necessary when TWAIN containers are created. Any direct access will require
* the caller to do their own conversion. 
*/
class CTWAINContainer
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
  CTWAINContainer(const TW_UINT16 _unCapID, 
                  const TW_UINT16 _unItemType, 
                  const TW_UINT16 _unGetType, 
                  const TW_INT32  _nSupportedQueries = TWQC_ALL,
                  const TW_UINT16 _unGetCurrentType = TWON_ONEVALUE,
                  const TW_UINT16 _unGetDefaultType = TWON_ONEVALUE);
virtual ~CTWAINContainer();

  /**
  * Return the capability ID.
  * @return the capability ID
  */
  TW_UINT16 GetCapID();

  /**
  * Return the TWTY_xxxx item type.
  * @return the item type
  */
  TW_UINT16 GetItemType();

  /**
  * Return the TWON_xxxx containor type.
  * @param[in] _MSG the MSG_GETxxxx get message.
  * @return the containor type
  */
  virtual TW_UINT16 GetGetType(const TW_UINT16 _unMsg);

  //////////////////////////////////////////////////////////////////////////////
  /**
  * @name Pure Virtuals
  * @{
  */
  // TWAIN operations

  /**
  * Return the container.
  * @param[in] _unMsg the MSG_GETxxxx get message.
  * @return a handle to containor
  */
  virtual TW_HANDLE GetContainer(const TW_UINT16 _unMsg) = 0;

  /**
  * Set a container.
  * @param[in] _pCap a pointer to TW_CAPABILITY structure.
  * @param[out] Condition the TWCC_xxx code if failed
  * @return a valid TWRC_xxxx return code.
  */
  virtual TW_INT16 Set(pTW_CAPABILITY _pCap, TW_INT16 &Condition) = 0;

  /**
  * Reset container back to default,
  * @return true if success.
  */
  virtual bool Reset() = 0;

protected:
  /**
  * Check to see if type is valid.
  * @param[in] _unTWType type to check
  * @return true if valid
  */
  virtual bool isValidType(const TW_UINT16 _unTWType) = 0;

  // END Pure Virtuals
  ////////////////////////////////////////////////////////////
  /**
  * @}
  */

public:
  /**
  * Return the size in bytes of the TWAIN type.
  * @param[in] _TWType the type to retrieve the size of.
  * @return the number of bytes of type.
  */
  unsigned int getTWTYSize(TW_UINT16 _TWType);

  /**
  * checks m_nMSG_QUERYSUPPORT to see if the operation in _unMsg is allowed
  * @param[in] _unMsg message to check.
  * @return true if allowed.
  */
  bool isOperationAllowed(const TW_UINT16 _unMsg);

  /**
  * Return a OR compasition of what messages are supported.
  * @return messages supported.
  */
  TW_INT32 getMSG_QUERYSUPPORT()
  {
    return m_nMSG_QUERYSUPPORT;
  }

protected:
  TW_UINT16 m_unCapID;          /**< Capability ID */
  TW_UINT16 m_unItemType;       /**< Item TWTY_xxxx Type */
  TW_UINT16 m_unGetType;        /**< Prefered Containor TWON_xxxx Type.  Used with MSG_GET */
  TW_UINT16 m_unGetCurrentType;        /**< Prefered Containor TWON_xxxx Type.  Used with MSG_GETCURRENT */
  TW_UINT16 m_unGetDefaultType;        /**< Prefered Containor TWON_xxxx Type.  Used with MSG_GETCURRENT */

  int       m_nCurrent;         /**< Holds an index of the currently set value.  An Index into the appropriate Vector array. */
  int       m_nDefault;         /**< Holds an index of the default value.  An Index into the appropriate Vector array. */

  TW_INT32 m_nMSG_QUERYSUPPORT; /**< Holds the supported operations for this capability, all OR together. */
};

#endif // __CTWAINCONTAINER_H__

