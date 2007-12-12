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
 * @file CTiffWriter.cpp
 * Write an image to disk as a tiff file.
 * @author JFL Peripheral Solutions Inc.
 * @date October 2007
 */


#include "CTiffWriter.h"

CTiffWriter::CTiffWriter(const string& _filename,
                         const long int _width,
                         const long int _height,
                         const int _bitsPerPixel,
                         const unsigned long int _bytesPerRow)
{
  m_pImageStream = 0;
  m_nOffset = 0;
  m_filename = _filename;

  m_ImageWidth.TagID = kTIFF_TAG_IMGWIDTH;
  m_ImageWidth.DataType = kTIFF_TY_LONG;
  m_ImageWidth.DataCount = 1;
  m_ImageWidth.DataOffset = _width;

  m_ImageLength.TagID = kTIFF_TAG_IMGLENGTH;
  m_ImageLength.DataType = kTIFF_TY_LONG;
  m_ImageLength.DataCount = 1;
  m_ImageLength.DataOffset = _height;

  m_BitsPerSample.TagID = kTIFF_TAG_BITSPERSAMPLE;
  m_BitsPerSample.DataType = kTIFF_TY_SHORT;
  m_BitsPerSample.DataCount = 1;

  if(24 == _bitsPerPixel)
  {
    m_BitsPerSample.DataOffset = 8;
  }
  else
  {
    m_BitsPerSample.DataOffset = _bitsPerPixel;
  }


  m_Compression.TagID = kTIFF_TAG_COMPRESSION;
  m_Compression.DataType = kTIFF_TY_SHORT;
  m_Compression.DataCount = 1;
  m_Compression.DataOffset = 1;

  m_PhotometricInterp.TagID = kTIFF_TAG_PHOTOMETRICINT;
  m_PhotometricInterp.DataType = kTIFF_TY_SHORT;
  m_PhotometricInterp.DataCount = 1;

  if(24 == _bitsPerPixel)
  {
    m_PhotometricInterp.DataOffset = 2;
  }
  else
  {
    m_PhotometricInterp.DataOffset = 1;
  }

  // -there is only one strip that contains all the row data, and it starts right after the header.
  // -There are always 12 tags being written for each tiff.
  m_StripOffsets.TagID = kTIFF_TAG_STRIPOFFSETS;
  m_StripOffsets.DataType = kTIFF_TY_SHORT;
  m_StripOffsets.DataCount = 1;
  m_StripOffsets.DataOffset = getSizeofHeader();

  m_SamplesPerPixel.TagID = kTIFF_TAG_SAMPLESPERPIXEL;
  m_SamplesPerPixel.DataType = kTIFF_TY_SHORT;

  if(24 == _bitsPerPixel)
  {
    m_SamplesPerPixel.DataCount = 1;
    m_SamplesPerPixel.DataOffset = 3;
  }
  else
  {
    m_SamplesPerPixel.DataCount = 1;
    m_SamplesPerPixel.DataOffset = 1;
  }

  m_RowsPerStrip.TagID = kTIFF_TAG_ROWSPERSTRIP;
  m_RowsPerStrip.DataType = kTIFF_TY_LONG;
  m_RowsPerStrip.DataCount = 1;
  m_RowsPerStrip.DataOffset = _height;

  m_StripByteCounts.TagID = kTIFF_TAG_STRIPBYTECOUNTS;
  m_StripByteCounts.DataType = kTIFF_TY_LONG;
  m_StripByteCounts.DataCount = 1;
  m_StripByteCounts.DataOffset = _bytesPerRow * _height;

  m_XResolution.TagID = kTIFF_TAG_XRESOLUTION;
  m_XResolution.DataType = kTIFF_TY_RATIONAL;
  m_XResolution.DataCount = 1;
  m_XResolution.DataOffset = m_StripOffsets.DataOffset - sizeof(DWORD)*4; // fixed offset from the end of the header
  setXResolution(100, 1);

  m_YResolution.TagID = kTIFF_TAG_YRESOLUTION;
  m_YResolution.DataType = kTIFF_TY_RATIONAL;
  m_YResolution.DataCount = 1;
  m_YResolution.DataOffset = m_StripOffsets.DataOffset - sizeof(DWORD)*2; // fixed offset from the end of the header
  setYResolution(100, 1);

  m_ResolutionUnit.TagID = kTIFF_TAG_RESOLUTIONUNIT;
  m_ResolutionUnit.DataType = kTIFF_TY_SHORT;
  m_ResolutionUnit.DataCount = 1;
  m_ResolutionUnit.DataOffset = 2;
}

