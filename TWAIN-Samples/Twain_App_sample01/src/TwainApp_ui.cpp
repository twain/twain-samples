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
 * @file TwainApp_ui.cpp
 * User interface for TWAIN Application
 * @author JFL Peripheral Solutions Inc.
 * @date October 2006
 */

#ifdef _WINDOWS
  #include "stdafx.h"
#endif

#include <iostream>
#include <sstream>
using namespace std;

#include "TwainApp_ui.h"

#define TEMPBUFSIZE 80

/**
* return a temp buffer from an array.  Rotating through the queue.
* @return pointer to the buffer to use.
*/
char * nextTempBuffer()
{
  static char szTempBuffer[3][TEMPBUFSIZE];
  static int  nTempBuffer = 0;

  if(++nTempBuffer >= 3)
  {
    nTempBuffer = 0;
  }
  return szTempBuffer[nTempBuffer];
}

//////////////////////////////////////////////////////////////////////////////
string getErrorString_UnexpectedCap(const TW_UINT16 _unExpected, const TW_UINT16 _unReceived)
{
  ostringstream errMsg;  
  
  errMsg 
    << "The capability is an unexpected type. "
    << "Expecting " << convertICAP_toString(_unExpected)
    << " got " << convertICAP_toString(_unReceived);  
  
  return errMsg.str();
}

//////////////////////////////////////////////////////////////////////////////
string getErrorString_UnexpectedType(const TW_UINT16 _unExpected, const TW_UINT16 _unReceived)
{
  ostringstream errMsg;  
  
  errMsg 
    << "The type is an unexpected value. "
    << "Expecting " << convertTWTY_toString(_unExpected)
    << " got " << convertTWTY_toString(_unReceived);  
  
  return errMsg.str();
}

//////////////////////////////////////////////////////////////////////////////
void printOptions()
{
  cout
    << "\n"
    << "Please enter an option\n"
    << "----------------------\n"
    << "q    - quit\n"
    << "h    - help\n"
    << "cdsm - connect to the dsm\n"
    << "xdsm - disconnect from the dsm\n"
    << "lds  - list data sources available\n"
    << "pds# - print identity structure for data source with id#. ex: pds2\n"
    << "cds# - connect to data source with id#. ex: cds2\n"
    << "xds  - disconnect from data source\n"
    << "caps - negotiate capabilities\n"
    << "scan - start the scan\n"
    << endl;
  return;
}

//////////////////////////////////////////////////////////////////////////////
void printMainCaps()
{
  cout
    << "\n"
    << "Capabilites\n"
    << "-----------\n"
    << "q - done negotiating, go back to main menu\n"
    << "h - help\n"
    << "1 - ICAP_XFERMECH\n"
    << "2 - ICAP_PIXELTYPE\n"
    << "3 - ICAP_BITDEPTH\n"
    << "4 - ICAP_XRESOLUTION\n"
    << "5 - ICAP_YRESOLUTION\n"
    << "6 - ICAP_FRAMES\n"
    << "7 - ICAP_UNITS\n"
    << endl;
  return;
}

//////////////////////////////////////////////////////////////////////////////
void print_ICAP(const TW_UINT16 _unCap, pTW_ONEVALUE _pCap)
{
  if(0 == _pCap)
  {
    return;
  }

  cout
    << "\n"
    << convertICAP_toString(_unCap) << "\n"
    << "--------------\n"
    << "Showing supported types. * indicates current setting.\n\n"
    << "q - done\n";
  
  if(TWTY_UINT16 == _pCap->ItemType)
  {
    cout << "1 - " << convertICAP_Item_toString(_unCap, TW_UINT16(_pCap->Item)) << "*" << endl;
  }
  else// if(TWTY_UINT16 != _pCap->ItemType)
  {
    cerr << getErrorString_UnexpectedType(TWTY_UINT16, _pCap->ItemType) << endl;
  }
  
  return;
}

