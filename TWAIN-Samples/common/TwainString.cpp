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
 * @file TwainString.cpp
 * Convert TWAIN Constants to Strings
 * @author JFL Peripheral Solutions Inc.
 * @date Dec 2008
 */

#include "TwainString.h"

extern TW_HANDLE _DSM_Alloc(TW_UINT32 _size);
extern void _DSM_Free(TW_HANDLE _hMemory);
extern TW_MEMREF _DSM_LockMemory(TW_HANDLE _hMemory);
extern void _DSM_UnlockMemory(TW_HANDLE _hMemory);

/** the size of the temp buffer */
#define TEMPBUFSIZE 1024

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
  szTempBuffer[nTempBuffer][0] = '\0';
  return szTempBuffer[nTempBuffer];
}


//////////////////////////////////////////////////////////////////////////////
const char* convertCAP_Item_toString(const TW_UINT16 _unCap, const TW_UINT32 _unItem, const TW_UINT16 _unType)
{
  const char* pszString = "Unknown capability";

  switch(_unCap)
  {
    case CAP_ALARMS:
      pszString = convertCAP_ALARMS_toString((TW_UINT16)_unItem);
      break;

    case ICAP_AUTOSIZE:
      pszString = convertICAP_AUTOSIZE_toString((TW_UINT16)_unItem);
      break;

    case ICAP_COMPRESSION:
      pszString = convertICAP_COMPRESSION_toString((TW_UINT16)_unItem);
      break;

    case ICAP_BARCODESEARCHMODE:
      pszString = convertICAP_BARCODESEARCHMODE_toString((TW_UINT16)_unItem);
      break;

    case ICAP_BITORDER:
      pszString = convertICAP_BITORDER_toString((TW_UINT16)_unItem);
      break;

    case ICAP_AUTODISCARDBLANKPAGES:
      pszString = convertICAP_AUTODISCARDBLANKPAGES_toString((TW_INT32)_unItem);
      break;

    case ICAP_BITDEPTH:
      {
      char *buff = nextTempBuffer();
      SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "%u", _unItem);
      pszString = buff;
      }
      break;

    case ICAP_BITDEPTHREDUCTION:
      pszString = convertICAP_BITDEPTHREDUCTION_toString((TW_UINT16)_unItem);
      break;

    case ICAP_SUPPORTEDBARCODETYPES:
      pszString = convertICAP_SUPPORTEDBARCODETYPES_toString((TW_UINT16)_unItem);
      break;

    case CAP_CAMERASIDE:
      pszString = convertCAP_CAMERASIDE_toString((TW_UINT16)_unItem);
      break;

    case CAP_CLEARBUFFERS:
      pszString = convertCAP_CLEARBUFFERS_toString((TW_UINT16)_unItem);
      break;

    case CAP_DEVICEEVENT:
      pszString = convertCAP_DEVICEEVENT_toString((TW_UINT16)_unItem);
      break;

    case CAP_DUPLEX:
      pszString = convertCAP_DUPLEX_toString((TW_UINT16)_unItem);
      break;

    case CAP_EXTENDEDCAPS:
      pszString = convertCAP_toString((TW_UINT16)_unItem);
      break;

    case CAP_FEEDERALIGNMENT:
      pszString = convertCAP_FEEDERALIGNMENT_toString((TW_UINT16)_unItem);
      break;

    case ICAP_FEEDERTYPE:
      pszString = convertICAP_FEEDERTYPE_toString((TW_UINT16)_unItem);
      break;

    case ICAP_IMAGEFILEFORMAT:
      pszString = convertICAP_IMAGEFILEFORMAT_toString((TW_UINT16)_unItem);
      break;

    case ICAP_FLASHUSED2:
      pszString = convertICAP_FLASHUSED2_toString((TW_UINT16)_unItem);
      break;

    case CAP_FEEDERORDER:
      pszString = convertCAP_FEEDERORDER_toString((TW_UINT16)_unItem);
      break;

    case CAP_FEEDERPOCKET:
      pszString = convertCAP_FEEDERPOCKET_toString((TW_UINT16)_unItem);
      break;

    case ICAP_FLIPROTATION:
      pszString = convertICAP_FLIPROTATION_toString((TW_UINT16)_unItem);
      break;

    case ICAP_FILTER:
      pszString = convertICAP_FILTER_toString((TW_UINT16)_unItem);
      break;

    case ICAP_ICCPROFILE:
      pszString = convertICAP_ICCPROFILE_toString((TW_UINT16)_unItem);
      break;

    case ICAP_IMAGEFILTER:
      pszString = convertICAP_IMAGEFILTER_toString((TW_UINT16)_unItem);
      break;

    case ICAP_IMAGEMERGE:
      pszString = convertICAP_IMAGEMERGE_toString((TW_UINT16)_unItem);
      break;

    case CAP_JOBCONTROL:
      pszString = convertCAP_JOBCONTROL_toString((TW_UINT16)_unItem);
      break;

    case ICAP_JPEGQUALITY:
      pszString = convertICAP_JPEGQUALITY_toString((TW_UINT16)_unItem);
      break;

    case ICAP_LIGHTPATH:
      pszString = convertICAP_LIGHTPATH_toString((TW_UINT16)_unItem);
      break;

    case ICAP_LIGHTSOURCE:
      pszString = convertICAP_LIGHTSOURCE_toString((TW_UINT16)_unItem);
      break;

    case ICAP_NOISEFILTER:
      pszString = convertICAP_NOISEFILTER_toString((TW_UINT16)_unItem);
      break;

    case ICAP_ORIENTATION:
      pszString = convertICAP_ORIENTATION_toString((TW_UINT16)_unItem);
      break;

    case ICAP_OVERSCAN:
      pszString = convertICAP_OVERSCAN_toString((TW_UINT16)_unItem);
      break;

    case ICAP_PLANARCHUNKY:
      pszString = convertICAP_PLANARCHUNKY_toString((TW_UINT16)_unItem);
      break;

    case ICAP_PIXELFLAVOR:
      pszString = convertICAP_PIXELFLAVOR_toString((TW_UINT16)_unItem);
      break;

    case CAP_PRINTERMODE:
      pszString = convertCAP_PRINTERMODE_toString((TW_UINT16)_unItem);
      break;

    case CAP_PRINTER:
      pszString = convertCAP_PRINTER_toString((TW_UINT16)_unItem);
      break;

    case CAP_POWERSUPPLY:
      pszString = convertCAP_POWERSUPPLY_toString((TW_UINT16)_unItem);
      break;

    case ICAP_PIXELTYPE:
      pszString = convertICAP_PIXELTYPE_toString((TW_UINT16)_unItem);
      break;

    case CAP_SEGMENTED:
      pszString = convertCAP_SEGMENTED_toString((TW_UINT16)_unItem);
      break;

    case ICAP_SUPPORTEDEXTIMAGEINFO:
      pszString = convertExtImageInfoName_toString((TW_UINT16)_unItem);
      break;

    case ICAP_SUPPORTEDSIZES:
      pszString = convertICAP_SUPPORTEDSIZES_toString((TW_UINT16)_unItem);
      break;

    case ICAP_XFERMECH:
      pszString = convertICAP_XFERMECH_toString((TW_UINT16)_unItem);
      break;

    case ICAP_UNITS:
      pszString = convertICAP_UNITS_toString((TW_UINT16)_unItem);
      break;

    default:
      {
      char *buff = nextTempBuffer();
      switch(_unType)
      {
        case TWTY_UINT8:
        case TWTY_UINT16:
        case TWTY_UINT32:
          SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "%u [0x%x]", _unItem, _unItem);
          break;

        case TWTY_INT8:
        case TWTY_INT16:
        case TWTY_INT32:
          SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "%d", _unItem);
          break;

        case TWTY_BOOL:
          SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "%s", (TW_BOOL)_unItem? "True" : "False");
          break;
      }
      pszString = buff;
      }
      break;
  }

  return pszString;
}

//////////////////////////////////////////////////////////////////////////////
const char* convertEI_Item_toString(const TW_UINT16 _unEI, const TW_UINT32 _unItem)
{
  const char* pszString = NULL;
  TW_UINT16 twItem = (TW_UINT16)_unItem;
  switch(_unEI)
  {
    case TWEI_BARCODEROTATION:
      pszString = convertTWEI_BARCODEROTATION_toString((TW_UINT16)_unItem);
      break;

    case TWEI_BARCODETYPE:
      pszString = CapabilityValueToString(ICAP_SUPPORTEDBARCODETYPES, TWTY_UINT16, &twItem);//convertICAP_SUPPORTEDBARCODETYPES_toString((TW_UINT16)_unItem);
      break;

    case TWEI_PAGESIDE: //CAP_CAMERASIDE
      pszString = CapabilityValueToString(CAP_CAMERASIDE, TWTY_UINT16, &twItem);//convertTWEI_PAGESIDE_toString((TW_UINT16)_unItem);
      break;

    case TWEI_DESKEWSTATUS:
      pszString = convertTWEI_DESKEWSTATUS_toString((TW_UINT16)_unItem);
      break;

    case TWEI_FILESYSTEMSOURCE:
      pszString = convertFileType_toString((TW_UINT16)_unItem);
      break;

    case TWEI_PIXELFLAVOR:
      pszString = CapabilityValueToString(ICAP_PIXELFLAVOR, TWTY_UINT16, &twItem);//convertICAP_PIXELFLAVOR_toString((TW_UINT16)_unItem);
      break;

    case TWEI_MAGTYPE:
      pszString = convertTWEI_MAGTYPE_toString((TW_UINT16)_unItem);
      break;

    case TWEI_PATCHCODE:
      pszString = convertTWEI_PATCHCODE_toString((TW_UINT16)_unItem);
      break;
  }

  return pszString;
}


//////////////////////////////////////////////////////////////////////////////
const char* convertCAP_toString(const TW_UINT16 _unCap)
{
  const char* text;

  switch(_unCap)
  {
  case ICAP_AUTODISCARDBLANKPAGES:
    text = "ICAP_AUTODISCARDBLANKPAGES";
    break;
  case CAP_CUSTOMBASE:
    text = "CAP_CUSTOMBASE";
    break;

  case CAP_XFERCOUNT:
    text = "CAP_XFERCOUNT";
    break;

  case ICAP_COMPRESSION:
    text = "ICAP_COMPRESSION";
    break;

  case ICAP_PIXELTYPE:
    text = "ICAP_PIXELTYPE";
    break;

  case ICAP_UNITS:
    text = "ICAP_UNITS";
    break;

  case ICAP_XFERMECH:
    text = "ICAP_XFERMECH";
    break;

  case CAP_AUTHOR:
    text = "CAP_AUTHOR";
    break;

  case CAP_CAPTION:
    text = "CAP_CAPTION";
    break;

  case CAP_FEEDERENABLED:
    text = "CAP_FEEDERENABLED";
    break;

  case CAP_FEEDERLOADED:
    text = "CAP_FEEDERLOADED";
    break;

  case CAP_TIMEDATE:
    text = "CAP_TIMEDATE";
    break;

  case CAP_SUPPORTEDCAPS:
    text = "CAP_SUPPORTEDCAPS";
    break;

  case CAP_EXTENDEDCAPS:
    text = "CAP_EXTENDEDCAPS";
    break;

  case CAP_AUTOFEED:
    text = "CAP_AUTOFEED";
    break;

  case CAP_CLEARPAGE:
    text = "CAP_CLEARPAGE";
    break;

  case CAP_FEEDPAGE:
    text = "CAP_FEEDPAGE";
    break;

  case CAP_REWINDPAGE:
    text = "CAP_REWINDPAGE";
    break;

  case CAP_INDICATORS:
    text = "CAP_INDICATORS";
    break;

  case CAP_SUPPORTEDCAPSEXT:
    text = "CAP_SUPPORTEDCAPSEXT";
    break;

  case CAP_PAPERDETECTABLE:
    text = "CAP_PAPERDETECTABLE";
    break;

  case CAP_UICONTROLLABLE:
    text = "CAP_UICONTROLLABLE";
    break;

  case CAP_DEVICEONLINE:
    text = "CAP_DEVICEONLINE";
    break;

  case CAP_AUTOSCAN:
    text = "CAP_AUTOSCAN";
    break;

  case CAP_THUMBNAILSENABLED:
    text = "CAP_THUMBNAILSENABLED";
    break;

  case CAP_DUPLEX:
    text = "CAP_DUPLEX";
    break;

  case CAP_DUPLEXENABLED:
    text = "CAP_DUPLEXENABLED";
    break;

  case CAP_ENABLEDSUIONLY:
    text = "CAP_ENABLEDSUIONLY";
    break;

  case CAP_CUSTOMDSDATA:
    text = "CAP_CUSTOMDSDATA";
    break;

  case CAP_ENDORSER:
    text = "CAP_ENDORSER";
    break;

  case CAP_JOBCONTROL:
    text = "CAP_JOBCONTROL";
    break;

  case CAP_ALARMS:
    text = "CAP_ALARMS";
    break;

  case CAP_ALARMVOLUME:
    text = "CAP_ALARMVOLUME";
    break;

  case CAP_AUTOMATICCAPTURE:
    text = "CAP_AUTOMATICCAPTURE";
    break;

  case CAP_TIMEBEFOREFIRSTCAPTURE:
    text = "CAP_TIMEBEFOREFIRSTCAPTURE";
    break;

  case CAP_TIMEBETWEENCAPTURES:
    text = "CAP_TIMEBETWEENCAPTURES";
    break;

  case CAP_CLEARBUFFERS:
    text = "CAP_CLEARBUFFERS";
    break;

  case CAP_MAXBATCHBUFFERS:
    text = "CAP_MAXBATCHBUFFERS";
    break;

  case CAP_DEVICETIMEDATE:
    text = "CAP_DEVICETIMEDATE";
    break;

  case CAP_POWERSUPPLY:
    text = "CAP_POWERSUPPLY";
    break;

  case CAP_CAMERAPREVIEWUI:
    text = "CAP_CAMERAPREVIEWUI";
    break;

  case CAP_DEVICEEVENT:
    text = "CAP_DEVICEEVENT";
    break;

  case CAP_SERIALNUMBER:
    text = "CAP_SERIALNUMBER";
    break;

  case CAP_PRINTER:
    text = "CAP_PRINTER";
    break;

  case CAP_PRINTERENABLED:
    text = "CAP_PRINTERENABLED";
    break;

  case CAP_PRINTERINDEX:
    text = "CAP_PRINTERINDEX";
    break;

  case CAP_PRINTERMODE:
    text = "CAP_PRINTERMODE";
    break;

  case CAP_PRINTERSTRING:
    text = "CAP_PRINTERSTRING";
    break;

  case CAP_PRINTERSUFFIX:
    text = "CAP_PRINTERSUFFIX";
    break;

  case CAP_LANGUAGE:
    text = "CAP_LANGUAGE";
    break;

  case CAP_FEEDERALIGNMENT:
    text = "CAP_FEEDERALIGNMENT";
    break;

  case CAP_FEEDERORDER:
    text = "CAP_FEEDERORDER";
    break;

  case CAP_REACQUIREALLOWED:
    text = "CAP_REACQUIREALLOWED";
    break;

  case CAP_BATTERYMINUTES:
    text = "CAP_BATTERYMINUTES";
    break;

  case CAP_BATTERYPERCENTAGE:
    text = "CAP_BATTERYPERCENTAGE";
    break;

  case CAP_CAMERASIDE:
    text = "CAP_CAMERASIDE";
    break;

  case CAP_SEGMENTED:
    text = "CAP_SEGMENTED";
    break;

  case CAP_CAMERAENABLED:
    text = "CAP_CAMERAENABLED";
    break;

  case CAP_CAMERAORDER:
    text = "CAP_CAMERAORDER";
    break;

  case CAP_MICRENABLED:
    text = "CAP_MICRENABLED";
    break;

  case CAP_FEEDERPREP:
    text = "CAP_FEEDERPREP";
    break;

  case CAP_FEEDERPOCKET:
    text = "CAP_FEEDERPOCKET";
    break;

  case CAP_AUTOMATICSENSEMEDIUM:
    text = "CAP_AUTOMATICSENSEMEDIUM";
    break;

  case CAP_CUSTOMINTERFACEGUID:
    text = "CAP_CUSTOMINTERFACEGUID";
    break;

  case ICAP_AUTOBRIGHT:
    text = "ICAP_AUTOBRIGHT";
    break;

  case ICAP_BRIGHTNESS:
    text = "ICAP_BRIGHTNESS";
    break;

  case ICAP_CONTRAST:
    text = "ICAP_CONTRAST";
    break;

  case ICAP_CUSTHALFTONE:
    text = "ICAP_CUSTHALFTONE";
    break;

  case ICAP_EXPOSURETIME:
    text = "ICAP_EXPOSURETIME";
    break;

  case ICAP_FILTER:
    text = "ICAP_FILTER";
    break;

  case ICAP_FLASHUSED:
    text = "ICAP_FLASHUSED";
    break;

  case ICAP_GAMMA:
    text = "ICAP_GAMMA";
    break;

  case ICAP_HALFTONES:
    text = "ICAP_HALFTONES";
    break;

  case ICAP_HIGHLIGHT:
    text = "ICAP_HIGHLIGHT";
    break;

  case ICAP_IMAGEFILEFORMAT:
    text = "ICAP_IMAGEFILEFORMAT";
    break;

  case ICAP_LAMPSTATE:
    text = "ICAP_LAMPSTATE";
    break;

  case ICAP_LIGHTSOURCE:
    text = "ICAP_LIGHTSOURCE";
    break;

  case ICAP_ORIENTATION:
    text = "ICAP_ORIENTATION";
    break;

  case ICAP_PHYSICALWIDTH:
    text = "ICAP_PHYSICALWIDTH";
    break;

  case ICAP_PHYSICALHEIGHT:
    text = "ICAP_PHYSICALHEIGHT";
    break;

  case ICAP_SHADOW:
    text = "ICAP_SHADOW";
    break;

  case ICAP_FRAMES:
    text = "ICAP_FRAMES";
    break;

  case ICAP_XNATIVERESOLUTION:
    text = "ICAP_XNATIVERESOLUTION";
    break;

  case ICAP_YNATIVERESOLUTION:
    text = "ICAP_YNATIVERESOLUTION";
    break;

  case ICAP_XRESOLUTION:
    text = "ICAP_XRESOLUTION";
    break;

  case ICAP_YRESOLUTION:
    text = "ICAP_YRESOLUTION";
    break;

  case ICAP_MAXFRAMES:
    text = "ICAP_MAXFRAMES";
    break;

  case ICAP_TILES:
    text = "ICAP_TILES";
    break;

  case ICAP_BITORDER:
    text = "ICAP_BITORDER";
    break;

  case ICAP_CCITTKFACTOR:
    text = "ICAP_CCITTKFACTOR";
    break;

  case ICAP_LIGHTPATH:
    text = "ICAP_LIGHTPATH";
    break;

  case ICAP_PIXELFLAVOR:
    text = "ICAP_PIXELFLAVOR";
    break;

  case ICAP_PLANARCHUNKY:
    text = "ICAP_PLANARCHUNKY";
    break;

  case ICAP_ROTATION:
    text = "ICAP_ROTATION";
    break;

  case ICAP_SUPPORTEDSIZES:
    text = "ICAP_SUPPORTEDSIZES";
    break;

  case ICAP_THRESHOLD:
    text = "ICAP_THRESHOLD";
    break;

  case ICAP_XSCALING:
    text = "ICAP_XSCALING";
    break;

  case ICAP_YSCALING:
    text = "ICAP_YSCALING";
    break;

  case ICAP_BITORDERCODES:
    text = "ICAP_BITORDERCODES";
    break;

  case ICAP_PIXELFLAVORCODES:
    text = "ICAP_PIXELFLAVORCODES";
    break;

  case ICAP_JPEGPIXELTYPE:
    text = "ICAP_JPEGPIXELTYPE";
    break;

  case ICAP_TIMEFILL:
    text = "ICAP_TIMEFILL";
    break;

  case ICAP_BITDEPTH:
    text = "ICAP_BITDEPTH";
    break;

  case ICAP_BITDEPTHREDUCTION:
    text = "ICAP_BITDEPTHREDUCTION";
    break;

  case ICAP_UNDEFINEDIMAGESIZE:
    text = "ICAP_UNDEFINEDIMAGESIZE";
    break;

  case ICAP_IMAGEDATASET:
    text = "ICAP_IMAGEDATASET";
    break;

  case ICAP_EXTIMAGEINFO:
    text = "ICAP_EXTIMAGEINFO";
    break;

  case ICAP_MINIMUMHEIGHT:
    text = "ICAP_MINIMUMHEIGHT";
    break;

  case ICAP_MINIMUMWIDTH:
    text = "ICAP_MINIMUMWIDTH";
    break;

  case ICAP_FLIPROTATION:
    text = "ICAP_FLIPROTATION";
    break;

  case ICAP_BARCODEDETECTIONENABLED:
    text = "ICAP_BARCODEDETECTIONENABLED";
    break;

  case ICAP_SUPPORTEDBARCODETYPES:
    text = "ICAP_SUPPORTEDBARCODETYPES";
    break;

  case ICAP_BARCODEMAXSEARCHPRIORITIES:
    text = "ICAP_BARCODEMAXSEARCHPRIORITIES";
    break;

  case ICAP_BARCODESEARCHPRIORITIES:
    text = "ICAP_BARCODESEARCHPRIORITIES";
    break;

  case ICAP_BARCODESEARCHMODE:
    text = "ICAP_BARCODESEARCHMODE";
    break;

  case ICAP_BARCODEMAXRETRIES:
    text = "ICAP_BARCODEMAXRETRIES";
    break;

  case ICAP_BARCODETIMEOUT:
    text = "ICAP_BARCODETIMEOUT";
    break;

  case ICAP_ZOOMFACTOR:
    text = "ICAP_ZOOMFACTOR";
    break;

  case ICAP_PATCHCODEDETECTIONENABLED:
    text = "ICAP_PATCHCODEDETECTIONENABLED";
    break;

  case ICAP_SUPPORTEDPATCHCODETYPES:
    text = "ICAP_SUPPORTEDPATCHCODETYPES";
    break;

  case ICAP_PATCHCODEMAXSEARCHPRIORITIES:
    text = "ICAP_PATCHCODEMAXSEARCHPRIORITIES";
    break;

  case ICAP_PATCHCODESEARCHPRIORITIES:
    text = "ICAP_PATCHCODESEARCHPRIORITIES";
    break;

  case ICAP_PATCHCODESEARCHMODE:
    text = "ICAP_PATCHCODESEARCHMODE";
    break;

  case ICAP_PATCHCODEMAXRETRIES:
    text = "ICAP_PATCHCODEMAXRETRIES";
    break;

  case ICAP_PATCHCODETIMEOUT:
    text = "ICAP_PATCHCODETIMEOUT";
    break;

  case ICAP_FLASHUSED2:
    text = "ICAP_FLASHUSED2";
    break;

  case ICAP_IMAGEFILTER:
    text = "ICAP_IMAGEFILTER";
    break;

  case ICAP_NOISEFILTER:
    text = "ICAP_NOISEFILTER";
    break;

  case ICAP_OVERSCAN:
    text = "ICAP_OVERSCAN";
    break;

  case ICAP_AUTOMATICBORDERDETECTION:
    text = "ICAP_AUTOMATICBORDERDETECTION";
    break;

  case ICAP_AUTOMATICDESKEW:
    text = "ICAP_AUTOMATICDESKEW";
    break;

  case ICAP_AUTOMATICROTATE:
    text = "ICAP_AUTOMATICROTATE";
    break;

  case ICAP_JPEGQUALITY:
    text = "ICAP_JPEGQUALITY";
    break;

  case ICAP_FEEDERTYPE:
    text = "ICAP_FEEDERTYPE";
    break;

  case ICAP_ICCPROFILE:
    text = "ICAP_ICCPROFILE";
    break;

  case ICAP_AUTOSIZE:
    text = "ICAP_AUTOSIZE";
    break;

    case ICAP_AUTOMATICCROPUSESFRAME:
    text = "ICAP_AUTOMATICCROPUSESFRAME";
    break;

  case ICAP_AUTOMATICLENGTHDETECTION:
    text = "ICAP_AUTOMATICLENGTHDETECTION";
    break;

  case ICAP_AUTOMATICCOLORENABLED:
    text = "ICAP_AUTOMATICCOLORENABLED";
    break;

  case ICAP_AUTOMATICCOLORNONCOLORPIXELTYPE:
    text = "ICAP_AUTOMATICCOLORNONCOLORPIXELTYPE";
    break;

  case ICAP_COLORMANAGEMENTENABLED:
    text = "ICAP_COLORMANAGEMENTENABLED";
    break;

  case ICAP_IMAGEMERGE:
    text = "ICAP_IMAGEMERGE";
    break;

  case ICAP_IMAGEMERGEHEIGHTTHRESHOLD:
    text = "ICAP_IMAGEMERGEHEIGHTTHRESHOLD";
    break;

  case ICAP_SUPPORTEDEXTIMAGEINFO:
    text = "ICAP_SUPPORTEDEXTIMAGEINFO";
    break;

  case ACAP_AUDIOFILEFORMAT:
    text = "ACAP_AUDIOFILEFORMAT";
    break;

  case ACAP_XFERMECH:
    text = "ACAP_XFERMECH";
    break;

  default:
    {
      char * buff = nextTempBuffer();
      if(_unCap < CAP_CUSTOMBASE)
      {
        SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "Unknown CAP 0x:%04X", _unCap);
      }
      else if (_unCap > CAP_CUSTOMBASE)
      {
        SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "Custom CAP 0x:%04X", _unCap);
      }
      else // == CAP_CUSTOMBASE
      {
        SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "Invalid CAP 0x:%04X", _unCap);
      }
      text = buff;
    }
    break;
  }

  return text;
}



