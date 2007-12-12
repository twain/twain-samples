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
* @file TWAINContainerFrame.h
* Frame Container class for negotiating capabilities.
* @author TWAIN Working Group
* @date April 2007
*/


#ifndef __CTWAINCONTAINERFRAME_H__
#define __CTWAINCONTAINERFRAME_H__

#include "TWAINContainer.h"
//#include "TWAINContainerInt.h"
//#include "TWAINContainerFix32.h"

/**
* This structure is used to hold the values for a frame. 
* All values are stored using internal units of 1/1000th of an inch
*/
struct InternalFrame
{
  UINT unLeft;           /**< left */
  UINT unTop;            /**< top */
  UINT unRight;          /**< right */
  UINT unBottom;         /**< bottom */

  /**
  * Constructor
  */
  InternalFrame();

  /**
  * Constructor
  * @param[in] left Left
  * @param[in] lop Top
  * @param[in] right Right
  * @param[in] bottom Bottom
  */
  InternalFrame(UINT left, UINT top, UINT right, UINT bottom);

  /**
  * Constructor Convert to interanal from external units
  * @param[in] left Left
  * @param[in] lop Top
  * @param[in] right Right
  * @param[in] bottom Bottom
  * @param[in] fromUnits the units to convert to
  * @param[in] Xresolution the X resolution
  * @param[in] Yresolution the Y resolution
  * @return true if success.
  */
  InternalFrame(float left, float top, float right, float bottom, int fromUnits, float Xresolution, float Yresolution);

  /**
  * Constructor Convert to interanal from TW_FRAME
  * @param[in] twFrame external value to convert.
  * @param[in] fromUnits the units to convert to
  * @param[in] Xresolution the X resolution
  * @param[in] Yresolution the Y resolution
  * @return true if success.
  */
  InternalFrame(const TW_FRAME twFrame, int fromUnits, float Xresolution, float Yresolution);

  /**
  * Convert interanal frame value to external TW_FRAME value converting the units
  * @param[in] toUnits the units to convert to
  * @param[in] Xresolution the X resolution
  * @param[in] Yresolution the Y resolution
  * @return the frame as a TW_FRAME
  */
  TW_FRAME AsTW_FRAME(int toUnits, float Xresolution, float Yresolution);

  /**
  * create interanal frame from a SupportedSize
  * @param[in] ss the CSupportedSize to create internal frame from
  * @return the InternalFrame
  */
  InternalFrame(const TW_UINT16 ss);
};

/**
* test if two frames are equal.
* @return true if equal.
*/
bool operator== (const InternalFrame& _frame1, const InternalFrame& _frame2);

/**
* Frame Vector
*/
typedef vector<InternalFrame> FrameVector; 



/**
* This class can be used for any TW_FRAME based TWAIN container.  
*
* All values are stored internally as 1/1000th of an inch. They are converted
* as necessary when TWAIN containers are created. Any direct access will require
* the caller to do their own conversion. 
*/
class CTWAINContainerFrame : public CTWAINContainer
{
public:
  /**
  * Constructor.  
  * @param[in] _unCapID Capability ID
  * @param[in] _unItemType TWAIN Type TWTY_xxxx being stored.
  * @param[in] _unGetType TWON_xxxx container
  * @param[in] _nSupportedQueries the supported querie types TWQC_xxxx
  */
  CTWAINContainerFrame(const TW_UINT16 _unCapID, 
                       const TW_UINT16 _unItemType, 
                       const TW_UINT16 _unGetType, 
                       const TW_INT32  _nSupportedQueries = TWQC_ALL);
  virtual ~CTWAINContainerFrame();

  virtual TW_HANDLE GetContainer(const TW_UINT16 _unMsg);
  virtual TW_INT16 Set(pTW_CAPABILITY _pCap, TW_INT16 &Condition);
  virtual bool Reset();

  /**
  * Try to add a value for container.  The first value added to a capabiltiy is set as the default and current value.
  * @param[in] _frame the value to be added.
  * @param[in] _bDefault if true explisitly sets this value to be the default and current.  
  * @return true if success.
  */
  bool Add(const InternalFrame& _frame, bool _bDefault = false);

  /**
  * Try to add a value for container.  The first value added to a capabiltiy is set as the default and current value.
  * @param[in] _unLeft left
  * @param[in] _unTop top
  * @param[in] _unRight right
  * @param[in] _unBottom bottom
  * @param[in] _bDefault if true explisitly sets this value to be the default and current.  
  * @return true if success.
  */
  bool Add(const UINT _unLeft, const UINT _unTop, const UINT _unRight, const UINT _unBottom, bool _bDefault = false);  

  /**
  * Try to set the current value for container.
  * The value must already be part of the container.
  * @param[in] _frame the value to be set as current.
  * @return true if success.
  */
  bool SetCurrent(const InternalFrame& _frame);

  /**
  * Try to set the current value for container.
  * The value must already be part of the container.
  * @param[in] _unLeft left
  * @param[in] _unTop top
  * @param[in] _unRight right
  * @param[in] _unBottom bottom
  * @return true if success.
  */
  bool SetCurrent(const UINT _unLeft, const UINT _unTop, const UINT _unRight, const UINT _unBottom);  

  /**
  * Return the default value through _frame if set.
  * @param[out] _frame set the default value on return.
  * @return true if success.
  */
  bool GetDefault(InternalFrame& _frame);

  /**
  * Return the current value through _frame if set.
  * @param[out] _frame set the current value on return.
  * @return true if success.
  */
  bool GetCurrent(InternalFrame& _frame);

  /**
  * Return a vector of supported values.
  * @return supported values.
  */
  const FrameVector &GetSupported();

  /**
  * Return the weather or not the value is supported by this capability.
  * @param[in] _frame the value to check to see if it is supported
  * @return true is the _frame is supported.
  */
  bool isValueSupported(const InternalFrame& _frame) {return -1 != getIndexForValue(_frame);}

  /**
  * Return the index in vector list for value.
  * @param[in] _frame value to search for.
  * @return the index of value, or -1 if does not exist.
  */
  int getIndexForValue(const InternalFrame& _frame);

  /**
  * set the current unit and resolution.  Used to translate the Frames coming and going
  * @param[in] Unit the current unit value.
  * @param[in] Xres the current X resolution value.
  * @param[in] Yres the current Y resolution value.
  */
  void setCurrentUnits(int Unit, float Xres, float Yres);

 protected:
  /**
  * Check to see if type is valid.
  * @param[in] _unTWType type to check
  * @return true if valid
  */
  bool isValidType(const TW_UINT16 _unTWType);

  FrameVector         m_listFrames;                          /**< vector of valid container values. */
  FrameVector         m_listFramesDefault;                   /**< vector of valid container default values. */
  int                 m_Unit;                                /**< used to translate the units coming and going */
  float               m_Xres;                                /**< used to translate the units coming and going */
  float               m_Yres;                                /**< used to translate the units coming and going */
};

#endif // __CTWAINCONTAINERFRAME_H__
