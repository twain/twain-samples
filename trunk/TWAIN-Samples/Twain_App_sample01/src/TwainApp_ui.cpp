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
#include "TwainString.h"


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

  for(TW_UINT32 x = 0; x < _pCap->NumItems; ++x)
  {
    switch(_pCap->ItemType)
    {
      case TWTY_UINT16:
        cout << x << " - " << convertICAP_Item_toString(_unCap, ((pTW_UINT16)(&_pCap->ItemList))[x]);
        break;

      default:
        cerr << getErrorString_UnexpectedType(TWTY_UINT16, _pCap->ItemType) << endl;
        break;
    }

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
void print_ICAP(const TW_UINT16 _unCap, pTW_ONEVALUE_FIX32 _pCap)
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

  if(TWTY_FIX32 == _pCap->ItemType)
  {
    cout << "1 - " << _pCap->Item.Whole << "." << _pCap->Item.Frac << "*\n" << endl;
  }
  else //if(TWTY_FIX32 != _pCap->ItemType)
  {
    cerr << getErrorString_UnexpectedType(TWTY_FIX32, _pCap->ItemType) << endl;
  }
  
  return;
}

//////////////////////////////////////////////////////////////////////////////
void print_ICAP(const TW_UINT16 _unCap, pTW_ENUMERATION_FIX32 _pCap)
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

  if(TWTY_FIX32 == _pCap->ItemType)
  {
    for(TW_UINT32 x = 0; x < _pCap->NumItems; ++x)
    {
      cout << x << " - ";

      cout
        << _pCap->ItemList[x].Whole << "." << _pCap->ItemList[x].Frac;

      if(x == _pCap->CurrentIndex)
      {
        cout << "*";
      }

      cout << "\n";
    }
    cout << endl;
  }
  else //if(TWTY_FIX32 != _pCap->ItemType)
  {
    cerr << getErrorString_UnexpectedType(TWTY_FIX32, _pCap->ItemType) << endl;
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
