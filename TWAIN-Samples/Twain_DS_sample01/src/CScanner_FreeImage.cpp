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
* @file CScanner_FreeImage.cpp
* Defines a scanner.
* Create a virtual scanner.
* @author TWAIN Working Group
* @date April 2007
*/


#include "CScanner_FreeImage.h"

#include <iostream>
#include <time.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#ifdef TWH_CMP_MSC
#include <io.h>
#else //#ifdef TWH_CMP_MSC
#include <sys/io.h>
#endif //#ifdef TWH_CMP_MSC


#include "DSMInterface.h"

using namespace std;

/**
* Environment vars to get the Xfer Count.  Create this enviroment Varable on your system to simulate the 
* number of pages sitting in the scanner waiting to be scanned.
*/
#define kGETENV_XFERCOUNT "CAP_XFERCOUNT"

#ifdef TWH_CMP_MSC
extern HINSTANCE   g_hinstance;
#endif

#define kTWAIN_DS_DIR         "/usr/local/lib/twain/sample2"

//////////////////////////////////////////////////////////////////////////////
void FreeImageErrorHandler(FREE_IMAGE_FORMAT fif, const char *message)
{
  cout << "\n*** ";
  cout << FreeImage_GetFormatFromFIF(fif) << " Format" << endl;
  cout << message << " ***" << endl;
  return;
}


//////////////////////////////////////////////////////////////////////////////
CScanner_FreeImage::CScanner_FreeImage() :
  m_pDIB(0),
  m_nScanLine(0),
  m_bReadOnly(false),
  m_nDestBytesPerRow(0),
  m_nDocCount(0),
  m_nSourceWidth(0),
  m_nSourceHeight(0)
{
  memset(m_szSourceImagePath, 0, PATH_MAX);

  char szTWAIN_DS_DIR[PATH_MAX];

#ifdef TWH_CMP_MSC
  GetModuleFileName(g_hinstance, szTWAIN_DS_DIR, PATH_MAX);
  // strip filename from path
  size_t x = strlen(szTWAIN_DS_DIR);
  while(x > 0)
  {
	  if(PATH_SEPERATOR == szTWAIN_DS_DIR[x-1])
	  {
		  szTWAIN_DS_DIR[x-1] = 0;
		  break;
	  }
	  --x;
  }
#else
  // In non-windows, kTWAIN_DS_DIR is set on the compiler command line
  strncpy(szTWAIN_DS_DIR, kTWAIN_DS_DIR, PATH_MAX);
#endif

  SSNPRINTF(m_szSourceImagePath, sizeof(m_szSourceImagePath), PATH_MAX, "%s%cTWAIN_logo.png", szTWAIN_DS_DIR, PATH_SEPERATOR);

  // set default caps
  resetScanner();

  FreeImage_Initialise();
  FreeImage_SetOutputMessage(FreeImageErrorHandler);
}

//////////////////////////////////////////////////////////////////////////////
CScanner_FreeImage::~CScanner_FreeImage()
{
  if(0 != m_pDIB)
  {
    FreeImage_Unload(m_pDIB);
  }

  FreeImage_DeInitialise();
}


//////////////////////////////////////////////////////////////////////////////
bool CScanner_FreeImage::resetScanner()
{
  bool bret = true;

  // Unlock the scanner 
  Unlock();

  m_nScanLine        = 0;
  m_nDestBytesPerRow = 0;

  m_nDocCount     = m_nMaxDocCount = getDocumentCount();// Reloaded the scanner with paper
  m_nPixelType    = TWPT_RGB;
  m_nPaperSource  = SFI_PAPERSOURCE_ADF;
  m_bDuplex       = false;
  m_nWidth        = 0;
  m_nHeight       = 0;
  m_fXResolution  = 200.0;
  m_fYResolution  = 200.0;

  if(0 != m_pDIB)
  {
    FreeImage_Unload(m_pDIB);
    m_pDIB = 0;
  }

  return bret;
}

//////////////////////////////////////////////////////////////////////////////
SFreeImage CScanner_FreeImage::getSetting() const
{
  return (SFreeImage)(*this);
}

//////////////////////////////////////////////////////////////////////////////
void CScanner_FreeImage::setSetting(SFreeImage settings)
{
  m_nPaperSource = settings.m_nPaperSource;
  m_bDuplex      = settings.m_bDuplex;
  m_nPixelType   = settings.m_nPixelType;
  m_nWidth       = settings.m_nWidth;
  m_nHeight      = settings.m_nHeight;
  m_fXResolution = settings.m_fXResolution;
  m_fYResolution = settings.m_fYResolution;
  m_nDocCount = m_nMaxDocCount;
}