//////////////////////////////////////////////////////////////////////////////
void print_ICAP(const TW_UINT16 _unCap, pTW_ENUMERATION _pCap)
{
  if(0 == _pCap)
  {
    return;
  }

  cout
    << "\n"
    << convertICAP_toString(_unCap) << "\n"
    << "--------------\n"
    << "Showing supported types. * indicates current setting.\n\n"
    << "q - done\n";

  if(TWTY_UINT16 != _pCap->ItemType)
  {
    cerr << getErrorString_UnexpectedType(TWTY_UINT16, _pCap->ItemType) << endl;
    return;
  }

  for(TW_UINT32 x = 0; x < _pCap->NumItems; ++x)
  {
    cout << x << " - " << convertICAP_Item_toString(_unCap, ((pTW_UINT16)(&_pCap->ItemList))[x]);

    if(x == _pCap->CurrentIndex)
    {
      cout << "*";
    }

    cout << "\n";
  }

  cout << endl;
  return;
}



//////////////////////////////////////////////////////////////////////////////
void print_ICAP(const TW_UINT16 _unCap, pTW_ONEVALUE_FIX32 _pVal)
{
  if(0 == _pVal)
  {
    return;
  }

  cout 
    << "\n"
    << convertICAP_toString(_unCap) << "\n"
    << "--------------\n"
    << "Showing supported types. * indicates current setting.\n\n"
    << "q - done\n";

  if(TWTY_FIX32 == _pVal->ItemType)
  {
    cout << "1 - " << _pVal->Item.Whole << "." << _pVal->Item.Frac << "*\n" << endl;
  }
  else //if(TWTY_FIX32 != _pVal->ItemType)
  {
    cerr << getErrorString_UnexpectedType(TWTY_FIX32, _pVal->ItemType) << endl;
  }
  
  return;
}

//////////////////////////////////////////////////////////////////////////////
void print_ICAP(const TW_UINT16 _unCap, pTW_ENUMERATION_FIX32 _pVal)
{
  if(0 == _pVal)
  {
    return;
  }

  cout 
    << "\n"
    << convertICAP_toString(_unCap) << "\n"
    << "--------------\n"
    << "Showing supported types. * indicates current setting.\n\n"
    << "q - done\n";

  if(TWTY_FIX32 == _pVal->ItemType)
  {
    for(TW_UINT32 x = 0; x < _pVal->NumItems; ++x)
    {
      cout << x << " - ";

      cout
        << _pVal->ItemList[x].Whole << "." << _pVal->ItemList[x].Frac;

      if(x == _pVal->CurrentIndex)
      {
        cout << "*";
      }

      cout << "\n";
    }
    cout << endl;
  }
  else //if(TWTY_FIX32 != _pVal->ItemType)
  {
    cerr << getErrorString_UnexpectedType(TWTY_FIX32, _pVal->ItemType) << endl;
  }

  return;
}

//////////////////////////////////////////////////////////////////////////////
void print_ICAP(const TW_UINT16 _unCap, pTW_ONEVALUE_FRAME _pCap)
{
  if(0 == _pCap)
  {
    return;
  }

  cout
    << "\n"
    << convertICAP_toString(_unCap) << "\n"
    << "--------------\n"
    << "Showing supported types. * indicates current setting.\n\n"
    << "q - done\n";

  if(TWTY_FRAME != _pCap->ItemType)
  {
    cerr << getErrorString_UnexpectedType(TWTY_FRAME, _pCap->ItemType) << endl;
    return;
  }

  pTW_FRAME pframe = &(_pCap->Item);

  cout
    << "1 - Frame Data:*\n"
    << "\tLeft,\tTop,\tRight,\tBottom\n" 
    << "\t" << FIX32ToFloat(pframe->Left) << ",\t"
    << FIX32ToFloat(pframe->Top) << ",\t"
    << FIX32ToFloat(pframe->Right) << ",\t"
    << FIX32ToFloat(pframe->Bottom) << "\n"
    << "\n"
    << endl;

  return;
}

