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
* @file CTWAINDS_FreeImage.cpp
* The main Data Source class.
* This class is derived from the Base class describing a TWAIN DS.
* @author TWAIN Working Group
* @date April 2007
*/

#include "CTWAINDS_FreeImage.h"
#include "FreeImage.h"

#include <iostream>
#include <cstdlib>
#include <assert.h>
#include <signal.h>

#ifdef TWH_CMP_MSC
  #include <Winbase.h>
#endif

// I found that compiling using the sunfreeware.com stuff on Solaris 9
// required this typedef. This is related to the inclusion of signal.h
#if defined (__SVR4) && defined (__sun)
typedef union {
  long double  _q;
  uint32_t     _l[4];
} upad128_t;
#endif

using namespace std;

//* Initialize global identiy for this DS. */
TW_IDENTITY CTWAINDS_Base::m_TheIdentity =
{
  0,                                  // TW_UINT32  Id;               Unique number.  In Windows, application hWnd
  {                                       // TW_VERSION Version;          Identifies the piece of code
    1,                                    // TW_UINT16  MajorNum;         Major revision number of the software
    0,                                    // TW_UINT16  MinorNum;         Incremental revision number of the software
    TWLG_ENGLISH,                         // TW_UINT16  Language;         e.g. TWLG_SWISSFRENCH
    TWCY_USA,                             // TW_UINT16  Country;          e.g. TWCY_SWITZERLAND
    "1.0 sample"                          // TW_STR32   Info;             e.g. "1.0b3 Beta release"
#ifdef _DEBUG
    " debug"
#else
    " release"
#endif
#ifdef TWH_32BIT
    " 32bit"
#else
    " 64bit"
#endif
  },
  2,                                  // TW_UINT16  ProtocolMajor;    Application and DS must set to TWON_PROTOCOLMAJOR
  0,                                  // TW_UINT16  ProtocolMinor;    Application and DS must set to TWON_PROTOCOLMINOR
  DG_IMAGE | DG_CONTROL | DF_DS2,     // TW_UINT32  SupportedGroups;  Bit field OR combination of DG_ constants
  "TWAIN Working Group",              // TW_STR32   Manufacturer;     Manufacturer name, e.g. "Hewlett-Packard"
  "Software Scan",                    // TW_STR32   ProductFamily;    Product family name, e.g. "ScanJet"
  "TWAIN2 FreeImage Software Scanner" // TW_STR32   ProductName;      Product name, e.g. "ScanJet Plus"
};

//////////////////////////////////////////////////////////////////////////////
CTWAINDS_FreeImage::CTWAINDS_FreeImage()
{
  // Setup our identity
  fillIdentityStructure(*getIdentity());
  return;
}

