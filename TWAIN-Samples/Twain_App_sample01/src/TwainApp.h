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
 * @file TwainApp.h
 * TWAIN Application.
 * A TWAIN Application communicates with the DSM to acquire images. 
 * The goal of the application is to acquire data from a Source.  
 * However, applications cannot contact the Source directly.  All requests for
 * data, capability information, error information, etc. must be handled 
 * Through the Source Manager.
 * @author JFL Peripheral Solutions Inc.
 * @date April 2007
 */

/*! \mainpage TWAIN Application
 *
 * The goal of the application is to acquire data from a Source.  
 * However, applications cannot contact the Source directly.  All requests for
 * data, capability information, error information, etc. must be handled 
 * Through the Source Manager.
 *
 *
 *
 *
 *
 *
 *
 *
 * Copyright © 2007 TWAIN Working Group:  Adobe Systems Incorporated,
 * AnyDoc Software Inc., Eastman Kodak Company, 
 * Fujitsu Computer Products of America, JFL Peripheral Solutions Inc., 
 * Ricoh Corporation, and Xerox Corporation.
 * All rights reserved.
*/
#ifndef __TWAINAPP_H__
#define __TWAINAPP_H__

#pragma once

#ifdef _WINDOWS
  #ifndef __AFXWIN_H__
    #error include 'stdafx.h' before including this file for PCH
  #endif
#endif

#include "main.h"
#include <string>
#include <vector>

#include "CommonTWAIN.h"
#include "FreeImage.h"
#include "DSMInterface.h"

#ifndef TWH_CMP_MSC
  typedef void* HWND;
#endif

using namespace std;

/**
* Determine if two TW_FIX32 variables are equal.
* @param[in] _fix1 First TW_FIX32
* @param[in] _fix2 Second TW_FIX32
* @return bool TRUE if equal
*/
bool operator== (const TW_FIX32& _fix1, const TW_FIX32& _fix2);

/**
* The Main Application Class.
*/
class TwainApp
{
public:
/**
* Constructor for the main application.
* Initialize the application.
* @param[in] parent handle to Window
*/
  TwainApp(HWND parent = NULL);

/**
* Deconstructor for the main application TwainApp.  
* Clean up any alocated memory. 
*/
  ~TwainApp();