//////////////////////////////////////////////////////////////////////////////
const char* convertTWTY_toString(const TW_UINT16 _unItem)
{
  const char* text;

  switch(_unItem)
  {
  case TWTY_INT8:
    text = "TWTY_INT8";
    break;

  case TWTY_INT16:
    text = "TWTY_INT16";
    break;

  case TWTY_INT32:
    text = "TWTY_INT32";
    break;

  case TWTY_UINT8:
    text = "TWTY_UINT8";
    break;

  case TWTY_UINT16:
    text = "TWTY_UINT16";
    break;

  case TWTY_UINT32:
    text = "TWTY_UINT32";
    break;

  case TWTY_BOOL:
    text = "TWTY_BOOL";
    break;

  case TWTY_FIX32:
    text = "TWTY_FIX32";
    break;

  case TWTY_FRAME:
    text = "TWTY_FRAME";
    break;

  case TWTY_STR32:
    text = "TWTY_STR32";
    break;

  case TWTY_STR64:
    text = "TWTY_STR64";
    break;

  case TWTY_STR128:
    text = "TWTY_STR128";
    break;

  case TWTY_STR255:
    text = "TWTY_STR255";
    break;

  case TWTY_STR1024:
    text = "TWTY_STR1024";
    break;

  case TWTY_UNI512:
    text = "TWTY_UNI512";
    break;

  case TWTY_HANDLE:
    text = "TWTY_HANDLE";
    break;    

  default:
    {
      char * buff = nextTempBuffer();
      SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "Unknown TWTY 0x:%04X", _unItem);
      text = buff;
    }
    break;
  }

  return text;
}



//////////////////////////////////////////////////////////////////////////////
// Convert Extended Image Info Attributes to String

const char* convertExtImageInfoName_toString(int InfoID)
{
  const char* text;

  switch(InfoID)
  {
  case TWEI_BARCODEX:
    text = "TWEI_BARCODEX";
    break;

  case TWEI_BARCODEY:
    text = "TWEI_BARCODEY";
    break;

	case TWEI_BARCODETEXT:
    text = "TWEI_BARCODETEXT";
    break;

	case TWEI_BARCODETYPE:
    text = "TWEI_BARCODETYPE";
    break;

	case TWEI_DESHADETOP:
    text = "TWEI_DESHADETOP";
    break;

	case TWEI_DESHADELEFT:
    text = "TWEI_DESHADELEFT";
    break;

	case TWEI_DESHADEHEIGHT:
    text = "TWEI_DESHADEHEIGHT";
    break;

	case TWEI_DESHADEWIDTH:
    text = "TWEI_DESHADEWIDTH";
    break;

	case TWEI_DESHADESIZE:
    text = "TWEI_DESHADESIZE";
    break;

	case TWEI_SPECKLESREMOVED:
    text = "TWEI_SPECKLESREMOVED";
    break;

	case TWEI_HORZLINEXCOORD:
    text = "TWEI_HORZLINEXCOORD";
    break;

	case TWEI_HORZLINEYCOORD:
    text = "TWEI_HORZLINEYCOORD";
    break;

	case TWEI_HORZLINELENGTH:
    text = "TWEI_HORZLINELENGTH";
    break;

	case TWEI_HORZLINETHICKNESS:
    text = "TWEI_HORZLINETHICKNESS";
    break;

	case TWEI_VERTLINEXCOORD:
    text = "TWEI_VERTLINEXCOORD";
    break;

	case TWEI_VERTLINEYCOORD:
    text = "TWEI_VERTLINEYCOORD";
    break;

	case TWEI_VERTLINELENGTH:
    text = "TWEI_VERTLINELENGTH";
    break;

	case TWEI_VERTLINETHICKNESS:
    text = "TWEI_VERTLINETHICKNESS";
    break;

	case TWEI_PATCHCODE:
    text = "TWEI_PATCHCODE";
    break;

	case TWEI_ENDORSEDTEXT:
    text = "TWEI_ENDORSEDTEXT";
    break;

	case TWEI_FORMCONFIDENCE:
    text = "TWEI_FORMCONFIDENCE";
    break;

	case TWEI_FORMTEMPLATEMATCH:
    text = "TWEI_FORMTEMPLATEMATCH";
    break;

	case TWEI_FORMTEMPLATEPAGEMATCH:
    text = "TWEI_FORMTEMPLATEPAGEMATCH";
    break;

	case TWEI_FORMHORZDOCOFFSET:
    text = "TWEI_FORMHORZDOCOFFSET";
    break;

	case TWEI_FORMVERTDOCOFFSET:
    text = "TWEI_FORMVERTDOCOFFSET";
    break;

	case TWEI_BARCODECOUNT:
    text = "TWEI_BARCODECOUNT";
    break;

	case TWEI_BARCODECONFIDENCE:
    text = "TWEI_BARCODECONFIDENCE";
    break;

	case TWEI_BARCODEROTATION:
    text = "TWEI_BARCODEROTATION";
    break;

	case TWEI_BARCODETEXTLENGTH:
    text = "TWEI_BARCODETEXTLENGTH";
    break;

	case TWEI_DESHADECOUNT:
    text = "TWEI_DESHADECOUNT";
    break;

	case TWEI_DESHADEBLACKCOUNTOLD:
    text = "TWEI_DESHADEBLACKCOUNTOLD";
    break;

	case TWEI_DESHADEBLACKCOUNTNEW:
    text = "TWEI_DESHADEBLACKCOUNTNEW";
    break;

	case TWEI_DESHADEBLACKRLMIN:
    text = "TWEI_DESHADEBLACKRLMIN";
    break;

	case TWEI_DESHADEBLACKRLMAX:
    text = "TWEI_DESHADEBLACKRLMAX";
    break;

	case TWEI_DESHADEWHITECOUNTOLD:
    text = "TWEI_DESHADEWHITECOUNTOLD";
    break;

	case TWEI_DESHADEWHITECOUNTNEW:
    text = "TWEI_DESHADEWHITECOUNTNEW";
    break;

	case TWEI_DESHADEWHITERLMIN:
    text = "TWEI_DESHADEWHITERLMIN";
    break;

	case TWEI_DESHADEWHITERLAVE:
    text = "TWEI_DESHADEWHITERLAVE";
    break;

	case TWEI_DESHADEWHITERLMAX:
    text = "TWEI_DESHADEWHITERLMAX";
    break;

	case TWEI_BLACKSPECKLESREMOVED:
    text = "TWEI_BLACKSPECKLESREMOVED";
    break;

	case TWEI_WHITESPECKLESREMOVED:
    text = "TWEI_WHITESPECKLESREMOVED";
    break;

	case TWEI_HORZLINECOUNT:
    text = "TWEI_HORZLINECOUNT";
    break;

	case TWEI_VERTLINECOUNT:
    text = "TWEI_VERTLINECOUNT";
    break;

	case TWEI_DESKEWSTATUS:
    text = "TWEI_DESKEWSTATUS";
    break;

	case TWEI_SKEWORIGINALANGLE:
    text = "TWEI_SKEWORIGINALANGLE";
    break;

	case TWEI_SKEWFINALANGLE:
    text = "TWEI_SKEWFINALANGLE";
    break;

	case TWEI_SKEWCONFIDENCE:
    text = "TWEI_SKEWCONFIDENCE";
    break;

	case TWEI_SKEWWINDOWX1:
    text = "TWEI_SKEWWINDOWX1";
    break;

	case TWEI_SKEWWINDOWY1:
    text = "TWEI_SKEWWINDOWY1";
    break;

	case TWEI_SKEWWINDOWX2:
    text = "TWEI_SKEWWINDOWX2";
    break;

	case TWEI_SKEWWINDOWY2:
    text = "TWEI_SKEWWINDOWY2";
    break;

	case TWEI_SKEWWINDOWX3:
    text = "TWEI_SKEWWINDOWX3";
    break;

	case TWEI_SKEWWINDOWY3:
    text = "TWEI_SKEWWINDOWY3";
    break;

	case TWEI_SKEWWINDOWX4:
    text = "TWEI_SKEWWINDOWX4";
    break;

	case TWEI_SKEWWINDOWY4:
    text = "TWEI_SKEWWINDOWY4";
    break;

	case TWEI_BOOKNAME:
    text = "TWEI_BOOKNAME";
    break;

	case TWEI_CHAPTERNUMBER:
    text = "TWEI_CHAPTERNUMBER";
    break;

	case TWEI_DOCUMENTNUMBER:
    text = "TWEI_DOCUMENTNUMBER";
    break;

	case TWEI_PAGENUMBER:
    text = "TWEI_PAGENUMBER";
    break;

	case TWEI_CAMERA:
    text = "TWEI_CAMERA";
    break;

	case TWEI_FRAMENUMBER:
    text = "TWEI_FRAMENUMBER";
    break;

	case TWEI_FRAME:
    text = "TWEI_FRAME";
    break;

	case TWEI_PIXELFLAVOR:
    text = "TWEI_PIXELFLAVOR";
    break;

	case TWEI_PAGESIDE:
    text = "TWEI_PAGESIDE";
    break;

	case TWEI_MAGDATA:
    text = "TWEI_MAGDATA";
    break;

	case TWEI_MAGTYPE:
    text = "TWEI_MAGTYPE";
    break;

  default:
    {
      char * buff = nextTempBuffer();
      SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "ExtImageInfo ID 0x:%04X",InfoID);
      text = buff;
    }
    break;
  }

  return text;
}

//////////////////////////////////////////////////////////////////////////////
// Convert CAP_ALARMS values to String
const char* convertCAP_ALARMS_toString(const TW_UINT16 InfoID)
{
  const char* text;

  switch(InfoID)
  {
	case TWAL_ALARM:
    text = "Alarm";
    break;

	case TWAL_FEEDERERROR:
    text = "Feeder Error";
    break;

	case TWAL_FEEDERWARNING:
    text = "Feeder Warning";
    break;

	case TWAL_BARCODE:
    text = "Barcode";
    break;

	case TWAL_DOUBLEFEED:
    text = "Double Feed";
    break;

	case TWAL_JAM:
    text = "Paper Jam";
    break;

	case TWAL_PATCHCODE:
    text = "Patch Code";
    break;

	case TWAL_POWER:
    text = "Power";
    break;

	case TWAL_SKEW:
    text = "Skew";
    break;

  default:
    {
      char * buff = nextTempBuffer();
      SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "Unknown TWAL 0x:%04X", InfoID);
      text = buff;
    }
    break;
  }

  return text;
}

//////////////////////////////////////////////////////////////////////////////
// Convert ICAP_AUTOSIZE values to String
const char* convertICAP_AUTOSIZE_toString(const TW_UINT16 InfoID)
{
  const char* text;

  switch(InfoID)
  {
	case TWAS_NONE:
    text = "None";
    break;

	case TWAS_AUTO:
    text = "Auto";
    break;

	case TWAS_CURRENT:
    text = "Current";
    break;

  default:
    {
      char * buff = nextTempBuffer();
      SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "Unknown TWAS 0x:%04X", InfoID);
      text = buff;
    }
    break;
  }

  return text;
}

//////////////////////////////////////////////////////////////////////////////
// Convert TWEI_BARCODEROTATION values to String
const char* convertTWEI_BARCODEROTATION_toString(const TW_UINT16 InfoID)
{
  const char* text;

  switch(InfoID)
  {
	case TWBCOR_ROT0:
    text = "TWBCOR_ROT0";
    break;

	case TWBCOR_ROT90:
    text = "TWBCOR_ROT90";
    break;

	case TWBCOR_ROT180:
    text = "TWBCOR_ROT180";
    break;

	case TWBCOR_ROT270:
    text = "TWBCOR_ROT270";
    break;

	case TWBCOR_ROTX:
    text = "TWBCOR_ROTX";
    break;

  default:
    {
      char * buff = nextTempBuffer();
      SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "Unknown TWBCOR 0x:%04X", InfoID);
      text = buff;
    }
    break;
  }

  return text;
}

//////////////////////////////////////////////////////////////////////////////
// Convert ICAP_BARCODESEARCHMODE values to String
const char* convertICAP_BARCODESEARCHMODE_toString(const TW_UINT16 InfoID)
{
  const char* text;

  switch(InfoID)
  {
	case TWBD_HORZ:
    text = "Horizontal";
    break;

	case TWBD_VERT:
    text = "Vertical";
    break;

	case TWBD_HORZVERT:
    text = "Horz Vert";
    break;

	case TWBD_VERTHORZ:
    text = "Vert Horz";
    break;

  default:
    {
      char * buff = nextTempBuffer();
      SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "Unknown TWBD 0x:%04X", InfoID);
      text = buff;
    }
    break;
  }

  return text;
}

//////////////////////////////////////////////////////////////////////////////
// Convert ICAP_BITORDER values to String
const char* convertICAP_BITORDER_toString(const TW_UINT16 InfoID)
{
  const char* text;

  switch(InfoID)
  {
	case TWBO_LSBFIRST:
    text = "LSB first";
    break;

	case TWBO_MSBFIRST:
    text = "MSB first";
    break;

  default:
    {
      char * buff = nextTempBuffer();
      SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "Unknown TWBO 0x:%04X", InfoID);
      text = buff;
    }
    break;
  }

  return text;
}

//////////////////////////////////////////////////////////////////////////////
// Convert ICAP_AUTODISCARDBLANKPAGES values to String
const char* convertICAP_AUTODISCARDBLANKPAGES_toString(const TW_INT32 InfoID)
{
  const char* text;

  switch(InfoID)
  {
  case TWBP_DISABLE:
    text = "Disable";
    break;

  case TWBP_AUTO:
    text = "Auto";
    break;

  default:
    {
      char * buff = nextTempBuffer();
      SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "Keep compressed images > %d bytes", InfoID);
      text = buff;
    }
    break;
  }

  return text;
}

//////////////////////////////////////////////////////////////////////////////
// Convert ICAP_BITDEPTHREDUCTION values to String
const char* convertICAP_BITDEPTHREDUCTION_toString(const TW_UINT16 InfoID)
{
  const char* text;

  switch(InfoID)
  {
	case TWBR_THRESHOLD:
    text = "Threshold";
    break;

	case TWBR_HALFTONE:
    text = "Halftone";
    break;

	case TWBR_CUSTHALFTONE:
    text = "Custom Halftone";
    break;

	case TWBR_DIFFUSION:
    text = "Diffusion";
    break;

  default:
    {
      char * buff = nextTempBuffer();
      SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "Unknown TWBR 0x:%04X", InfoID);
      text = buff;
    }
    break;
  }

  return text;
}

//////////////////////////////////////////////////////////////////////////////
// Convert ICAP_SUPPORTEDBARCODETYPES and TWEI_BARCODETYPE values to String
const char* convertICAP_SUPPORTEDBARCODETYPES_toString(const TW_UINT16 InfoID)
{
  const char* text;

  switch(InfoID)
  {
	case TWBT_3OF9:
    text = "3of9";
    break;

	case TWBT_2OF5INTERLEAVED:
    text = "2of5 interleaved";
    break;

	case TWBT_2OF5NONINTERLEAVED:
    text = "2of5 noninterleaved";
    break;

	case TWBT_CODE93:
    text = "code93";
    break;

	case TWBT_CODE128:
    text = "code128";
    break;

	case TWBT_UCC128:
    text = "UCC128";
    break;

	case TWBT_CODABAR:
    text = "Codebar";
    break;

	case TWBT_UPCA:
    text = "UPCA";
    break;

	case TWBT_UPCE:
    text = "UPCE";
    break;

	case TWBT_EAN8:
    text = "EAN8";
    break;

	case TWBT_EAN13:
    text = "EAN13";
    break;

	case TWBT_POSTNET:
    text = "POSTNET";
    break;

	case TWBT_PDF417:
    text = "PDF417";
    break;

	case TWBT_2OF5INDUSTRIAL:
    text = "2of5 industrial";
    break;

	case TWBT_2OF5MATRIX:
    text = "2of5 matrix";
    break;

	case TWBT_2OF5DATALOGIC:
    text = "2of5 datalogic";
    break;

	case TWBT_2OF5IATA:
    text = "2of5 IATA";
    break;

	case TWBT_3OF9FULLASCII:
    text = "3of9 fullASCII";
    break;

	case TWBT_CODABARWITHSTARTSTOP:
    text = "Codabar with start stop";
    break;

	case TWBT_MAXICODE:
    text = "MAXICODE";
    break;

  default:
    {
      char * buff = nextTempBuffer();
      SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "Unknown TWBT 0x:%04X", InfoID);
      text = buff;
    }
    break;
  }

  return text;
}

//////////////////////////////////////////////////////////////////////////////
// Convert ICAP_COMPRESSION values to String
const char* convertICAP_COMPRESSION_toString(const TW_UINT16 _unItem)
{
  const char* text;

  switch(_unItem)
  {
  case TWCP_NONE:
    text = "None";
    break;

  case TWCP_PACKBITS:
    text = "PACKBITS";
    break;

  case TWCP_GROUP31D:
    text = "GROUP31D";
    break;

  case TWCP_GROUP31DEOL:
    text = "GROUP31DEOL";
    break;

  case TWCP_GROUP32D:
    text = "GROUP32D";
    break;

  case TWCP_GROUP4:
    text = "GROUP4";
    break;

  case TWCP_JPEG:
    text = "JPEG";
    break;

  case TWCP_LZW:
    text = "LZW";
    break;

  case TWCP_JBIG:
    text = "JBIG";
    break;

  case TWCP_PNG:
    text = "PNG";
    break;

  case TWCP_RLE4:
    text = "RLE4";
    break;

  case TWCP_RLE8:
    text = "RLE8";
    break;

  case TWCP_BITFIELDS:
    text = "BITFIELDS";
    break;

  default:
    {
      char * buff = nextTempBuffer();
      SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "Unknown TWCP 0x:%04X", _unItem);
      text = buff;
    }
    break;
  }

  return text;
}


//////////////////////////////////////////////////////////////////////////////
// Convert CAP_CAMERASIDE values to String
const char* convertCAP_CAMERASIDE_toString(const TW_UINT16 InfoID)
{
  const char* text;

  switch(InfoID)
  {
	case TWCS_BOTH:
    text = "Both";
    break;

	case TWCS_TOP:
    text = "Top";
    break;

	case TWCS_BOTTOM:
    text = "Bottom";
    break;

  default:
    {
      char * buff = nextTempBuffer();
      SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "Unknown TWCS 0x:%04X", InfoID);
      text = buff;
    }
    break;
  }

  return text;
}

//////////////////////////////////////////////////////////////////////////////
// Convert TWEI_PAGESIDE values to String
const char* convertTWEI_PAGESIDE_toString(const TW_UINT16 InfoID)
{
  const char* text;

  switch(InfoID)
  {
	case TWCS_TOP:
    text = "TWCS_TOP";
    break;

	case TWCS_BOTTOM:
    text = "TWCS_BOTTOM";
    break;

  default:
    {
      char * buff = nextTempBuffer();
      SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "Unknown TWCS 0x:%04X", InfoID);
      text = buff;
    }
    break;
  }

  return text;
}

//////////////////////////////////////////////////////////////////////////////
// Convert CAP_CLEARBUFFERS values to String
const char* convertCAP_CLEARBUFFERS_toString(const TW_UINT16 InfoID)
{
  const char* text;

  switch(InfoID)
  {
	case TWCB_AUTO:
    text = "Auto";
    break;

	case TWCB_CLEAR:
    text = "Clear";
    break;

	case TWCB_NOCLEAR:
    text = "No Clear";
    break;

  default:
    {
      char * buff = nextTempBuffer();
      SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "Unknown TWCB 0x:%04X", InfoID);
      text = buff;
    }
    break;
  }

  return text;
}