//////////////////////////////////////////////////////////////////////////////
bool CScanner_FreeImage::acquireImage()
{
  // first, avoid a memory leak by cleaning up past images
  if(0 != m_pDIB)
  {
    FreeImage_Unload(m_pDIB);
    m_pDIB = 0;
  }

  // get the image if it exists
  if(FALSE == FILE_EXISTS(m_szSourceImagePath))
  {
    cerr << "ds: Could not find required file: " << m_szSourceImagePath << endl;
    return false;
  }

  FREE_IMAGE_FORMAT fif = FreeImage_GetFileType(m_szSourceImagePath);
  m_pDIB = FreeImage_Load(fif, m_szSourceImagePath);

  if(0 == m_pDIB)
  {
    cout << "ds: Failed - could not acquire image" << endl;
    return false;
  }

  //Document scanned, remove it from simulated intray
  m_nDocCount--;

  // do whatever tranforms to the scanned image that was requested by the app
  // before the image is sent to the app.
  if(false == preScanPrep())
  {
    return false;
  }

  return true;
}

//////////////////////////////////////////////////////////////////////////////
bool CScanner_FreeImage::preScanPrep()
{
  // convert all scanned images to 24 bit as a starting point, if they are less
  // then 24 bit.  This is because the scaling functions only work on 8/24/32
  // bit images.
  if(FreeImage_GetBPP(m_pDIB) < 24 )
  {
    FIBITMAP *pDib = FreeImage_ConvertTo24Bits(m_pDIB);
    FreeImage_Unload(m_pDIB);
    m_pDIB = pDib;
  }

  FIBITMAP *pDib = 0;

  m_nSourceWidth   = FreeImage_GetWidth(m_pDIB);
  m_nSourceHeight  = FreeImage_GetHeight(m_pDIB);
  WORD res = 0;

  res = FreeImage_GetDotsPerMeterX( m_pDIB );
  WORD unNewWidth  = WORD(m_nSourceWidth /(WORD)((float)res/39.37 + 0.5)* m_fXResolution);

  res = FreeImage_GetDotsPerMeterY( m_pDIB );
  WORD unNewHeight = WORD(m_nSourceHeight/(WORD)((float)res/39.37 + 0.5)* m_fYResolution);

  cout << "ds: rescaling... to " << unNewWidth << " x " << unNewHeight << endl;
  pDib = FreeImage_Rescale( m_pDIB, unNewWidth, unNewHeight, FILTER_BILINEAR);
  if(0 == pDib)
  {
    return false;
  }
  else
  {
    FreeImage_Unload(m_pDIB);
    m_pDIB = pDib;
  }

  if(m_nWidth <= 0 || m_nHeight <= 0)
  {
    m_nWidth  = m_nSourceWidth  = FreeImage_GetWidth(m_pDIB);
    m_nHeight = m_nSourceHeight = FreeImage_GetHeight(m_pDIB);
  }
  else
  {
    m_nSourceWidth  = FreeImage_GetWidth(m_pDIB);
    m_nSourceHeight = FreeImage_GetHeight(m_pDIB);
  }

  FreeImage_SetDotsPerMeterX( m_pDIB, WORD(m_fXResolution*39.37 + 0.5) );
  FreeImage_SetDotsPerMeterY( m_pDIB, WORD(m_fYResolution*39.37 + 0.5) );

  if(m_nPixelType != TWPT_RGB)
  {
    // Apply bit depth color transforms
    switch(m_nPixelType)
    {
      case TWPT_BW:
        cout << "ds: converting to TWPT_BW..." << endl;
        /// @todo impliment Dithering
        /// @todo add Threshold setting
        pDib = FreeImage_Threshold(m_pDIB, 128);
      break;

      case TWPT_GRAY:
        cout << "ds: converting to TWPT_GRAY..." << endl;
        pDib = FreeImage_ConvertTo8Bits(m_pDIB);
      break;

      case TWPT_RGB:
        cout << "ds: converting toTWPT_RGB..." << endl;
        pDib = FreeImage_ConvertTo24Bits(m_pDIB);
      break;
    }

    if(0 == pDib)
    {
      return false;
    }
    else
    {
      FreeImage_Unload(m_pDIB);
      m_pDIB = pDib;
    }
  }

  switch(m_nPixelType)
  {
    case TWPT_BW:
      m_nDestBytesPerRow = BYTES_PERLINE(m_nWidth, 1);
      break;

    case TWPT_GRAY:
      m_nDestBytesPerRow = BYTES_PERLINE(m_nWidth, 8);
      break;

    case TWPT_RGB:
      m_nDestBytesPerRow = BYTES_PERLINE(m_nWidth, 24);
      break;
  }

  // setup some convenience vars because they are used during 
  // every strip request
  m_nScanLine       = 0;

  return true;
}