CTiffWriter::~CTiffWriter()
{
  if(0 != m_pImageStream)
  {
    if(m_pImageStream->is_open())
    {
      m_pImageStream->close();
    }
    delete m_pImageStream;
  }
}

void CTiffWriter::setImageWidth(const long int _v)
{
  m_ImageWidth.DataOffset = _v;
}

void CTiffWriter::setImageHeight(const long int _v)
{
  m_ImageLength.DataOffset = _v;
}

void CTiffWriter::setBitsPerSample(const int _v)
{
  m_BitsPerSample.DataOffset = _v;
}

void CTiffWriter::setCompression(const int _v)
{
  m_Compression.DataOffset = _v;
}

void CTiffWriter::setPhotometricInterp(const int _v)
{
  m_PhotometricInterp.DataOffset = _v;
}

void CTiffWriter::setSamplesPerPixel(const int _v)
{
  m_SamplesPerPixel.DataOffset = _v;
}

void CTiffWriter::setXResolution(const int _numerator, const int _denominator)
{
  m_xres[0] = _numerator;
  m_xres[1] = _denominator;
}

void CTiffWriter::setYResolution(const int _numerator, const int _denominator)
{
  m_yres[0] = _numerator;
  m_yres[1] = _denominator;
}

void CTiffWriter::setBytesPerRow(const int _v)
{
  m_StripByteCounts.DataOffset = _v * m_ImageLength.DataOffset;
}

bool CTiffWriter::writeImageHeader()
{
  // create the out stream if not done so already
  if(0 == m_pImageStream)
  {
    m_pImageStream = new ofstream();
  }

  // open the stream. If already open, reset it
  if(m_pImageStream->is_open())
  {
    m_pImageStream->seekp(0);
  }
  else
  {
    m_pImageStream->open(m_filename.c_str(), ios_base::out|ios_base::binary|ios_base::trunc);
  }

  // write the header
  TIFFIFH hdr = {0x4949, 0x002a, sizeof(TIFFIFH)};
  m_pImageStream->write(reinterpret_cast<char*>(&hdr), sizeof(TIFFIFH));

  // write the Tags immediately after the header
  WORD numTags = 12;
  m_pImageStream->write(reinterpret_cast<char*>(&numTags), sizeof(numTags));

  const int nsize = sizeof(TIFFTag);

  m_pImageStream->write(reinterpret_cast<char*>(&m_ImageWidth), nsize);
  m_pImageStream->write(reinterpret_cast<char*>(&m_ImageLength), nsize);
  m_pImageStream->write(reinterpret_cast<char*>(&m_BitsPerSample), nsize);
  m_pImageStream->write(reinterpret_cast<char*>(&m_Compression), nsize);
  m_pImageStream->write(reinterpret_cast<char*>(&m_PhotometricInterp), nsize);
  m_pImageStream->write(reinterpret_cast<char*>(&m_StripOffsets), nsize);
  m_pImageStream->write(reinterpret_cast<char*>(&m_SamplesPerPixel), nsize);
  m_pImageStream->write(reinterpret_cast<char*>(&m_RowsPerStrip), nsize);
  m_pImageStream->write(reinterpret_cast<char*>(&m_StripByteCounts), nsize);
  m_pImageStream->write(reinterpret_cast<char*>(&m_XResolution), nsize);
  m_pImageStream->write(reinterpret_cast<char*>(&m_YResolution), nsize);
  m_pImageStream->write(reinterpret_cast<char*>(&m_ResolutionUnit), nsize);

  // end the header by setting the next image offset to null
  DWORD end = 0;
  m_pImageStream->write(reinterpret_cast<char*>(&end), sizeof(end));

  // write the X and Y resolutions
  m_pImageStream->write(reinterpret_cast<char*>(&m_xres), sizeof(DWORD)*2);

  m_pImageStream->write(reinterpret_cast<char*>(&m_yres), sizeof(DWORD)*2);

  m_nOffset = getSizeofHeader();

  return true;
}

bool CTiffWriter::WriteTIFFData(char *_pData, DWORD _nCount)
{
  bool bret = false;

  if(0 != m_pImageStream &&
     m_pImageStream->good())
  {
    m_pImageStream->seekp(m_nOffset);
    m_pImageStream->write(_pData, _nCount);
    m_nOffset += _nCount;
    bret = true;
  }

  return bret;
}

unsigned int CTiffWriter::getSizeofHeader()
{
  // Header is as follows:
  // TIFFIFH + Num. of Tags + each tag + Xres Data (2 dwords) + Yres Data (2 dwords) + Next Image offset (1 dword)
  return sizeof(TIFFIFH)+sizeof(WORD)+sizeof(TIFFTag)*12+sizeof(DWORD)*5;
}