//////////////////////////////////////////////////////////////////////////////
TW_INT16 CTWAINDS_FreeImage::Initialize()
{
  // When adding to Capabiltiy remember the order of operations 
  // Some capabilities are dependent on others.
  // see: http://www.twain.org/docs/CapOrderForWeb.PDF

  // setup the supported independant caps
  CTWAINContainerInt* pnCap = 0;

  m_IndependantCapMap[CAP_SUPPORTEDCAPS] = new CTWAINContainerInt(CAP_SUPPORTEDCAPS, TWTY_UINT16, TWON_ARRAY, TWQC_GETS);
  if( NULL == (pnCap = dynamic_cast<CTWAINContainerInt*>(m_IndependantCapMap[CAP_SUPPORTEDCAPS]))
   || !pnCap->Add(CAP_DEVICEONLINE)
   || !pnCap->Add(CAP_PAPERDETECTABLE)
   || !pnCap->Add(CAP_FEEDERENABLED)
   || !pnCap->Add(CAP_FEEDERLOADED)
   || !pnCap->Add(CAP_AUTOFEED)
   || !pnCap->Add(CAP_SUPPORTEDCAPS)
   || !pnCap->Add(CAP_UICONTROLLABLE)
   || !pnCap->Add(CAP_XFERCOUNT)
   || !pnCap->Add(ICAP_BITDEPTH)
   || !pnCap->Add(ICAP_BITORDER)
   || !pnCap->Add(ICAP_COMPRESSION)
   || !pnCap->Add(ICAP_FRAMES)
   || !pnCap->Add(ICAP_MAXFRAMES)
   || !pnCap->Add(ICAP_IMAGEFILEFORMAT)
   || !pnCap->Add(ICAP_PHYSICALHEIGHT)
   || !pnCap->Add(ICAP_PHYSICALWIDTH)
   || !pnCap->Add(ICAP_PIXELFLAVOR)
   || !pnCap->Add(ICAP_PIXELTYPE)
   || !pnCap->Add(ICAP_PLANARCHUNKY)
   || !pnCap->Add(ICAP_SUPPORTEDSIZES)
   || !pnCap->Add(ICAP_UNITS)
   || !pnCap->Add(ICAP_XFERMECH)
   || !pnCap->Add(ICAP_XRESOLUTION)
   || !pnCap->Add(ICAP_YRESOLUTION) )
  {
    cerr << "Could not create CAP_SUPPORTEDCAPS" << endl;
    setConditionCode(TWCC_LOWMEMORY);
    return TWRC_FAILURE;
  }

  m_IndependantCapMap[ICAP_COMPRESSION] = new CTWAINContainerInt(ICAP_COMPRESSION, TWTY_UINT16, TWON_ENUMERATION);
  if( NULL == (pnCap = dynamic_cast<CTWAINContainerInt*>(m_IndependantCapMap[ICAP_COMPRESSION]))
   || !pnCap->Add(TWCP_NONE, true) )
  {
    cerr << "Could not create ICAP_COMPRESSION" << endl;
    setConditionCode(TWCC_LOWMEMORY);
    return TWRC_FAILURE;
  }

  m_IndependantCapMap[ICAP_PLANARCHUNKY] = new CTWAINContainerInt(ICAP_PLANARCHUNKY, TWTY_UINT16, TWON_ENUMERATION);
  if( NULL == (pnCap = dynamic_cast<CTWAINContainerInt*>(m_IndependantCapMap[ICAP_PLANARCHUNKY]))
   || !pnCap->Add(TWPC_CHUNKY, true) )
  /// @todo support TWPC_PLANAR
  // || !pnCap->Add(TWPC_PLANAR)
  {
    cerr << "Could not create ICAP_PLANARCHUNKY" << endl;
    setConditionCode(TWCC_LOWMEMORY);
    return TWRC_FAILURE;
  }

  m_IndependantCapMap[ICAP_UNITS] = new CTWAINContainerInt(ICAP_UNITS, TWTY_UINT16, TWON_ENUMERATION);
  if( NULL == (pnCap = dynamic_cast<CTWAINContainerInt*>(m_IndependantCapMap[ICAP_UNITS]))
   || !pnCap->Add(TWUN_INCHES, true)
   || !pnCap->Add(TWUN_PIXELS)
   || !pnCap->Add(TWUN_CENTIMETERS)
   || !pnCap->Add(TWUN_PICAS)
   || !pnCap->Add(TWUN_POINTS)
   || !pnCap->Add(TWUN_TWIPS) )
  {
    cerr << "Could not create ICAP_UNITS" << endl;
    setConditionCode(TWCC_LOWMEMORY);
    return TWRC_FAILURE;
  }

  m_IndependantCapMap[ICAP_XFERMECH] = new CTWAINContainerInt(ICAP_XFERMECH, TWTY_UINT16, TWON_ENUMERATION);
  if( NULL == (pnCap = dynamic_cast<CTWAINContainerInt*>(m_IndependantCapMap[ICAP_XFERMECH]))
   || !pnCap->Add(TWSX_FILE)
   || !pnCap->Add(TWSX_MEMORY)
   || !pnCap->Add(TWSX_NATIVE, true)
   )
  {
    cerr << "Could not create ICAP_XFERMECH" << endl;
    setConditionCode(TWCC_LOWMEMORY);
    return TWRC_FAILURE;
  }

  m_IndependantCapMap[ICAP_PIXELTYPE] = new CTWAINContainerInt(ICAP_PIXELTYPE, TWTY_UINT16, TWON_ENUMERATION);
  if( NULL == (pnCap = dynamic_cast<CTWAINContainerInt*>(m_IndependantCapMap[ICAP_PIXELTYPE]))
   || !pnCap->Add(TWPT_BW)
   || !pnCap->Add(TWPT_GRAY)
   || !pnCap->Add(TWPT_RGB, true) )
  {
    cerr << "Could not create ICAP_PIXELTYPE" << endl;
    setConditionCode(TWCC_LOWMEMORY);
    return TWRC_FAILURE;
  }

  m_IndependantCapMap[CAP_UICONTROLLABLE] = new CTWAINContainerInt(CAP_UICONTROLLABLE, TWTY_BOOL, TWON_ONEVALUE, TWQC_GETS);
  if( NULL == (pnCap = dynamic_cast<CTWAINContainerInt*>(m_IndependantCapMap[CAP_UICONTROLLABLE]))
  /// @todo add UI
  // || !pnCap->Add(TRUE, true)
   || !pnCap->Add(FALSE, true) )
  {
    cerr << "Could not create CAP_UICONTROLLABLE" << endl;
    setConditionCode(TWCC_LOWMEMORY);
    return TWRC_FAILURE;
  }

  m_IndependantCapMap[CAP_XFERCOUNT] = new CTWAINContainerInt(CAP_XFERCOUNT, TWTY_INT16, TWON_ONEVALUE);
  if( NULL == (pnCap = dynamic_cast<CTWAINContainerInt*>(m_IndependantCapMap[CAP_XFERCOUNT]))
   || !pnCap->Add(TWON_DONTCARE32, true) )
  {
    cerr << "Could not create CAP_XFERCOUNT" << endl;
    setConditionCode(TWCC_LOWMEMORY);
    return TWRC_FAILURE;
  }

  m_IndependantCapMap[ICAP_BITORDER] = new CTWAINContainerInt(ICAP_BITORDER, TWTY_UINT16, TWON_ENUMERATION);
  if( NULL == (pnCap = dynamic_cast<CTWAINContainerInt*>(m_IndependantCapMap[ICAP_BITORDER]))
   || !pnCap->Add(TWBO_LSBFIRST, true)
   || !pnCap->Add(TWBO_MSBFIRST) )
  {
    cerr << "Could not create ICAP_BITORDER" << endl;
    setConditionCode(TWCC_LOWMEMORY);
    return TWRC_FAILURE;
  }

  m_IndependantCapMap[ICAP_IMAGEFILEFORMAT] = new CTWAINContainerInt(ICAP_IMAGEFILEFORMAT, TWTY_UINT16, TWON_ENUMERATION);
  if( NULL == (pnCap = dynamic_cast<CTWAINContainerInt*>(m_IndependantCapMap[ICAP_IMAGEFILEFORMAT]))
   || !pnCap->Add(TWFF_BMP, true)
   || !pnCap->Add(TWFF_TIFF) )
  {
    cerr << "Could not create ICAP_IMAGEFILEFORMAT" << endl;
    setConditionCode(TWCC_LOWMEMORY);
    return TWRC_FAILURE;
  }
  SSTRCPY(m_CurFileExferName, sizeof(m_CurFileExferName), "sample.bmp");
  SSTRCPY(m_DefFileExferName, sizeof(m_DefFileExferName), "sample.bmp");

  m_IndependantCapMap[ICAP_PIXELFLAVOR] = new CTWAINContainerInt(ICAP_PIXELFLAVOR, TWTY_UINT16, TWON_ENUMERATION);
  if( NULL == (pnCap = dynamic_cast<CTWAINContainerInt*>(m_IndependantCapMap[ICAP_PIXELFLAVOR]))
   || !pnCap->Add(TWPF_CHOCOLATE, true)
   || !pnCap->Add(TWPF_VANILLA) )
  {
    cerr << "Could not create ICAP_PIXELFLAVOR" << endl;
    setConditionCode(TWCC_LOWMEMORY);
    return TWRC_FAILURE;
  }

  m_IndependantCapMap[ICAP_SUPPORTEDSIZES] = new CTWAINContainerInt(ICAP_SUPPORTEDSIZES, TWTY_UINT16, TWON_ENUMERATION);
  if( NULL == (pnCap = dynamic_cast<CTWAINContainerInt*>(m_IndependantCapMap[ICAP_SUPPORTEDSIZES]))
   || !pnCap->Add(TWSS_NONE)
   || !pnCap->Add(TWSS_USLETTER, true)
   || !pnCap->Add(TWSS_USLEGAL) )
  {
    cerr << "Could not create ICAP_SUPPORTEDSIZES" << endl;
    setConditionCode(TWCC_LOWMEMORY);
    return TWRC_FAILURE;
  }

  m_IndependantCapMap[CAP_DEVICEONLINE] = new CTWAINContainerInt(CAP_DEVICEONLINE, TWTY_BOOL, TWON_ONEVALUE, TWQC_GETS);
  if( NULL == (pnCap = dynamic_cast<CTWAINContainerInt*>(m_IndependantCapMap[CAP_DEVICEONLINE]))
   || !pnCap->Add(TRUE, true)
   || !pnCap->Add(FALSE) )
  {
    cerr << "Could not create CAP_DEVICEONLINE" << endl;
    setConditionCode(TWCC_LOWMEMORY);
    return TWRC_FAILURE;
  }

  m_IndependantCapMap[ICAP_MAXFRAMES] = new CTWAINContainerInt(ICAP_MAXFRAMES, TWTY_UINT16, TWON_ONEVALUE, TWQC_GETS);
  if( NULL == (pnCap = dynamic_cast<CTWAINContainerInt*>(m_IndependantCapMap[ICAP_MAXFRAMES]))
   || !pnCap->Add(1, true) )
  {
    cerr << "Could not create ICAP_MAXFRAMES" << endl;
    setConditionCode(TWCC_LOWMEMORY);
    return TWRC_FAILURE;
  }

  m_IndependantCapMap[CAP_FEEDERENABLED] = new CTWAINContainerInt(CAP_FEEDERENABLED, TWTY_BOOL, TWON_ONEVALUE, TWQC_GETS);
  /// @todo to add support for flatbed set to TWQC_ALL and add False as posible value.
  if( NULL == (pnCap = dynamic_cast<CTWAINContainerInt*>(m_IndependantCapMap[CAP_FEEDERENABLED]))
   || !pnCap->Add(TRUE, true) )
  {
    cerr << "Could not create CAP_FEEDERENABLED" << endl;
    setConditionCode(TWCC_LOWMEMORY);
    return TWRC_FAILURE;
  }

  m_IndependantCapMap[CAP_FEEDERLOADED] = new CTWAINContainerInt(CAP_FEEDERLOADED, TWTY_BOOL, TWON_ONEVALUE, TWQC_GETS);
  if( NULL == (pnCap = dynamic_cast<CTWAINContainerInt*>(m_IndependantCapMap[CAP_FEEDERLOADED]))
   || !pnCap->Add(TRUE)
   || !pnCap->Add(FALSE, true) )
  {
    cerr << "Could not create CAP_FEEDERLOADED" << endl;
    setConditionCode(TWCC_LOWMEMORY);
    return TWRC_FAILURE;
  }

  m_IndependantCapMap[CAP_AUTOFEED] = new CTWAINContainerInt(CAP_AUTOFEED, TWTY_BOOL, TWON_ONEVALUE, TWQC_GETS);
  if( NULL == (pnCap = dynamic_cast<CTWAINContainerInt*>(m_IndependantCapMap[CAP_AUTOFEED]))
   || !pnCap->Add(TRUE, true)
   || !pnCap->Add(FALSE) )
  {
    cerr << "Could not create CAP_AUTOFEED" << endl;
    setConditionCode(TWCC_LOWMEMORY);
    return TWRC_FAILURE;
  }
  
  m_IndependantCapMap[CAP_PAPERDETECTABLE] = new CTWAINContainerInt(CAP_PAPERDETECTABLE, TWTY_BOOL, TWON_ONEVALUE, TWQC_GETS);
  if( NULL == (pnCap = dynamic_cast<CTWAINContainerInt*>(m_IndependantCapMap[CAP_PAPERDETECTABLE]))
   || !pnCap->Add(TRUE, true) )
  {
    cerr << "Could not create CAP_PAPERDETECTABLE" << endl;
    setConditionCode(TWCC_LOWMEMORY);
    return TWRC_FAILURE;
  }

  // setup dependant caps
  if( NULL == (m_BitDepthMap[TWPT_BW] = new CTWAINContainerInt(ICAP_BITDEPTH, TWTY_UINT16, TWON_ENUMERATION))
   || !m_BitDepthMap[TWPT_BW]->Add(1, true) )
  {
    cerr << "Could not create ICAP_BITDEPTH" << endl;
    setConditionCode(TWCC_LOWMEMORY);
    return TWRC_FAILURE;
  }

  if( NULL == (m_BitDepthMap[TWPT_GRAY] = new CTWAINContainerInt(ICAP_BITDEPTH, TWTY_UINT16, TWON_ENUMERATION))
   || !m_BitDepthMap[TWPT_GRAY]->Add(8, true) )
  {
    cerr << "Could not create ICAP_BITDEPTH" << endl;
    setConditionCode(TWCC_LOWMEMORY);
    return TWRC_FAILURE;
  }

  if( NULL == (m_BitDepthMap[TWPT_RGB] = new CTWAINContainerInt(ICAP_BITDEPTH, TWTY_UINT16, TWON_ENUMERATION))
   || !m_BitDepthMap[TWPT_RGB]->Add(24, true) )
  {
    cerr << "Could not create ICAP_BITDEPTH" << endl;
    setConditionCode(TWCC_LOWMEMORY);
    return TWRC_FAILURE;
  }

  // expressed internally as pixels per inch
  if( NULL == (m_ICAP_UNIT_Dependant[ICAP_XRESOLUTION] = new CTWAINContainerFix32(ICAP_XRESOLUTION, TWTY_FIX32, TWON_ENUMERATION, TWQC_ALL))
   || !m_ICAP_UNIT_Dependant[ICAP_XRESOLUTION]->Add(50)
   || !m_ICAP_UNIT_Dependant[ICAP_XRESOLUTION]->Add(100)
   || !m_ICAP_UNIT_Dependant[ICAP_XRESOLUTION]->Add(150)
   || !m_ICAP_UNIT_Dependant[ICAP_XRESOLUTION]->Add(200, true)
   || !m_ICAP_UNIT_Dependant[ICAP_XRESOLUTION]->Add(300)
   || !m_ICAP_UNIT_Dependant[ICAP_XRESOLUTION]->Add(400)
   || !m_ICAP_UNIT_Dependant[ICAP_XRESOLUTION]->Add(500)
   || !m_ICAP_UNIT_Dependant[ICAP_XRESOLUTION]->Add(600) )
  {
    cerr << "Could not create ICAP_XRESOLUTION" << endl;
    setConditionCode(TWCC_LOWMEMORY);
    return TWRC_FAILURE;
  }

  // expressed internally as pixels per inch
  if( NULL == (m_ICAP_UNIT_Dependant[ICAP_YRESOLUTION] = new CTWAINContainerFix32(ICAP_YRESOLUTION, TWTY_FIX32, TWON_ENUMERATION, TWQC_ALL))
   || !m_ICAP_UNIT_Dependant[ICAP_YRESOLUTION]->Add(50)
   || !m_ICAP_UNIT_Dependant[ICAP_YRESOLUTION]->Add(100)
   || !m_ICAP_UNIT_Dependant[ICAP_YRESOLUTION]->Add(150)
   || !m_ICAP_UNIT_Dependant[ICAP_YRESOLUTION]->Add(200, true)
   || !m_ICAP_UNIT_Dependant[ICAP_YRESOLUTION]->Add(300)
   || !m_ICAP_UNIT_Dependant[ICAP_YRESOLUTION]->Add(400)
   || !m_ICAP_UNIT_Dependant[ICAP_YRESOLUTION]->Add(500)
   || !m_ICAP_UNIT_Dependant[ICAP_YRESOLUTION]->Add(600) )
  {
    cerr << "Could not create ICAP_YRESOLUTION" << endl;
    setConditionCode(TWCC_LOWMEMORY);
    return TWRC_FAILURE;
  }

  // expressed internally as 1000 pixels per inch
  /// @todo Get physical dimentions from scanner
  /// @todo Add seperate dimentions for ADF and Flatbed
  //  Flatbed   - A4 letter paper
  //  ConvertUnits(29.7f, TWUN_CENTIMETERS, TWUN_INCHES, 1000);
  //  ConvertUnits(21.0f, TWUN_CENTIMETERS, TWUN_INCHES, 1000);
  if( NULL == (m_ICAP_UNIT_Dependant[ICAP_PHYSICALWIDTH] = new CTWAINContainerFix32(ICAP_PHYSICALWIDTH, TWTY_FIX32, TWON_ONEVALUE, TWQC_GETS))
   || !m_ICAP_UNIT_Dependant[ICAP_PHYSICALWIDTH]->Add(8500, true) )
  {
    cerr << "Could not create ICAP_PHYSICALWIDTH" << endl;
    setConditionCode(TWCC_LOWMEMORY);
    return TWRC_FAILURE;
  }

  if( NULL == (m_ICAP_UNIT_Dependant[ICAP_PHYSICALHEIGHT] = new CTWAINContainerFix32(ICAP_PHYSICALHEIGHT, TWTY_FIX32, TWON_ONEVALUE, TWQC_GETS))
   || !m_ICAP_UNIT_Dependant[ICAP_PHYSICALHEIGHT]->Add(14000, true) )
  {
    cerr << "Could not create ICAP_PHYSICALHEIGHT" << endl;
    setConditionCode(TWCC_LOWMEMORY);
    return TWRC_FAILURE;
  }

  // setup the ICAP_FRAMES
  // expressed internally as 1000 pixels per inch
  // Currently only supports one frame see: ICAP_MAXFRAMES
  if( NULL == (m_pICAP_FRAMES = new CTWAINContainerFrame(ICAP_FRAMES, TWTY_FRAME, TWON_ENUMERATION, TWQC_ALL))
   || !m_pICAP_FRAMES->Add(0, 0, 8500, 11000, true) )
  {
    cerr << "Could not create ICAP_FRAMES" << endl;
    setConditionCode(TWCC_LOWMEMORY);
    return TWRC_FAILURE;
  }

  return TWRC_SUCCESS;
}