//////////////////////////////////////////////////////////////////////////////
// Convert CAP_DEVICEEVENT values to String
const char* convertCAP_DEVICEEVENT_toString(const TW_UINT16 InfoID)
{
  const char* text;

  switch(InfoID)
  {
	case TWDE_CUSTOMEVENTS:
    text = "Custom Events";
    break;

	case TWDE_CHECKAUTOMATICCAPTURE:
    text = "Check Automatic Capture";
    break;

	case TWDE_CHECKBATTERY:
    text = "Check Battery";
    break;

	case TWDE_CHECKDEVICEONLINE:
    text = "Check Device Online";
    break;

	case TWDE_CHECKFLASH:
    text = "Check Flash";
    break;

	case TWDE_CHECKPOWERSUPPLY:
    text = "Check Power Supply";
    break;

	case TWDE_CHECKRESOLUTION:
    text = "Check Resolution";
    break;

	case TWDE_DEVICEADDED:
    text = "Device Added";
    break;

	case TWDE_DEVICEOFFLINE:
    text = "Device Offline";
    break;

	case TWDE_DEVICEREADY:
    text = "Device Ready";
    break;

	case TWDE_DEVICEREMOVED:
    text = "Device Removed";
    break;

	case TWDE_IMAGECAPTURED:
    text = "Image Captured";
    break;

	case TWDE_IMAGEDELETED:
    text = "Image Deleted";
    break;

	case TWDE_PAPERDOUBLEFEED:
    text = "Paper Doublefeed";
    break;

	case TWDE_PAPERJAM:
    text = "Paperjam";
    break;

	case TWDE_LAMPFAILURE:
    text = "Lamp Failure";
    break;

	case TWDE_POWERSAVE:
    text = "Power Save";
    break;

	case TWDE_POWERSAVENOTIFY:
    text = "Power Save Notify";
    break;

  default:
    {
      char * buff = nextTempBuffer();
      SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "Unknown TWDE 0x:%04X", InfoID);
      text = buff;
    }
    break;
  }

  return text;
}

//////////////////////////////////////////////////////////////////////////////
// Convert TWEI_DESKEWSTATUS values to String
const char* convertTWEI_DESKEWSTATUS_toString(const TW_UINT16 InfoID)
{
  const char* text;

  switch(InfoID)
  {
	case TWDSK_SUCCESS:
    text = "TWDSK_SUCCESS";
    break;

	case TWDSK_REPORTONLY:
    text = "TWDSK_REPORTONLY";
    break;

	case TWDSK_FAIL:
    text = "TWDSK_FAIL";
    break;

	case TWDSK_DISABLED:
    text = "TWDSK_DISABLED";
    break;

  default:
    {
      char * buff = nextTempBuffer();
      SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "Unknown TWDSK 0x:%04X", InfoID);
      text = buff;
    }
    break;
  }

  return text;
}

//////////////////////////////////////////////////////////////////////////////
// Convert CAP_DUPLEX values to String
const char* convertCAP_DUPLEX_toString(const TW_UINT16 InfoID)
{
  const char* text;

  switch(InfoID)
  {
	case TWDX_NONE:
    text = "None";
    break;

	case TWDX_1PASSDUPLEX:
    text = "1 Pass Duplex";
    break;

	case TWDX_2PASSDUPLEX:
    text = "2 Pass Duplex";
    break;

  default:
    {
      char * buff = nextTempBuffer();
      SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "Unknown TWDX 0x:%04X", InfoID);
      text = buff;
    }
    break;
  }

  return text;
}

//////////////////////////////////////////////////////////////////////////////
// Convert CAP_FEEDERALIGNMENT values to String
const char* convertCAP_FEEDERALIGNMENT_toString(const TW_UINT16 InfoID)
{
  const char* text;

  switch(InfoID)
  {
	case TWFA_NONE:
    text = "None";
    break;

	case TWFA_LEFT:
    text = "Left";
    break;

	case TWFA_CENTER:
    text = "Center";
    break;

	case TWFA_RIGHT:
    text = "Right";
    break;

  default:
    {
      char * buff = nextTempBuffer();
      SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "Unknown TWFA 0x:%04X", InfoID);
      text = buff;
    }
    break;
  }

  return text;
}

//////////////////////////////////////////////////////////////////////////////
// Convert ICAP_FEEDERTYPE values to String
const char* convertICAP_FEEDERTYPE_toString(const TW_UINT16 InfoID)
{
  const char* text;

  switch(InfoID)
  {
	case TWFE_GENERAL:
    text = "General";
    break;

	case TWFE_PHOTO:
    text = "Photo";
    break;

  default:
    {
      char * buff = nextTempBuffer();
      SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "Unknown TWFE 0x:%04X", InfoID);
      text = buff;
    }
    break;
  }

  return text;
}

//////////////////////////////////////////////////////////////////////////////
// Cover ICAP_IMAGEFILEFORMAT values to String
const char* convertICAP_IMAGEFILEFORMAT_toString(const TW_UINT16 _unItem)
{
  const char* text;

  switch(_unItem)
  {
  case TWFF_TIFF:
    text = "tiff";
    break;

  case TWFF_PICT:
    text = "pict";
    break;

  case TWFF_BMP:
    text = "bmp";
    break;

  case TWFF_XBM:
    text = "xbm";
    break;

  case TWFF_JFIF:
    text = "jpeg";
    break;

  case TWFF_FPX:
    text = "fpx";
    break;

  case TWFF_TIFFMULTI:
    text = "multi image tiff";
    break;

  case TWFF_PNG:
    text = "png";
    break;

  case TWFF_SPIFF:
    text = "spiff";
    break;

  case TWFF_EXIF:
    text = "exif";
    break;

  case TWFF_PDF:
    text = "PDF";
    break;

  case TWFF_JP2:
    text = "JP2";
    break;

  case TWFF_JPN:
    text = "JPN";
    break;

  case TWFF_JPX:
    text = "JPX";
    break;

  case TWFF_DEJAVU:
    text = "DEJAVU";
    break;

  case TWFF_PDFA:
    text = "PDF-A1";/* 2.0 Adobe PDF/A, Version 1*/
    break;

  case TWFF_PDFA2:
    text = "PDF-A2";/* 2.1 Adobe PDF/A, Version 2*/
    break;

  default:
    {
      char * buff = nextTempBuffer();
      SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "Unknown TWFF 0x:%04X", _unItem);
      text = buff;
    }
    break;
  }

  return text;
}

//////////////////////////////////////////////////////////////////////////////
// Cover ICAP_IMAGEFILEFORMAT values to String
const char* convertICAP_IMAGEFILEFORMAT_toExt(const TW_UINT16 _unItem)
{
  const char* text;

  switch(_unItem)
  {
  case TWFF_PICT:
    text = ".pict";
    break;

  case TWFF_BMP:
    text = ".bmp";
    break;

  case TWFF_XBM:
    text = ".xbm";
    break;

  case TWFF_JFIF:
    text = ".jpeg";
    break;

  case TWFF_FPX:
    text = ".fpx";
    break;

  case TWFF_TIFF:
  case TWFF_TIFFMULTI:
    text = ".tiff";
    break;

  case TWFF_PNG:
    text = ".png";
    break;

  case TWFF_SPIFF:
    text = ".spiff";
    break;

  case TWFF_EXIF:
    text = ".exif";
    break;

  case TWFF_JP2:
    text = ".jp2";
    break;

  case TWFF_JPN:
    text = ".jpn";
    break;

  case TWFF_JPX:
    text = ".jpx";
    break;

  case TWFF_DEJAVU:
    text = ".dejavu";
    break;

  case TWFF_PDF:
  case TWFF_PDFA:
  case TWFF_PDFA2:
    text = ".pdf";
    break;

  default:
    {
      char * buff = nextTempBuffer();
      SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, ".0x:%04X", _unItem);//Unknown TWFF 
      text = buff;
    }
    break;
  }

  return text;
}

//////////////////////////////////////////////////////////////////////////////
// Convert ICAP_FLASHUSED2 values to String
const char* convertICAP_FLASHUSED2_toString(const TW_UINT16 InfoID)
{
  const char* text;

  switch(InfoID)
  {
	case TWFL_NONE:
    text = "None";
    break;

	case TWFL_OFF:
    text = "Off";
    break;

	case TWFL_ON:
    text = "On";
    break;

	case TWFL_AUTO:
    text = "Auto";
    break;

	case TWFL_REDEYE:
    text = "Redeye";
    break;

  default:
    {
      char * buff = nextTempBuffer();
      SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "Unknown TWFL 0x:%04X", InfoID);
      text = buff;
    }
    break;
  }

  return text;
}

//////////////////////////////////////////////////////////////////////////////
// Convert CAP_FEEDERORDER values to String
const char* convertCAP_FEEDERORDER_toString(const TW_UINT16 InfoID)
{
  const char* text;

  switch(InfoID)
  {
	case TWFO_FIRSTPAGEFIRST:
    text = "First Page First";
    break;

	case TWFO_LASTPAGEFIRST:
    text = "Last Page First";
    break;

  default:
    {
      char * buff = nextTempBuffer();
      SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "Unknown TWFO 0x:%04X", InfoID);
      text = buff;
    }
    break;
  }

  return text;
}

//////////////////////////////////////////////////////////////////////////////
// Convert CAP_FEEDERPOCKET values to String
const char* convertCAP_FEEDERPOCKET_toString(const TW_UINT16 InfoID)
{
  const char* text;

  switch(InfoID)
  {
	case TWFP_POCKETERROR:
    text = "Pocket Error";
    break;

	case TWFP_POCKET1:
    text = "Pocket 1";
    break;

	case TWFP_POCKET2:
    text = "Pocket 2";
    break;

	case TWFP_POCKET3:
    text = "Pocket 3";
    break;

	case TWFP_POCKET4:
    text = "Pocket 4";
    break;

	case TWFP_POCKET5:
    text = "Pocket 5";
    break;

	case TWFP_POCKET6:
    text = "Pocket 6";
    break;

	case TWFP_POCKET7:
    text = "Pocket 7";
    break;

	case TWFP_POCKET8:
    text = "Pocket 8";
    break;

	case TWFP_POCKET9:
    text = "Pocket 9";
    break;

	case TWFP_POCKET10:
    text = "Pocket 10";
    break;

	case TWFP_POCKET11:
    text = "Pocket 11";
    break;

	case TWFP_POCKET12:
    text = "Pocket 12";
    break;

	case TWFP_POCKET13:
    text = "Pocket 13";
    break;

	case TWFP_POCKET14:
    text = "Pocket 14";
    break;

	case TWFP_POCKET15:
    text = "Pocket 15";
    break;

	case TWFP_POCKET16:
    text = "Pocket 16";
    break;

  default:
    {
      char * buff = nextTempBuffer();
      SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "Unknown TWFP 0x:%04X", InfoID);
      text = buff;
    }
    break;
  }

  return text;
}

//////////////////////////////////////////////////////////////////////////////
// Convert ICAP_FLIPROTATION values to String
const char* convertICAP_FLIPROTATION_toString(const TW_UINT16 InfoID)
{
  const char* text;

  switch(InfoID)
  {
	case TWFR_BOOK:
    text = "Book";
    break;

	case TWFR_FANFOLD:
    text = "Fanfold";
    break;

  default:
    {
      char * buff = nextTempBuffer();
      SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "Unknown TWFR 0x:%04X", InfoID);
      text = buff;
    }
    break;
  }

  return text;
}

//////////////////////////////////////////////////////////////////////////////
// Convert ICAP_FILTER values to String
const char* convertICAP_FILTER_toString(const TW_UINT16 InfoID)
{
  const char* text;

  switch(InfoID)
  {
	case TWFT_RED:
    text = "Red";
    break;

	case TWFT_GREEN:
    text = "Green";
    break;

	case TWFT_BLUE:
    text = "Blue";
    break;

	case TWFT_NONE:
    text = "None";
    break;

	case TWFT_WHITE:
    text = "White";
    break;

	case TWFT_CYAN:
    text = "Cyan";
    break;

	case TWFT_MAGENTA:
    text = "Magenta";
    break;

	case TWFT_YELLOW:
    text = "Yellow";
    break;

	case TWFT_BLACK:
    text = "Black";
    break;

  default:
    {
      char * buff = nextTempBuffer();
      SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "Unknown TWFT 0x:%04X", InfoID);
      text = buff;
    }
    break;
  }

  return text;
}

//////////////////////////////////////////////////////////////////////////////
// Convert TW_FILESYSTEM values to String
const char* convertTWEI_FILESYSTEM_toString(const TW_UINT16 InfoID)
{
  const char* text;

  switch(InfoID)
  {
	case TWFY_CAMERA:
    text = "Camera";
    break;

	case TWFY_CAMERATOP:
    text = "Camera Top";
    break;

	case TWFY_CAMERABOTTOM:
    text = "Camera Bottom";
    break;

	case TWFY_CAMERAPREVIEW:
    text = "Camera Preview";
    break;

	case TWFY_DOMAIN:
    text = "Domain";
    break;

	case TWFY_HOST:
    text = "Host";
    break;

	case TWFY_DIRECTORY:
    text = "Directory";
    break;

	case TWFY_IMAGE:
    text = "Image";
    break;

	case TWFY_UNKNOWN:
    text = "Unknown";
    break;

  default:
    {
      char * buff = nextTempBuffer();
      SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "Unknown TWFY 0x:%04X", InfoID);
      text = buff;
    }
    break;
  }

  return text;
}

//////////////////////////////////////////////////////////////////////////////
// Convert ICAP_ICCPROFILE values to String
const char* convertICAP_ICCPROFILE_toString(const TW_UINT16 InfoID)
{
  const char* text;

  switch(InfoID)
  {
	case TWIC_NONE:
    text = "None";
    break;

	case TWIC_LINK:
    text = "Link";
    break;

	case TWIC_EMBED:
    text = "Embed";
    break;

  default:
    {
      char * buff = nextTempBuffer();
      SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "Unknown TWIC 0x:%04X", InfoID);
      text = buff;
    }
    break;
  }

  return text;
}

//////////////////////////////////////////////////////////////////////////////
// Convert ICAP_IMAGEFILTER values to String
const char* convertICAP_IMAGEFILTER_toString(const TW_UINT16 InfoID)
{
  const char* text;

  switch(InfoID)
  {
	case TWIF_NONE:
    text = "None";
    break;

	case TWIF_AUTO:
    text = "Auto";
    break;

	case TWIF_LOWPASS:
    text = "Low Pass";
    break;

	case TWIF_BANDPASS:
    text = "Band Pass";
    break;

	case TWIF_HIGHPASS:
    text = "High Pass";
    break;

  default:
    {
      char * buff = nextTempBuffer();
      SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "Unknown TWIF 0x:%04X", InfoID);
      text = buff;
    }
    break;
  }

  return text;
}

//////////////////////////////////////////////////////////////////////////////
// Convert ICAP_IMAGEMERGE values to String
const char* convertICAP_IMAGEMERGE_toString(const TW_UINT16 InfoID)
{
  const char* text;

  switch(InfoID)
  {
	case TWIM_NONE:
    text = "None";
    break;

	case TWIM_FRONTONTOP:
    text = "Front on Top";
    break;

	case TWIM_FRONTONBOTTOM:
    text = "Front on Bottom";
    break;

	case TWIM_FRONTONLEFT:
    text = "Front on Left";
    break;

	case TWIM_FRONTONRIGHT:
    text = "Front on Right";
    break;

  default:
    {
      char * buff = nextTempBuffer();
      SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "Unknown TWIM 0x:%04X", InfoID);
      text = buff;
    }
    break;
  }

  return text;
}

//////////////////////////////////////////////////////////////////////////////
// Convert CAP_JOBCONTROL values to String
const char* convertCAP_JOBCONTROL_toString(const TW_UINT16 InfoID)
{
  const char* text;

  switch(InfoID)
  {
	case TWJC_NONE:
    text = "None";
    break;

	case TWJC_JSIC:
    text = "JSIC";
    break;

	case TWJC_JSIS:
    text = "JSIS";
    break;

	case TWJC_JSXC:
    text = "JSXC";
    break;

	case TWJC_JSXS:
    text = "JSXS";
    break;

  default:
    {
      char * buff = nextTempBuffer();
      SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "Unknown TWJC 0x:%04X", InfoID);
      text = buff;
    }
    break;
  }

  return text;
}

//////////////////////////////////////////////////////////////////////////////
// Convert ICAP_JPEGQUALITY values to String
const char* convertICAP_JPEGQUALITY_toString(const TW_UINT16 InfoID)
{
  const char* text;

  switch(InfoID)
  {
	case TWJQ_UNKNOWN:
    text = "Unknown";
    break;

	case TWJQ_LOW:
    text = "Low";
    break;

	case TWJQ_MEDIUM:
    text = "Medium";
    break;

	case TWJQ_HIGH:
    text = "High";
    break;

  default:
    {
      char * buff = nextTempBuffer();
      SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "%d", InfoID);
      text = buff;
    }
    break;
  }

  return text;
}

//////////////////////////////////////////////////////////////////////////////
// Convert ICAP_LIGHTPATH values to String
const char* convertICAP_LIGHTPATH_toString(const TW_UINT16 InfoID)
{
  const char* text;

  switch(InfoID)
  {
	case TWLP_REFLECTIVE:
    text = "Reflective";
    break;

	case TWLP_TRANSMISSIVE:
    text = "Transmissive";
    break;

  default:
    {
      char * buff = nextTempBuffer();
      SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "Unknown TWLP 0x:%04X", InfoID);
      text = buff;
    }
    break;
  }

  return text;
}

//////////////////////////////////////////////////////////////////////////////
// Convert ICAP_LIGHTSOURCE values to String
const char* convertICAP_LIGHTSOURCE_toString(const TW_UINT16 InfoID)
{
  const char* text;

  switch(InfoID)
  {
	case TWLS_RED:
    text = "Red";
    break;

	case TWLS_GREEN:
    text = "Green";
    break;

	case TWLS_BLUE:
    text = "Blue";
    break;

	case TWLS_NONE:
    text = "None";
    break;

	case TWLS_WHITE:
    text = "White";
    break;

	case TWLS_UV:
    text = "UV";
    break;

	case TWLS_IR:
    text = "IR";
    break;

  default:
    {
      char * buff = nextTempBuffer();
      SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "Unknown TWLS 0x:%04X", InfoID);
      text = buff;
    }
    break;
  }

  return text;
}

//////////////////////////////////////////////////////////////////////////////
// Convert TWEI_MAGTYPE values to String
const char* convertTWEI_MAGTYPE_toString(const TW_UINT16 InfoID)
{
  const char* text;

  switch(InfoID)
  {
	case TWMD_MICR:
    text = "TWMD_MICR";
    break;

	case TWMD_RAW:
    text = "TWMD_RAW";
    break;

	case TWMD_INVALID:
    text = "TWMD_INVALID";
    break;

  default:
    {
      char * buff = nextTempBuffer();
      SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "Unknown TWMD 0x:%04X", InfoID);
      text = buff;
    }
    break;
  }

  return text;
}

//////////////////////////////////////////////////////////////////////////////
// Convert ICAP_NOISEFILTER values to String
const char* convertICAP_NOISEFILTER_toString(const TW_UINT16 InfoID)
{
  const char* text;

  switch(InfoID)
  {
	case TWNF_NONE:
    text = "None";
    break;

	case TWNF_AUTO:
    text = "Auto";
    break;

	case TWNF_LONEPIXEL:
    text = "Low Pixel";
    break;

	case TWNF_MAJORITYRULE:
    text = "Majoriry Rule";
    break;

  default:
    {
      char * buff = nextTempBuffer();
      SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "Unknown TWNF 0x:%04X", InfoID);
      text = buff;
    }
    break;
  }

  return text;
}

//////////////////////////////////////////////////////////////////////////////
// Convert ICAP_ORIENTATION values to String
const char* convertICAP_ORIENTATION_toString(const TW_UINT16 InfoID)
{
  const char* text;

  switch(InfoID)
  {
//	case TWOR_ROT0:
//    text = "";
//    break;

	case TWOR_ROT90:
    text = "Orientation 90";
    break;

	case TWOR_ROT180:
    text = "Orientation 180";
    break;

//	case TWOR_ROT270:
//    text = "";
//    break;

	case TWOR_PORTRAIT:
    text = "Portrait";
    break;

	case TWOR_LANDSCAPE:
    text = "Landscape";
    break;

	case TWOR_AUTO:
    text = "Auto";
    break;

	case TWOR_AUTOTEXT:
    text = "Auto Text";
    break;

	case TWOR_AUTOPICTURE:
    text = "Auto Picture";
    break;

  default:
    {
      char * buff = nextTempBuffer();
      SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "Unknown TWOR 0x:%04X", InfoID);
      text = buff;
    }
    break;
  }

  return text;
}

//////////////////////////////////////////////////////////////////////////////
// Convert ICAP_OVERSCAN values to String
const char* convertICAP_OVERSCAN_toString(const TW_UINT16 InfoID)
{
  const char* text;

  switch(InfoID)
  {
	case TWOV_NONE:
    text = "None";
    break;

	case TWOV_AUTO:
    text = "Auto";
    break;

	case TWOV_TOPBOTTOM:
    text = "Top Bottom";
    break;

	case TWOV_LEFTRIGHT:
    text = "Left Right";
    break;

	case TWOV_ALL:
    text = "All";
    break;

  default:
    {
      char * buff = nextTempBuffer();
      SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "Unknown TWOV 0x:%04X", InfoID);
      text = buff;
    }
    break;
  }

  return text;
}

//////////////////////////////////////////////////////////////////////////////
// Convert ICAP_PLANARCHUNKY values to String
const char* convertICAP_PLANARCHUNKY_toString(const TW_UINT16 InfoID)
{
  const char* text;

  switch(InfoID)
  {
	case TWPC_CHUNKY:
    text = "Chunky";
    break;

	case TWPC_PLANAR:
    text = "Planar";
    break;

  default:
    {
      char * buff = nextTempBuffer();
      SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "Unknown TWPC 0x:%04X", InfoID);
      text = buff;
    }
    break;
  }

  return text;
}

//////////////////////////////////////////////////////////////////////////////
// Convert TWEI_PATCHCODE values to String
const char* convertTWEI_PATCHCODE_toString(const TW_UINT16 InfoID)
{
  const char* text;

  switch(InfoID)
  {
	case TWPCH_PATCH1:
    text = "TWPCH_PATCH1";
    break;

	case TWPCH_PATCH2:
    text = "TWPCH_PATCH2";
    break;

	case TWPCH_PATCH3:
    text = "TWPCH_PATCH3";
    break;

	case TWPCH_PATCH4:
    text = "TWPCH_PATCH4";
    break;

	case TWPCH_PATCH6:
    text = "TWPCH_PATCH6";
    break;

	case TWPCH_PATCHT:
    text = "TWPCH_PATCHT";
    break;

  default:
    {
      char * buff = nextTempBuffer();
      SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "Unknown TWPCH 0x:%04X", InfoID);
      text = buff;
    }
    break;
  }

  return text;
}