//////////////////////////////////////////////////////////////////////////////
void print_ICAP(const TW_UINT16 _unCap, pTW_ENUMERATION_FRAME _pCap)
{
  if(0 == _pCap)
  {
    return;
  }

  cout
    << "\n"
    << convertICAP_toString(_unCap) << "\n"
    << "--------------\n"
    << "Showing supported types. * indicates current setting.\n\n"
    << "q - done\n";

  if(TWTY_FRAME != _pCap->ItemType)
  {
    cerr << getErrorString_UnexpectedType(TWTY_FRAME, _pCap->ItemType) << endl;
    return;
  }

  pTW_FRAME pframe = 0;

  for(TW_UINT32 x = 0; x < _pCap->NumItems; ++x)
  {
    pframe = &((_pCap->ItemList)[x]);

    cout 
      << x << " - Frame Data:";

    if(x == _pCap->CurrentIndex)
    {
      cout << "*";
    }
      
    cout << "\n" 
      << "\tLeft,\tTop,\tRight,\tBottom\n" 
      << "\t" << FIX32ToFloat(pframe->Left) << ",\t"
      << FIX32ToFloat(pframe->Top) << ",\t"
      << FIX32ToFloat(pframe->Right) << ",\t"
      << FIX32ToFloat(pframe->Bottom) << "\n"
      << "\n";
  }
  cout << endl;

  return;
}

