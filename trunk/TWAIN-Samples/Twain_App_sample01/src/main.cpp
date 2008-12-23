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
* @file main.cpp
* The entry point to launch the application
* @author TWAIN Working Group
* @date October 2007
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "main.h"

// I found that compiling using the sunfreeware.com stuff on Solaris 9
// required this typedef. This is related to the inclusion of signal.h
#if defined (__SVR4) && defined (__sun)
typedef union {
  long double  _q;
  uint32_t     _l[4];
} upad128_t;
#endif

#include <signal.h>

#include "CommonTWAIN.h"
#include "TwainApp.h"
#include "TwainApp_ui.h"

using namespace std;

//////////////////////////////////////////////////////////////////////////////
// Global Variables
TW_UINT16   gDSMessage;           /**< global statis to indicate if we are waiting for DS */
TwainApp   *gpTwainApplication;   /**< The main application */
extern bool gUSE_CALLBACKS;       // defined in TwainApp.cpp

//////////////////////////////////////////////////////////////////////////////
/** 
* Display exit message.
* @param[in] _sig not used.
*/
void onSigINT(int _sig)
{
  cout << "\nGoodbye!" << endl;
  exit(0);
}

//////////////////////////////////////////////////////////////////////////////
/** 
* Negotiate a capabilities between the app and the DS
* @param[in] _pCap the capabilities to negotiate
*/
void negotiate_CAP(const pTW_CAPABILITY _pCap)
{
  string input;

  // -Setting one cap could change another cap so always refresh the caps
  // before working with another one.  
  // -Another method of doing this is to let the DS worry about the state
  // of the caps instead of keeping a copy in the app like I'm doing.
  gpTwainApplication->initCaps();

  while(1)
  {
    if((TWON_ENUMERATION == _pCap->ConType) || 
       (TWON_ONEVALUE == _pCap->ConType))
    {
      TW_MEMREF pVal = _DSM_LockMemory(_pCap->hContainer);

      // print the caps current value
      if(TWON_ENUMERATION == _pCap->ConType)
      {
        switch(((pTW_ENUMERATION)pVal)->ItemType)
        {
          case TWTY_UINT16:
            print_ICAP(_pCap->Cap, (pTW_ENUMERATION)(pVal));
          break;

          case TWTY_FIX32:
            print_ICAP(_pCap->Cap, (pTW_ENUMERATION_FIX32)(pVal));
          break;
          
          case TWTY_FRAME:
            print_ICAP(_pCap->Cap, (pTW_ENUMERATION_FRAME)(pVal));
          break;
        }
      }
      else // TWON_ONEVALUE
      {
        switch(((pTW_ONEVALUE)pVal)->ItemType)
        {
          case TWTY_UINT16:
            print_ICAP(_pCap->Cap, (pTW_ONEVALUE)(pVal));
          break;
          
          case TWTY_FIX32:
            print_ICAP(_pCap->Cap, (pTW_ONEVALUE_FIX32)(pVal));
          break;
          
          case TWTY_FRAME:
            print_ICAP(_pCap->Cap, (pTW_ONEVALUE_FRAME)(pVal));
          break;
        }
      }

      cout << "\nset cap# > ";
      cin >> input;
      cout << endl;

      if("q" == input)
      {
        _DSM_UnlockMemory(_pCap->hContainer);
        break;
      }
      else
      {
        int n = atoi(input.c_str());
        TW_UINT16  valUInt16 = 0;
        pTW_FIX32  pValFix32;
        pTW_FRAME  pValFrame;

        // print the caps current value
        if(TWON_ENUMERATION == _pCap->ConType)
        {
          switch(((pTW_ENUMERATION)pVal)->ItemType)
          {
            case TWTY_UINT16:
              valUInt16 = ((pTW_UINT16)(&((pTW_ENUMERATION)pVal)->ItemList))[n];
            break;

            case TWTY_FIX32:
              pValFix32 = &((pTW_ENUMERATION_FIX32)pVal)->ItemList[n];
            break;
            
            case TWTY_FRAME:
              pValFrame = &((pTW_ENUMERATION_FRAME)pVal)->ItemList[n];
            break;
          }

          switch(_pCap->Cap)
          {
            case ICAP_PIXELTYPE:
              gpTwainApplication->set_ICAP_PIXELTYPE(valUInt16);
            break;

            case ICAP_BITDEPTH:
              gpTwainApplication->set_ICAP_BITDEPTH(valUInt16);
            break;

            case ICAP_UNITS:
              gpTwainApplication->set_ICAP_UNITS(valUInt16);
            break;
            
            case ICAP_XFERMECH:
              gpTwainApplication->set_ICAP_XFERMECH(valUInt16);
            break;
          
            case ICAP_XRESOLUTION:
            case ICAP_YRESOLUTION:
              gpTwainApplication->set_ICAP_RESOLUTION(_pCap->Cap, pValFix32);
            break;

            case ICAP_FRAMES:
              gpTwainApplication->set_ICAP_FRAMES(pValFrame);
            break;

            default:
              cerr << "Unsupported capability" << endl;
            break;
          }
        }
      }
      _DSM_UnlockMemory(_pCap->hContainer);
    }
    else
    {
      cerr << "Unsupported capability" << endl;
      break;
    }
  }

  return;
}