//////////////////////////////////////////////////////////////////////////////
// Convert ICAP_PIXELFLAVOR values to String
const char* convertICAP_PIXELFLAVOR_toString(const TW_UINT16 _unItem)
{
  const char* text;

  switch(_unItem)
  {
  case TWPF_CHOCOLATE:
    text = "Chocolate";
    break;

  case TWPF_VANILLA:
    text = "Vanilla";
    break;

  default:
    {
      char * buff = nextTempBuffer();
      SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "Unknown TWPF 0x:%04X", _unItem);
      text = buff;
    }
    break;
  }

  return text;
}


//////////////////////////////////////////////////////////////////////////////
// Convert CAP_PRINTERMODE values to String
const char* convertCAP_PRINTERMODE_toString(const TW_UINT16 InfoID)
{
  const char* text;

  switch(InfoID)
  {
	case TWPM_SINGLESTRING:
    text = "Single String";
    break;

	case TWPM_MULTISTRING:
    text = "Multi String";
    break;

	case TWPM_COMPOUNDSTRING:
    text = "Compound String";
    break;

  default:
    {
      char * buff = nextTempBuffer();
      SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "Unknown TWPM 0x:%04X", InfoID);
      text = buff;
    }
    break;
  }

  return text;
}

//////////////////////////////////////////////////////////////////////////////
// Convert CAP_PRINTER values to String
const char* convertCAP_PRINTER_toString(const TW_UINT16 InfoID)
{
  const char* text;

  switch(InfoID)
  {
	case TWPR_IMPRINTERTOPBEFORE:
    text = "Imprinter Top Before";
    break;

	case TWPR_IMPRINTERTOPAFTER:
    text = "Imprinter Top After";
    break;

	case TWPR_IMPRINTERBOTTOMBEFORE:
    text = "Imprinter Bottom Before";
    break;

	case TWPR_IMPRINTERBOTTOMAFTER:
    text = "Imprinter Bottom After";
    break;

	case TWPR_ENDORSERTOPBEFORE:
    text = "Endorser Top Before";
    break;

	case TWPR_ENDORSERTOPAFTER:
    text = "Endorser Top After";
    break;

	case TWPR_ENDORSERBOTTOMBEFORE:
    text = "Endorser Bottom Before";
    break;

	case TWPR_ENDORSERBOTTOMAFTER:
    text = "Endorser Bottom After";
    break;

  default:
    {
      char * buff = nextTempBuffer();
      SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "Unknown TWPR 0x:%04X", InfoID);
      text = buff;
    }
    break;
  }

  return text;
}

//////////////////////////////////////////////////////////////////////////////
// Convert CAP_POWERSUPPLY values to String
const char* convertCAP_POWERSUPPLY_toString(const TW_UINT16 InfoID)
{
  const char* text;

  switch(InfoID)
  {
	case TWPS_EXTERNAL:
    text = "External";
    break;

	case TWPS_BATTERY:
    text = "Battery";
    break;

  default:
    {
      char * buff = nextTempBuffer();
      SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "Unknown TWPS 0x:%04X", InfoID);
      text = buff;
    }
    break;
  }

  return text;
}

//////////////////////////////////////////////////////////////////////////////
// Convert ICAP_PIXELTYPE values to String
const char* convertICAP_PIXELTYPE_toString(const TW_UINT16 _unItem)
{
  const char* text;

  switch(_unItem)
  {
  case TWPT_BW:
    text = "BW";
    break;

  case TWPT_GRAY:
    text = "GRAY";
    break;

  case TWPT_RGB:
    text = "RGB";
    break;

  case TWPT_PALETTE:
    text = "PALETTE";
    break;

  case TWPT_CMY:
    text = "CMY";
    break;

  case TWPT_CMYK:
    text = "CMYK";
    break;

  case TWPT_YUV:
    text = "YUV";
    break;

  case TWPT_YUVK:
    text = "YUVK";
    break;

  case TWPT_CIEXYZ:
    text = "CIEXYZ";
    break;

  default:
    {
      char * buff = nextTempBuffer();
      SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "Unknown TWPT 0x:%04X", _unItem);
      text = buff;
    }
    break;
  }

  return text;
}

//////////////////////////////////////////////////////////////////////////////
// Convert CAP_SEGMENTED values to String
const char* convertCAP_SEGMENTED_toString(const TW_UINT16 InfoID)
{
  const char* text;

  switch(InfoID)
  {
	case TWSG_NONE:
    text = "None";
    break;

	case TWSG_AUTO:
    text = "Auto";
    break;

  default:
    {
      char * buff = nextTempBuffer();
      SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "Unknown TWSG 0x:%04X", InfoID);
      text = buff;
    }
    break;
  }

  return text;
}

//////////////////////////////////////////////////////////////////////////////
// Convert ICAP_SUPPORTEDSIZES values to String
const char* convertICAP_SUPPORTEDSIZES_toString(const TW_UINT16 InfoID)
{
  const char* text;

  switch(InfoID)
  {
	case TWSS_NONE:
    text = "None";
    break;

	case TWSS_A4:
    text = "A4";
    break;

	case TWSS_JISB5:
    text = "JIS B5";
    break;

	case TWSS_USLETTER:
    text = "US Letter";
    break;

	case TWSS_USLEGAL:
    text = "US Legal";
    break;

	case TWSS_A5:
    text = "A5";
    break;

	case TWSS_ISOB4:
    text = "ISO B4";
    break;

	case TWSS_ISOB6:
    text = "ISO B6";
    break;

	case TWSS_USLEDGER:
    text = "US Ledger";
    break;

	case TWSS_USEXECUTIVE:
    text = "US Executive";
    break;

	case TWSS_A3:
    text = "A3";
    break;

	case TWSS_ISOB3:
    text = "ISO B3";
    break;

	case TWSS_A6:
    text = "A6";
    break;

	case TWSS_C4:
    text = "C4";
    break;

	case TWSS_C5:
    text = "C5";
    break;

	case TWSS_C6:
    text = "C6";
    break;

	case TWSS_4A0:
    text = "4A0";
    break;

	case TWSS_2A0:
    text = "2A0";
    break;

	case TWSS_A0:
    text = "A0";
    break;

	case TWSS_A1:
    text = "A1";
    break;

	case TWSS_A2:
    text = "A2";
    break;

	case TWSS_A7:
    text = "A7";
    break;

	case TWSS_A8:
    text = "A8";
    break;

	case TWSS_A9:
    text = "A9";
    break;

	case TWSS_A10:
    text = "A10";
    break;

	case TWSS_ISOB0:
    text = "ISO B0";
    break;

	case TWSS_ISOB1:
    text = "ISO B1";
    break;

	case TWSS_ISOB2:
    text = "ISO B2";
    break;

	case TWSS_ISOB5:
    text = "ISO B5";
    break;

	case TWSS_ISOB7:
    text = "ISO B7";
    break;

	case TWSS_ISOB8:
    text = "ISO B8";
    break;

	case TWSS_ISOB9:
    text = "ISO B9";
    break;

	case TWSS_ISOB10:
    text = "ISO B10";
    break;

	case TWSS_JISB0:
    text = "JIS B0";
    break;

	case TWSS_JISB1:
    text = "JIS B1";
    break;

	case TWSS_JISB2:
    text = "JIS B2";
    break;

	case TWSS_JISB3:
    text = "JIS B3";
    break;

	case TWSS_JISB4:
    text = "JIS B4";
    break;

	case TWSS_JISB6:
    text = "JIS B6";
    break;

	case TWSS_JISB7:
    text = "JIS B7";
    break;

	case TWSS_JISB8:
    text = "JIS B8";
    break;

	case TWSS_JISB9:
    text = "JIS B9";
    break;

	case TWSS_JISB10:
    text = "JIS B10";
    break;

	case TWSS_C0:
    text = "C0";
    break;

	case TWSS_C1:
    text = "C1";
    break;

	case TWSS_C2:
    text = "C2";
    break;

	case TWSS_C3:
    text = "C3";
    break;

	case TWSS_C7:
    text = "C7";
    break;

	case TWSS_C8:
    text = "C8";
    break;

	case TWSS_C9:
    text = "C9";
    break;

	case TWSS_C10:
    text = "C10";
    break;

	case TWSS_USSTATEMENT:
    text = "US Statement";
    break;

	case TWSS_BUSINESSCARD:
    text = "Business Card";
    break;

	case TWSS_MAXSIZE:
    text = "MAX size";
    break;

  default:
    {
      char * buff = nextTempBuffer();
      SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "Unknown TWSS 0x:%04X", InfoID);
      text = buff;
    }
    break;
  }

  return text;
}

//////////////////////////////////////////////////////////////////////////////
// Covert ICAP_XFERMECH values to String
const char* convertICAP_XFERMECH_toString(const TW_UINT16 _unItem)
{
  const char* text;

  switch(_unItem)
  {
  case TWSX_NATIVE:
    text = "Native";
    break;

  case TWSX_FILE:
    text = "File";
    break;

  case TWSX_MEMORY:
    text = "Memory";
    break;

  case TWSX_MEMFILE:
    text = "Memory of File";
    break;

  default:
    {
      char * buff = nextTempBuffer();
      SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "Unknown TWSX 0x:%04X", _unItem);
      text = buff;
    }
    break;
  }

  return text;
}


//////////////////////////////////////////////////////////////////////////////
// Covert ICAP_UNITS values to String
const char* convertICAP_UNITS_toString(const TW_UINT16 _unItem)
{
  const char* text;

  switch(_unItem)
  {
  case TWUN_INCHES:
    text = "Inches";
    break;

  case TWUN_CENTIMETERS:
    text = "Centimeters";
    break;

  case TWUN_PICAS:
    text = "Picas";
    break;

  case TWUN_POINTS:
    text = "Points";
    break;

  case TWUN_TWIPS:
    text = "Twips";
    break;

  case TWUN_PIXELS:
    text = "Pixels";
    break;

  case TWUN_MILLIMETERS:
    text = "Millimeters";
    break;

  default:
    {
      char * buff = nextTempBuffer();
      SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "Unknown TWUN 0x:%04X", _unItem);
      text = buff;
    }
    break;
  }

  return text;
}


const char* convertExtImageInfoItem_toString(const TW_INFO &ImgInfo)
{
   char * buff = nextTempBuffer();

   if(TWRC_SUCCESS == ImgInfo.ReturnCode)
   {
      switch(ImgInfo.ItemType)
      {
       case TWTY_INT8:
       case TWTY_INT16:
       case TWTY_INT32:
       case TWTY_UINT8:
       case TWTY_UINT32:
       case TWTY_UINT16:
       case TWTY_BOOL:
         // If the size of the data is larger than 4 bytes (regardless of 32bit or 64bit OS) then the data
         // is no longer an array of data but a handle to the data.
         // If the data does fit inside then it is possible to have an array of data.
         if(getTWTYsize(ImgInfo.ItemType)*ImgInfo.NumItems <= sizeof(TW_UINT32))
         {
            char        *pTemp    = buff;
            const char  *pItem    = NULL;
            int          TempSize = TEMPBUFSIZE;

            for( int nItem=0; nItem<ImgInfo.NumItems; nItem++)
            {
               switch(ImgInfo.ItemType)
               {
                case TWTY_INT8:
                  SSNPRINTF(pTemp, TempSize, TempSize, "%d ", ((TW_INT8*)&ImgInfo.Item)[nItem]);
                  break;

                case TWTY_INT16:
                  SSNPRINTF(pTemp, TempSize, TempSize, "%d ", ((TW_INT16*)&ImgInfo.Item)[nItem]);
                  break;

                case TWTY_INT32:
                  SSNPRINTF(pTemp, TempSize, TempSize, "%d ", ((TW_INT32*)&ImgInfo.Item)[nItem]);
                  break;

                case TWTY_UINT8:
                  SSNPRINTF(pTemp, TempSize, TempSize, "%u ", ((TW_UINT8*)&ImgInfo.Item)[nItem]);
                  break;

                case TWTY_UINT32:
                  pItem = convertEI_Item_toString(ImgInfo.InfoID, ((TW_UINT32*)&ImgInfo.Item)[nItem]);
                  if(pItem)
                  {
                    SSNPRINTF(pTemp, TempSize, TempSize, "%s ", pItem);
                  }
                  else
                  {
                    SSNPRINTF(pTemp, TempSize, TempSize, "%u ", ((TW_UINT32*)&ImgInfo.Item)[nItem]);
                  }
                  break;

                case TWTY_UINT16:
                  pItem = convertEI_Item_toString(ImgInfo.InfoID, ((TW_UINT16*)&ImgInfo.Item)[nItem]);
                  if(pItem)
                  {
                    SSNPRINTF(pTemp, TempSize, TempSize, "%s ", pItem);
                  }
                  else
                  {
                    SSNPRINTF(pTemp, TempSize, TempSize, "%u ", ((TW_UINT16*)&ImgInfo.Item)[nItem]);
                  }
                  break;

                case TWTY_BOOL:
                  SSNPRINTF(pTemp, TempSize, TempSize, "%s ", ((TW_BOOL*)&ImgInfo.Item)[nItem]? "True" : "False");
                  break;
               }
               TempSize -= (int)strlen(pTemp);
               pTemp += strlen(pTemp);
               if(TempSize <= 0)
               {
                  break;
               }
            }
         }
         else
         {
            //Did not fit into the INT32
            char        *pTemp    = buff;
            const char  *pItem    = NULL;
            int          TempSize = TEMPBUFSIZE;

            BYTE * pBData = (BYTE *)_DSM_LockMemory((TW_HANDLE)ImgInfo.Item);

            for( int nItem=0; nItem<ImgInfo.NumItems; nItem++)
            {
               switch(ImgInfo.ItemType)
               {
                case TWTY_INT8:
                  SSNPRINTF(pTemp, TempSize, TempSize, "%d ", ((TW_INT8*)pBData)[nItem]);
                  break;

                case TWTY_INT16:
                  SSNPRINTF(pTemp, TempSize, TempSize, "%d ", ((TW_INT16*)pBData)[nItem]);
                  break;

                case TWTY_INT32:
                  SSNPRINTF(pTemp, TempSize, TempSize, "%d ", ((TW_INT32*)pBData)[nItem]);
                  break;

                case TWTY_UINT8:
                  SSNPRINTF(pTemp, TempSize, TempSize, "%u ", ((TW_UINT8*)pBData)[nItem]);
                  break;

                case TWTY_UINT32:
                  pItem = convertEI_Item_toString(ImgInfo.InfoID, ((TW_UINT32*)pBData)[nItem]);
                  if(pItem)
                  {
                    SSNPRINTF(pTemp, TempSize, TempSize, "%s ", pItem);
                  }
                  else
                  {
                    SSNPRINTF(pTemp, TempSize, TempSize, "%u ", ((TW_UINT32*)pBData)[nItem]);
                  }
                  break;

                case TWTY_UINT16:
                  pItem = convertEI_Item_toString(ImgInfo.InfoID, ((TW_UINT16*)pBData)[nItem]);
                  if(pItem)
                  {
                    SSNPRINTF(pTemp, TempSize, TempSize, "%s ", pItem);
                  }
                  else
                  {
                    SSNPRINTF(pTemp, TempSize, TempSize, "%u ", ((TW_UINT16*)pBData)[nItem]);
                  }
                  break;

                case TWTY_BOOL:
                  SSNPRINTF(pTemp, TempSize, TempSize, "%s ", ((TW_BOOL*)pBData)[nItem]? "True" : "False");
                  break;
               }
               TempSize -= (int)strlen(pTemp);
               pTemp += strlen(pTemp);
               if(TempSize <= 0)
               {
                  break;
               }
            }

             _DSM_UnlockMemory((TW_HANDLE)ImgInfo.Item);
         }
         break;

       case TWTY_STR32:
       case TWTY_STR64:
       case TWTY_STR128:
       case TWTY_STR255:
       case TWTY_STR1024:
         {
         char *chTest = (char *)_DSM_LockMemory((TW_HANDLE)ImgInfo.Item);

         SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "%.*s", getTWTYsize(ImgInfo.ItemType), chTest);

         _DSM_UnlockMemory((TW_HANDLE)ImgInfo.Item);
         }
         break;

       case TWTY_HANDLE:
         {
         // Need to look at the item to know how to handle :) this 
         // It might be a handle to a string or a handle to a chunk of memory
         SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "Handle 0x:%8X", (TW_HANDLE)ImgInfo.Item);
         }
         break;

       case TWTY_FRAME:
         {
         TW_FRAME *pFrame = (TW_FRAME *)_DSM_LockMemory((TW_HANDLE)ImgInfo.Item);

         SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "%0.2f  %.2f  %.2f  %.2f",
                   FIX32ToFloat(pFrame->Left), FIX32ToFloat(pFrame->Top),
                   FIX32ToFloat(pFrame->Right), FIX32ToFloat(pFrame->Bottom));

         _DSM_UnlockMemory((TW_HANDLE)ImgInfo.Item);
         }
         break;

       default:
          SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "Unsupporetd Type 0x:%04X", ImgInfo.ItemType);
         break;
      }
   }
   else if(TWRC_INFONOTSUPPORTED == ImgInfo.CondCode)
   {
      SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "Unsupporetd");
   }
   else if(TWRC_DATANOTAVAILABLE == ImgInfo.CondCode)
   {
      SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "Data Not Available");
   }
   else
   {
      SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "unknown failure %d", ImgInfo.CondCode);
   }

   return buff;
}

const char* convertReturnCode_toString(const TW_UINT16 _unItem)
{
  const char* text;

  switch(_unItem)
  {
  case TWRC_SUCCESS:
    text = "TWRC_SUCCESS";
    break;

  case TWRC_FAILURE:
    text = "TWRC_FAILURE";
    break;

  case TWRC_CHECKSTATUS:
    text = "TWRC_CHECKSTATUS";
    break;

  case TWRC_CANCEL:
    text = "TWRC_CANCEL";
    break;

  case TWRC_DSEVENT:
    text = "TWRC_DSEVENT";
    break;

  case TWRC_NOTDSEVENT:
    text = "TWRC_NOTDSEVENT";
    break;

  case TWRC_XFERDONE:
    text = "TWRC_XFERDONE";
    break;

  case TWRC_ENDOFLIST:
    text = "TWRC_ENDOFLIST";
    break;

  case TWRC_INFONOTSUPPORTED:
    text = "TWRC_INFONOTSUPPORTED";
    break;

  case TWRC_DATANOTAVAILABLE:
    text = "TWRC_DATANOTAVAILABLE";
    break;

  default:
    {
      char * buff = nextTempBuffer();
      SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "ReturnCode 0x:%04X",_unItem);
      text = buff;
    }
    break;
  }

  return text;
}

const char* convertConditionCode_toString(const TW_UINT16 _unItem)
{
  const char* text;

  switch(_unItem)
  {
  case TWCC_SUCCESS:
    text = "TWCC_SUCCESS";
    break;

  case TWCC_BUMMER:
    text = "TWCC_BUMMER";
    break;

  case TWCC_LOWMEMORY:
    text = "TWCC_LOWMEMORY";
    break;

  case TWCC_NODS:
    text = "TWCC_NODS";
    break;

  case TWCC_MAXCONNECTIONS:
    text = "TWCC_MAXCONNECTIONS";
    break;

  case TWCC_OPERATIONERROR:
    text = "TWCC_OPERATIONERROR";
    break;

  case TWCC_BADCAP:
    text = "TWCC_BADCAP";
    break;

  case TWCC_BADPROTOCOL:
    text = "TWCC_BADPROTOCOL";
    break;

  case TWCC_BADVALUE:
    text = "TWCC_BADVALUE";
    break;

  case TWCC_SEQERROR:
    text = "TWCC_SEQERROR";
    break;

  case TWCC_BADDEST:
    text = "TWCC_BADDEST";
    break;

  case TWCC_CAPUNSUPPORTED:
    text = "TWCC_CAPUNSUPPORTED";
    break;

  case TWCC_CAPBADOPERATION:
    text = "TWCC_CAPBADOPERATION";
    break;

  case TWCC_CAPSEQERROR:
    text = "TWCC_CAPSEQERROR";
    break;

  case TWCC_DENIED:
    text = "TWCC_DENIED";
    break;

  case TWCC_FILEEXISTS:
    text = "TWCC_FILEEXISTS";
    break;

  case TWCC_FILENOTFOUND:
    text = "TWCC_FILENOTFOUND";
    break;

  case TWCC_NOTEMPTY:
    text = "TWCC_NOTEMPTY";
    break;

  case TWCC_PAPERJAM:
    text = "TWCC_PAPERJAM";
    break;

  case TWCC_PAPERDOUBLEFEED:
    text = "TWCC_PAPERDOUBLEFEED";
    break;

  case TWCC_FILEWRITEERROR:
    text = "TWCC_FILEWRITEERROR";
    break;

  case TWCC_CHECKDEVICEONLINE:
    text = "TWCC_CHECKDEVICEONLINE";
    break;

  case TWCC_INTERLOCK:
    text = "TWCC_INTERLOCK";
    break;

  case TWCC_DAMAGEDCORNER:
    text = "TWCC_DAMAGEDCORNER";
    break;

  case TWCC_FOCUSERROR:
    text = "TWCC_FOCUSERROR";
    break;

  case TWCC_DOCTOOLIGHT:
    text = "TWCC_DOCTOOLIGHT";
    break;

  case TWCC_DOCTOODARK:
    text = "TWCC_DOCTOODARK";
    break;

  case TWCC_NOMEDIA:
    text = "TWCC_NOMEDIA";
    break;

  default:
    {
      char * buff = nextTempBuffer();
      SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "ConditionCode 0x:%04X",_unItem);
      text = buff;
    }
    break;
  }

  return text;
}

