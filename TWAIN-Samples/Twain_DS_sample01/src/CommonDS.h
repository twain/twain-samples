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
* @file CommonDS.h
* Common defines and typedefs used by the DS
* @author TWAIN Working Group
* @date April 2007
*/


#ifndef __COMMONDS_H__
#define __COMMONDS_H__


#include "Common.h"
#include "FreeImage.h"
#include "CommonTWAIN.h"

#ifdef TWH_CMP_MSC
  #include "resource.h"
#endif

#include "DSMInterface.h"
#include "TWAINContainer.h"
#include "TWAINContainerInt.h"
#include "TWAINContainerFix32.h"
#include "TWAINContainerBool.h"
#include "TWAINContainerFrame.h"
#include "CTiffWriter.h"

#include <map>
using namespace std;

/**
* Base TWAIN Container
*/
typedef map<int, CTWAINContainer*> TWAINCapabilitiesMap;

/**
* Int TWAIN Container
*/
typedef map<int, CTWAINContainerInt*> TWAINCapabilitiesMap_int;

/**
* Fix32 TWAIN Container
*/
typedef map<int, CTWAINContainerFix32*> TWAINCapabilitiesMap_FIX32;


/**
* Our internal dimention we use to store data
*/
#define TWUN_1000INCHES 0x8000

/**
* A commonly used conversion function for converting a dimention of one unit to a dimention of another unit.
* converts the given value from fromUnits to toUnits; resolution is optional if pixels are not involved, and should be in dots-per-inch
* @param[in] val the value in float to convert.
* @param[in] fromUnits the original unit of the value.
* @param[in] toUnits the result unit ot convert to
* @param[in] resolution the resolution in dots per inch, used to express the unit.  Required if one of the units is pixels.
* @return the converted value as float
*/
float ConvertUnits(float val, int fromUnits, int toUnits, float resolution);

/**
* A commonly used conversion function for converting a dimention of one unit to a dimention of another unit.
* converts the given value from fromUnits to toUnits; resolution is optional if pixels are not involved, and should be in dots-per-inch
* @param[in] val the value in TW_FIX32 to convert.
* @param[in] fromUnits the original unit of the value.
* @param[in] toUnits the result unit ot convert to
* @param[in] resolution the resolution in dots per inch, used to express the unit.  Required if one of the units is pixels.
* @return the converted value as TW_FIX32
*/
TW_FIX32 ConvertUnits(TW_FIX32 val, int fromUnits, int toUnits, float resolution);

/**
* A commonly used conversion function for converting a frame of one unit to a frame of another unit.
* converts the given frame from fromUnits to toUnits; resolution is optional if pixels are not involved, and should be in dots-per-inch
* @param[in] val the value as TW_FRAME to convert.
* @param[in] fromUnits the original unit of the value.
* @param[in] toUnits the result unit ot convert to
* @param[in] Xresolution the X resolution in dots per inch, used to express the unit.  Required if one of the units is pixels.
* @param[in] Yresolution the Y resolution in dots per inch, used to express the unit.  Required if one of the units is pixels.
* @return the converted frame as TW_FRAME
*/
TW_FRAME ConvertUnits(TW_FRAME val, int fromUnits, int toUnits, float Xresolution, float Yresolution);

#endif // __COMMONDS_H__