//////////////////////////////////////////////////////////////////////////////
/**
* drives main capabilities menu.  Negotiate all capabilities
*/
void negotiateCaps()
{
  // If the app is not in state 4, don't even bother showing this menu.
  if(gpTwainApplication->m_DSMState < 4)
  {
    cerr << "\nNeed to open a source first\n" << endl;
    return;
  }

  string input;

  // Loop forever until either SIGINT is heard or user types done to go back
  // to the main menu.
  while(1)
  {
    printMainCaps();
    cout << "\n(h for help) > ";
    cin >> input;
    cout << endl;

    if("q" == input)
    {
      break;
    }
    else if("h" == input)
    {
      printMainCaps();
    }
    else if("1" == input)
    {
      negotiate_CAP(&(gpTwainApplication->m_ICAP_XFERMECH));
    }
    else if("2" == input)
    {
      negotiate_CAP(&(gpTwainApplication->m_ICAP_PIXELTYPE));
    }
    else if("3" == input)
    {
      negotiate_CAP(&(gpTwainApplication->m_ICAP_BITDEPTH));
    }
    else if("4" == input)
    {
      negotiate_CAP(&(gpTwainApplication->m_ICAP_XRESOLUTION));
    }
    else if("5" == input)
    {
      negotiate_CAP(&(gpTwainApplication->m_ICAP_YRESOLUTION));
    }
    else if("6" == input)
    {
      negotiate_CAP(&(gpTwainApplication->m_ICAP_FRAMES));
    }
    else if("7" == input)
    {
      negotiate_CAP(&(gpTwainApplication->m_ICAP_UNITS));
    }
    else
    {
      printMainCaps();
    }
  }

  return;
}

//////////////////////////////////////////////////////////////////////////////
/**
* Enables the source. The source will let us know when it is ready to scan by
* calling our registered callback function.
*/
void EnableDS()
{
  gDSMessage = 0;

  // -Enable the data source. This puts us in state 5 which means that we
  // have to wait for the data source to tell us to move to state 6 and
  // start the transfer.  Once in state 5, no more set ops can be done on the
  // caps, only get ops.
  // -The scan will not start until the source calls the callback function
  // that was registered earlier.
  if(!gpTwainApplication->enableDS())
  {
    return;
  }

  // now we have to wait until we hear something back from the DS.
  while(!gDSMessage)
  {

    // If we are using callbacks, there is nothing to do here except sleep
    // and wait for our callback from the DS.  If we are not using them, 
    // then we have to poll the DSM.
	  MSG Msg;

	  if(!GetMessage((LPMSG)&Msg, NULL, 0, 0))
    {
      break;//WM_QUIT
    }
    TW_EVENT twEvent = {0};
    twEvent.pEvent = (TW_MEMREF)&Msg;
    twEvent.TWMessage = MSG_NULL;
    TW_UINT16  twRC = TWRC_NOTDSEVENT;
    twRC = _DSM_Entry( gpTwainApplication->getAppIdentity(),
                gpTwainApplication->getDataSource(),
                DG_CONTROL,
                DAT_EVENT,
                MSG_PROCESSEVENT,
                (TW_MEMREF)&twEvent);

    if(!gUSE_CALLBACKS && twRC==TWRC_DSEVENT)
    {
      // check for message from Source
      switch (twEvent.TWMessage)
      {
        case MSG_XFERREADY:
        case MSG_CLOSEDSREQ:
        case MSG_CLOSEDSOK:
        case MSG_NULL:
          gDSMessage = twEvent.TWMessage;
          break;

        default:
          cerr << "\nError - Unknown message in MSG_PROCESSEVENT loop\n" << endl;
          break;
      }
    }
    if(twRC!=TWRC_DSEVENT)
	  {   
		  TranslateMessage ((LPMSG)&Msg);
		  DispatchMessage ((LPMSG)&Msg);
	  }
  }

  // At this point the source has sent us a callback saying that it is ready to
  // transfer the image.

  if(gDSMessage == MSG_XFERREADY)
  {
    // move to state 6 as a result of the data source. We can start a scan now.
    gpTwainApplication->m_DSMState = 6;

    gpTwainApplication->startScan();
  }

  // Scan is done, disable the ds, thus moving us back to state 4 where we
  // can negotiate caps again.
  gpTwainApplication->disableDS();

  return;
}

