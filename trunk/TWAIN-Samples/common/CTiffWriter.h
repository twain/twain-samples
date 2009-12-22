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
* @file CTiffWriter.h
* Write an image to disk as a tiff file.
* @author TWAIN Working Group
* @date October 2007
*/

#ifndef __TIFFWRITER_H__
#define __TIFFWRITER_H__

#include <string>
#include <fstream>

using namespace std;

#ifdef _MSC_VER
  #include <windows.h>
#else
  typedef uint16_t WORD;
  typedef uint32_t DWORD;
#endif // _MSC_VER

#define TIFF_UNCOMPRESSED   1 /**< TIFF compression types */
#define TIFF_CCITTGROUP3    3
#define TIFF_CCITTGROUP4    4

// TIFF types
#define kTIFF_TY_BYTE       1 /**< 8-bit unsigned int */
#define kTIFF_TY_ASCII      2 /**< 8-bit byte that contains a 7-bit ASCII code; last byte must be binary 0 (NULL) */
#define kTIFF_TY_SHORT      3 /**< 16-bit (2-byte) unsigned int */
#define kTIFF_TY_LONG       4 /**< 32-bit (4-byte) unsigned int */
#define kTIFF_TY_RATIONAL   5 /**< two LONGs; the first is the numerator of a fraction; the second, the denominator */

// these field types where introduced in TIFF 6.0
#define kTIFF_TY_SBYTE      6 /**< 8-bit signed int */
#define kTIFF_TY_UNDEFINED  7 /**< 8-bit byte that may contain anything, depending on the definition of the field */
#define kTIFF_TY_SSHORT     8 /**< 16-bit (2-byte) signed int */
#define kTIFF_TY_SLONG      9 /**< 32-bit (4-byte) signed int */
#define kTIFF_TY_SRATIONAL 10 /**< two SLONG's; first is numerator of fraction, second is denominator */
#define kTIFF_TY_FLOAT     11 /**< single precision (4-byte) IEEE format */
#define kTIFF_TY_DOUBLE    12 /**< double precision (8-byte) IEEE format */

// TIFF Tags
#define kTIFF_TAG_IMGWIDTH        0x0100 /**< Image width, short or long */
#define kTIFF_TAG_IMGLENGTH       0x0101 /**< Image length, short or long */
#define kTIFF_TAG_BITSPERSAMPLE   0x0102 /**< BitsPerSample, short */
#define kTIFF_TAG_COMPRESSION     0x0103 /**< Compression, short */
#define kTIFF_TAG_PHOTOMETRICINT  0x0106 /**< PhotometricInterpretation, short */
#define kTIFF_TAG_STRIPOFFSETS    0x0111 /**< StripOffsets, short or long */
#define kTIFF_TAG_SAMPLESPERPIXEL 0x0115 /**< Samples per pixel, short */
#define kTIFF_TAG_ROWSPERSTRIP    0x0116 /**< RowsPerStrip, short or long */
#define kTIFF_TAG_STRIPBYTECOUNTS 0x0117 /**< StripByteCounts, short or long */
#define kTIFF_TAG_XRESOLUTION     0x011A /**< X Resolution, rational */
#define kTIFF_TAG_YRESOLUTION     0x011B /**< Y Resolution, rational */
#define kTIFF_TAG_RESOLUTIONUNIT  0x0128 /**< Resolution unit, short */
#define kTIFF_TAG_COLORMAP        0x0140 /**< ColorMap, short, RGB order, black = 0,0,0, TWAIN supports max 256 entry pallette */

/**
* TIFF Image File Header
*/
struct TIFFIFH
{
  WORD Identifier;
  WORD Version;
  DWORD IFDOffset;
};

/**
* A TIFF Tag
* If the actual value of the tag is less then a DWORD, then offset will contain
* it, else offset is truly an offset to the value.
*/
struct TIFFTag
{
  WORD TagID;
  WORD DataType;
  DWORD DataCount;
  DWORD DataOffset;
};