//////////////////////////////////////////////////////////////////////////////
// We want to simulate getting a scan form a scanner.
// if a size larger than the paper is scanned then there will be black on the bottom 
// and to the right of the image.  We want to transfer the image top to bottom, 
// the black will be transfered after the image if neccessary.
bool CScanner_FreeImage::getScanStrip(BYTE *pTransferBuffer, DWORD dwRead, DWORD &dwReceived)
{
  dwReceived = 0;

  if( NULL == pTransferBuffer ||    // Invalid paramiter
      dwRead < m_nDestBytesPerRow ) // Need enough memory to transfer at least an entire row
  {
    return false;
  }
  
  BYTE   *pBits     = NULL;
  WORD    nRow      = 0;
  WORD    nMaxRows  = (WORD)(dwRead / m_nDestBytesPerRow); //number of rows to be transfered during this call (function of buffer size and line size)

  if( m_nScanLine < MIN(m_nSourceHeight, m_nHeight) )
  {
    //fill the buffer line by line to take care of alignment differences
    for(nRow = 0; nRow<nMaxRows; nRow++)
    {
      //get the next scan line position and copy it
      pBits = (BYTE*)FreeImage_GetScanLine(m_pDIB, m_nSourceHeight-m_nScanLine-1);

      memcpy( pTransferBuffer, pBits, MIN(m_nDestBytesPerRow, FreeImage_GetLine(m_pDIB)) );

      // Check to see if the result image width is wider than what we have.
      // If it is wider fill it in with 0es
      if(m_nDestBytesPerRow > FreeImage_GetLine(m_pDIB))
      {
        memset( pTransferBuffer+FreeImage_GetLine(m_pDIB), 0, m_nDestBytesPerRow - FreeImage_GetLine(m_pDIB) );
      }

      //increment the destination by the aligned line size
      pTransferBuffer += m_nDestBytesPerRow;

      // increment the current scanline for next pass
      m_nScanLine++;

      //update the number of bytes written
      dwReceived += m_nDestBytesPerRow;

      // check for finished scan
      if( m_nScanLine >= m_nSourceHeight ||
          m_nScanLine >= m_nHeight )
      {
        //we are done early
        break;
      }
    }
  }

  // Check to see if the result image length is longer than we have.
  // If it is longer fill it in with 0es
  if(m_nHeight > m_nScanLine )
  {
    nMaxRows  = (WORD)((dwRead-dwReceived) / m_nDestBytesPerRow);
    memset( pTransferBuffer, 0, m_nDestBytesPerRow * nMaxRows );
    m_nScanLine += nMaxRows;
    dwReceived += m_nDestBytesPerRow * nMaxRows;
  }

  return true;
}

//////////////////////////////////////////////////////////////////////////////
short CScanner_FreeImage::getDocumentCount() const
{
  // Simulate the number of pages sitting in the scanner.
  int nCount = 1;

  // Read this value from the environment. This will allow the simulation
  // of a sheet feeder.
  // If the value is <= 0, then a random number of pages will be scanned, else
  // the exact number will be used.
  char szCount[10];
  memset(szCount, 0, sizeof(szCount));

  if( 0 != SGETENV(szCount, sizeof(szCount), kGETENV_XFERCOUNT) )
  {
    // something found, convert it to an int
    nCount = atoi(szCount);

    if(nCount <= 0)
    {
      srand(int(time(0)));
      nCount = rand();
      nCount = nCount%15;// upto 15 pages
    }
  }

  return nCount;
}

//////////////////////////////////////////////////////////////////////////////
bool CScanner_FreeImage::isFeederLoaded()
{
  bool rtn = true;
  if(m_nDocCount<=0)
  {
    rtn = false;
    m_nDocCount = m_nMaxDocCount;// Reloaded the scanner with paper
  }
  return rtn;
}

//////////////////////////////////////////////////////////////////////////////
bool CScanner_FreeImage::getDeviceOnline() const
{
  return true;
}