//////////////////////////////////////////////////////////////////////////////
CTWAINDS_FreeImage::~CTWAINDS_FreeImage()
{
  // free all resources belonging to m_IndependantCapMap
  TWAINCapabilitiesMap_int::iterator cur_int = m_BitDepthMap.begin();
  while(cur_int != m_BitDepthMap.end())
  {
    delete cur_int->second;
    cur_int++;
  }

  TWAINCapabilitiesMap_FIX32::iterator cur_Fix32 = m_ICAP_UNIT_Dependant.begin();
  while(cur_Fix32 != m_ICAP_UNIT_Dependant.end())
  {
    delete cur_Fix32->second;
    cur_Fix32++;
  }

  if(m_pICAP_FRAMES)
  {
    delete m_pICAP_FRAMES;
  }
  m_pICAP_FRAMES = 0;
}

//////////////////////////////////////////////////////////////////////////////
void CTWAINDS_FreeImage::fillIdentityStructure(TW_IDENTITY& _idStruct)
{
  _idStruct = m_TheIdentity;
}

//////////////////////////////////////////////////////////////////////////////
TW_INT16 CTWAINDS_FreeImage::getImageInfo(pTW_IMAGEINFO _pImageInfo)
{
  TW_INT16 twrc = TWRC_SUCCESS;

  memset(_pImageInfo, 0, sizeof(TW_IMAGEINFO));

  // Only valid in state 6 and 7(State 7 only after receiving TWRC_XFERDONE)
  if( dsState_XferReady > m_CurrentState )
  {
    setConditionCode(TWCC_SEQERROR);
    return TWRC_FAILURE;
  }

  // Get the actual values used by the scanner.
  SFreeImage settings = m_Scanner.getSetting();

  _pImageInfo->XResolution = FloatToFIX32(settings.m_fXResolution);
  _pImageInfo->YResolution = FloatToFIX32(settings.m_fYResolution);
  _pImageInfo->ImageWidth  = settings.m_nWidth;
  _pImageInfo->ImageLength = settings.m_nHeight;
  
  // Our sample scanner only does one combination for each PixelType.
  switch(settings.m_nPixelType)
  {
    case TWPT_BW:
      _pImageInfo->PixelType = TWPT_BW;
      _pImageInfo->BitsPerPixel = 1;
      _pImageInfo->SamplesPerPixel = 1;
      _pImageInfo->BitsPerSample[0] = 1;
    break;

    case TWPT_GRAY:
      _pImageInfo->PixelType = TWPT_GRAY;
      _pImageInfo->BitsPerPixel = 8;
      _pImageInfo->SamplesPerPixel = 1;
      _pImageInfo->BitsPerSample[0] = 8;
    break;

    case TWPT_RGB:
      _pImageInfo->PixelType = TWPT_RGB;
      _pImageInfo->BitsPerPixel = 24;
      _pImageInfo->SamplesPerPixel = 3;
      _pImageInfo->BitsPerSample[0] = 8;
      _pImageInfo->BitsPerSample[1] = 8;
      _pImageInfo->BitsPerSample[2] = 8;
    break;
  }

  _pImageInfo->Planar = FALSE;
  _pImageInfo->Compression = TWCP_NONE;

  return twrc;
}