//////////////////////////////////////////////////////////////////////////////
const char* convertICAP_toString(const TW_UINT16 _unCap)
{
  static struct {
    TW_UINT16 unCap;
    char* pszString;
  } capMap[] = 
  {
    {CAP_CUSTOMBASE, "CAP_CUSTOMBASE"},
    {CAP_XFERCOUNT, "CAP_XFERCOUNT"},
    {ICAP_COMPRESSION, "ICAP_COMPRESSION"},
    {ICAP_PIXELTYPE, "ICAP_PIXELTYPE"},
    {ICAP_UNITS, "ICAP_UNITS"},
    {ICAP_XFERMECH, "ICAP_XFERMECH"},
    {CAP_AUTHOR, "CAP_AUTHOR"},
    {CAP_CAPTION, "CAP_CAPTION"},
    {CAP_FEEDERENABLED, "CAP_FEEDERENABLED"},
    {CAP_FEEDERLOADED, "CAP_FEEDERLOADED"},
    {CAP_TIMEDATE, "CAP_TIMEDATE"},
    {CAP_SUPPORTEDCAPS, "CAP_SUPPORTEDCAPS"},
    {CAP_EXTENDEDCAPS, "CAP_EXTENDEDCAPS"},
    {CAP_AUTOFEED, "CAP_AUTOFEED"},
    {CAP_CLEARPAGE, "CAP_CLEARPAGE"},
    {CAP_FEEDPAGE, "CAP_FEEDPAGE"},
    {CAP_REWINDPAGE, "CAP_REWINDPAGE"},
    {CAP_INDICATORS, "CAP_INDICATORS"},
    {CAP_SUPPORTEDCAPSEXT, "CAP_SUPPORTEDCAPSEXT"},
    {CAP_PAPERDETECTABLE, "CAP_PAPERDETECTABLE"},
    {CAP_UICONTROLLABLE, "CAP_UICONTROLLABLE"},
    {CAP_DEVICEONLINE, "CAP_DEVICEONLINE"},
    {CAP_AUTOSCAN, "CAP_AUTOSCAN"},
    {CAP_THUMBNAILSENABLED, "CAP_THUMBNAILSENABLED"},
    {CAP_DUPLEX, "CAP_DUPLEX"},
    {CAP_DUPLEXENABLED, "CAP_DUPLEXENABLED"},
    {CAP_ENABLEDSUIONLY, "CAP_ENABLEDSUIONLY"},
    {CAP_CUSTOMDSDATA, "CAP_CUSTOMDSDATA"},
    {CAP_ENDORSER, "CAP_ENDORSER"},
    {CAP_JOBCONTROL, "CAP_JOBCONTROL"},
    {CAP_ALARMS, "CAP_ALARMS"},
    {CAP_ALARMVOLUME, "CAP_ALARMVOLUME"},
    {CAP_AUTOMATICCAPTURE, "CAP_AUTOMATICCAPTURE"},
    {CAP_TIMEBEFOREFIRSTCAPTURE, "CAP_TIMEBEFOREFIRSTCAPTURE"},
    {CAP_TIMEBETWEENCAPTURES, "CAP_TIMEBETWEENCAPTURES"},
    {CAP_CLEARBUFFERS, "CAP_CLEARBUFFERS"},
    {CAP_MAXBATCHBUFFERS, "CAP_MAXBATCHBUFFERS"},
    {CAP_DEVICETIMEDATE, "CAP_DEVICETIMEDATE"},
    {CAP_POWERSUPPLY, "CAP_POWERSUPPLY"},
    {CAP_CAMERAPREVIEWUI, "CAP_CAMERAPREVIEWUI"},
    {CAP_DEVICEEVENT, "CAP_DEVICEEVENT"},
    {CAP_SERIALNUMBER, "CAP_SERIALNUMBER"},
    {CAP_PRINTER, "CAP_PRINTER"},
    {CAP_PRINTERENABLED, "CAP_PRINTERENABLED"},
    {CAP_PRINTERINDEX, "CAP_PRINTERINDEX"},
    {CAP_PRINTERMODE, "CAP_PRINTERMODE"},
    {CAP_PRINTERSTRING, "CAP_PRINTERSTRING"},
    {CAP_PRINTERSUFFIX, "CAP_PRINTERSUFFIX"},
    {CAP_LANGUAGE, "CAP_LANGUAGE"},
    {CAP_FEEDERALIGNMENT, "CAP_FEEDERALIGNMENT"},
    {CAP_FEEDERORDER, "CAP_FEEDERORDER"},
    {CAP_REACQUIREALLOWED, "CAP_REACQUIREALLOWED"},
    {CAP_BATTERYMINUTES, "CAP_BATTERYMINUTES"},
    {CAP_BATTERYPERCENTAGE, "CAP_BATTERYPERCENTAGE"},
    {ICAP_AUTOBRIGHT, "ICAP_AUTOBRIGHT"},
    {ICAP_BRIGHTNESS, "ICAP_BRIGHTNESS"},
    {ICAP_CONTRAST, "ICAP_CONTRAST"},
    {ICAP_CUSTHALFTONE, "ICAP_CUSTHALFTONE"},
    {ICAP_EXPOSURETIME, "ICAP_EXPOSURETIME"},
    {ICAP_FILTER, "ICAP_FILTER"},
    {ICAP_FLASHUSED, "ICAP_FLASHUSED"},
    {ICAP_GAMMA, "ICAP_GAMMA"},
    {ICAP_HALFTONES, "ICAP_HALFTONES"},
    {ICAP_HIGHLIGHT, "ICAP_HIGHLIGHT"},
    {ICAP_IMAGEFILEFORMAT, "ICAP_IMAGEFILEFORMAT"},
    {ICAP_LAMPSTATE, "ICAP_LAMPSTATE"},
    {ICAP_LIGHTSOURCE, "ICAP_LIGHTSOURCE"},
    {ICAP_ORIENTATION, "ICAP_ORIENTATION"},
    {ICAP_PHYSICALWIDTH, "ICAP_PHYSICALWIDTH"},
    {ICAP_PHYSICALHEIGHT, "ICAP_PHYSICALHEIGHT"},
    {ICAP_SHADOW, "ICAP_SHADOW"},
    {ICAP_FRAMES, "ICAP_FRAMES"},
    {ICAP_XNATIVERESOLUTION, "ICAP_XNATIVERESOLUTION"},
    {ICAP_YNATIVERESOLUTION, "ICAP_YNATIVERESOLUTION"},
    {ICAP_XRESOLUTION, "ICAP_XRESOLUTION"},
    {ICAP_YRESOLUTION, "ICAP_YRESOLUTION"},
    {ICAP_MAXFRAMES, "ICAP_MAXFRAMES"},
    {ICAP_TILES, "ICAP_TILES"},
    {ICAP_BITORDER, "ICAP_BITORDER"},
    {ICAP_CCITTKFACTOR, "ICAP_CCITTKFACTOR"},
    {ICAP_LIGHTPATH, "ICAP_LIGHTPATH"},
    {ICAP_PIXELFLAVOR, "ICAP_PIXELFLAVOR"},
    {ICAP_PLANARCHUNKY, "ICAP_PLANARCHUNKY"},
    {ICAP_ROTATION, "ICAP_ROTATION"},
    {ICAP_SUPPORTEDSIZES, "ICAP_SUPPORTEDSIZES"},
    {ICAP_THRESHOLD, "ICAP_THRESHOLD"},
    {ICAP_XSCALING, "ICAP_XSCALING"},
    {ICAP_YSCALING, "ICAP_YSCALING"},
    {ICAP_BITORDERCODES, "ICAP_BITORDERCODES"},
    {ICAP_PIXELFLAVORCODES, "ICAP_PIXELFLAVORCODES"},
    {ICAP_JPEGPIXELTYPE, "ICAP_JPEGPIXELTYPE"},
    {ICAP_TIMEFILL, "ICAP_TIMEFILL"},
    {ICAP_BITDEPTH, "ICAP_BITDEPTH"},
    {ICAP_BITDEPTHREDUCTION, "ICAP_BITDEPTHREDUCTION"},
    {ICAP_UNDEFINEDIMAGESIZE, "ICAP_UNDEFINEDIMAGESIZE"},
    {ICAP_IMAGEDATASET, "ICAP_IMAGEDATASET"},
    {ICAP_EXTIMAGEINFO, "ICAP_EXTIMAGEINFO"},
    {ICAP_MINIMUMHEIGHT, "ICAP_MINIMUMHEIGHT"},
    {ICAP_MINIMUMWIDTH, "ICAP_MINIMUMWIDTH"},
    {ICAP_FLIPROTATION, "ICAP_FLIPROTATION"},
    {ICAP_BARCODEDETECTIONENABLED, "ICAP_BARCODEDETECTIONENABLED"},
    {ICAP_SUPPORTEDBARCODETYPES, "ICAP_SUPPORTEDBARCODETYPES"},
    {ICAP_BARCODEMAXSEARCHPRIORITIES, "ICAP_BARCODEMAXSEARCHPRIORITIES"},
    {ICAP_BARCODESEARCHPRIORITIES, "ICAP_BARCODESEARCHPRIORITIES"},
    {ICAP_BARCODESEARCHMODE, "ICAP_BARCODESEARCHMODE"},
    {ICAP_BARCODEMAXRETRIES, "ICAP_BARCODEMAXRETRIES"},
    {ICAP_BARCODETIMEOUT, "ICAP_BARCODETIMEOUT"},
    {ICAP_ZOOMFACTOR, "ICAP_ZOOMFACTOR"},
    {ICAP_PATCHCODEDETECTIONENABLED, "ICAP_PATCHCODEDETECTIONENABLED"},
    {ICAP_SUPPORTEDPATCHCODETYPES, "ICAP_SUPPORTEDPATCHCODETYPES"},
    {ICAP_PATCHCODEMAXSEARCHPRIORITIES, "ICAP_PATCHCODEMAXSEARCHPRIORITIES"},
    {ICAP_PATCHCODESEARCHPRIORITIES, "ICAP_PATCHCODESEARCHPRIORITIES"},
    {ICAP_PATCHCODESEARCHMODE, "ICAP_PATCHCODESEARCHMODE"},
    {ICAP_PATCHCODEMAXRETRIES, "ICAP_PATCHCODEMAXRETRIES"},
    {ICAP_PATCHCODETIMEOUT, "ICAP_PATCHCODETIMEOUT"},
    {ICAP_FLASHUSED2, "ICAP_FLASHUSED2"},
    {ICAP_IMAGEFILTER, "ICAP_IMAGEFILTER"},
    {ICAP_NOISEFILTER, "ICAP_NOISEFILTER"},
    {ICAP_OVERSCAN, "ICAP_OVERSCAN"},
    {ICAP_AUTOMATICBORDERDETECTION, "ICAP_AUTOMATICBORDERDETECTION"},
    {ICAP_AUTOMATICDESKEW, "ICAP_AUTOMATICDESKEW"},
    {ICAP_AUTOMATICROTATE, "ICAP_AUTOMATICROTATE"},
    {ICAP_JPEGQUALITY, "ICAP_JPEGQUALITY"},
    {ACAP_AUDIOFILEFORMAT, "ACAP_AUDIOFILEFORMAT"},
    {ACAP_XFERMECH, "ACAP_XFERMECH"},
    {0, 0}
  };

  for(int x = 0; 0 != capMap[x].pszString; ++x)
  {
    if(_unCap == capMap[x].unCap)
    {
      return capMap[x].pszString;
    }
  }
  
  return "Unknown capability";
}

