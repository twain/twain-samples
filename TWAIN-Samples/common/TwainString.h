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
 * @file TwainString.h
 * Convert TWAIN Constants to Strings
 * @author JFL Peripheral Solutions Inc.
 * @date Dec 2008
 */


#ifndef __TWAINSTRING_H__
#define __TWAINSTRING_H__

#pragma once

#ifdef _WINDOWS
  #ifndef __AFXWIN_H__
    #error include 'stdafx.h' before including this file for PCH
  #endif
#endif

#include "CommonTWAIN.h"

/**
* converts the integer CAP value into string form
* @param[in] _unCap the cap
* @return the equivalent cap string
*/
const char* convertCAP_toString(const TW_UINT16 _unCap);

/**
* converts the integer CAP transfer method into string form
* @param[in] _unCap the cap
* @param[in] _unItem the value
* @return the equivalent string
*/
const char* convertCAP_Item_toString(const TW_UINT16 _unCap, const TW_UINT16 _unItem);

/**
* converts the integer CAP transfer method into string form
* @param[in] _unItem the value
* @return the equivalent string
*/
const char* convertICAP_XFERMECH_toString(const TW_UINT16 _unItem);

/**
* converts the integer CAP unit into string form
* @param[in] _unItem the value
* @return the equivalent string
*/
const char* convertICAP_UNITS_toString(const TW_UINT16 _unItem);

/**
* converts the integer CAP Pixel type value into string form
* @param[in] _unItem the value
* @return the equivalent string
*/
const char* convertICAP_PIXELTYPE_toString(const TW_UINT16 _unItem);

/**
* converts the integer CAP Image File format type value into string form
* @param[in] _unItem the value
* @return the equivalent string
*/
const char* convertICAP_IMAGEFILEFORMAT_toString(const TW_UINT16 _unItem);

/**
* converts the integer CAP Compression type value into string form
* @param[in] _unItem the value
* @return the equivalent string
*/
const char* convertICAP_COMPRESSION_toString(const TW_UINT16 _unItem);

/**
* converts the integer TWTY value into string form
* @param[in] _unItem the TWTY value
* @return the equivalent string
*/
const char* convertTWTY_toString(const TW_UINT16 _unItem);

/**
* retrieve the extended image info name for a given Info ID for the current image
* @param[in] InfoID the id to retrieve the info of
* @return string of the extended image info
*/
const char* convertExtImageInfoName_toString(int InfoID);

/**
* retrieve the extended image info value for a given Info
* @param[in] ImgInfo the TW_INFO to retrieve the extended image info of
* @return string of the extended image info value
*/
const char* convertExtImageInfoItem_toString(TW_INFO &ImgInfo);

/**
* converts the TWAIN Return Code into string form
* @param[in] _unItem the value
* @return the equivalent string
*/
const char* convertReturnCode_toString(const TW_UINT16 _unItem);

/**
* converts the TWAIN Condition Code into string form
* @param[in] _unItem the value
* @return the equivalent string
*/
const char* convertConditionCode_toString(const TW_UINT16 _unItem);

#endif // __TWAINSTRING_H__