/**
* This is a class that will progressively write a TIFF image to a file.
*/
class CTiffWriter
{
public:
/**
* Constructor for CTiffWriter.  This is a class that will progressively 
* write a TIFF image to a file.
* @param[in] _filename name of file to write to.
* @param[in] _width image width.
* @param[in] _height image height.
* @param[in] _bitsPerPixel number of bits per each pixel.
* @param[in] _bytesPerRow number of bytes per row of data.
*/
  CTiffWriter(const string& _filename,
              const long int _width,
              const long int _height,
              const int _bitsPerPixel,
              const unsigned long int _bytesPerRow);

/**
* Deconstructor for CTiffWriter.
*/
  virtual ~CTiffWriter();

/**
* Set the width of the image.
* @param[in] _v the new image width
*/
  void setImageWidth(const long int _v);

/**
* Set the height of the image.
* @param[in] _v the new image height
*/
  void setImageHeight(const long int _v);

/**
* Set the bits per sample of the image.
* @param[in] _v the new bits per sample
*/
  void setBitsPerSample(const int _v);

/**
* Set the compression method to use.
* @param[in] _v the new compression method
*/
  void setCompression(const int _v);

/**
* Set the Photometric Interpretation.
* @param[in] _v the new Photometric Interpretation
*/
  void setPhotometricInterp(const int _v);

/**
* Set the number of samples per pixel of the image.
* @param[in] _v the new samples per pixel
*/
  void setSamplesPerPixel(const int _v);

/**
* Set the x resolution of the image.  Using type kTIFF_TY_RATIONAL (fraction)
* @param[in] _numerator the numerator part of the fraction
* @param[in] _denominator the denominator part of the fraction
*/
  void setXResolution(const int _numerator, const int _denominator);

/**
* Set the y resolution of the image.  Using type kTIFF_TY_RATIONAL (fraction)
* @param[in] _numerator the numerator part of the fraction
* @param[in] _denominator the denominator part of the fraction
*/
  void setYResolution(const int _numerator, const int _denominator);

/**
* Set the Bytes per row of the image.
* @param[in] _v the new bytes per row
*/
  void setBytesPerRow(const int _v);


/**
* Write the prepaired image header to the file.
* @return true for succes
*/
  bool writeImageHeader();

/**
* Write the data for the image to the file.
* @param[in] _pData pointer to the image data
* @param[in] _nCount number of bytes to write
* @return true for success
*/
  bool WriteTIFFData(char *_pData, DWORD _nCount);

/**
* Return the size of the TIFF header for the image file.
* @return the size of the header
*/
  unsigned int getSizeofHeader();

  void GetImageHeader(stringstream &Header);

protected:
  string m_filename;                        /**< Name and or path of file */
  int m_nOffset;                            /**< Current offset into file */

  DWORD m_xres[2];                          /**< The X resolution of the image */
  DWORD m_yres[2];                          /**< The Y resolution of the image */

  TIFFTag m_ImageWidth;                     /**< The image width in pixels */
  TIFFTag m_ImageLength;                    /**< The image height in pixels */
  TIFFTag m_BitsPerSample;                  /**< The number of Bits per sample */
  TIFFTag m_Compression;                    /**< The compression method to use */
  TIFFTag m_PhotometricInterp;              /**< The Photometric Interpretation to use */
  TIFFTag m_StripOffsets;                   /**< The strip offset, where image data starts */
  TIFFTag m_SamplesPerPixel;                /**< The number of channels (RGB, G, )*/
  TIFFTag m_RowsPerStrip;                   /**< The number of rows that make up each strip */
  TIFFTag m_StripByteCounts;                /**< The size of each strip of image data */
  TIFFTag m_XResolution;                    /**< The offset to the X resolution */
  TIFFTag m_YResolution;                    /**< The offset to the Y resolution */
  TIFFTag m_ResolutionUnit;                 /**< The units of the Resolution */

  ofstream* m_pImageStream;                 /**< The output stream to write the file to */
};

#endif // __TIFFWRITER_H__