const char* convertICAP_Item_toString(const TW_UINT16 _unCap, const TW_UINT16 _unItem)
{
  const char* pszString = "Unknown capability";

  switch(_unCap)
  {
    case ICAP_XFERMECH:
      pszString = convertICAP_XFERMECH_toString(_unItem);
      break;

    case ICAP_IMAGEFILEFORMAT:
      pszString = convertICAP_IMAGEFILEFORMAT_toString(_unItem);
      break;

    case ICAP_COMPRESSION:
      pszString = convertICAP_COMPRESSION_toString(_unItem);
      break;

    case ICAP_UNITS:
      pszString = convertICAP_UNITS_toString(_unItem);
      break;

    case ICAP_PIXELTYPE:
      pszString = convertICAP_PIXELTYPE_toString(_unItem);
      break;

    case ICAP_BITDEPTH:
      {
      char *buff = nextTempBuffer();
      SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "%u", _unItem);
      pszString = buff;
      }
      break;

  }

  return pszString;
}

//////////////////////////////////////////////////////////////////////////////
const char* convertICAP_XFERMECH_toString(const TW_UINT16 _unItem)
{
  static struct {
    TW_UINT16 unItem;
    char* pszString;
  } capMap[] = 
  {
    {TWSX_NATIVE, "TWSX_NATIVE"},
    {TWSX_FILE, "TWSX_FILE"},
    {TWSX_MEMORY, "TWSX_MEMORY"},
    {0, 0},
  };
  
  for(int x = 0; 0 != capMap[x].pszString; ++x)
  {
    if(_unItem == capMap[x].unItem)
    {
      return capMap[x].pszString;
    }
  }
  
  return "Unsupported Value";
}