//////////////////////////////////////////////////////////////////////////////
TW_INT16 CTWAINDS_FreeImage::openDS(pTW_IDENTITY  _pOrigin)
{
  TW_INT16 ret = TWRC_SUCCESS;

  // this basic version of the DS only supports one connection from the DSM
  if( m_App.Id != 0 )
  {
    setConditionCode(TWCC_MAXCONNECTIONS);
    return TWRC_FAILURE;
  }

  if( dsState_Loaded != m_CurrentState )
  {
    setConditionCode(TWCC_SEQERROR);
    return TWRC_FAILURE;
  }

  // store the pointer of the originating scan requestor.
  m_App = *_pOrigin;

  m_DocumentNumber = 0;
  m_PageNumber = 0;

  if(!m_Scanner.resetScanner())
  {
    setConditionCode(TWCC_BUMMER);
    assert(0);
    ret = TWRC_FAILURE;
  }
  else
  {
    m_CurrentState = dsState_Open;
  }

  return ret;
}

//////////////////////////////////////////////////////////////////////////////
TW_INT16 CTWAINDS_FreeImage::closeDS()
{
  // check if ok to return success
  // Must be in state 4
  if( dsState_Open != m_CurrentState )
  {
    setConditionCode(TWCC_SEQERROR);
    return TWRC_FAILURE;
  }

  memset(&m_App, 0, sizeof(m_App));
  return TWRC_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
TW_INT16 CTWAINDS_FreeImage::enableDS(pTW_USERINTERFACE _pData)
{
  if( dsState_Open != m_CurrentState )
  {
    setConditionCode(TWCC_SEQERROR);
    return TWRC_FAILURE;
  }

  // @todo Impliment UI in the DS
  if(1)//FALSE == _pData->ShowUI)
  {
    m_CurrentState = dsState_Enabled;

    // Update the scanner with the latest negotiated caps
    if(!updateScannerFromCaps())
    {
      cerr << "ds: There was an error while prepping the image for scanning" << endl;
      setConditionCode(TWCC_BADVALUE);
      return TWRC_FAILURE;
    }

    //set pending xfers to whatever the user configured for XferCount
    int Count = TWON_DONTCARE32;
    CTWAINContainerInt *pnCap = dynamic_cast<CTWAINContainerInt*>(findCapability(CAP_XFERCOUNT));
    if(pnCap)
    {
      pnCap->GetCurrent(Count);
    }
    m_Xfers.Count = Count;

    // Indicate we have not transferred any images yet
    m_DocumentNumber = 0;
    m_PageNumber     = 0;

    // The application will move to state 5 after this triplet which means that
    // no more capabilities can be set until we are brought back to state 4.
    m_Scanner.Lock();

    // Because there is no user interface, there isn't anything to show here.
    // But, at this point, the application is not allowed to set any more
    // capabilities.  This means that we can do any initializations we
    // need in order to prepare for the next few calls and the scan.
    // get the scanner to load the image so that image info calls can be done
    if(!m_Scanner.acquireImage())
    {
      cerr << "ds: There was an error while trying to get scanner to acquire image" << endl;
      m_CurrentState = dsState_Open;
      setConditionCode(TWCC_SEQERROR);
      return TWRC_FAILURE;
    }

    if(! DoXferReadyEvent())
    {
      m_CurrentState = dsState_Open;
      setConditionCode(TWCC_SEQERROR);
      return TWRC_FAILURE;
    }

    // @todo remove this when UI is working
    if(TRUE == _pData->ShowUI)
    {
      setConditionCode(TWCC_BADPROTOCOL);
      return TWRC_CHECKSTATUS;
    }
  }
  else
  {
  /// @todo Hook up the GUI
  /*
    if(!DisplayTWAINGUI(*_pData))
    {
      // A user interface is not supported as of right now because we are
      // in text mode.
      setConditionCode(TWCC_OPERATIONERROR);
      twrc = TWRC_FAILURE;
    }
    else
    {
      m_CurrentState = dsState_Enabled;
    }
   */
  return TWRC_FAILURE;

  }

  return TWRC_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
TW_INT16 CTWAINDS_FreeImage::disableDS(pTW_USERINTERFACE _pData)
{
  if( dsState_Enabled != m_CurrentState )
  {
    setConditionCode(TWCC_SEQERROR);
    return TWRC_FAILURE;
  }

  // allow the scanners caps to be writeable again because we are moving back
  // to state 4. If this source had a UI, it would be lowered at this time.
  m_Scanner.Unlock();

  if(TRUE == _pData->ShowUI)
  {
    // @todo Add this line back in later:
    //DestroyTWAINGUI();
  }
  
  // There is no UI in this text interface so there is nothing
  // to do here.
  m_CurrentState = dsState_Open;
  return TWRC_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
TW_INT16 CTWAINDS_FreeImage::getMemoryXfer(pTW_SETUPMEMXFER _pData)
{
  // valid to call for 4, 5, & 6
  if( !(dsState_Open      == m_CurrentState ||
        dsState_Enabled   == m_CurrentState ||
        dsState_XferReady == m_CurrentState   ))
  {
    setConditionCode(TWCC_SEQERROR);
    return TWRC_FAILURE;
  }

  // Min is the physical width at highest res and hishest bits/pixel count
  // chose 64k because this is a typical sweet spot for most real world scanners
  // (max transfer size for SCSI read of typical SCSI devices)
  TW_UINT32 bytes = (TW_UINT32)BYTES_PERLINE_ALIGN4(8.5*600, 24);
  _pData->MinBufSize = bytes;
  _pData->MaxBufSize = MAX(64*1024, bytes);//bytes;
  _pData->Preferred  = MAX(64*1024, bytes);//bytes;

  return TWRC_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
TW_INT16 CTWAINDS_FreeImage::processEvent(pTW_EVENT _pEvent)
{
  TW_UINT16 twRc = TWRC_SUCCESS;

  if( dsState_Enabled > m_CurrentState )
  {
    setConditionCode(TWCC_SEQERROR);
    return TWRC_FAILURE;
  }

  if(0 == _pEvent)
  {
    setConditionCode(TWCC_BADVALUE);
    return TWRC_FAILURE;
  }

  #ifdef TWH_CMP_MSC
  // There is only a GUI in windows
  /// @todo once we have an UI then we will need to determine if this message is ours.
  if (0)//hImageDlg && IsDialogMessage(hImageDlg, (LPMSG)(((pTW_EVENT)pData)->pEvent)))
  {
    twRc = TWRC_DSEVENT;
    // The source should, for proper form, return a MSG_NULL for
    // all Windows messages processed by the Data Source
    _pEvent->TWMessage = MSG_NULL;
  }
  else
  #endif
  {
    // notify the application that the source did not
    // consume this message
    twRc = TWRC_NOTDSEVENT;
    _pEvent->TWMessage = MSG_NULL;
  }
  return twRc;
}


//////////////////////////////////////////////////////////////////////////////
TW_INT16 CTWAINDS_FreeImage::transfer()
{
  TW_INT16 twrc = TWRC_SUCCESS;

  if( dsState_XferReady != m_CurrentState )
  {
    setConditionCode(TWCC_SEQERROR);
    return TWRC_FAILURE;
  }
  m_nDestScanLine = 0;

  //This is for know image size.
  if(m_ImageInfo.ImageWidth>0 && m_ImageInfo.ImageLength>0)
  {
    DWORD nDestBytesPerRow = BYTES_PERLINE(m_ImageInfo.ImageWidth, m_ImageInfo.BitsPerPixel);
    DWORD nImageSize       = nDestBytesPerRow * m_ImageInfo.ImageLength;

    //If we had a previous image then get rid of it.
    if(m_hImageData)
    {
      _DSM_Free(m_hImageData);
    }
    m_hImageData           = _DSM_Alloc(nImageSize);
    if(!m_hImageData)
    {
      setConditionCode(TWCC_LOWMEMORY);
      return TWRC_FAILURE;
    }

    TW_MEMREF   pData       = _DSM_LockMemory(m_hImageData);
    BYTE       *pImageData  = (BYTE*)pData;
    TW_HANDLE   hImageBuff  = 0;

    DWORD dwRead;
    DWORD dwReceived;

    do
    {
      dwRead = MIN(64000, nImageSize) / nDestBytesPerRow * nDestBytesPerRow;
      dwReceived =0;

      if( !m_Scanner.getScanStrip(pImageData, dwRead, dwReceived) ||
          dwReceived != dwReceived / nDestBytesPerRow * nDestBytesPerRow)
      {
        //No more data to recieve fill the rest
        break;
      }
      pImageData += dwReceived;
      
      nImageSize -= dwReceived;
    }while(nImageSize>0 && twrc == TWRC_SUCCESS);

    _DSM_UnlockMemory(pData);
    if(hImageBuff)
    {
      _DSM_Free(hImageBuff);
    }
  }
  else
  {
    /// @todo Unknow paper size
    // for unknow paper size need to use reallocate and keep transfering and adding to image data.
    setConditionCode(TWCC_CAPUNSUPPORTED);
    return TWRC_FAILURE;
  }

  if(twrc == TWRC_FAILURE)
  {
    setConditionCode(TWCC_BADVALUE);
  }

  return twrc;
}

//////////////////////////////////////////////////////////////////////////////
TW_INT16 CTWAINDS_FreeImage::endXfer(pTW_PENDINGXFERS _pXfers)
{
  TW_INT16 twrc = TWRC_SUCCESS;

  if( !( dsState_XferReady == m_CurrentState ||
         dsState_Xferring  == m_CurrentState    ) )
  {
    setConditionCode(TWCC_SEQERROR);
    return TWRC_FAILURE;
  }

  if(m_Xfers.Count != TW_UINT16(TWON_DONTCARE16))
  {
    --m_Xfers.Count;
  }
  else if(m_Xfers.Count == TW_UINT16(TWON_DONTCARE16) && !m_Scanner.isFeederLoaded())
  {
    m_Xfers.Count = 0;
  }

  if(0 != m_Xfers.Count)
  {
    // Check to see if autofeed is turned on if so automaticly go get next image.
    CTWAINContainerInt *pnCap = dynamic_cast<CTWAINContainerInt*>(findCapability(CAP_AUTOFEED));
    BOOL bAutoFeed = FALSE;
    if(pnCap)
    {
      pnCap->GetCurrent(bAutoFeed);
    }
    if(bAutoFeed)
    {
      // More images are requested, go to scan and try to get the next
      // image in pre-emption of the app asking for it
      if(!m_Scanner.acquireImage())
      {
        cerr << "ds: There was an error while prepping the image for scanning" << endl;
        setConditionCode(TWCC_BUMMER);
        twrc = TWRC_FAILURE;
      }

      m_CurrentState = dsState_XferReady;
    }
  }
  else
  {
    m_CurrentState = dsState_Enabled;
    m_Scanner.Unlock();
  }

  if( _pXfers == 0 )
  {
    setConditionCode(TWCC_BADVALUE);
    // Did everyting but return the currect count.
    return TWRC_CHECKSTATUS;
  }

  *_pXfers = m_Xfers;

  return twrc;
}

//////////////////////////////////////////////////////////////////////////////
TW_INT16 CTWAINDS_FreeImage::getXfer(pTW_PENDINGXFERS _pXfers)
{
  TW_INT16 twrc = TWRC_SUCCESS;

  if( dsState_XferReady != m_CurrentState )
  {
    setConditionCode(TWCC_SEQERROR);
    return TWRC_FAILURE;
  }

  if( _pXfers == 0 )
  {
    setConditionCode(TWCC_BADVALUE);
    // Did everyting but return the currect count.
    return TWRC_CHECKSTATUS;
  }

  *_pXfers = m_Xfers;
  return twrc;
}

//////////////////////////////////////////////////////////////////////////////
TW_INT16 CTWAINDS_FreeImage::resetXfer(pTW_PENDINGXFERS _pXfers)
{
  TW_INT16 twrc = TWRC_SUCCESS;

  if( dsState_Loaded == m_CurrentState )
  {
    setConditionCode(TWCC_SEQERROR);
    return TWRC_FAILURE;
  }

  m_Xfers.Count = 0;
  m_CurrentState = dsState_Enabled;
  m_Scanner.Unlock();

  if( _pXfers == 0 )
  {
    setConditionCode(TWCC_BADVALUE);
    return TWRC_FAILURE;
  }

  *_pXfers = m_Xfers;
  return twrc;
}


//////////////////////////////////////////////////////////////////////////////
bool CTWAINDS_FreeImage::updateScannerFromCaps()
{
  int   nVal;
  float fVal;
  bool  bret = true;  // Set to false if anything fails
  SFreeImage settings;

  // Get current before updating
  settings = m_Scanner.getSetting();

  CTWAINContainerInt    *pnCap = 0;
  CTWAINContainerFix32  *pfCap = 0;
 
  if(0 == (pnCap = dynamic_cast<CTWAINContainerInt*>(findCapability(ICAP_PIXELTYPE))))
  {
    cerr << "Could not get ICAP_PIXELTYPE" << endl;
    bret = false;
  }
  else
  {
    pnCap->GetCurrent(nVal);
    settings.m_nPixelType = nVal;
  }

  // X resolution the same.
  if(0 == (pfCap = dynamic_cast<CTWAINContainerFix32*>(findCapability(ICAP_XRESOLUTION))))
  {
    cerr << "Could not get ICAP_XRESOLUTION" << endl;
    bret = false;
  }
  else
  {
    pfCap->GetCurrent(fVal);
    settings.m_fXResolution = fVal;
  }

  // Y resolution the same.
  if(0 == (pfCap = dynamic_cast<CTWAINContainerFix32*>(findCapability(ICAP_YRESOLUTION))))
  {
    cerr << "Could not get ICAP_YRESOLUTION" << endl;
    bret = false;
  }
  else
  {
    pfCap->GetCurrent(fVal);
    settings.m_fYResolution = fVal;
  }

  // set the image dimensions
  InternalFrame frame;
  if(!m_pICAP_FRAMES->GetCurrent(frame))
  {
    cerr << "Could not get ICAP_FRAMES" << endl;
    bret = false;
  }
  else
  {
    // prepare to set the scanning window by converting from our internal units to pixels.
    TW_FRAME TWframe = frame.AsTW_FRAME(TWUN_PIXELS, settings.m_fXResolution, settings.m_fYResolution);

    // I'm not expecting any fractional values here so casting to int is safe
    /** @todo the scanner only supports width and height if the image is intended to be cropped on the 
    *    left or top then need to crop the image as it is coming off the scanner.
    *    Should be passing to the scanner the width as the right and height as bottom; not the diff.
    */
    settings.m_nWidth  = int(FIX32ToFloat(TWframe.Right)  - FIX32ToFloat(TWframe.Left));
    settings.m_nHeight = int(FIX32ToFloat(TWframe.Bottom) - FIX32ToFloat(TWframe.Top));
  }

  m_Scanner.setSetting(settings);
  
  return bret;
}

//////////////////////////////////////////////////////////////////////////////
bool CTWAINDS_FreeImage::DoCloseDSOkEvent()
{
  if( dsState_Enabled != m_CurrentState )
  {
    setConditionCode(TWCC_SEQERROR);
    return false;
  }

  // Update the scanner with the latest negotiated caps
  if(!updateScannerFromCaps())
  {
    return false;
  }

  return CTWAINDS_Base::DoCloseDSOkEvent();
}


//////////////////////////////////////////////////////////////////////////////
CTWAINContainer* CTWAINDS_FreeImage::findCapability(const TW_UINT16 _unCap)
{
  CTWAINContainer *pRet = 0;
  // When adding to findCapabiltiy remember the order of operations 
  // Some capabilities are dependent on others.
  // see: http://www.twain.org/docs/CapOrderForWeb.PDF
  switch(_unCap)
  {
    case ICAP_FRAMES:
      pRet = m_pICAP_FRAMES;
    break;

    case ICAP_BITDEPTH:
      pRet = getICAP_BITDEPTH();
    break;

    case CAP_DEVICEONLINE:
      {
        CTWAINContainerInt *pIntCon = dynamic_cast<CTWAINContainerInt*>(m_IndependantCapMap[CAP_DEVICEONLINE]);
        if( NULL != pIntCon )
        {
          pIntCon->SetCurrent( m_Scanner.getDeviceOnline()?TRUE:FALSE );
          pRet = pIntCon;
        }
      }
    break;

    case CAP_FEEDERLOADED:
      {
        CTWAINContainerInt *pIntCon = dynamic_cast<CTWAINContainerInt*>(m_IndependantCapMap[CAP_FEEDERLOADED]);
        if( NULL != pIntCon )
        {
          pIntCon->SetCurrent( m_Scanner.isFeederLoaded()?TRUE:FALSE );
          pRet = pIntCon;
        }
      }
    break;

    case ICAP_XRESOLUTION:
    case ICAP_YRESOLUTION:
    case ICAP_PHYSICALWIDTH:
    case ICAP_PHYSICALHEIGHT:
    {
      TWAINCapabilitiesMap_FIX32::iterator itCap = m_ICAP_UNIT_Dependant.find(_unCap);

      if(itCap != m_ICAP_UNIT_Dependant.end())
      {
        pRet = itCap->second;
      }
    }
    break;

    default:
      // cycle through m_IndependantCapMap
      pRet = CTWAINDS_Base::findCapability(_unCap);
    break;
  }
  
  return pRet;
}

//////////////////////////////////////////////////////////////////////////////
CTWAINContainer* CTWAINDS_FreeImage::getICAP_BITDEPTH()
{
  CTWAINContainer* pRet = 0;

  // BitDepth depends on PixelType
  TWAINCapabilitiesMap::iterator itCap = m_IndependantCapMap.find(ICAP_PIXELTYPE);

  if(itCap != m_IndependantCapMap.end())
  {
    int nCurrentVal = -1;
    CTWAINContainerInt* pnCap = dynamic_cast<CTWAINContainerInt*>(itCap->second);

    if((0 != pnCap) && (pnCap->GetCurrent(nCurrentVal)))
    {
      TWAINCapabilitiesMap_int::iterator itBDCap = m_BitDepthMap.find(nCurrentVal);
      if(itBDCap != m_BitDepthMap.end())
      {
        pRet = itBDCap->second;
      }
    }
  }
  
  return pRet;
}

//////////////////////////////////////////////////////////////////////////////
TW_INT16 CTWAINDS_FreeImage::dat_imagelayout(TW_UINT16         _MSG,
                                             pTW_IMAGELAYOUT   _pData)
{
  TW_INT16 twrc = TWRC_SUCCESS;

  switch(_MSG)
  {
    case MSG_GET:
    case MSG_GETCURRENT:
    case MSG_GETDEFAULT:
      if( !( dsState_Open      == m_CurrentState || 
             dsState_Enabled   == m_CurrentState || 
             dsState_XferReady == m_CurrentState   ) )
      {
        setConditionCode(TWCC_SEQERROR);
        return TWRC_FAILURE;
      }
      break;

    case MSG_RESET:
    case MSG_SET:
      if( dsState_Open != m_CurrentState )
      {
        setConditionCode(TWCC_SEQERROR);
        return TWRC_FAILURE;
      }
      break;
  }

  if((0 == _pData) || (0 == m_pICAP_FRAMES))
  {
    setConditionCode(TWCC_BADVALUE);
    return TWRC_FAILURE;
  }

  CTWAINContainerInt* pDepCap = dynamic_cast<CTWAINContainerInt*>(findCapability(ICAP_SUPPORTEDSIZES));

  switch(_MSG)
  {
    case MSG_RESET:
      if(!pDepCap->Reset())
      {
        setConditionCode(TWCC_CAPBADOPERATION);
        twrc = TWRC_FAILURE;
      }
      else
      {
        // this will update the frame to match the papersize
        updatePostDependencies(MSG_RESET, ICAP_SUPPORTEDSIZES);
      }

      //change the behavior to a get current
      _MSG = MSG_GETCURRENT;
    case MSG_GET:
    case MSG_GETCURRENT:
    case MSG_GETDEFAULT:
    {
      InternalFrame frame;
      
      if(MSG_GETDEFAULT == _MSG)
      {
        if(!m_pICAP_FRAMES->GetDefault(frame))
        {
          twrc = TWRC_FAILURE;
          setConditionCode(TWCC_CAPBADOPERATION);
        }
      }
      else
      {
        if(!m_pICAP_FRAMES->GetCurrent(frame))
        {
          twrc = TWRC_FAILURE;
          setConditionCode(TWCC_CAPBADOPERATION);
        }
      }
      
      if(TWRC_SUCCESS == twrc)
      {
        int   unit = TWUN_PIXELS;
        float Xres = 100;
        float Yres = 100;
        getCurrentUnits(unit, Xres, Yres);
        _pData->Frame = frame.AsTW_FRAME(unit, Xres, Yres);
        _pData->FrameNumber = m_pICAP_FRAMES->getIndexForValue(frame);
        _pData->DocumentNumber = m_DocumentNumber;
        _pData->PageNumber = m_PageNumber;
      }
    }
    break;

    case MSG_SET:
    {
      /// @todo check frame with physical extents.  If frame is too big or too small modify it 
      ///  and return TWCC_BADVALUE if too big else TWRC_CHECKSTATUS if changed.

      //  Application that want multiple frames should negotiated frames through ICAP_FRAMES
      //  Layout can only set the current frame.
      //  Our simple source only supports one frame.
      TW_CAPABILITY cap;

      TW_HANDLE hContainer = _DSM_Alloc(sizeof(TW_ONEVALUE_FRAME));
      if(0 != hContainer)
      {
        cap.hContainer = hContainer;
        cap.Cap = ICAP_FRAMES;
        cap.ConType = TWON_ONEVALUE;
        pTW_ONEVALUE_FRAME pCap = (pTW_ONEVALUE_FRAME)_DSM_LockMemory(cap.hContainer);
        pCap->Item = _pData->Frame;
        pCap->ItemType = TWTY_FRAME;
        _DSM_UnlockMemory((TW_MEMREF)pCap);

        TW_INT16 Condition;
        if(!m_pICAP_FRAMES->Set(&cap, Condition))
        {
          twrc = TWRC_FAILURE;
          setConditionCode(Condition);
        }
        else
        {
          updatePostDependencies(MSG_SET, ICAP_FRAMES);
        }
        _DSM_Free(hContainer);
      }
      else
      {
        twrc = TWRC_FAILURE;
        setConditionCode(TWCC_CAPBADOPERATION);
      }

      if(_pData->DocumentNumber != TWON_DONTCARE32)
      {
        m_DocumentNumber = _pData->DocumentNumber;
      }
      if(_pData->PageNumber != TWON_DONTCARE32)
      {
        m_PageNumber = _pData->PageNumber;
      }
    }
    break;

    default:
      setConditionCode(TWCC_BADPROTOCOL);
      assert(0);
      twrc = TWRC_FAILURE;
    break;
  }

  return twrc;
}