  TW_UINT16 DSM_Entry(TW_UINT32    _DG,
                      TW_UINT16    _DAT,
                      TW_UINT16    _MSG,
                      TW_MEMREF    _pData);
/**
* Print out error message and condition code from DS.  Looks up the 
* condition code using the _pdestID and prints it after the _errorMsg.
* If _errorMsg is empty, a default message is displayed.
* @param[in] _pdestID the destination to get the condition code. 0 is DSM
*            anything else is a source.
* @param[in] _errorMsg the error message to display. If empty a standard message
*            is displayed
*/
  void printError(pTW_IDENTITY _pdestID = 0, const string& _errorMsg = "");

/**
* Retrieves the condition code from the DSM.
* @param[in] _pdestID a pointer to the source to query, or 0 for the DSM
* @param[out] _cc this will be set to the condition code
* @return a valid TWRC_xxxx return code
*/
  TW_UINT16 getTWCC(pTW_IDENTITY _pdestID, TW_INT16& _cc);

/**
* Fills an identity structure with this applications identity details.
* Initialize the Apps identity so the DSM can uniquely identify it.
* @param[out] _identity a structure that will get filled with Applications details
*/
  void fillIdentity(TW_IDENTITY& _identity);

/**
* Exit application clean up everything.
*/
  void exit();

/**
* Try to connect to the DSM.  Set the App to correct state.
* m_DSMState is set to state 3 if successful.
*/
  void connectDSM();

/**
* Try to disconnect from the DSM.  Set the App to correct state.
* m_DSMState is set to state 2 if successful.
*/
  void disconnectDSM();

/**
* Queries the DSM for all available Data Sources.  Discover all Data 
* Sources available and build a list of DS and store them in m_DataSources.
*/
  void getSources();

/**
* Print a list of Data Sources.  Prints to stdout a quick list of the 
* Data Sources the app knows about.
*/
  void printAvailableDataSources();

/**
* Prints an identity structure that uniquely identifies an Application 
* or Data Source out in detail to stdout.
* @param[in] _ident the TW_IDENTITY structure to print
*/
  void printIdentStruct(const TW_IDENTITY& _ident);

/**
* Goes through the list of known identity structures and prints the one
* with an ID that matches the passed in id.
* @param[in] _identityID The ID of the identity structure to print
*/
  void printIdentityStruct(const TW_UINT32 _identityID);

/**
* Try to load a Data Source.  Loads the data source with the ID passed in. 
* To open a Data Source the DSM must be in state 3 (Open).  If successful 
* moves the app to state 4
* @param[in] _dsID the ID of the datasource to load
*/
  void loadDS(const TW_UINT32 _dsID);

/**
* Unloads the currently loaded datasource.  If successful moves the 
* app to state 3.
*/
  void unloadDS();

/**
* Initialize default capabilities.
* Negotiates the default capabilities with the currently opened data source.
*/
  void initCaps();

/**
* Request the acquisition of data from the currently opened source.  If 
* successful moves app to state 5.
* @return true if successfully enabled DS.
*/
  bool enableDS();

/**
* Tries to disable the sources UI.  If successful moves app to state 4
*/
  void disableDS();

/**
* Tries to starts the scanning process.  Must be in state 6 to start.  
* If successfully finishes scan moves app to state 5.
*/
  void startScan();

/**
* Starts the scanning process using native mode.  Data will be transfered 
* in native mode.
*/
  void initiateTransfer_Native();

/**
* Starts the scanning process using file mode.  Data will be transfered 
* via a file.
*/
  void initiateTransfer_File();

/**
* Starts the scanning process using the memory strip mode.  Data will be transfered 
* in memory using a buffer.
*/
  void initiateTransfer_Memory();

/**
* Try to sets a OneValue Capability to the value passed
* @param[in] Cap the capability to update to set
* @param[in] _value the value to set
* @return true if successful
*/
  bool set_CapabilityOneValue(TW_UINT16 Cap, const TW_INT16 _value);

/**
* Try to sets a OneValue Capability to the value passed
* @param[in] Cap the capability to update to set
* @param[in] _value the value to set
* @return true if successful
*/
  bool set_CapabilityOneValue(TW_UINT16 Cap, const TW_UINT16 _value);

/**
* Try to sets a OneValue Capability to the value passed
* @param[in] Cap the capability to update to set
* @param[in] _pValue the value to set
* @return true if successful
*/
  bool set_CapabilityOneValue(TW_UINT16 Cap, const pTW_FIX32 _pValue);

/**
* Gets the capability.
* @param[in,out] _cap the capability struct to fill. _cap.Cap needs to be
* filled with the capability to retrieve.
* @return true if the cap was successfully retrieved.
*/
  bool get_CAP(TW_CAPABILITY& _cap);

/**
* Sets the CAP_XFERCOUNT capability
* @param[in] _count the value to set
*/
  void set_CAP_XFERCOUNT(const TW_INT16 _count);
  
/**
* Sets the ICAP_UNITS capability
* @param[in] _val the value to set
*/
  void set_ICAP_UNITS(const TW_UINT16 _val);

/**
* Sets the ICAP_PIXELTYPE capability
* @param[in] _pt the value to set
*/
  void set_ICAP_PIXELTYPE(const TW_UINT16 _pt);

/**
* Sets the ICAP_RESOLUTION specified in _Cap
* @param[in] _Cap the RESOLUTION cap to set, either ICAP_XRESOLUTION or ICAP_YRESOLUTION
* @param[in] _pVal the value to set the cap to.
*/
  void set_ICAP_RESOLUTION(const TW_UINT16 _Cap, const pTW_FIX32 _pVal);

/**
* Sets the ICAP_XFERMECH capability
* @param[in] _mech the mechanism to use. (TWSX_NATIVE, TWSX_FILE, TWSX_MEMORY)
*/
  void set_ICAP_XFERMECH(const TW_UINT16 _mech);

/**
* Sets the ICAP_IMAGEFILEFORMAT capability
* @param[in] _fileformat the compression to use. (TWFF_TIFF, TWFF_PICT, TWFF_BMP, TWFF_XBM, TWFF_JFIF, TWFF_FPX, TWFF_TIFFMULTI, TWFF_PNG, TWFF_SPIFF, TWFF_EXIF)
*/
  void set_ICAP_IMAGEFILEFORMAT(const TW_UINT16 _fileformat);

/**
* Sets the ICAP_COMPRESSION capability
* @param[in] _comp the compression to use. (TWCP_NONE, TWCP_PACKBITS, TWCP_GROUP4, TWCP_JPEG, ...)
*/
  void set_ICAP_COMPRESSION(const TW_UINT16 _comp);

/**
* Sets the ICAP_FRAMES capability
* @param[in] _pFrame the frame data to set
*/
  void set_ICAP_FRAMES(const pTW_FRAME _pFrame);

/**
* Sets the ICAP_BITDEPTH capability using a TW_ENUMERATION struct.
* @param[in] _nVal the bit depth to set
*/
  void set_ICAP_BITDEPTH(const TW_UINT16 _nVal);

/**
* Gets the current setting for the ICAP_UNITS cap
* @param[out] _val set to the caps value
* @return true on succes
*/
  bool getICAP_UNITS(TW_UINT16& _val);

/**
* Gets the current setting for the CAP_XFERCOUNT cap
* @param[out] _val set to the caps value
* @return true on succes
*/
  bool getCAP_XFERCOUNT(TW_INT16& _val);

/**
* Gets the current setting for the ICAP_XFERMECH cap
* @param[out] _val a TW_UINT16 holding the current transfer method (TWSX_NATIVE, etc...)
* @return true on succes
*/
  bool getICAP_XFERMECH(TW_UINT16& _val);

/**
* Gets the current setting for the ICAP_IMAGEFILEFORMAT cap
* @param[out] _val a TW_UINT16 holding the current compression method (TWFF_TIFF, TWFF_PICT, TWFF_BMP, TWFF_XBM, TWFF_JFIF, TWFF_FPX, TWFF_TIFFMULTI, TWFF_PNG, TWFF_SPIFF, TWFF_EXIF)
* @return true on succes
*/
  bool getICAP_IMAGEFILEFORMAT(TW_UINT16& _val);

/**
* Gets the current setting for the ICAP_COMPRESSION cap
* @param[out] _val a TW_UINT16 holding the current compression method (TWCP_NONE, TWCP_PACKBITS, TWCP_GROUP4, TWCP_JPEG, ...)
* @return true on succes
*/
  bool getICAP_COMPRESSION(TW_UINT16& _val);

/**
* Gets the current setting for the ICAP_PIXELTYPE cap
* @param[out] _val a TW_UINT16 holding the current pixel type (TWPT_BW, etc...)
* @return true on succes
*/
  bool getICAP_PIXELTYPE(TW_UINT16& _val);

/**
* Gets the current setting for the ICAP_BITDEPTH cap
* @param[out] _val a TW_UINT16 holding the current bit depth
* @return true on succes
*/
  bool getICAP_BITDEPTH(TW_UINT16& _val);

/**
* Returns a pointer to the applications identity structure.
* @return a TW_IDENTITY pointer to the applications identity struct.
*/
  pTW_IDENTITY getAppIdentity();

/**
* Returns a pointer to the currently opened data source.
* @return a TW_IDENTITY pointer to the opened data source 
* if nothing open then return NULL.
*/
  pTW_IDENTITY getDataSource(TW_INT16 _index = -1) const;

/**
* Returns a pointer to the default data source.
* @return a TW_IDENTITY pointer to the default.
*/
  pTW_IDENTITY getDefaultDataSource(void);

/**
* Gets the current setting for the ICAP_XRESOLUTION cap
* @param[out] _xres a TW_FIX32 holding the current x resolution
* @return true on succes
*/
  bool getICAP_XRESOLUTION(TW_FIX32& _xres);

/**
* Gets the current setting for the ICAP_YRESOLUTION cap
* @param[out] _yres a TW_FIX32 holding the current y resolution
* @return true on succes
*/
  bool getICAP_YRESOLUTION(TW_FIX32& _yres);

/**
* Gets the imageinfo of the currently transfered image
* @return pointer to the image info
*/
  pTW_IMAGEINFO getIMAGEINFO() {return &m_ImageInfo;}

/**
* Gets the extended image info of the currently transfered image
* @return pointer to the extended image info
*/
  string getEXIMAGEINFO(){return m_strExImageInfo;}

/**
* retrieve the imageinfo for the current image
* @return true on succes
*/
  bool updateIMAGEINFO();

/**
* retrieve the extended image info for the current image
* @return true on succes
*/
  void updateEXIMAGEINFO();

/**
* retrieve the extended image info name for a given Info ID for the current image
* @param[in] InfoID the id to retrieve the info of
* @return string of the extended image info
*/
  string GetExtImageInfoName(int InfoID);