const char* convertMessage_toString(const TW_UINT16 _unItem)
{
  const char* strRet;
  switch(_unItem)
  {
    case MSG_GET:
      strRet = "MSG_GET";
      break;
    case MSG_GETCURRENT:
      strRet = "MSG_GETCURRENT";
      break;
    case MSG_GETDEFAULT:
      strRet = "MSG_GETDEFAULT";
      break;
    case MSG_GETFIRST:
      strRet = "MSG_GETFIRST";
      break;
    case MSG_GETNEXT:
      strRet = "MSG_GETNEXT";
      break;
    case MSG_SET:
      strRet = "MSG_SET";
      break;
    case MSG_RESET:
      strRet = "MSG_RESET";
      break;
    case MSG_QUERYSUPPORT:
      strRet = "MSG_QUERYSUPPORT";
      break;
    case MSG_GETHELP:
      strRet = "MSG_GETHELP";
      break;
    case MSG_GETLABEL:
      strRet = "MSG_GETLABEL";
      break;
    case MSG_GETLABELENUM:
      strRet = "MSG_GETLABELENUM";
      break;
    case MSG_XFERREADY:
      strRet = "MSG_XFERREADY";
      break;
    case MSG_CLOSEDSREQ:
      strRet = "MSG_CLOSEDSREQ";
      break;
    case MSG_CLOSEDSOK:
      strRet = "MSG_CLOSEDSOK";
      break;
    case MSG_DEVICEEVENT:
      strRet = "MSG_DEVICEEVENT";
      break;
    case MSG_OPENDSM:
      strRet = "MSG_OPENDSM";
      break;
    case MSG_CLOSEDSM:
      strRet = "MSG_CLOSEDSM";
      break;
    case MSG_OPENDS:
      strRet = "MSG_OPENDS";
      break;
    case MSG_CLOSEDS:
      strRet = "MSG_CLOSEDS";
      break;
    case MSG_USERSELECT:
      strRet = "MSG_USERSELECT";
      break;
    case MSG_DISABLEDS:
      strRet = "MSG_DISABLEDS";
      break;
    case MSG_ENABLEDS:
      strRet = "MSG_ENABLEDS";
      break;
    case MSG_ENABLEDSUIONLY:
      strRet = "MSG_ENABLEDSUIONLY";
      break;
    case MSG_PROCESSEVENT:
      strRet = "MSG_PROCESSEVENT";
      break;
    case MSG_ENDXFER:
      strRet = "MSG_ENDXFER";
      break;
    case MSG_STOPFEEDER:
      strRet = "MSG_STOPFEEDER";
      break;
    case MSG_CHANGEDIRECTORY:
      strRet = "MSG_CHANGEDIRECTORY";
      break;
    case MSG_CREATEDIRECTORY:
      strRet = "MSG_CREATEDIRECTORY";
      break;
    case MSG_DELETE:
      strRet = "MSG_DELETE";
      break;
    case MSG_FORMATMEDIA:
      strRet = "MSG_FORMATMEDIA";
      break;
    case MSG_GETCLOSE:
      strRet = "MSG_GETCLOSE";
      break;
    case MSG_GETFIRSTFILE:
      strRet = "MSG_GETFIRSTFILE";
      break;
    case MSG_GETINFO:
      strRet = "MSG_GETINFO";
      break;
    case MSG_GETNEXTFILE:
      strRet = "MSG_GETNEXTFILE";
      break;
    case MSG_RENAME:
      strRet = "MSG_RENAME";
      break;
    case MSG_COPY:
      strRet = "MSG_COPY";
      break;
    case MSG_AUTOMATICCAPTUREDIRECTORY:
      strRet = "MSG_AUTOMATICCAPTUREDIRECTORY";
      break;
    case MSG_PASSTHRU:
      strRet = "MSG_PASSTHRU";
      break;
    case MSG_REGISTER_CALLBACK:
      strRet = "MSG_REGISTER_CALLBACK";
      break;
    case MSG_RESETALL:
      strRet = "MSG_RESETALL";
      break;
    default:
      {
        char * buff = nextTempBuffer();
        SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "Message 0x:%04X",_unItem);
        strRet = buff;
      }
      break;
  }
  return strRet;
}

const char* convertDataGroup_toString(const TW_UINT16 _unItem)
{
  const char* strRet;
  switch(_unItem)
  {
        case DG_CONTROL:
          strRet = "DG_CONTROL";
          break;
        case DG_IMAGE:
          strRet = "DG_IMAGE";
          break;
        case DG_AUDIO:
          strRet = "DG_AUDIO";
          break;
    default:
      {
        char * buff = nextTempBuffer();
        SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "DataGroup 0x:%04X",_unItem);
        strRet = buff;
      }
      break;
  }
  return strRet;
}

const char* convertDataArgType_toString(const TW_UINT16 _unItem)
{
  const char* strRet;
  switch(_unItem)
  {
    case DAT_NULL:
      strRet = "DAT_NULL";
      break;
    case DAT_CAPABILITY:
      strRet = "DAT_CAPABILITY";
      break;
    case DAT_EVENT:
      strRet = "DAT_EVENT";
      break;
    case DAT_IDENTITY:
      strRet = "DAT_IDENTITY";
      break;
    case DAT_PARENT:
      strRet = "DAT_PARENT";
      break;
    case DAT_PENDINGXFERS:
      strRet = "DAT_PENDINGXFERS";
      break;
    case DAT_SETUPMEMXFER:
      strRet = "DAT_SETUPMEMXFER";
      break;
    case DAT_SETUPFILEXFER:
      strRet = "DAT_SETUPFILEXFER";
      break;
    case DAT_STATUS:
      strRet = "DAT_STATUS";
      break;
    case DAT_USERINTERFACE:
      strRet = "DAT_USERINTERFACE";
      break;
    case DAT_XFERGROUP:
      strRet = "DAT_XFERGROUP";
      break;
    case DAT_CUSTOMDSDATA:
      strRet = "DAT_CUSTOMDSDATA";
      break;
    case DAT_DEVICEEVENT:
      strRet = "DAT_DEVICEEVENT";
      break;
    case DAT_FILESYSTEM:
      strRet = "DAT_FILESYSTEM";
      break;
    case DAT_PASSTHRU:
      strRet = "DAT_PASSTHRU";
      break;
    case DAT_CALLBACK:
      strRet = "DAT_CALLBACK";
      break;
    case DAT_STATUSUTF8:
      strRet = "DAT_STATUSUTF8";
      break;
    case DAT_IMAGEINFO:
      strRet = "DAT_IMAGEINFO";
      break;
    case DAT_IMAGELAYOUT:
      strRet = "DAT_IMAGELAYOUT";
      break;
    case DAT_IMAGEMEMXFER:
      strRet = "DAT_IMAGEMEMXFER";
      break;
    case DAT_IMAGENATIVEXFER:
      strRet = "DAT_IMAGENATIVEXFER";
      break;
    case DAT_IMAGEFILEXFER:
      strRet = "DAT_IMAGEFILEXFER";
      break;
    case DAT_CIECOLOR:
      strRet = "DAT_CIECOLOR";
      break;
    case DAT_GRAYRESPONSE:
      strRet = "DAT_GRAYRESPONSE";
      break;
    case DAT_RGBRESPONSE:
      strRet = "DAT_RGBRESPONSE";
      break;
    case DAT_JPEGCOMPRESSION:
      strRet = "DAT_JPEGCOMPRESSION";
      break;
    case DAT_PALETTE8:
      strRet = "DAT_PALETTE8";
      break;
    case DAT_EXTIMAGEINFO:
      strRet = "DAT_EXTIMAGEINFO";
      break;
    case DAT_AUDIOFILEXFER:
      strRet = "DAT_AUDIOFILEXFER";
      break;
    case DAT_AUDIOINFO:
      strRet = "DAT_AUDIOINFO";
      break;
    case DAT_AUDIONATIVEXFER:
      strRet = "DAT_AUDIONATIVEXFER";
      break;
    case DAT_ICCPROFILE:
      strRet = "DAT_ICCPROFILE";
      break;
    case DAT_IMAGEMEMFILEXFER:
      strRet = "DAT_IMAGEMEMFILEXFER";
      break;
    case DAT_ENTRYPOINT:
      strRet = "DAT_ENTRYPOINT";
      break;
    default:
      {
        char * buff = nextTempBuffer();
        SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "DataArgumentType 0x:%04X",_unItem);
        strRet = buff;
      }
      break;
  }
  return strRet;
}

const char* convertContainerType_toString(const TW_UINT16 _unItem)
{
  const char* strRet;
  switch(_unItem)
  {
    case TWON_ARRAY:
      strRet = "TWON_ARRAY";
      break;
    case TWON_ENUMERATION:
      strRet = "TWON_ENUMERATION";
      break;
    case TWON_ONEVALUE:
      strRet = "TWON_ONEVALUE";
      break;
    case TWON_RANGE:
      strRet = "TWON_RANGE";
      break;
    default:
      {
        char * buff = nextTempBuffer();
        SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "DataArgumentType 0x:%04X",_unItem);
        strRet = buff;
      }
      break;
  }
  return strRet;
}

const char* convertQuerySupport_toString(const TW_UINT32 _unItem)
{
  const char* strRet = NULL;
  string strVal;
  if(_unItem&TWQC_GET)
  {
    strVal+="TWQC_GET";
  }
  if(_unItem&TWQC_SET)
  {
    if(strVal.length())
    {
      strVal+="|";
    }
    strVal+="TWQC_SET";
  }
  if(_unItem&TWQC_GETDEFAULT)
  {
    if(strVal.length())
    {
      strVal+="|";
    }
    strVal+="TWQC_GETDEFAULT";
  }
  if(_unItem&TWQC_GETCURRENT)
  {
    if(strVal.length())
    {
      strVal+="|";
    }
    strVal+="TWQC_GETCURRENT";
  }
  if(_unItem&TWQC_RESET)
  {
    if(strVal.length())
    {
      strVal+="|";
    }
    strVal+="TWQC_RESET";
  }

  if(strVal.length())
  {
    char * buff = nextTempBuffer();
    SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "%s",strVal.c_str());
    strRet = buff;
  }
  if(NULL==strRet)
  {
    strRet = "NONE";
  }
  return strRet;
}

const char* convertFileFmt_toExtension(const TW_UINT16 _unItem)
{
  const char* strRet = NULL;
  switch(_unItem)
  {
    case TWFF_PICT:
      strRet = "pct";
      break;
    case TWFF_BMP:
      strRet = "bmp";
      break;
    case TWFF_XBM:
      strRet = "xbm";
      break;
    case TWFF_JFIF:
      strRet = "jpg";
      break;
    case TWFF_FPX:
      strRet = "fpx";
      break;
    case TWFF_TIFF:
    case TWFF_TIFFMULTI:
      strRet = "tif";
      break;
    case TWFF_PNG:
      strRet = "png";
      break;
    case TWFF_SPIFF:
      strRet = "spf";
      break;
    case TWFF_EXIF:
      strRet = "xif";
      break;
    case TWFF_JP2:
      strRet = "jp2";
      break;
    case TWFF_JPX:
      strRet = "jpx";
      break;
    case TWFF_DEJAVU:
      strRet = "djv";
      break;
    case TWFF_PDF:
    case TWFF_PDFA:
    case TWFF_PDFA2:
      strRet = "pdf";
      break;
    default:
      strRet = "bin";
      break;
  }
  return strRet;
}

