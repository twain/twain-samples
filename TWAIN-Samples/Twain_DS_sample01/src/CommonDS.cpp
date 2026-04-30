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
* @file CommonDS.cpp
* Utilities functions used by TWAIN Data Sources
* 
* @author TWAIN Working Group
* @date October 2007
*/

#include "CommonDS.h"


//////////////////////////////////////////////////////////////////////////////
float ConvertUnits(float val, int fromUnits, int toUnits, float resolution)
{
	double result = val;	//assume we don't have to convert

	if( fromUnits != toUnits )	//if we do have to convert
	{
    //see what we're converting from, and convert to inches
    switch(fromUnits)
    {
      case TWUN_INCHES:
        // nothing to do
        break;

      case TWUN_CENTIMETERS:
        result = val / 2.54; 
        break;

      case TWUN_PICAS:
        result = val / 6.0; 
        break;

      case TWUN_POINTS:
        result = val / 72.0; 
        break;

      case TWUN_TWIPS:
        result = val / 1440.0; 
        break;

      case TWUN_PIXELS:
        if(resolution != 0)
        {
          result = val / resolution;
        }
        break;

      case TWUN_1000INCHES:
        result = val / 1000.0;
        break;

      default:
        // problem
        break;
    }

    // We are now in inches
    // see what we're converting to, and convert the result to those units
    switch(toUnits)
    {
      case TWUN_INCHES:
        // nothing to do
        break;
      
      case TWUN_CENTIMETERS:
        result *= 2.54; 
        break;

      case TWUN_PICAS:
        result *= 6.0; 
        break;

      case TWUN_POINTS:
        result *= 72.0;
        break;

      case TWUN_TWIPS:
        result *= 1440.0;
        break;

      case TWUN_PIXELS:
        result *= resolution;
        break;

      case TWUN_1000INCHES:
        result *= 1000;
        break;

      default:
        // problem
        break;
    }    
	}
	return (float)result;	//return the result
}

TW_FIX32 ConvertUnits(TW_FIX32 val, int fromUnits, int toUnits, float resolution)
{
  TW_FIX32 result = val;
  if( fromUnits != toUnits )	//if we do have to convert
  {
    result = FloatToFIX32(ConvertUnits( FIX32ToFloat(val), fromUnits, toUnits, resolution));
  }
  return result;
}

//////////////////////////////////////////////////////////////////////////////
TW_FRAME ConvertUnits(TW_FRAME val, int fromUnits, int toUnits, float Xresolution, float Yresolution)
{
  TW_FRAME result = val;
  if( fromUnits != toUnits )	//if we do have to convert
  {
    result.Left   = ConvertUnits( val.Left,   fromUnits, toUnits, Xresolution);
    result.Top    = ConvertUnits( val.Top,    fromUnits, toUnits, Yresolution);
    result.Right  = ConvertUnits( val.Right,  fromUnits, toUnits, Xresolution);
    result.Bottom = ConvertUnits( val.Bottom, fromUnits, toUnits, Yresolution);
  }
  return result;
}


//////////////////////////////////////////////////////////////////////////////