  ////////////////
  // CAPABILITIES

  TW_CAPABILITY   m_CAP_XFERCOUNT;        /**< Number of images the application is willing to accept this session. */
  TW_CAPABILITY   m_ICAP_XFERMECH;        /**< Transfer mechanism - used to learn options and set-up for upcoming transfer. */
  TW_CAPABILITY   m_ICAP_IMAGEFILEFORMAT; /**< File format saved when using File Xfer Mechanism. */
  TW_CAPABILITY   m_ICAP_COMPRESSION;     /**< Compression method used for upcoming transfer. */
  TW_CAPABILITY   m_ICAP_UNITS;           /**< Unit of measure (inches, centimeters, etc). */
  TW_CAPABILITY   m_ICAP_PIXELTYPE;       /**< The type of pixel data (B/W, gray, color, etc). */
  TW_CAPABILITY   m_ICAP_BITDEPTH;        /**< Pixel bit depth for Current value of ICAP_PIXELTYPE. */
  TW_CAPABILITY   m_ICAP_XRESOLUTION;     /**< Current/Available optical resolutions for x-axis. */
  TW_CAPABILITY   m_ICAP_YRESOLUTION;     /**< Current/Available optical resolutions for y-axis */
  TW_CAPABILITY   m_ICAP_FRAMES;          /**< Size and location of frames on page. */

  int             m_DSMState;             /**< The current TWAIN state of the dsm (2-7) */

protected:
/**
* Create a DIB of the dimention set by m_ImageInfo and bitdepth of 
* data source.
* @return the DIB created on succes or 0 if failed.
*/
  FIBITMAP* createDIB();

  HWND                m_Parent;               /**< Handle to Window to recieve window messages. */
  TW_IDENTITY         m_MyInfo;               /**< Identity structure with this applications identity details */
  pTW_IDENTITY        m_pDataSource;          /**< Pointer of Identity structure to current loaded data souce */
  vector<TW_IDENTITY> m_DataSources;        /**< Store a list of available data sources located by DSM */

  TW_IMAGEINFO        m_ImageInfo;            /**< Detailed image info retrieved from the DS */
  pTW_EXTIMAGEINFO    m_pExtImageInfo;        /**< Detailed extended image info retrieved from the DS */
  TW_UINT16           m_nXferNum;             /**< Keep track of the number of images attempted to transfer - used when saving file to disk*/
  string              m_strExImageInfo;       /**< Extended image info retrieved from the DS */

  TW_USERINTERFACE    m_ui;                   /**< Coordinates UI between application and data source. */
};

#endif //  __TWAINAPP_H__
