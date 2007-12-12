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
* @file CScanner_FreeImage.h
* Defines a scanner.
* Create a virtual scanner.
* @author TWAIN Working Group
* @date April 2007
*/
#ifndef __CSCANNER_H__
#define __CSCANNER_H__

#include "Common.h"
#include "FreeImage.h"
#include "twain.h"

/**
* This is a virtual scanner.  The virtual scanner is used by the data source 
* to try and somewhat simulate some real hardware interaction.
* - It acquires the same image each time from an image file on the hard disk.
* - It uses the FreeImage library to apply transforms to the base image.
*
* For simulation purposes the virtual Scanner is set up with the 
* following specifications:
*
*   - Scanner Types
*           - ADF(Automatic Document Feeder)
*           - Flatbed
*
*   - Image sensors
*           - CCD x 3 ( Front / Back / Flatbed )
*
*   - Scanning area Minimum
*           - ADF       - A8 @ 127g/m2 paper
*           - Flatbed   - unlimited
*     
*   - Scanning area Maximum
*           - ADF       - Legal paper
*           - Flatbed   - A4 letter paper
*
*   - Internal Optical Resolution
*           - 600dpi
*
*   - Output Resolution
*           - Binary    - 50, 100, 150, 200, 300, 400, 500, & 600
*           - Grayscale - 50, 100, 150, 200, 300, 400, 500, & 600
*           - Color     - 50, 100, 150, 200, 300, 400, 500, & 600
*
*   - Internal Bit Depth
*           - 8 bits per color
*
*   - Output Bit Depth
*           - Binary    - 1 bits
*           - Grayscale - 8 bits 
*           - Color     - 8 bits per color
*
*   - Halftone Patterns
*           - Dither /error diffusion
*
*   - compression: JPEG, and FAX4 (CCITT G4)
*
*/

// Defines used by Scanner FreeImage class to set and get attributes of the vurtual scanner.

/**
* The FreeImage scanner define for PaperSource is ADF
*/
#define SFI_PAPERSOURCE_ADF 0
/**
* The FreeImage scanner define for PaperSource is Flatbed
*/
#define SFI_PAPERSOURCE_FB  1

/**
* The FreeImage scanner data structure.  This data is passed back and forth between the scanner class and driver.
* 
*/
struct SFreeImage
{
  WORD              m_nPaperSource;           /**< the current paper source ADF or Flatbed*/
  bool              m_bDuplex;                /**< True to use duplex false for simplex, ignored if flatbed*/
  WORD              m_nPixelType;             /**< type of pixels to transfer image as */
  int               m_nWidth;                 /**< frame left edge */
  int               m_nHeight;                /**< frame top edge */
  float             m_fXResolution;           /**< horizontal resolution */
  float             m_fYResolution;           /**< vertical resolution */
};



/**
* The FreeImage scanner.  The software scanner using FreeImage.
* 
*/
class CScanner_FreeImage : public SFreeImage
{
public:
  /**
  * Constructor for CScanner_FreeImage.
  */
  CScanner_FreeImage();

  /**
  * Deconstructor for CScanner_FreeImage.
  */
  ~CScanner_FreeImage();

  /**
  * Resets the scanner to factory default settings.
  * Sets the scanners caps back to defaults.
  * @return true if successfully reset.
  */
  bool resetScanner();

  /**
  * Get a scan line and put it into the _ImageXfer, fill values in _ImageXfer.
  * @param[out] pTransferBuffer a pointer to an array of bytes to store the image data
  * @param[in]  dwRead the number of bytes to read from scanner
  * @param[out] dwReceived the actual number of bytes transfered
  * @return true if successful
  */
  bool getScanStrip(BYTE *pTransferBuffer, DWORD dwRead, DWORD &dwReceived);

  /**
  * Sets all the scanners capabilities to read only.
  */
  void Lock(void) {m_bReadOnly = true;}

  /**
  * Sets all the scanners capabilities to read and write.
  */
  void Unlock(void) {m_bReadOnly = false;}

  /**
  * Retrieves the image to scan from disk into memory.
  * @return true if image was loaded into memory successfully.
  */
  bool acquireImage();

  ////////////////
  // Accessors

  /**
  * get the current settings
  * @return the current scanner settngs
  */
  SFreeImage getSetting() const;

  /**
  * set the current settings
  * @param[in] settings the new settings for the scanner
  */
  void setSetting(SFreeImage settings);

  /**
  * Determine if there is paper sitting in the feeder.
  * IF empty loads again for next time.
  * @return true if paper in feeder else return false.
  */
  bool isFeederLoaded();

  /**
  * Return status of the device is online or not.
  * @return true if online.
  */
  bool getDeviceOnline() const;


protected:
  /**
  * Return the number of documents sitting in the feeder.
  * This number can be set with enviroment varible kGETENV_XFERCOUNT "CAP_XFERCOUNT"
  * this is so we can tell when the feeder is empty
  * - If CAP_XFERCOUNT is not set will return 1.
  * - if < 0 will return random number.
  * @return default number of documents.
  */
  short getDocumentCount() const;

  /**
  * Transform the image according to the caps set by the application.
  * @return true if successful
  */
  bool preScanPrep();



protected:
  FIBITMAP         *m_pDIB;                   /**< Pointer to current scanned image */
  WORD              m_nScanLine;              /**< Current scan line of image in FreeImage */
  bool              m_bReadOnly;              /**< current mode */
  DWORD             m_nDestBytesPerRow;       /**< number of bytes needed for a row of data */
  short             m_nDocCount;              /**< number of documents waiting to transfer */
  WORD              m_nSourceWidth;           /**< Width of image in FreeImage */
  WORD              m_nSourceHeight;          /**< Height of image in FreeImage */
  char              m_szSourceImagePath[PATH_MAX];  /**< image used with FreeImage */
};

/**
* FreeImage error handler.
* @param[in] fif format  / plugin responsible for the error.
* @param[in] message Error Message.
*/
void FreeImageErrorHandler(FREE_IMAGE_FORMAT fif, const char *message);

#endif // __CSCANNER_H__