const char *CapabilityValueToString(TW_UINT16 twCapId, TW_UINT16 twItemType, const void *pValue)
{
  const char *strRet = NULL;

  //Setup various pointers for later
  const TW_FRAME *pFrame = static_cast<const TW_FRAME *>(pValue);
  const TW_FIX32 *pFix32 = static_cast<const TW_FIX32 *>(pValue);
  const TW_UINT16 *pUint16 = static_cast<const TW_UINT16 *>(pValue);
  //const TW_UINT32 *pUint32 = static_cast<const TW_UINT32 *>(pValue);
  const TW_INT16 *pInt16 = static_cast<const TW_INT16 *>(pValue);
  const TW_BOOL *pBool = static_cast<const TW_BOOL *>(pValue);
  const char *pStr = static_cast<const char *>(pValue);

  //knock-out some standards first
  switch(twItemType)
  {
    case TWTY_INT16:
      {
        char * buff = nextTempBuffer();
        SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "%d", *pInt16);
        strRet = buff;
      }
      break;
    case TWTY_BOOL:
      strRet = *pBool?"TRUE":"FALSE";
      break;
    case TWTY_FIX32:
      {
        char * buff = nextTempBuffer();
        SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "%.2f", FIX32ToFloat(*pFix32));
        strRet = buff;
      }
      break;
    case TWTY_FRAME:
      {
        char * buff = nextTempBuffer();
        SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "%.2f,%.2f,%.2f,%.2f", 
          FIX32ToFloat(pFrame->Left), FIX32ToFloat(pFrame->Top), FIX32ToFloat(pFrame->Right), FIX32ToFloat(pFrame->Bottom));
        strRet = buff;
      }
      break;
    case TWTY_STR32:
    case TWTY_STR64:
    case TWTY_STR128:
    case TWTY_STR255:
      {
        char * buff = nextTempBuffer();
        SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "%s", pStr);
        strRet = buff;
      }
      break;
  }

  if(NULL==strRet)
  {
    switch(twCapId)
    {
#if 0
      case CAP_CUSTOMDSDATA:
      case CAP_FEEDERENABLED:
      case CAP_FEEDERLOADED:
      case CAP_AUTOFEED:
      case CAP_CLEARPAGE:
      case CAP_FEEDPAGE:
      case CAP_REWINDPAGE:
      case CAP_INDICATORS:
      case CAP_PAPERDETECTABLE:
      case CAP_UICONTROLLABLE:
      case CAP_DEVICEONLINE:
      case CAP_AUTOSCAN:
      case CAP_THUMBNAILSENABLED:
      case CAP_DUPLEXENABLED:
      case CAP_ENABLEDSUIONLY:
        strRet = *pBool?"TRUE":"FALSE";
        break;
#endif //#if 0
      case CAP_EXTENDEDCAPS:
      case CAP_SUPPORTEDCAPS:
        switch(*pUint16)
        {
          case CAP_XFERCOUNT :
            strRet = "CAP_XFERCOUNT";
            break;
          case ICAP_COMPRESSION:
            strRet = "ICAP_COMPRESSION";
            break;
          case ICAP_PIXELTYPE:
            strRet = "ICAP_PIXELTYPE";
            break;
          case ICAP_UNITS:
            strRet = "ICAP_UNITS";
            break;
          case ICAP_XFERMECH:
            strRet = "ICAP_XFERMECH";
            break;
          case CAP_AUTHOR:
            strRet = "CAP_AUTHOR";
            break;
          case CAP_CAPTION:
            strRet = "CAP_CAPTION";
            break;
          case CAP_FEEDERENABLED:
            strRet = "CAP_FEEDERENABLED";
            break;
          case CAP_FEEDERLOADED:
            strRet = "CAP_FEEDERLOADED";
            break;
          case CAP_TIMEDATE:
            strRet = "CAP_TIMEDATE";
            break;
          case CAP_SUPPORTEDCAPS:
            strRet = "CAP_SUPPORTEDCAPS";
            break;
          case CAP_EXTENDEDCAPS:
            strRet = "CAP_EXTENDEDCAPS";
            break;
          case CAP_AUTOFEED:
            strRet = "CAP_AUTOFEED";
            break;
          case CAP_CLEARPAGE:
            strRet = "CAP_CLEARPAGE";
            break;
          case CAP_FEEDPAGE:
            strRet = "CAP_FEEDPAGE";
            break;
          case CAP_REWINDPAGE:
            strRet = "CAP_REWINDPAGE";
            break;
          case CAP_INDICATORS:
            strRet = "CAP_INDICATORS";
            break;
          case CAP_SUPPORTEDCAPSEXT:
            strRet = "CAP_SUPPORTEDCAPSEXT";
            break;
          case CAP_PAPERDETECTABLE:
            strRet = "CAP_PAPERDETECTABLE";
            break;
          case CAP_UICONTROLLABLE:
            strRet = "CAP_UICONTROLLABLE";
            break;
          case CAP_DEVICEONLINE:
            strRet = "CAP_DEVICEONLINE";
            break;
          case CAP_AUTOSCAN:
            strRet = "CAP_AUTOSCAN";
            break;
          case CAP_THUMBNAILSENABLED:
            strRet = "CAP_THUMBNAILSENABLED";
            break;
          case CAP_DUPLEX:
            strRet = "CAP_DUPLEX";
            break;
          case CAP_DUPLEXENABLED:
            strRet = "CAP_DUPLEXENABLED";
            break;
          case CAP_ENABLEDSUIONLY:
            strRet = "CAP_ENABLEDSUIONLY";
            break;
          case CAP_CUSTOMDSDATA:
            strRet = "CAP_CUSTOMDSDATA";
            break;
          case CAP_ENDORSER:
            strRet = "CAP_ENDORSER";
            break;
          case CAP_JOBCONTROL:
            strRet = "CAP_JOBCONTROL";
            break;
          case CAP_ALARMS:
            strRet = "CAP_ALARMS";
            break;
          case CAP_ALARMVOLUME:
            strRet = "CAP_ALARMVOLUME";
            break;
          case CAP_AUTOMATICCAPTURE:
            strRet = "CAP_AUTOMATICCAPTURE";
            break;
          case CAP_TIMEBEFOREFIRSTCAPTURE:
            strRet = "CAP_TIMEBEFOREFIRSTCAPTURE";
            break;
          case CAP_TIMEBETWEENCAPTURES:
            strRet = "CAP_TIMEBETWEENCAPTURES";
            break;
          case CAP_CLEARBUFFERS:
            strRet = "CAP_CLEARBUFFERS";
            break;
          case CAP_MAXBATCHBUFFERS:
            strRet = "CAP_MAXBATCHBUFFERS";
            break;
          case CAP_DEVICETIMEDATE:
            strRet = "CAP_DEVICETIMEDATE";
            break;
          case CAP_POWERSUPPLY:
            strRet = "CAP_POWERSUPPLY";
            break;
          case CAP_CAMERAPREVIEWUI:
            strRet = "CAP_CAMERAPREVIEWUI";
            break;
          case CAP_DEVICEEVENT:
            strRet = "CAP_DEVICEEVENT";
            break;
          case CAP_SERIALNUMBER:
            strRet = "CAP_SERIALNUMBER";
            break;
          case CAP_PRINTER:
            strRet = "CAP_PRINTER";
            break;
          case CAP_PRINTERENABLED:
            strRet = "CAP_PRINTERENABLED";
            break;
          case CAP_PRINTERINDEX:
            strRet = "CAP_PRINTERINDEX";
            break;
          case CAP_PRINTERMODE:
            strRet = "CAP_PRINTERMODE";
            break;
          case CAP_PRINTERSTRING:
            strRet = "CAP_PRINTERSTRING";
            break;
          case CAP_PRINTERSUFFIX:
            strRet = "CAP_PRINTERSUFFIX";
            break;
          case CAP_LANGUAGE:
            strRet = "CAP_LANGUAGE";
            break;
          case CAP_FEEDERALIGNMENT:
            strRet = "CAP_FEEDERALIGNMENT";
            break;
          case CAP_FEEDERORDER:
            strRet = "CAP_FEEDERORDER";
            break;
          case CAP_REACQUIREALLOWED:
            strRet = "CAP_REACQUIREALLOWED";
            break;
          case CAP_BATTERYMINUTES:
            strRet = "CAP_BATTERYMINUTES";
            break;
          case CAP_BATTERYPERCENTAGE:
            strRet = "CAP_BATTERYPERCENTAGE";
            break;
          case CAP_CAMERASIDE:
            strRet = "CAP_CAMERASIDE";
            break;
          case CAP_SEGMENTED:
            strRet = "CAP_SEGMENTED";
            break;
          case CAP_CAMERAENABLED:
            strRet = "CAP_CAMERAENABLED";
            break;
          case CAP_CAMERAORDER:
            strRet = "CAP_CAMERAORDER";
            break;
          case CAP_MICRENABLED:
            strRet = "CAP_MICRENABLED";
            break;
          case CAP_FEEDERPREP:
            strRet = "CAP_FEEDERPREP";
            break;
          case CAP_FEEDERPOCKET:
            strRet = "CAP_FEEDERPOCKET";
            break;
          case CAP_AUTOMATICSENSEMEDIUM:
            strRet = "CAP_AUTOMATICSENSEMEDIUM";
            break;
          case CAP_CUSTOMINTERFACEGUID:
            strRet = "CAP_CUSTOMINTERFACEGUID";
            break;
          case ICAP_AUTOBRIGHT:
            strRet = "ICAP_AUTOBRIGHT";
            break;
          case ICAP_BRIGHTNESS:
            strRet = "ICAP_BRIGHTNESS";
            break;
          case ICAP_CONTRAST:
            strRet = "ICAP_CONTRAST";
            break;
          case ICAP_CUSTHALFTONE:
            strRet = "ICAP_CUSTHALFTONE";
            break;
          case ICAP_EXPOSURETIME:
            strRet = "ICAP_EXPOSURETIME";
            break;
          case ICAP_FILTER:
            strRet = "ICAP_FILTER";
            break;
          case ICAP_FLASHUSED:
            strRet = "ICAP_FLASHUSED";
            break;
          case ICAP_GAMMA:
            strRet = "ICAP_GAMMA";
            break;
          case ICAP_HALFTONES:
            strRet = "ICAP_HALFTONES";
            break;
          case ICAP_HIGHLIGHT:
            strRet = "ICAP_HIGHLIGHT";
            break;
          case ICAP_IMAGEFILEFORMAT:
            strRet = "ICAP_IMAGEFILEFORMAT";
            break;
          case ICAP_LAMPSTATE:
            strRet = "ICAP_LAMPSTATE";
            break;
          case ICAP_LIGHTSOURCE:
            strRet = "ICAP_LIGHTSOURCE";
            break;
          case ICAP_ORIENTATION:
            strRet = "ICAP_ORIENTATION";
            break;
          case ICAP_PHYSICALWIDTH:
            strRet = "ICAP_PHYSICALWIDTH";
            break;
          case ICAP_PHYSICALHEIGHT:
            strRet = "ICAP_PHYSICALHEIGHT";
            break;
          case ICAP_SHADOW:
            strRet = "ICAP_SHADOW";
            break;
          case ICAP_FRAMES:
            strRet = "ICAP_FRAMES";
            break;
          case ICAP_XNATIVERESOLUTION:
            strRet = "ICAP_XNATIVERESOLUTION";
            break;
          case ICAP_YNATIVERESOLUTION:
            strRet = "ICAP_YNATIVERESOLUTION";
            break;
          case ICAP_XRESOLUTION:
            strRet = "ICAP_XRESOLUTION";
            break;
          case ICAP_YRESOLUTION:
            strRet = "ICAP_YRESOLUTION";
            break;
          case ICAP_MAXFRAMES:
            strRet = "ICAP_MAXFRAMES";
            break;
          case ICAP_TILES:
            strRet = "ICAP_TILES";
            break;
          case ICAP_BITORDER:
            strRet = "ICAP_BITORDER";
            break;
          case ICAP_CCITTKFACTOR:
            strRet = "ICAP_CCITTKFACTOR";
            break;
          case ICAP_LIGHTPATH:
            strRet = "ICAP_LIGHTPATH";
            break;
          case ICAP_PIXELFLAVOR:
            strRet = "ICAP_PIXELFLAVOR";
            break;
          case ICAP_PLANARCHUNKY:
            strRet = "ICAP_PLANARCHUNKY";
            break;
          case ICAP_ROTATION:
            strRet = "ICAP_ROTATION";
            break;
          case ICAP_SUPPORTEDSIZES:
            strRet = "ICAP_SUPPORTEDSIZES";
            break;
          case ICAP_THRESHOLD:
            strRet = "ICAP_THRESHOLD";
            break;
          case ICAP_XSCALING:
            strRet = "ICAP_XSCALING";
            break;
          case ICAP_YSCALING:
            strRet = "ICAP_YSCALING";
            break;
          case ICAP_BITORDERCODES:
            strRet = "ICAP_BITORDERCODES";
            break;
          case ICAP_PIXELFLAVORCODES:
            strRet = "ICAP_PIXELFLAVORCODES";
            break;
          case ICAP_JPEGPIXELTYPE:
            strRet = "ICAP_JPEGPIXELTYPE";
            break;
          case ICAP_TIMEFILL:
            strRet = "ICAP_TIMEFILL";
            break;
          case ICAP_BITDEPTH:
            strRet = "ICAP_BITDEPTH";
            break;
          case ICAP_BITDEPTHREDUCTION:
            strRet = "ICAP_BITDEPTHREDUCTION";
            break;
          case ICAP_UNDEFINEDIMAGESIZE:
            strRet = "ICAP_UNDEFINEDIMAGESIZE";
            break;
          case ICAP_IMAGEDATASET:
            strRet = "ICAP_IMAGEDATASET";
            break;
          case ICAP_EXTIMAGEINFO:
            strRet = "ICAP_EXTIMAGEINFO";
            break;
          case ICAP_MINIMUMHEIGHT:
            strRet = "ICAP_MINIMUMHEIGHT";
            break;
          case ICAP_MINIMUMWIDTH:
            strRet = "ICAP_MINIMUMWIDTH";
            break;
          case ICAP_AUTODISCARDBLANKPAGES:
            strRet = "ICAP_AUTODISCARDBLANKPAGES";
            break;
          case ICAP_FLIPROTATION:
            strRet = "ICAP_FLIPROTATION";
            break;
          case ICAP_BARCODEDETECTIONENABLED:
            strRet = "ICAP_BARCODEDETECTIONENABLED";
            break;
          case ICAP_SUPPORTEDBARCODETYPES:
            strRet = "ICAP_SUPPORTEDBARCODETYPES";
            break;
          case ICAP_BARCODEMAXSEARCHPRIORITIES:
            strRet = "ICAP_BARCODEMAXSEARCHPRIORITIES";
            break;
          case ICAP_BARCODESEARCHPRIORITIES:
            strRet = "ICAP_BARCODESEARCHPRIORITIES";
            break;
          case ICAP_BARCODESEARCHMODE:
            strRet = "ICAP_BARCODESEARCHMODE";
            break;
          case ICAP_BARCODEMAXRETRIES:
            strRet = "ICAP_BARCODEMAXRETRIES";
            break;
          case ICAP_BARCODETIMEOUT:
            strRet = "ICAP_BARCODETIMEOUT";
            break;
          case ICAP_ZOOMFACTOR:
            strRet = "ICAP_ZOOMFACTOR";
            break;
          case ICAP_PATCHCODEDETECTIONENABLED:
            strRet = "ICAP_PATCHCODEDETECTIONENABLED";
            break;
          case ICAP_SUPPORTEDPATCHCODETYPES:
            strRet = "ICAP_SUPPORTEDPATCHCODETYPES";
            break;
          case ICAP_PATCHCODEMAXSEARCHPRIORITIES:
            strRet = "ICAP_PATCHCODEMAXSEARCHPRIORITIES";
            break;
          case ICAP_PATCHCODESEARCHPRIORITIES:
            strRet = "ICAP_PATCHCODESEARCHPRIORITIES";
            break;
          case ICAP_PATCHCODESEARCHMODE:
            strRet = "ICAP_PATCHCODESEARCHMODE";
            break;
          case ICAP_PATCHCODEMAXRETRIES:
            strRet = "ICAP_PATCHCODEMAXRETRIES";
            break;
          case ICAP_PATCHCODETIMEOUT:
            strRet = "ICAP_PATCHCODETIMEOUT";
            break;
          case ICAP_FLASHUSED2:
            strRet = "ICAP_FLASHUSED2";
            break;
          case ICAP_IMAGEFILTER:
            strRet = "ICAP_IMAGEFILTER";
            break;
          case ICAP_NOISEFILTER:
            strRet = "ICAP_NOISEFILTER";
            break;
          case ICAP_OVERSCAN:
            strRet = "ICAP_OVERSCAN";
            break;
          case ICAP_AUTOMATICBORDERDETECTION:
            strRet = "ICAP_AUTOMATICBORDERDETECTION";
            break;
          case ICAP_AUTOMATICDESKEW:
            strRet = "ICAP_AUTOMATICDESKEW";
            break;
          case ICAP_AUTOMATICROTATE:
            strRet = "ICAP_AUTOMATICROTATE";
            break;
          case ICAP_JPEGQUALITY:
            strRet = "ICAP_JPEGQUALITY";
            break;
          case ICAP_FEEDERTYPE:
            strRet = "ICAP_FEEDERTYPE";
            break;
          case ICAP_ICCPROFILE:
            strRet = "ICAP_ICCPROFILE";
            break;
          case ICAP_AUTOSIZE:
            strRet = "ICAP_AUTOSIZE";
            break;
          case ICAP_AUTOMATICCROPUSESFRAME:
            strRet = "ICAP_AUTOMATICCROPUSESFRAME";
            break;
          case ICAP_AUTOMATICLENGTHDETECTION:
            strRet = "ICAP_AUTOMATICLENGTHDETECTION";
            break;
          case ICAP_AUTOMATICCOLORENABLED:
            strRet = "ICAP_AUTOMATICCOLORENABLED";
            break;
          case ICAP_AUTOMATICCOLORNONCOLORPIXELTYPE:
            strRet = "ICAP_AUTOMATICCOLORNONCOLORPIXELTYPE";
            break;
          case ICAP_COLORMANAGEMENTENABLED:
            strRet = "ICAP_COLORMANAGEMENTENABLED";
            break;
          case ICAP_IMAGEMERGE:
            strRet = "ICAP_IMAGEMERGE";
            break;
          case ICAP_IMAGEMERGEHEIGHTTHRESHOLD:
            strRet = "ICAP_IMAGEMERGEHEIGHTTHRESHOLD";
            break;
          case ICAP_SUPPORTEDEXTIMAGEINFO:
            strRet = "ICAP_SUPPORTEDEXTIMAGEINFO";
            break;
          default:
            {
              char * buff = nextTempBuffer();
              SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "0x%04X", *pUint16);
              strRet = buff;
            }
            break;
        }
        break;
      case ICAP_SUPPORTEDEXTIMAGEINFO:
        switch(*pUint16)
        {
          case TWEI_BARCODEX:
            strRet = "TWEI_BARCODEX";
            break;
          case TWEI_BARCODEY:
            strRet = "TWEI_BARCODEY";
            break;
          case TWEI_BARCODETEXT:
            strRet = "TWEI_BARCODETEXT";
            break;
          case TWEI_BARCODETYPE:
            strRet = "TWEI_BARCODETYPE";
            break;
          case TWEI_DESHADETOP:
            strRet = "TWEI_DESHADETOP";
            break;
          case TWEI_DESHADELEFT:
            strRet = "TWEI_DESHADELEFT";
            break;
          case TWEI_DESHADEHEIGHT:
            strRet = "TWEI_DESHADEHEIGHT";
            break;
          case TWEI_DESHADEWIDTH:
            strRet = "TWEI_DESHADEWIDTH";
            break;
          case TWEI_DESHADESIZE:
            strRet = "TWEI_DESHADESIZE";
            break;
          case TWEI_SPECKLESREMOVED:
            strRet = "TWEI_SPECKLESREMOVED";
            break;
          case TWEI_HORZLINEXCOORD:
            strRet = "TWEI_HORZLINEXCOORD";
            break;
          case TWEI_HORZLINEYCOORD:
            strRet = "TWEI_HORZLINEYCOORD";
            break;
          case TWEI_HORZLINELENGTH:
            strRet = "TWEI_HORZLINELENGTH";
            break;
          case TWEI_HORZLINETHICKNESS:
            strRet = "TWEI_HORZLINETHICKNESS";
            break;
          case TWEI_VERTLINEXCOORD:
            strRet = "TWEI_VERTLINEXCOORD";
            break;
          case TWEI_VERTLINEYCOORD:
            strRet = "TWEI_VERTLINEYCOORD";
            break;
          case TWEI_VERTLINELENGTH:
            strRet = "TWEI_VERTLINELENGTH";
            break;
          case TWEI_VERTLINETHICKNESS:
            strRet = "TWEI_VERTLINETHICKNESS";
            break;
          case TWEI_PATCHCODE:
            strRet = "TWEI_PATCHCODE";
            break;
          case TWEI_ENDORSEDTEXT:
            strRet = "TWEI_ENDORSEDTEXT";
            break;
          case TWEI_FORMCONFIDENCE:
            strRet = "TWEI_FORMCONFIDENCE";
            break;
          case TWEI_FORMTEMPLATEMATCH:
            strRet = "TWEI_FORMTEMPLATEMATCH";
            break;
          case TWEI_FORMTEMPLATEPAGEMATCH:
            strRet = "TWEI_FORMTEMPLATEPAGEMATCH";
            break;
          case TWEI_FORMHORZDOCOFFSET:
            strRet = "TWEI_FORMHORZDOCOFFSET";
            break;
          case TWEI_FORMVERTDOCOFFSET:
            strRet = "TWEI_FORMVERTDOCOFFSET";
            break;
          case TWEI_BARCODECOUNT:
            strRet = "TWEI_BARCODECOUNT";
            break;
          case TWEI_BARCODECONFIDENCE:
            strRet = "TWEI_BARCODECONFIDENCE";
            break;
          case TWEI_BARCODEROTATION:
            strRet = "TWEI_BARCODEROTATION";
            break;
          case TWEI_BARCODETEXTLENGTH:
            strRet = "TWEI_BARCODETEXTLENGTH";
            break;
          case TWEI_DESHADECOUNT:
            strRet = "TWEI_DESHADECOUNT";
            break;
          case TWEI_DESHADEBLACKCOUNTOLD:
            strRet = "TWEI_DESHADEBLACKCOUNTOLD";
            break;
          case TWEI_DESHADEBLACKCOUNTNEW:
            strRet = "TWEI_DESHADEBLACKCOUNTNEW";
            break;
          case TWEI_DESHADEBLACKRLMIN:
            strRet = "TWEI_DESHADEBLACKRLMIN";
            break;
          case TWEI_DESHADEBLACKRLMAX:
            strRet = "TWEI_DESHADEBLACKRLMAX";
            break;
          case TWEI_DESHADEWHITECOUNTOLD:
            strRet = "TWEI_DESHADEWHITECOUNTOLD";
            break;
          case TWEI_DESHADEWHITECOUNTNEW:
            strRet = "TWEI_DESHADEWHITECOUNTNEW";
            break;
          case TWEI_DESHADEWHITERLMIN:
            strRet = "TWEI_DESHADEWHITERLMIN";
            break;
          case TWEI_DESHADEWHITERLAVE:
            strRet = "TWEI_DESHADEWHITERLAVE";
            break;
          case TWEI_DESHADEWHITERLMAX:
            strRet = "TWEI_DESHADEWHITERLMAX";
            break;
          case TWEI_BLACKSPECKLESREMOVED:
            strRet = "TWEI_BLACKSPECKLESREMOVED";
            break;
          case TWEI_WHITESPECKLESREMOVED:
            strRet = "TWEI_WHITESPECKLESREMOVED";
            break;
          case TWEI_HORZLINECOUNT:
            strRet = "TWEI_HORZLINECOUNT";
            break;
          case TWEI_VERTLINECOUNT:
            strRet = "TWEI_VERTLINECOUNT";
            break;
          case TWEI_DESKEWSTATUS:
            strRet = "TWEI_DESKEWSTATUS";
            break;
          case TWEI_SKEWORIGINALANGLE:
            strRet = "TWEI_SKEWORIGINALANGLE";
            break;
          case TWEI_SKEWFINALANGLE:
            strRet = "TWEI_SKEWFINALANGLE";
            break;
          case TWEI_SKEWCONFIDENCE:
            strRet = "TWEI_SKEWCONFIDENCE";
            break;
          case TWEI_SKEWWINDOWX1:
            strRet = "TWEI_SKEWWINDOWX1";
            break;
          case TWEI_SKEWWINDOWY1:
            strRet = "TWEI_SKEWWINDOWY1";
            break;
          case TWEI_SKEWWINDOWX2:
            strRet = "TWEI_SKEWWINDOWX2";
            break;
          case TWEI_SKEWWINDOWY2:
            strRet = "TWEI_SKEWWINDOWY2";
            break;
          case TWEI_SKEWWINDOWX3:
            strRet = "TWEI_SKEWWINDOWX3";
            break;
          case TWEI_SKEWWINDOWY3:
            strRet = "TWEI_SKEWWINDOWY3";
            break;
          case TWEI_SKEWWINDOWX4:
            strRet = "TWEI_SKEWWINDOWX4";
            break;
          case TWEI_SKEWWINDOWY4:
            strRet = "TWEI_SKEWWINDOWY4";
            break;
          case TWEI_BOOKNAME:
            strRet = "TWEI_BOOKNAME";
            break;
          case TWEI_CHAPTERNUMBER:
            strRet = "TWEI_CHAPTERNUMBER";
            break;
          case TWEI_DOCUMENTNUMBER:
            strRet = "TWEI_DOCUMENTNUMBER";
            break;
          case TWEI_PAGENUMBER:
            strRet = "TWEI_PAGENUMBER";
            break;
          case TWEI_CAMERA:
            strRet = "TWEI_CAMERA";
            break;
          case TWEI_FRAMENUMBER:
            strRet = "TWEI_FRAMENUMBER";
            break;
          case TWEI_FRAME:
            strRet = "TWEI_FRAME";
            break;
          case TWEI_PIXELFLAVOR:
            strRet = "TWEI_PIXELFLAVOR";
            break;
          case TWEI_ICCPROFILE:
            strRet = "TWEI_ICCPROFILE";
            break;
          case TWEI_LASTSEGMENT:
            strRet = "TWEI_LASTSEGMENT";
            break;
          case TWEI_SEGMENTNUMBER:
            strRet = "TWEI_SEGMENTNUMBER";
            break;
          case TWEI_MAGDATA:
            strRet = "TWEI_MAGDATA";
            break;
          case TWEI_MAGTYPE:
            strRet = "TWEI_MAGTYPE";
            break;
          case TWEI_PAGESIDE:
            strRet = "TWEI_PAGESIDE";
            break;
          case TWEI_FILESYSTEMSOURCE:
            strRet = "TWEI_FILESYSTEMSOURCE";
            break;
          case TWEI_IMAGEMERGED:
            strRet = "TWEI_IMAGEMERGED";
            break;
          case TWEI_MAGDATALENGTH:
            strRet = "TWEI_MAGDATALENGTH";
            break;
          default:
            {
              char * buff = nextTempBuffer();
              SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "0x%04X", *pUint16);
              strRet = buff;
            }
            break;
        }
        break;
#if 0
      case ICAP_FRAMES:
        {
          char * buff = nextTempBuffer();
          SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "%.2f,%.2f,%.2f,%.2f", 
            FIX32ToFloat(pFrame->Left), FIX32ToFloat(pFrame->Top), FIX32ToFloat(pFrame->Right), FIX32ToFloat(pFrame->Bottom));
          strRet = buff;
        }
        break;
#endif //#if 0
      case ICAP_MAXFRAMES:
      case ICAP_BITDEPTH:
        {
          char * buff = nextTempBuffer();
          SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "%d", *pUint16);
          strRet = buff;
        }
        break;
#if 0
      case CAP_XFERCOUNT:
        {
          char * buff = nextTempBuffer();
          SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "%d", *pInt16);
          strRet = buff;
        }
        break;
#endif //#if 0
#if 0
      case ICAP_GAMMA:
      case ICAP_XRESOLUTION:
      case ICAP_YRESOLUTION:
      case ICAP_BRIGHTNESS:
      case ICAP_CONTRAST:
      case ICAP_THRESHOLD:
      case ICAP_PHYSICALWIDTH:
      case ICAP_PHYSICALHEIGHT:
      case ICAP_MINIMUMHEIGHT:
      case ICAP_MINIMUMWIDTH:
        {
          char * buff = nextTempBuffer();
          SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "%.2f", FIX32ToFloat(*pFix32));
          strRet = buff;
        }
        break;