//////////////////////////////////////////////////////////////////////////////
/**
* Callback funtion for DS.  This is a callback function that will be called by
* the source when it is ready for the application to start a scan. This 
* callback needs to be registered with the DSM before it can be called.
* It is important that the application returns right away after recieving this
* message.  Set a flag and return.  Do not process the callback in this function.
*/
#ifdef TWH_CMP_MSC
TW_UINT16 FAR PASCAL
#else
FAR PASCAL TW_UINT16 
#endif
DSMCallback(pTW_IDENTITY _pOrigin,
            pTW_IDENTITY _pDest,
            TW_UINT32    _DG,
            TW_UINT16    _DAT,
            TW_UINT16    _MSG,
            TW_MEMREF    _pData)
{
  TW_UINT16 twrc = TWRC_SUCCESS;

  // we are only waiting for callbacks from our datasource, so validate
  // that the originator.
  if(0 == _pOrigin ||
     _pOrigin->Id != gpTwainApplication->getDataSource()->Id)
  {
    return TWRC_FAILURE;
  }
  switch (_MSG)
  {
    case MSG_XFERREADY:
    case MSG_CLOSEDSREQ:
    case MSG_CLOSEDSOK:
    case MSG_NULL:
      gDSMessage = _MSG;
      break;

    default:
      cerr << "Error - Unknown message in callback routine" << endl;
      twrc = TWRC_FAILURE;
      break;
  }

  return twrc;
}


//////////////////////////////////////////////////////////////////////////////
/**
* main program loop
*/
#ifdef TWH_CMP_MSC
int _tmain(int argc, _TCHAR* argv[])
#else
int main(int argc, char *argv[])
#endif
{
  int ret = EXIT_SUCCESS;

  // Instantiate the TWAIN application class
  gpTwainApplication = new TwainApp;

  // setup a signal handler for SIGINT that will allow the program to stop
  signal(SIGINT, &onSigINT);

  string input;

  printOptions();

  // start the main event loop
  while(1)
  {
    cout << "\n(h for help) > ";
    cin >> input;
    cout << endl;

    if("q" == input)
    {
      break;
    }
    else if("h" == input)
    {
      printOptions();
    }
    else if("cdsm" == input)
    {
      gpTwainApplication->connectDSM();
    }
    else if("xdsm" == input)
    {
      gpTwainApplication->disconnectDSM();
    }
    else if("lds" == input)
    {
      gpTwainApplication->printAvailableDataSources();
    }
    else if("pds" == input.substr(0,3))
    {
      gpTwainApplication->printIdentityStruct(atoi(input.substr(3,input.length()-3).c_str()));
    }
    else if("cds" == input.substr(0,3))
    {
      gpTwainApplication->loadDS(atoi(input.substr(3,input.length()-3).c_str()));
    }
    else if("xds" == input)
    {
      gpTwainApplication->unloadDS();
    }
    else if("caps" == input)
    {
      if(gpTwainApplication->m_DSMState < 3)
      {
        cout << "\nYou need to select a source first!" << endl;
      }
      else
      {
        negotiateCaps();
        printOptions();
      }
    }
    else if("scan" == input)
    {
      EnableDS();
    }
    else
    {
      // default action
      printOptions();
    }
  }

  gpTwainApplication->exit();
  delete gpTwainApplication;
  gpTwainApplication = 0;

  return ret;
}