//////////////////////////////////////////////////////////////////////////////
const char* convertICAP_UNITS_toString(const TW_UINT16 _unItem)
{
  static struct {
    TW_UINT16 unItem;
    char* pszString;
  } capMap[] = 
  {
    {TWUN_INCHES, "TWUN_INCHES"},
    {TWUN_CENTIMETERS, "TWUN_CENTIMETERS"},
    {TWUN_PICAS, "TWUN_PICAS"},
    {TWUN_POINTS, "TWUN_POINTS"},
    {TWUN_TWIPS, "TWUN_TWIPS"},
    {TWUN_PIXELS, "TWUN_PIXELS"},
    {0, 0}
  };
  
  for(int x = 0; 0 != capMap[x].pszString; ++x)
  {
    if(_unItem == capMap[x].unItem)
    {
      return capMap[x].pszString;
    }
  }
  
  return "Unsupported Value";
}

//////////////////////////////////////////////////////////////////////////////
const char* convertTWTY_toString(const TW_UINT16 _unItem)
{
  static struct {
    TW_UINT16 unItem;
    char* pszString;
  } capMap[] = 
  {
    {TWTY_INT8, "TWTY_INT8"},
    {TWTY_INT16, "TWTY_INT16"},
    {TWTY_INT32, "TWTY_INT32"},
    {TWTY_UINT8, "TWTY_UINT8"},
    {TWTY_UINT16, "TWTY_UINT16"},
    {TWTY_UINT32, "TWTY_UINT32"},
    {TWTY_BOOL, "TWTY_BOOL"},
    {TWTY_FIX32, "TWTY_FIX32"},
    {TWTY_FRAME, "TWTY_FRAME"},
    {TWTY_STR32, "TWTY_STR32"},
    {TWTY_STR64, "TWTY_STR64"},
    {TWTY_STR128, "TWTY_STR128"},
    {TWTY_STR255, "TWTY_STR255"},
    {TWTY_STR1024, "TWTY_STR1024"},
    {TWTY_UNI512, "TWTY_UNI512"},
    {0, 0}
  };

  for(int x = 0; 0 != capMap[x].pszString; ++x)
  {
    if(_unItem == capMap[x].unItem)
    {
      return capMap[x].pszString;
    }
  }
  
  return "Unsupported Value";
}