#endif //#if 0
      case CAP_CUSTOMINTERFACEGUID:
      case ICAP_HALFTONES:
      case CAP_AUTHOR:
      case CAP_CAPTION:
      case CAP_TIMEDATE:
        {
          char * buff = nextTempBuffer();
          SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "%s", pStr);
          strRet = buff;
        }
        break;
      case CAP_ALARMS:
        switch(*pUint16)
        {
          case TWAL_ALARM:
            strRet = "TWAL_ALARM";
            break;
          case TWAL_FEEDERERROR:
            strRet = "TWAL_FEEDERERROR";
            break;
          case TWAL_FEEDERWARNING:
            strRet = "TWAL_FEEDERWARNING";
            break;
          case TWAL_BARCODE:
            strRet = "TWAL_BARCODE";
            break;
          case TWAL_DOUBLEFEED:
            strRet = "TWAL_DOUBLEFEED";
            break;
          case TWAL_JAM:
            strRet = "TWAL_JAM";
            break;
          case TWAL_PATCHCODE:
            strRet = "TWAL_PATCHCODE";
            break;
          case TWAL_POWER:
            strRet = "TWAL_POWER";
            break;
          case TWAL_SKEW:
            strRet = "TWAL_SKEW";
            break;
          default:
            {
              char * buff = nextTempBuffer();
              SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "0x%04X", *pUint16);
              strRet = buff;
            }
            break;
        }
        break;
      case ICAP_AUTOSIZE:
        switch(*pUint16)
        {
          case TWAS_NONE:
            strRet = "TWAS_NONE";
            break;
          case TWAS_AUTO:
            strRet = "TWAS_AUTO";
            break;
          case TWAS_CURRENT:
            strRet = "TWAS_CURRENT";
            break;
          default:
            {
              char * buff = nextTempBuffer();
              SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "0x%04X", *pUint16);
              strRet = buff;
            }
            break;
        }
        break;
      case ICAP_BARCODESEARCHMODE:
        switch(*pUint16)
        {
          case TWBD_HORZ:
            strRet = "TWBD_HORZ";
            break;
          case TWBD_VERT:
            strRet = "TWBD_VERT";
            break;
          case TWBD_HORZVERT:
            strRet = "TWBD_HORZVERT";
            break;
          case TWBD_VERTHORZ:
            strRet = "TWBD_VERTHORZ";
            break;
          default:
            {
              char * buff = nextTempBuffer();
              SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "0x%04X", *pUint16);
              strRet = buff;
            }
            break;
        }
        break;
      case ICAP_BITORDER:
        switch(*pUint16)
        {
          case TWBO_LSBFIRST:
            strRet = "TWBO_LSBFIRST";
            break;
          case TWBO_MSBFIRST:
            strRet = "TWBO_MSBFIRST";
            break;
          default:
            {
              char * buff = nextTempBuffer();
              SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "0x%04X", *pUint16);
              strRet = buff;
            }
            break;
        }
        break;
      case ICAP_AUTODISCARDBLANKPAGES:
        switch(*pUint16)
        {
          case TWBP_DISABLE:
            strRet = "TWBP_DISABLE";
            break;
          case TWBP_AUTO:
            strRet = "TWBP_AUTO";
            break;
          default:
            {
              char * buff = nextTempBuffer();
              SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "0x%04X", *pUint16);
              strRet = buff;
            }
            break;
        }
        break;
      case ICAP_BITDEPTHREDUCTION:
        switch(*pUint16)
        {
          case TWBR_THRESHOLD:
            strRet = "TWBR_THRESHOLD";
            break;
          case TWBR_HALFTONE:
            strRet = "TWBR_HALFTONE";
            break;
          case TWBR_CUSTHALFTONE:
            strRet = "TWBR_CUSTHALFTONE";
            break;
          case TWBR_DIFFUSION:
            strRet = "TWBR_DIFFUSION";
            break;
          default:
            {
              char * buff = nextTempBuffer();
              SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "0x%04X", *pUint16);
              strRet = buff;
            }
            break;
        }
        break;
      case ICAP_SUPPORTEDBARCODETYPES: /*and TWEI_BARCODETYPE values Added 1.7 */
        switch(*pUint16)
        {
          case TWBT_3OF9:
            strRet = "TWBT_3OF9";
            break;
          case TWBT_2OF5INTERLEAVED:
            strRet = "TWBT_2OF5INTERLEAVED";
            break;
          case TWBT_2OF5NONINTERLEAVED:
            strRet = "TWBT_2OF5NONINTERLEAVED";
            break;
          case TWBT_CODE93:
            strRet = "TWBT_CODE93";
            break;
          case TWBT_CODE128:
            strRet = "TWBT_CODE128";
            break;
          case TWBT_UCC128:
            strRet = "TWBT_UCC128";
            break;
          case TWBT_CODABAR:
            strRet = "TWBT_CODABAR";
            break;
          case TWBT_UPCA:
            strRet = "TWBT_UPCA";
            break;
          case TWBT_UPCE:
            strRet = "TWBT_UPCE";
            break;
          case TWBT_EAN8:
            strRet = "TWBT_EAN8";
            break;
          case TWBT_EAN13:
            strRet = "TWBT_EAN13";
            break;
          case TWBT_POSTNET:
            strRet = "TWBT_POSTNET";
            break;
          case TWBT_PDF417:
            strRet = "TWBT_PDF417";
            break;
          case TWBT_2OF5INDUSTRIAL:
            strRet = "TWBT_2OF5INDUSTRIAL";
            break;
          case TWBT_2OF5MATRIX:
            strRet = "TWBT_2OF5MATRIX";
            break;
          case TWBT_2OF5DATALOGIC:
            strRet = "TWBT_2OF5DATALOGIC";
            break;
          case TWBT_2OF5IATA:
            strRet = "TWBT_2OF5IATA";
            break;
          case TWBT_3OF9FULLASCII:
            strRet = "TWBT_3OF9FULLASCII";
            break;
          case TWBT_CODABARWITHSTARTSTOP:
            strRet = "TWBT_CODABARWITHSTARTSTOP";
            break;
          case TWBT_MAXICODE:
            strRet = "TWBT_MAXICODE";
            break;
          default:
            {
              char * buff = nextTempBuffer();
              SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "0x%04X", *pUint16);
              strRet = buff;
            }
            break;
        }
        break;
      case ICAP_COMPRESSION:
        switch(*pUint16)
        {
          case TWCP_NONE:
            strRet = "TWCP_NONE";
            break;
          case TWCP_PACKBITS:
            strRet = "TWCP_PACKBITS";
            break;
          case TWCP_GROUP31D:
            strRet = "TWCP_GROUP31D";
            break;
          case TWCP_GROUP31DEOL:
            strRet = "TWCP_GROUP31DEOL";
            break;
          case TWCP_GROUP32D:
            strRet = "TWCP_GROUP32D";
            break;
          case TWCP_GROUP4:
            strRet = "TWCP_GROUP4";
            break;
          case TWCP_JPEG:
            strRet = "TWCP_JPEG";
            break;
          case TWCP_LZW:
            strRet = "TWCP_LZW";
            break;
          case TWCP_JBIG:
            strRet = "TWCP_JBIG";
            break;
          case TWCP_PNG:
            strRet = "TWCP_PNG";
            break;
          case TWCP_RLE4:
            strRet = "TWCP_RLE4";
            break;
          case TWCP_RLE8:
            strRet = "TWCP_RLE8";
            break;
          case TWCP_BITFIELDS:
            strRet = "TWCP_BITFIELDS";
            break;
          default:
            {
              char * buff = nextTempBuffer();
              SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "0x%04X", *pUint16);
              strRet = buff;
            }
            break;
        }
        break;
      case CAP_CAMERASIDE: /* and TWEI_PAGESIDE values (CS_ means camera side) Added 1.91 */
        switch(*pUint16)
        {
          case TWCS_BOTH:
            strRet = "TWCS_BOTH";
            break;
          case TWCS_TOP:
            strRet = "TWCS_TOP";
            break;
          case TWCS_BOTTOM:
            strRet = "TWCS_BOTTOM";
            break;
          default:
            {
              char * buff = nextTempBuffer();
              SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "0x%04X", *pUint16);
              strRet = buff;
            }
            break;
        }
        break;
      case CAP_CLEARBUFFERS:
        switch(*pUint16)
        {
          case TWCB_AUTO:
            strRet = "TWCB_AUTO";
            break;
          case TWCB_CLEAR:
            strRet = "TWCB_CLEAR";
            break;
          case TWCB_NOCLEAR:
            strRet = "TWCB_NOCLEAR";
            break;
          default:
            {
              char * buff = nextTempBuffer();
              SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "0x%04X", *pUint16);
              strRet = buff;
            }
            break;
        }
        break;
      case CAP_DEVICEEVENT:
        switch(*pUint16)
        {
          case TWDE_CUSTOMEVENTS:
            strRet = "TWDE_CUSTOMEVENTS";
            break;
          case TWDE_CHECKAUTOMATICCAPTURE:
            strRet = "TWDE_CHECKAUTOMATICCAPTURE";
            break;
          case TWDE_CHECKBATTERY:
            strRet = "TWDE_CHECKBATTERY";
            break;
          case TWDE_CHECKDEVICEONLINE:
            strRet = "TWDE_CHECKDEVICEONLINE";
            break;
          case TWDE_CHECKFLASH:
            strRet = "TWDE_CHECKFLASH";
            break;
          case TWDE_CHECKPOWERSUPPLY:
            strRet = "TWDE_CHECKPOWERSUPPLY";
            break;
          case TWDE_CHECKRESOLUTION:
            strRet = "TWDE_CHECKRESOLUTION";
            break;
          case TWDE_DEVICEADDED:
            strRet = "TWDE_DEVICEADDED";
            break;
          case TWDE_DEVICEOFFLINE:
            strRet = "TWDE_DEVICEOFFLINE";
            break;
          case TWDE_DEVICEREADY:
            strRet = "TWDE_DEVICEREADY";
            break;
          case TWDE_DEVICEREMOVED:
            strRet = "TWDE_DEVICEREMOVED";
            break;
          case TWDE_IMAGECAPTURED:
            strRet = "TWDE_IMAGECAPTURED";
            break;
          case TWDE_IMAGEDELETED:
            strRet = "TWDE_IMAGEDELETED";
            break;
          case TWDE_PAPERDOUBLEFEED:
            strRet = "TWDE_PAPERDOUBLEFEED";
            break;
          case TWDE_PAPERJAM:
            strRet = "TWDE_PAPERJAM";
            break;
          case TWDE_LAMPFAILURE:
            strRet = "TWDE_LAMPFAILURE";
            break;
          case TWDE_POWERSAVE:
            strRet = "TWDE_POWERSAVE";
            break;
          case TWDE_POWERSAVENOTIFY:
            strRet = "TWDE_POWERSAVENOTIFY";
            break;
          default:
            {
              char * buff = nextTempBuffer();
              SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "0x%04X", *pUint16);
              strRet = buff;
            }
            break;
        }
        break;
      case CAP_DUPLEX:
        switch(*pUint16)
        {
          case TWDX_NONE:
            strRet = "TWDX_NONE";
            break;
          case TWDX_1PASSDUPLEX:
            strRet = "TWDX_1PASSDUPLEX";
            break;
          case TWDX_2PASSDUPLEX:
            strRet = "TWDX_2PASSDUPLEX";
            break;
          default:
            {
              char * buff = nextTempBuffer();
              SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "0x%04X", *pUint16);
              strRet = buff;
            }
            break;
        }
        break;
      case CAP_FEEDERALIGNMENT:
        switch(*pUint16)
        {
          case TWFA_NONE:
            strRet = "TWFA_NONE";
            break;
          case TWFA_LEFT:
            strRet = "TWFA_LEFT";
            break;
          case TWFA_CENTER:
            strRet = "TWFA_CENTER";
            break;
          case TWFA_RIGHT:
            strRet = "TWFA_RIGHT";
            break;
          default:
            {
              char * buff = nextTempBuffer();
              SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "0x%04X", *pUint16);
              strRet = buff;
            }
            break;
        }
        break;
      case ICAP_FEEDERTYPE:
        switch(*pUint16)
        {
          case TWFE_GENERAL:
            strRet = "TWFE_GENERAL";
            break;
          case TWFE_PHOTO:
            strRet = "TWFE_PHOTO";
            break;
          default:
            {
              char * buff = nextTempBuffer();
              SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "0x%04X", *pUint16);
              strRet = buff;
            }
            break;
        }
        break;
      case ICAP_IMAGEFILEFORMAT:
        switch(*pUint16)
        {
          case TWFF_TIFF:
            strRet = "TWFF_TIFF";
            break;
          case TWFF_PICT:
            strRet = "TWFF_PICT";
            break;
          case TWFF_BMP:
            strRet = "TWFF_BMP";
            break;
          case TWFF_XBM:
            strRet = "TWFF_XBM";
            break;
          case TWFF_JFIF:
            strRet = "TWFF_JFIF";
            break;
          case TWFF_FPX:
            strRet = "TWFF_FPX";
            break;
          case TWFF_TIFFMULTI:
            strRet = "TWFF_TIFFMULTI";
            break;
          case TWFF_PNG:
            strRet = "TWFF_PNG";
            break;
          case TWFF_SPIFF:
            strRet = "TWFF_SPIFF";
            break;
          case TWFF_EXIF:
            strRet = "TWFF_EXIF";
            break;
          case TWFF_PDF:
            strRet = "TWFF_PDF";
            break;
          case TWFF_JP2:
            strRet = "TWFF_JP2";
            break;
          case TWFF_JPN:
            strRet = "TWFF_JPN";
            break;
          case TWFF_JPX:
            strRet = "TWFF_JPX";
            break;
          case TWFF_DEJAVU:
            strRet = "TWFF_DEJAVU";
            break;
          case TWFF_PDFA:
            strRet = "TWFF_PDFA";
            break;
          case TWFF_PDFA2:
            strRet = "TWFF_PDFA2";
            break;
          default:
            {
              char * buff = nextTempBuffer();
              SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "0x%04X", *pUint16);
              strRet = buff;
            }
            break;
        }
        break;
      case ICAP_FLASHUSED2:
        switch(*pUint16)
        {
          case TWFL_NONE:
            strRet = "TWFL_NONE";
            break;
          case TWFL_OFF:
            strRet = "TWFL_OFF";
            break;
          case TWFL_ON:
            strRet = "TWFL_ON";
            break;
          case TWFL_AUTO:
            strRet = "TWFL_AUTO";
            break;
          case TWFL_REDEYE:
            strRet = "TWFL_REDEYE";
            break;
          default:
            {
              char * buff = nextTempBuffer();
              SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "0x%04X", *pUint16);
              strRet = buff;
            }
            break;
        }
        break;
      case CAP_FEEDERORDER:
        switch(*pUint16)
        {
          case TWFO_FIRSTPAGEFIRST:
            strRet = "TWFO_FIRSTPAGEFIRST";
            break;
          case TWFO_LASTPAGEFIRST:
            strRet = "TWFO_LASTPAGEFIRST";
            break;
          default:
            {
              char * buff = nextTempBuffer();
              SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "0x%04X", *pUint16);
              strRet = buff;
            }
            break;
        }
        break;
      case CAP_FEEDERPOCKET:
        switch(*pUint16)
        {
          case TWFP_POCKETERROR:
            strRet = "TWFP_POCKETERROR";
            break;
          case TWFP_POCKET1:
            strRet = "TWFP_POCKET1";
            break;
          case TWFP_POCKET2:
            strRet = "TWFP_POCKET2";
            break;
          case TWFP_POCKET3:
            strRet = "TWFP_POCKET3";
            break;
          case TWFP_POCKET4:
            strRet = "TWFP_POCKET4";
            break;
          case TWFP_POCKET5:
            strRet = "TWFP_POCKET5";
            break;
          case TWFP_POCKET6:
            strRet = "TWFP_POCKET6";
            break;
          case TWFP_POCKET7:
            strRet = "TWFP_POCKET7";
            break;
          case TWFP_POCKET8:
            strRet = "TWFP_POCKET8";
            break;
          case TWFP_POCKET9:
            strRet = "TWFP_POCKET9";
            break;
          case TWFP_POCKET10:
            strRet = "TWFP_POCKET10";
            break;
          case TWFP_POCKET11:
            strRet = "TWFP_POCKET11";
            break;
          case TWFP_POCKET12:
            strRet = "TWFP_POCKET12";
            break;
          case TWFP_POCKET13:
            strRet = "TWFP_POCKET13";
            break;
          case TWFP_POCKET14:
            strRet = "TWFP_POCKET14";
            break;
          case TWFP_POCKET15:
            strRet = "TWFP_POCKET15";
            break;
          case TWFP_POCKET16:
            strRet = "TWFP_POCKET16";
            break;
          default:
            {
              char * buff = nextTempBuffer();
              SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "0x%04X", *pUint16);
              strRet = buff;
            }
            break;
        }
        break;
      case ICAP_FLIPROTATION:
        switch(*pUint16)
        {
          case TWFR_BOOK:
            strRet = "TWFR_BOOK";
            break;
          case TWFR_FANFOLD:
            strRet = "TWFR_FANFOLD";
            break;
          default:
            {
              char * buff = nextTempBuffer();
              SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "0x%04X", *pUint16);
              strRet = buff;
            }
            break;
        }
        break;
      case ICAP_FILTER:
        switch(*pUint16)
        {
          case TWFT_RED:
            strRet = "TWFT_RED";
            break;
          case TWFT_GREEN:
            strRet = "TWFT_GREEN";
            break;
          case TWFT_BLUE:
            strRet = "TWFT_BLUE";
            break;
          case TWFT_NONE:
            strRet = "TWFT_NONE";
            break;
          case TWFT_WHITE:
            strRet = "TWFT_WHITE";
            break;
          case TWFT_CYAN:
            strRet = "TWFT_CYAN";
            break;
          case TWFT_MAGENTA:
            strRet = "TWFT_MAGENTA";
            break;
          case TWFT_YELLOW:
            strRet = "TWFT_YELLOW";
            break;
          case TWFT_BLACK:
            strRet = "TWFT_BLACK";
            break;
          default:
            {
              char * buff = nextTempBuffer();
              SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "0x%04X", *pUint16);
              strRet = buff;
            }
            break;
        }
        break;
      case ICAP_ICCPROFILE:
        switch(*pUint16)
        {
          case TWIC_NONE:
            strRet = "TWIC_NONE";
            break;
          case TWIC_LINK:
            strRet = "TWIC_LINK";
            break;
          case TWIC_EMBED:
            strRet = "TWIC_EMBED";
            break;
          default:
            {
              char * buff = nextTempBuffer();
              SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "0x%04X", *pUint16);
              strRet = buff;
            }
            break;
        }
        break;
      case ICAP_IMAGEFILTER:
        switch(*pUint16)
        {
          case TWIF_NONE:
            strRet = "TWIF_NONE";
            break;
          case TWIF_AUTO:
            strRet = "TWIF_AUTO";
            break;
          case TWIF_LOWPASS:
            strRet = "TWIF_LOWPASS";
            break;
          case TWIF_BANDPASS:
            strRet = "TWIF_BANDPASS";
            break;
          case TWIF_HIGHPASS:
            strRet = "TWIF_HIGHPASS";
            break;
  //        case TWIF_TEXT:
  //          strRet = "TWIF_TEXT";
  //          break;
  //        case TWIF_FINELINE:
  //          strRet = "TWIF_FINELINE";
  //          break;
          default:
            {
              char * buff = nextTempBuffer();
              SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "0x%04X", *pUint16);
              strRet = buff;
            }
            break;
        }
        break;
      case ICAP_IMAGEMERGE:
        switch(*pUint16)
        {
          case TWIM_NONE:
            strRet = "TWIM_NONE";
            break;
          case TWIM_FRONTONTOP:
            strRet = "TWIM_FRONTONTOP";
            break;
          case TWIM_FRONTONBOTTOM:
            strRet = "TWIM_FRONTONBOTTOM";
            break;
          case TWIM_FRONTONLEFT:
            strRet = "TWIM_FRONTONLEFT";
            break;
          case TWIM_FRONTONRIGHT:
            strRet = "TWIM_FRONTONRIGHT";
            break;
          default:
            {
              char * buff = nextTempBuffer();
              SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "0x%04X", *pUint16);
              strRet = buff;
            }
            break;
        }
        break;
      case CAP_JOBCONTROL:
        switch(*pUint16)
        {
          case TWJC_NONE:
            strRet = "TWJC_NONE";
            break;
          case TWJC_JSIC:
            strRet = "TWJC_JSIC";
            break;
          case TWJC_JSIS:
            strRet = "TWJC_JSIS";
            break;
          case TWJC_JSXC:
            strRet = "TWJC_JSXC";
            break;
          case TWJC_JSXS:
            strRet = "TWJC_JSXS";
            break;
          default:
            {
              char * buff = nextTempBuffer();
              SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "0x%04X", *pUint16);
              strRet = buff;
            }
            break;
        }
        break;
      case ICAP_JPEGQUALITY:
        switch(*pUint16)
        {
          case TWJQ_UNKNOWN:
            strRet = "TWJQ_UNKNOWN";
            break;
          case TWJQ_LOW:
            strRet = "TWJQ_LOW";
            break;
          case TWJQ_MEDIUM:
            strRet = "TWJQ_MEDIUM";
            break;
          case TWJQ_HIGH:
            strRet = "TWJQ_HIGH";
            break;
          default:
            {
              char * buff = nextTempBuffer();
              SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "0x%04X", *pUint16);
              strRet = buff;
            }
            break;
        }
        break;
      case ICAP_LIGHTPATH:
        switch(*pUint16)
        {
          case TWLP_REFLECTIVE:
            strRet = "TWLP_REFLECTIVE";
            break;
          case TWLP_TRANSMISSIVE:
            strRet = "TWLP_TRANSMISSIVE";
            break;
          default:
            {
              char * buff = nextTempBuffer();
              SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "0x%04X", *pUint16);
              strRet = buff;
            }
            break;
        }
        break;
      case ICAP_LIGHTSOURCE:
        switch(*pUint16)
        {
          case TWLS_RED:
            strRet = "TWLS_RED";
            break;
          case TWLS_GREEN:
            strRet = "TWLS_GREEN";
            break;
          case TWLS_BLUE:
            strRet = "TWLS_BLUE";
            break;
          case TWLS_NONE:
            strRet = "TWLS_NONE";
            break;
          case TWLS_WHITE:
            strRet = "TWLS_WHITE";
            break;
          case TWLS_UV:
            strRet = "TWLS_UV";
            break;
          case TWLS_IR:
            strRet = "TWLS_IR";
            break;
          default:
            {
              char * buff = nextTempBuffer();
              SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "0x%04X", *pUint16);
              strRet = buff;
            }
            break;
        }
        break;
      case ICAP_NOISEFILTER:
        switch(*pUint16)
        {
          case TWNF_NONE:
            strRet = "TWNF_NONE";
            break;
          case TWNF_AUTO:
            strRet = "TWNF_AUTO";
            break;
          case TWNF_LONEPIXEL:
            strRet = "TWNF_LONEPIXEL";
            break;
          case TWNF_MAJORITYRULE:
            strRet = "TWNF_MAJORITYRULE";
            break;
          default:
            {
              char * buff = nextTempBuffer();
              SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "0x%04X", *pUint16);
              strRet = buff;
            }
            break;
        }
        break;
      case ICAP_ORIENTATION:
        switch(*pUint16)
        {
  //        case TWOR_ROT0:
  //          strRet = "TWOR_ROT0";
  //          break;
          case TWOR_ROT90:
            strRet = "TWOR_ROT90";
            break;
          case TWOR_ROT180:
            strRet = "TWOR_ROT180";
            break;
  //        case TWOR_ROT270:
  //          strRet = "TWOR_ROT270";
  //          break;
          case TWOR_PORTRAIT:
            strRet = "TWOR_PORTRAIT";
            break;
          case TWOR_LANDSCAPE:
            strRet = "TWOR_LANDSCAPE";
            break;
          case TWOR_AUTO:
            strRet = "TWOR_AUTO";
            break;
          case TWOR_AUTOTEXT:
            strRet = "TWOR_AUTOTEXT";
            break;
          case TWOR_AUTOPICTURE:
            strRet = "TWOR_AUTOPICTURE";
            break;
          default:
            {
              char * buff = nextTempBuffer();
              SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "0x%04X", *pUint16);
              strRet = buff;
            }
            break;
        }
        break;
      case ICAP_OVERSCAN:
        switch(*pUint16)
        {
          case TWOV_NONE:
            strRet = "TWOV_NONE";
            break;
          case TWOV_AUTO:
            strRet = "TWOV_AUTO";
            break;
          case TWOV_TOPBOTTOM:
            strRet = "TWOV_TOPBOTTOM";
            break;
          case TWOV_LEFTRIGHT:
            strRet = "TWOV_LEFTRIGHT";
            break;
          case TWOV_ALL:
            strRet = "TWOV_ALL";
            break;
          default:
            {
              char * buff = nextTempBuffer();
              SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "0x%04X", *pUint16);
              strRet = buff;
            }
            break;
        }
        break;
      case ICAP_PLANARCHUNKY:
        switch(*pUint16)
        {
          case TWPC_CHUNKY:
            strRet = "TWPC_CHUNKY";
            break;
          case TWPC_PLANAR:
            strRet = "TWPC_PLANAR";
            break;
          default:
            {
              char * buff = nextTempBuffer();
              SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "0x%04X", *pUint16);
              strRet = buff;
            }
            break;
        }
        break;
      case ICAP_PIXELFLAVOR:
        switch(*pUint16)
        {
          case TWPF_CHOCOLATE:
            strRet = "TWPF_CHOCOLATE";
            break;
          case TWPF_VANILLA:
            strRet = "TWPF_VANILLA";
            break;
          default:
            {
              char * buff = nextTempBuffer();
              SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "0x%04X", *pUint16);
              strRet = buff;
            }
            break;
        }
        break;
      case CAP_PRINTERMODE:
        switch(*pUint16)
        {
          case TWPM_SINGLESTRING:
            strRet = "TWPM_SINGLESTRING";
            break;
          case TWPM_MULTISTRING:
            strRet = "TWPM_MULTISTRING";
            break;
          case TWPM_COMPOUNDSTRING:
            strRet = "TWPM_COMPOUNDSTRING";
            break;
          default:
            {
              char * buff = nextTempBuffer();
              SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "0x%04X", *pUint16);
              strRet = buff;
            }
            break;
        }
        break;
      case CAP_PRINTER:
        switch(*pUint16)
        {
          case TWPR_IMPRINTERTOPBEFORE:
            strRet = "TWPR_IMPRINTERTOPBEFORE";
            break;
          case TWPR_IMPRINTERTOPAFTER:
            strRet = "TWPR_IMPRINTERTOPAFTER";
            break;
          case TWPR_IMPRINTERBOTTOMBEFORE:
            strRet = "TWPR_IMPRINTERBOTTOMBEFORE";
            break;
          case TWPR_IMPRINTERBOTTOMAFTER:
            strRet = "TWPR_IMPRINTERBOTTOMAFTER";
            break;
          case TWPR_ENDORSERTOPBEFORE:
            strRet = "TWPR_ENDORSERTOPBEFORE";
            break;
          case TWPR_ENDORSERTOPAFTER:
            strRet = "TWPR_ENDORSERTOPAFTER";
            break;
          case TWPR_ENDORSERBOTTOMBEFORE:
            strRet = "TWPR_ENDORSERBOTTOMBEFORE";
            break;
          case TWPR_ENDORSERBOTTOMAFTER:
            strRet = "TWPR_ENDORSERBOTTOMAFTER";
            break;
          default:
            {
              char * buff = nextTempBuffer();
              SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "0x%04X", *pUint16);
              strRet = buff;
            }
            break;
        }
        break;
      case CAP_POWERSUPPLY:
        switch(*pUint16)
        {
          case TWPS_EXTERNAL:
            strRet = "TWPS_EXTERNAL";
            break;
          case TWPS_BATTERY:
            strRet = "TWPS_BATTERY";
            break;
          default:
            {
              char * buff = nextTempBuffer();
              SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "0x%04X", *pUint16);
              strRet = buff;
            }
            break;
        }
        break;
      case ICAP_PIXELTYPE:
        switch(*pUint16)
        {
          case TWPT_BW:
            strRet = "TWPT_BW";
            break;
          case TWPT_GRAY:
            strRet = "TWPT_GRAY";
            break;
          case TWPT_RGB:
            strRet = "TWPT_RGB";
            break;
          case TWPT_PALETTE:
            strRet = "TWPT_PALETTE";
            break;
          case TWPT_CMY:
            strRet = "TWPT_CMY";
            break;
          case TWPT_CMYK:
            strRet = "TWPT_CMYK";
            break;
          case TWPT_YUV:
            strRet = "TWPT_YUV";
            break;
          case TWPT_YUVK:
            strRet = "TWPT_YUVK";
            break;
          case TWPT_CIEXYZ:
            strRet = "TWPT_CIEXYZ";
            break;
          case TWPT_LAB:
            strRet = "TWPT_LAB";
            break;
          case TWPT_SRGB:
            strRet = "TWPT_SRGB";
            break;
          case TWPT_SCRGB:
            strRet = "TWPT_SCRGB";
            break;
          case TWPT_INFRARED:
            strRet = "TWPT_INFRARED";
            break;
          default:
            {
              char * buff = nextTempBuffer();
              SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "0x%04X", *pUint16);
              strRet = buff;
            }
            break;
        }
        break;
    case CAP_SEGMENTED:
        switch(*pUint16)
        {
          case TWSG_NONE:
            strRet = "TWSG_NONE";
            break;
          case TWSG_AUTO:
            strRet = "TWSG_AUTO";
            break;
          default:
            {
              char * buff = nextTempBuffer();
              SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "0x%04X", *pUint16);
              strRet = buff;
            }
            break;
        }
        break;
    case ICAP_SUPPORTEDSIZES:
        switch(*pUint16)
        {
          case TWSS_NONE:
            strRet = "TWSS_NONE";
            break;
          case TWSS_A4:
            strRet = "TWSS_A4";
            break;
          case TWSS_JISB5:
            strRet = "TWSS_JISB5";
            break;
          case TWSS_USLETTER:
            strRet = "TWSS_USLETTER";
            break;
          case TWSS_USLEGAL:
            strRet = "TWSS_USLEGAL";
            break;
          case TWSS_A5:
            strRet = "TWSS_A5";
            break;
          case TWSS_ISOB4:
            strRet = "TWSS_ISOB4";
            break;
          case TWSS_ISOB6:
            strRet = "TWSS_ISOB6";
            break;
          case TWSS_USLEDGER:
            strRet = "TWSS_USLEDGER";
            break;
          case TWSS_USEXECUTIVE:
            strRet = "TWSS_USEXECUTIVE";
            break;
          case TWSS_A3:
            strRet = "TWSS_A3";
            break;
          case TWSS_ISOB3:
            strRet = "TWSS_ISOB3";
            break;
          case TWSS_A6:
            strRet = "TWSS_A6";
            break;
          case TWSS_C4:
            strRet = "TWSS_C4";
            break;
          case TWSS_C5:
            strRet = "TWSS_C5";
            break;
          case TWSS_C6:
            strRet = "TWSS_C6";
            break;
          case TWSS_4A0:
            strRet = "TWSS_4A0";
            break;
          case TWSS_2A0:
            strRet = "TWSS_2A0";
            break;
          case TWSS_A0:
            strRet = "TWSS_A0";
            break;
          case TWSS_A1:
            strRet = "TWSS_A1";
            break;
          case TWSS_A2:
            strRet = "TWSS_A2";
            break;
          case TWSS_A7:
            strRet = "TWSS_A7";
            break;
          case TWSS_A8:
            strRet = "TWSS_A8";
            break;
          case TWSS_A9:
            strRet = "TWSS_A9";
            break;
          case TWSS_A10:
            strRet = "TWSS_A10";
            break;
          case TWSS_ISOB0:
            strRet = "TWSS_ISOB0";
            break;
          case TWSS_ISOB1:
            strRet = "TWSS_ISOB1";
            break;
          case TWSS_ISOB2:
            strRet = "TWSS_ISOB2";
            break;
          case TWSS_ISOB5:
            strRet = "TWSS_ISOB5";
            break;
          case TWSS_ISOB7:
            strRet = "TWSS_ISOB7";
            break;
          case TWSS_ISOB8:
            strRet = "TWSS_ISOB8";
            break;
          case TWSS_ISOB9:
            strRet = "TWSS_ISOB9";
            break;
          case TWSS_ISOB10:
            strRet = "TWSS_ISOB10";
            break;
          case TWSS_JISB0:
            strRet = "TWSS_JISB0";
            break;
          case TWSS_JISB1:
            strRet = "TWSS_JISB1";
            break;
          case TWSS_JISB2:
            strRet = "TWSS_JISB2";
            break;
          case TWSS_JISB3:
            strRet = "TWSS_JISB3";
            break;
          case TWSS_JISB4:
            strRet = "TWSS_JISB4";
            break;
          case TWSS_JISB6:
            strRet = "TWSS_JISB6";
            break;
          case TWSS_JISB7:
            strRet = "TWSS_JISB7";
            break;
          case TWSS_JISB8:
            strRet = "TWSS_JISB8";
            break;
          case TWSS_JISB9:
            strRet = "TWSS_JISB9";
            break;
          case TWSS_JISB10:
            strRet = "TWSS_JISB10";
            break;
          case TWSS_C0:
            strRet = "TWSS_C0";
            break;
          case TWSS_C1:
            strRet = "TWSS_C1";
            break;
          case TWSS_C2:
            strRet = "TWSS_C2";
            break;
          case TWSS_C3:
            strRet = "TWSS_C3";
            break;
          case TWSS_C7:
            strRet = "TWSS_C7";
            break;
          case TWSS_C8:
            strRet = "TWSS_C8";
            break;
          case TWSS_C9:
            strRet = "TWSS_C9";
            break;
          case TWSS_C10:
            strRet = "TWSS_C10";
            break;
          case TWSS_USSTATEMENT:
            strRet = "TWSS_USSTATEMENT";
            break;
          case TWSS_BUSINESSCARD:
            strRet = "TWSS_BUSINESSCARD";
            break;
          case TWSS_MAXSIZE:
            strRet = "TWSS_MAXSIZE";
            break;
          default:
            {
              char * buff = nextTempBuffer();
              SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "0x%04X", *pUint16);
              strRet = buff;
            }
            break;
        }
        break;
    case ICAP_XFERMECH:
        switch(*pUint16)
        {
          case TWSX_NATIVE:
            strRet = "TWSX_NATIVE";
            break;
          case TWSX_FILE:
            strRet = "TWSX_FILE";
            break;
          case TWSX_MEMORY:
            strRet = "TWSX_MEMORY";
            break;
          case TWSX_MEMFILE:
            strRet = "TWSX_MEMFILE";
            break;
          default:
            {
              char * buff = nextTempBuffer();
              SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "0x%04X", *pUint16);
              strRet = buff;
            }
            break;
        }
        break;
    case ICAP_UNITS:
        switch(*pUint16)
        {
          case TWUN_INCHES:
            strRet = "TWUN_INCHES";
            break;
          case TWUN_CENTIMETERS:
            strRet = "TWUN_CENTIMETERS";
            break;
          case TWUN_PICAS:
            strRet = "TWUN_PICAS";
            break;
          case TWUN_POINTS:
            strRet = "TWUN_POINTS";
            break;
          case TWUN_TWIPS:
            strRet = "TWUN_TWIPS";
            break;
          case TWUN_PIXELS:
            strRet = "TWUN_PIXELS";
            break;
          case TWUN_MILLIMETERS:
            strRet = "TWUN_MILLIMETERS";
            break;
          default:
            {
              char * buff = nextTempBuffer();
              SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "0x%04X", *pUint16);
              strRet = buff;
            }
            break;
        }
        break;
      case CAP_LANGUAGE:
        switch(*pUint16)
        {
          case TWLG_USERLOCALE:
            strRet = "TWLG_USERLOCALE";
            break;
          case TWLG_DANISH:
            strRet = "TWLG_DANISH";
            break;
          case TWLG_DUTCH:
            strRet = "TWLG_DUTCH";
            break;
          case TWLG_ENGLISH:
            strRet = "TWLG_ENGLISH";
            break;
          case TWLG_FRENCH_CANADIAN:
            strRet = "TWLG_FRENCH_CANADIAN";
            break;
          case TWLG_FINNISH:
            strRet = "TWLG_FINNISH";
            break;
          case TWLG_FRENCH:
            strRet = "TWLG_FRENCH";
            break;
          case TWLG_GERMAN:
            strRet = "TWLG_GERMAN";
            break;
          case TWLG_ICELANDIC:
            strRet = "TWLG_ICELANDIC";
            break;
          case TWLG_ITALIAN:
            strRet = "TWLG_ITALIAN";
            break;
          case TWLG_NORWEGIAN:
            strRet = "TWLG_NORWEGIAN";
            break;
          case TWLG_PORTUGUESE:
            strRet = "TWLG_PORTUGUESE";
            break;
          case TWLG_SPANISH:
            strRet = "TWLG_SPANISH";
            break;
          case TWLG_SWEDISH:
            strRet = "TWLG_SWEDISH";
            break;
          case TWLG_ENGLISH_USA:
            strRet = "TWLG_ENGLISH_USA";
            break;
          case TWLG_AFRIKAANS:
            strRet = "TWLG_AFRIKAANS";
            break;
          case TWLG_ALBANIA:
            strRet = "TWLG_ALBANIA";
            break;
          case TWLG_ARABIC:
            strRet = "TWLG_ARABIC";
            break;
          case TWLG_ARABIC_ALGERIA:
            strRet = "TWLG_ARABIC_ALGERIA";
            break;
          case TWLG_ARABIC_BAHRAIN:
            strRet = "TWLG_ARABIC_BAHRAIN";
            break;
          case TWLG_ARABIC_EGYPT:
            strRet = "TWLG_ARABIC_EGYPT";
            break;
          case TWLG_ARABIC_IRAQ:
            strRet = "TWLG_ARABIC_IRAQ";
            break;
          case TWLG_ARABIC_JORDAN:
            strRet = "TWLG_ARABIC_JORDAN";
            break;
          case TWLG_ARABIC_KUWAIT:
            strRet = "TWLG_ARABIC_KUWAIT";
            break;
          case TWLG_ARABIC_LEBANON:
            strRet = "TWLG_ARABIC_LEBANON";
            break;
          case TWLG_ARABIC_LIBYA:
            strRet = "TWLG_ARABIC_LIBYA";
            break;
          case TWLG_ARABIC_MOROCCO:
            strRet = "TWLG_ARABIC_MOROCCO";
            break;
          case TWLG_ARABIC_OMAN:
            strRet = "TWLG_ARABIC_OMAN";
            break;
          case TWLG_ARABIC_QATAR:
            strRet = "TWLG_ARABIC_QATAR";
            break;
          case TWLG_ARABIC_SAUDIARABIA:
            strRet = "TWLG_ARABIC_SAUDIARABIA";
            break;
          case TWLG_ARABIC_SYRIA:
            strRet = "TWLG_ARABIC_SYRIA";
            break;
          case TWLG_ARABIC_TUNISIA:
            strRet = "TWLG_ARABIC_TUNISIA";
            break;
          case TWLG_ARABIC_UAE:
            strRet = "TWLG_ARABIC_UAE";
            break;
          case TWLG_ARABIC_YEMEN:
            strRet = "TWLG_ARABIC_YEMEN";
            break;
          case TWLG_BASQUE:
            strRet = "TWLG_BASQUE";
            break;
          case TWLG_BYELORUSSIAN:
            strRet = "TWLG_BYELORUSSIAN";
            break;
          case TWLG_BULGARIAN:
            strRet = "TWLG_BULGARIAN";
            break;
          case TWLG_CATALAN:
            strRet = "TWLG_CATALAN";
            break;
          case TWLG_CHINESE:
            strRet = "TWLG_CHINESE";
            break;
          case TWLG_CHINESE_HONGKONG:
            strRet = "TWLG_CHINESE_HONGKONG";
            break;
          case TWLG_CHINESE_PRC:
            strRet = "TWLG_CHINESE_PRC";
            break;
          case TWLG_CHINESE_SINGAPORE:
            strRet = "TWLG_CHINESE_SINGAPORE";
            break;
          case TWLG_CHINESE_SIMPLIFIED:
            strRet = "TWLG_CHINESE_SIMPLIFIED";
            break;
          case TWLG_CHINESE_TAIWAN:
            strRet = "TWLG_CHINESE_TAIWAN";
            break;
          case TWLG_CHINESE_TRADITIONAL:
            strRet = "TWLG_CHINESE_TRADITIONAL";
            break;
          case TWLG_CROATIA:
            strRet = "TWLG_CROATIA";
            break;
          case TWLG_CZECH:
            strRet = "TWLG_CZECH";
            break;
          case TWLG_DUTCH_BELGIAN:
            strRet = "TWLG_DUTCH_BELGIAN";
            break;
          case TWLG_ENGLISH_AUSTRALIAN:
            strRet = "TWLG_ENGLISH_AUSTRALIAN";
            break;
          case TWLG_ENGLISH_CANADIAN:
            strRet = "TWLG_ENGLISH_CANADIAN";
            break;
          case TWLG_ENGLISH_IRELAND:
            strRet = "TWLG_ENGLISH_IRELAND";
            break;
          case TWLG_ENGLISH_NEWZEALAND:
            strRet = "TWLG_ENGLISH_NEWZEALAND";
            break;
          case TWLG_ENGLISH_SOUTHAFRICA:
            strRet = "TWLG_ENGLISH_SOUTHAFRICA";
            break;
          case TWLG_ENGLISH_UK:
            strRet = "TWLG_ENGLISH_UK";
            break;
          case TWLG_ESTONIAN:
            strRet = "TWLG_ESTONIAN";
            break;
          case TWLG_FAEROESE:
            strRet = "TWLG_FAEROESE";
            break;
          case TWLG_FARSI:
            strRet = "TWLG_FARSI";
            break;
          case TWLG_FRENCH_BELGIAN:
            strRet = "TWLG_FRENCH_BELGIAN";
            break;
          case TWLG_FRENCH_LUXEMBOURG:
            strRet = "TWLG_FRENCH_LUXEMBOURG";
            break;
          case TWLG_FRENCH_SWISS:
            strRet = "TWLG_FRENCH_SWISS";
            break;
          case TWLG_GERMAN_AUSTRIAN:
            strRet = "TWLG_GERMAN_AUSTRIAN";
            break;
          case TWLG_GERMAN_LUXEMBOURG:
            strRet = "TWLG_GERMAN_LUXEMBOURG";
            break;
          case TWLG_GERMAN_LIECHTENSTEIN:
            strRet = "TWLG_GERMAN_LIECHTENSTEIN";
            break;
          case TWLG_GERMAN_SWISS:
            strRet = "TWLG_GERMAN_SWISS";
            break;
          case TWLG_GREEK:
            strRet = "TWLG_GREEK";
            break;
          case TWLG_HEBREW:
            strRet = "TWLG_HEBREW";
            break;
          case TWLG_HUNGARIAN:
            strRet = "TWLG_HUNGARIAN";
            break;
          case TWLG_INDONESIAN:
            strRet = "TWLG_INDONESIAN";
            break;
          case TWLG_ITALIAN_SWISS:
            strRet = "TWLG_ITALIAN_SWISS";
            break;
          case TWLG_JAPANESE:
            strRet = "TWLG_JAPANESE";
            break;
          case TWLG_KOREAN:
            strRet = "TWLG_KOREAN";
            break;
          case TWLG_KOREAN_JOHAB:
            strRet = "TWLG_KOREAN_JOHAB";
            break;
          case TWLG_LATVIAN:
            strRet = "TWLG_LATVIAN";
            break;
          case TWLG_LITHUANIAN:
            strRet = "TWLG_LITHUANIAN";
            break;
          case TWLG_NORWEGIAN_BOKMAL:
            strRet = "TWLG_NORWEGIAN_BOKMAL";
            break;
          case TWLG_NORWEGIAN_NYNORSK:
            strRet = "TWLG_NORWEGIAN_NYNORSK";
            break;
          case TWLG_POLISH:
            strRet = "TWLG_POLISH";
            break;
          case TWLG_PORTUGUESE_BRAZIL:
            strRet = "TWLG_PORTUGUESE_BRAZIL";
            break;
          case TWLG_ROMANIAN:
            strRet = "TWLG_ROMANIAN";
            break;
          case TWLG_RUSSIAN:
            strRet = "TWLG_RUSSIAN";
            break;
          case TWLG_SERBIAN_LATIN:
            strRet = "TWLG_SERBIAN_LATIN";
            break;
          case TWLG_SLOVAK:
            strRet = "TWLG_SLOVAK";
            break;
          case TWLG_SLOVENIAN:
            strRet = "TWLG_SLOVENIAN";
            break;
          case TWLG_SPANISH_MEXICAN:
            strRet = "TWLG_SPANISH_MEXICAN";
            break;
          case TWLG_SPANISH_MODERN:
            strRet = "TWLG_SPANISH_MODERN";
            break;
          case TWLG_THAI:
            strRet = "TWLG_THAI";
            break;
          case TWLG_TURKISH:
            strRet = "TWLG_TURKISH";
            break;
          case TWLG_UKRANIAN:
            strRet = "TWLG_UKRANIAN";
            break;
          case TWLG_ASSAMESE:
            strRet = "TWLG_ASSAMESE";
            break;
          case TWLG_BENGALI:
            strRet = "TWLG_BENGALI";
            break;
          case TWLG_BIHARI:
            strRet = "TWLG_BIHARI";
            break;
          case TWLG_BODO:
            strRet = "TWLG_BODO";
            break;
          case TWLG_DOGRI:
            strRet = "TWLG_DOGRI";
            break;
          case TWLG_GUJARATI:
            strRet = "TWLG_GUJARATI";
            break;
          case TWLG_HARYANVI:
            strRet = "TWLG_HARYANVI";
            break;
          case TWLG_HINDI:
            strRet = "TWLG_HINDI";
            break;
          case TWLG_KANNADA:
            strRet = "TWLG_KANNADA";
            break;
          case TWLG_KASHMIRI:
            strRet = "TWLG_KASHMIRI";
            break;
          case TWLG_MALAYALAM:
            strRet = "TWLG_MALAYALAM";
            break;
          case TWLG_MARATHI:
            strRet = "TWLG_MARATHI";
            break;
          case TWLG_MARWARI:
            strRet = "TWLG_MARWARI";
            break;
          case TWLG_MEGHALAYAN:
            strRet = "TWLG_MEGHALAYAN";
            break;
          case TWLG_MIZO:
            strRet = "TWLG_MIZO";
            break;
          case TWLG_NAGA:
            strRet = "TWLG_NAGA";
            break;
          case TWLG_ORISSI:
            strRet = "TWLG_ORISSI";
            break;
          case TWLG_PUNJABI:
            strRet = "TWLG_PUNJABI";
            break;
          case TWLG_PUSHTU:
            strRet = "TWLG_PUSHTU";
            break;
          case TWLG_SERBIAN_CYRILLIC:
            strRet = "TWLG_SERBIAN_CYRILLIC";
            break;
          case TWLG_SIKKIMI:
            strRet = "TWLG_SIKKIMI";
            break;
          case TWLG_SWEDISH_FINLAND:
            strRet = "TWLG_SWEDISH_FINLAND";
            break;
          case TWLG_TAMIL:
            strRet = "TWLG_TAMIL";
            break;
          case TWLG_TELUGU:
            strRet = "TWLG_TELUGU";
            break;
          case TWLG_TRIPURI:
            strRet = "TWLG_TRIPURI";
            break;
          case TWLG_URDU:
            strRet = "TWLG_URDU";
            break;
          case TWLG_VIETNAMESE:
           strRet = "TWLG_VIETNAMESE";
            break;
          default:
            {
              char * buff = nextTempBuffer();
              SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "0x%04X", *pUint16);
              strRet = buff;
            }
            break;
        }
        break;
      default:
        {
          char * buff = nextTempBuffer();
          SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "Un-recognized Capability 0x%04X", twCapId);
          strRet = buff;
        }
        break;
    }
  }
  return strRet;
}