//////////////////////////////////////////////////////////////////////////////
const char* convertICAP_PIXELTYPE_toString(const TW_UINT16 _unItem)
{
  static struct {
    TW_UINT16 unItem;
    char* pszString;
  } capMap[] = 
  {
    {TWPT_BW, "TWPT_BW"},
    {TWPT_GRAY, "TWPT_GRAY"},
    {TWPT_RGB, "TWPT_RGB"},
    {TWPT_PALETTE, "TWPT_PALETTE"},
    {TWPT_CMY, "TWPT_CMY"},
    {TWPT_CMYK, "TWPT_CMYK"},
    {TWPT_YUV, "TWPT_YUV"},
    {TWPT_YUVK, "TWPT_YUVK"},
    {TWPT_CIEXYZ, "TWPT_CIEXYZ"},
    {0, 0}
  };

  for(int x = 0; 0 != capMap[x].pszString; ++x)
  {
    if(_unItem == capMap[x].unItem)
    {
      return capMap[x].pszString;
    }
  }
  
  return "Unsupported Value";
}

//////////////////////////////////////////////////////////////////////////////
const char* convertICAP_IMAGEFILEFORMAT_toString(const TW_UINT16 _unItem)
{
  static struct {
    TW_UINT16 unItem;
    char* pszString;
  } capMap[] = 
  {
    {TWFF_TIFF,".tiff"},
    {TWFF_PICT,".pict"},
    {TWFF_BMP,".bmp"},
    {TWFF_XBM,".xbm"},
    {TWFF_JFIF,".jfif"},
    {TWFF_FPX,".fpx"},
    {TWFF_TIFFMULTI,".tiff"},
    {TWFF_PNG,".png"},
    {TWFF_SPIFF,".spiff"},
    {TWFF_EXIF,".exif"},
    {0, 0}
  };

  for(int x = 0; 0 != capMap[x].pszString; ++x)
  {
    if(_unItem == capMap[x].unItem)
    {
      return capMap[x].pszString;
    }
  }
  
  return "Unsupported Value";
}

//////////////////////////////////////////////////////////////////////////////
const char* convertICAP_COMPRESSION_toString(const TW_UINT16 _unItem)
{
  static struct {
    TW_UINT16 unItem;
    char* pszString;
  } capMap[] = 
  {
    {TWCP_NONE,"TWCP_NONE"},
    {TWCP_PACKBITS,"TWCP_PACKBITS"},
    {TWCP_GROUP31D,"TWCP_GROUP31D"},
    {TWCP_GROUP31DEOL,"TWCP_GROUP31DEOL"},
    {TWCP_GROUP32D,"TWCP_GROUP32D"},
    {TWCP_GROUP4,"TWCP_GROUP4"},
    {TWCP_JPEG,"TWCP_JPEG"},
    {TWCP_LZW,"TWCP_LZW"},
    {TWCP_JBIG,"TWCP_JBIG"},
    {TWCP_PNG,"TWCP_PNG"},
    {TWCP_RLE4,"TWCP_RLE4"},
    {TWCP_RLE8,"TWCP_RLE8"},
    {TWCP_BITFIELDS,"TWCP_BITFIELDS"},
    {0, 0}
  };

  for(int x = 0; 0 != capMap[x].pszString; ++x)
  {
    if(_unItem == capMap[x].unItem)
    {
      return capMap[x].pszString;
    }
  }
  
  return "Unsupported Value";
}

//////////////////////////////////////////////////////////////////////////////