const char *convertFileType_toString(const TW_UINT16 _nItem)
{
  const char *strRet = NULL;
  switch(_nItem)
  {
    case TWFY_CAMERA:
      strRet = "TWFY_CAMERA";
      break;
    case TWFY_CAMERATOP:
      strRet = "TWFY_CAMERATOP";
      break;
    case TWFY_CAMERABOTTOM:
      strRet = "TWFY_CAMERABOTTOM";
      break;
    case TWFY_CAMERAPREVIEW:
      strRet = "TWFY_CAMERAPREVIEW";
      break;
    case TWFY_DOMAIN:
      strRet = "TWFY_DOMAIN";
      break;
    case TWFY_HOST:
      strRet = "TWFY_HOST";
      break;
    case TWFY_DIRECTORY:
      strRet = "TWFY_DIRECTORY";
      break;
    case TWFY_IMAGE:
      strRet = "TWFY_IMAGE";
      break;
    case TWFY_UNKNOWN:
      strRet = "TWFY_UNKNOWN";
      break;
    default:
      {
        char * buff = nextTempBuffer();
        SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "Un-recognized FileType 0x%04X", _nItem);
        strRet = buff;
      }
      break;
  }
  return strRet;
}

const char *convertFileFormat_toString(const TW_UINT16 _nItem)
{
  return CapabilityValueToString(ICAP_IMAGEFILEFORMAT, TWTY_UINT16, &_nItem);
}

const char *convertEOJ_toString(const TW_UINT16 _nItem)
{
  const char *strRet = NULL;
  switch(_nItem)
  {
    case TWEJ_NONE:
      strRet = "TWEJ_NONE";
      break;
    case TWEJ_MIDSEPARATOR:
      strRet = "TWEJ_MIDSEPARATOR";
      break;
    case TWEJ_PATCH1:
      strRet = "TWEJ_PATCH1";
      break;
    case TWEJ_PATCH2:
      strRet = "TWEJ_PATCH2";
      break;
    case TWEJ_PATCH3:
      strRet = "TWEJ_PATCH3";
      break;
    case TWEJ_PATCH4:
      strRet = "TWEJ_PATCH4";
      break;
    case TWEJ_PATCH6:
      strRet = "TWEJ_PATCH6";
      break;
    case TWEJ_PATCHT:
      strRet = "TWEJ_PATCHT";
      break;
    default:
      {
        char * buff = nextTempBuffer();
        SSNPRINTF(buff, TEMPBUFSIZE, TEMPBUFSIZE, "Un-recognized FileType 0x%04X", _nItem);
        strRet = buff;
      }
      break;
  }
  return strRet;
}

