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
 * @file TwainApp.cpp
 * TWAIN Application.
 * A TWAIN Application communicates with the DSM to acquire images. 
 * The goal of the application is to acquire data from a Source.  
 * However, applications cannot contact the Source directly.  All requests for
 * data, capability information, error information, etc. must be handled 
 * Through the Source Manager.
 * @author JFL Peripheral Solutions Inc.
 * @date April 2007
 */

#ifdef _WINDOWS
  #include "stdafx.h"
#endif

#include <iostream>
#include <cstdlib>
#include <assert.h>
#include <stdio.h>

#include "TwainApp.h"
#include "CTiffWriter.h"
#include "TwainString.h"

using namespace std;

// used in the OPENDSM logic
extern TW_ENTRYPOINT g_DSM_Entry; // found in DSMInterface.cpp
bool gUSE_CALLBACKS = false;     /**< This gets updated to true if the DSM is ver2 */
/*
* defined in main.cpp, this is the callback function to register for this app.
* It will be called by the source when its ready to start the transfer.
*/
extern 
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
            TW_MEMREF    _pData);

//////////////////////////////////////////////////////////////////////////////
bool operator== (const TW_FIX32& _fix1, const TW_FIX32& _fix2)
{
  return((_fix1.Whole == _fix2.Whole) &&
         (_fix1.Frac == _fix2.Frac));
}


void PrintCMDMessage(const char* const pStr, ...)
{
  char buffer[200];

  va_list valist;
  va_start(valist, pStr);
  #if (TWNDSM_CMP == TWNDSM_CMP_VISUALCPP) && (TWNDS_CMP_VERSION >= 1400)
    _vsnprintf_s(buffer, 200, 200, pStr, valist);
  #elif (TWNDSM_CMP == TWNDSM_CMP_VISUALCPP)
    _vsnprintf(buffer, 200, pStr, valist);
  #elif (TWNDSM_CMP == TWNDSM_CMP_GNUGPP)
    vsnprintf(buffer, 200, pStr, valist);
  #else
    #error Sorry, we do not recognize this system...
  #endif
  va_end(valist);

#ifdef _WINDOWS
  TRACE(buffer);
#else
  cout << buffer;
#endif
}

//////////////////////////////////////////////////////////////////////////////
/**
* Output error messages for Free Image Format.
* @param[in] fif Free Image Format
* @param[in] message error string to display
*/
void FreeImageErrorHandler(FREE_IMAGE_FORMAT fif, const char *message)
{
  PrintCMDMessage("\n*** %s Format\n%s ***\n", FreeImage_GetFormatFromFIF(fif), message);
}

//////////////////////////////////////////////////////////////////////////////
TwainApp::TwainApp(HWND parent /*=NULL*/)
: m_DSMState(1)
, m_pDataSource(NULL)
, m_pExtImageInfo(NULL)
{
  // fill our identity structure
  fillIdentity(m_MyInfo);

  m_Parent = parent;
  m_nXferNum = 0;

  FreeImage_Initialise();
  FreeImage_SetOutputMessage(FreeImageErrorHandler);

  return;
}

//////////////////////////////////////////////////////////////////////////////
TwainApp::~TwainApp()
{
  FreeImage_DeInitialise();
  unLoadDSMLib();
  m_DataSources.erase(m_DataSources.begin(), m_DataSources.end());
  if(m_pExtImageInfo)
  {
    //delete m_pExtImageInfo;
  }
}

//////////////////////////////////////////////////////////////////////////////
void TwainApp::fillIdentity(TW_IDENTITY& _identity)
{
  _identity.Id = 0;
  _identity.Version.MajorNum = 1;
  _identity.Version.MinorNum = 0;
  _identity.Version.Language = TWLG_ENGLISH_CANADIAN;
  _identity.Version.Country = TWCY_CANADA;
  _identity.ProtocolMajor = TWON_PROTOCOLMAJOR;
  _identity.ProtocolMinor = TWON_PROTOCOLMINOR;
  _identity.SupportedGroups = DF_APP2 | DG_IMAGE | DG_CONTROL;
  SSTRCPY(_identity.Manufacturer, sizeof(_identity.Manufacturer), "App's Manufacturer");
  SSTRCPY(_identity.ProductFamily, sizeof(_identity.ProductFamily), "App's Product Family");
  SSTRCPY(_identity.ProductName, sizeof(_identity.ProductName), "Specific App Product Name");

  return;
}

TW_UINT16 TwainApp::DSM_Entry(TW_UINT32 _DG,TW_UINT16 _DAT, TW_UINT16 _MSG, TW_MEMREF _pData)
{
  return _DSM_Entry(&m_MyInfo, m_pDataSource, _DG, _DAT, _MSG, _pData);
}

//////////////////////////////////////////////////////////////////////////////
void TwainApp::exit()
{
  if( 3 <= m_DSMState )
  {
    // If we have selected a source, then it is posible it is open
    if( 0 != m_pDataSource && 4 <= m_DSMState )
    {
      if( 5 <= m_DSMState )
      {
        disableDS();

        // If DisableDS succeeds then m_DSMState will be set down to 4
        // if it did NOT succeed, try to cancle any pending transfers.
        if( 5 <= m_DSMState )
        {
          TW_PENDINGXFERS pendxfers;
          memset( &pendxfers, 0, sizeof(pendxfers) );
          DSM_Entry(DG_CONTROL, DAT_PENDINGXFERS, MSG_ENDXFER, (TW_MEMREF)&pendxfers);

          // We need to get rid of any pending transfers
          if(0 != pendxfers.Count)
          {
            memset( &pendxfers, 0, sizeof(pendxfers) );
            DSM_Entry(DG_CONTROL, DAT_PENDINGXFERS, MSG_RESET, (TW_MEMREF)&pendxfers);
          }
          
          // Any pending transfers should now be cancled
          m_DSMState = 5;
          disableDS();
        }// END 5 <= m_DSMState
      }// END 5 <= m_DSMState
      unloadDS();
    }// END 4 <= m_DSMState
    disconnectDSM();
  }// END 3 <= m_DSMState

}


//////////////////////////////////////////////////////////////////////////////
// NOTE: this function needs to be refactored to:
//  - have better error handling
//  - have 1 output interface for both linux console and Windows
//  - look into if we need to cleanup on failures
void TwainApp::connectDSM()
{
  if(m_DSMState > 3)
  {
    PrintCMDMessage("The DSM has already been opened, close it first\n");
    return;
  }

  if(!LoadDSMLib(kTWAIN_DS_DIR kTWAIN_DSM_DLL_NAME))
  {
    printError(0, "The DSM could not be opened. Please ensure that it is installed into a directory that is in the library path:");
    printError(0, kTWAIN_DS_DIR kTWAIN_DSM_DLL_NAME);
    return;
  }
  else
  {
    m_DSMState = 2;
  }

  TW_UINT16 ret = 0;

  if(TWRC_SUCCESS != (ret = _DSM_Entry(
    &(m_MyInfo),
    0,
    DG_CONTROL,
    DAT_PARENT,
    MSG_OPENDSM,
    (TW_MEMREF)&m_Parent)))
  {
    PrintCMDMessage("DG_CONTROL / DAT_PARENT / MSG_OPENDSM Failed: %u\n", ret);
    return;
  }

  // check for DSM2 support
  if((m_MyInfo.SupportedGroups & DF_DSM2) == DF_DSM2)
  {
    g_DSM_Entry.Size = sizeof(TW_ENTRYPOINT);
    // do a MSG_GET to fill our entrypoints
    if(TWRC_SUCCESS != (ret = _DSM_Entry(
                                &(m_MyInfo),
                                0,
                                DG_CONTROL,
                                DAT_ENTRYPOINT,
                                MSG_GET,
                                (pTW_ENTRYPOINT)&g_DSM_Entry)))
    {
      PrintCMDMessage("DG_CONTROL / DAT_ENTRYPOINT / MSG_GET Failed: %d\n", ret);
      return;
    }
  }

  PrintCMDMessage("Successfully opened the DSM\n");
  m_DSMState = 3;

  // get list of available sources
  m_DataSources.erase(m_DataSources.begin(), m_DataSources.end());
  getSources();

  return;
}

//////////////////////////////////////////////////////////////////////////////
void TwainApp::disconnectDSM()
{
  if(m_DSMState < 3)
  {
    PrintCMDMessage("The DSM has not been opened, open it first\n");
  }

  TW_UINT16 ret = _DSM_Entry(
    &(m_MyInfo),
    0,
    DG_CONTROL,
    DAT_PARENT,
    MSG_CLOSEDSM,
    (TW_MEMREF)&m_Parent);

  if(TWRC_SUCCESS == ret)
  {
    PrintCMDMessage("Successfully closed the DSM\n");
    m_DSMState = 2;
  }
  else
  {
    printError(0, "Failed to close the DSM");
  }

  return;
}



//////////////////////////////////////////////////////////////////////////////
TW_IDENTITY _gSource; /**< used to store the source that is return by getDefaultDataSource */
//////////////////////////////////////////////////////////////////////////////
pTW_IDENTITY TwainApp::getDefaultDataSource()
{
  if(m_DSMState < 3)
  {
    cout << "You need to open the DSM first." << endl;
    return NULL;
  }

  // get default

  memset(&_gSource, 0, sizeof(TW_IDENTITY));

  TW_UINT16 twrc;

  twrc = _DSM_Entry(
    &m_MyInfo,
    0,
    DG_CONTROL,
    DAT_IDENTITY,
    MSG_GETDEFAULT,
    (TW_MEMREF) &_gSource);

  switch (twrc)
  {
    case TWRC_SUCCESS:
      break;

    case TWRC_FAILURE:
      printError(0, "Failed to get the data source info!");
      break;
  }

  return &_gSource;
}


//////////////////////////////////////////////////////////////////////////////
pTW_IDENTITY TwainApp::selectDefaultDataSource()
{
  if(m_DSMState < 3)
  {
    PrintCMDMessage("You need to open the DSM first.\n");
    return NULL;
  }

  // get default

  memset(&_gSource, 0, sizeof(TW_IDENTITY));

  TW_UINT16 twrc;

  twrc = _DSM_Entry(
    &m_MyInfo,
    0,
    DG_CONTROL,
    DAT_IDENTITY,
    MSG_USERSELECT,
    (TW_MEMREF) &_gSource);

  switch (twrc)
  {
    case TWRC_SUCCESS:
      break;

    case TWRC_CANCEL:
      printError(0, "Canceled select data source!");
      return NULL;
      break;

    case TWRC_FAILURE:
      printError(0, "Failed to select the data source!");
      return NULL;
      break;
  }

  return &_gSource;
}


//////////////////////////////////////////////////////////////////////////////
void TwainApp::loadDS(const TW_UINT32 _dsID)
{
  // The application must be in state 3 to open a Data Source.
  if(m_DSMState < 3)
  {
    PrintCMDMessage("The DSM needs to be opened first.\n");
    return;
  }
  else if(m_DSMState > 3)
  {
    PrintCMDMessage("A source has already been opened, please close it first\n");
    return;
  }

  if(_dsID > 0)
  {
    // first find the data source with id = _dsID
    m_pDataSource = 0;
    unsigned int x = 0;
    for(; x < m_DataSources.size(); ++x)
    {
      if(_dsID == m_DataSources[x].Id)
      {
        m_pDataSource = &(m_DataSources[x]);
        break;
      }
    }

    if(0 == m_pDataSource)
    {
      PrintCMDMessage("Data source with id: [%u] can not be found\n", _dsID);
      return;
    }
  }
  else
  {
    //Open the default
      memset(&_gSource, 0, sizeof(TW_IDENTITY));
      m_pDataSource = &_gSource;
  }

  TW_CALLBACK callback = {0};

  // open the specific data source
  TW_UINT16 twrc;
  twrc = _DSM_Entry(
    &m_MyInfo,
    0,
    DG_CONTROL,
    DAT_IDENTITY,
    MSG_OPENDS,
    (TW_MEMREF) m_pDataSource);

  switch (twrc)
  {
  case TWRC_SUCCESS:
    PrintCMDMessage("Data source successfully opened!\n");
    // Transition application to state 4
    m_DSMState = 4;

    callback.CallBackProc = (TW_MEMREF)DSMCallback;    

    if(TWRC_SUCCESS != (twrc = DSM_Entry(DG_CONTROL, DAT_CALLBACK, MSG_REGISTER_CALLBACK, (TW_MEMREF)&callback)))
    {
      PrintCMDMessage("DG_CONTROL / DAT_CALLBACK / MSG_REGISTER_CALLBACK Failed: %u\n", twrc);
    }
    else
    {
      gUSE_CALLBACKS = true;
    }

    break;

  default:
    printError(m_pDataSource, "Failed to open data source.");
    m_pDataSource = 0;
    break;
  }

  return;
}

//////////////////////////////////////////////////////////////////////////////
void TwainApp::unloadDS()
{
  if(m_DSMState < 4)
  {
    PrintCMDMessage("You need to open a data source first.\n");
    return;
  }

  TW_UINT16 twrc;
  twrc = _DSM_Entry(
    &m_MyInfo,
    0,
    DG_CONTROL,
    DAT_IDENTITY,
    MSG_CLOSEDS,
    (TW_MEMREF) m_pDataSource);

  switch (twrc)
  {
  case TWRC_SUCCESS:
    PrintCMDMessage("Data source successfully closed\n");

    // Transition application to state 3
    m_DSMState = 3;

    // reset the active source pointer
    m_pDataSource = 0;
    break;

  default:
    printError(0, "Failed to close data source.");
    break;
  }

  return;
}


//////////////////////////////////////////////////////////////////////////////
void TwainApp::getSources()
{
  if(m_DSMState < 3)
  {
    PrintCMDMessage("You need to open the DSM first.\n");
    return;
  }

  // the list should be empty if adding to it.
  assert( true ==  m_DataSources.empty() );

  // get first
  TW_IDENTITY Source;
  memset(&Source, 0, sizeof(TW_IDENTITY));

  TW_UINT16 twrc;

  twrc = _DSM_Entry(
    &m_MyInfo,
    0,
    DG_CONTROL,
    DAT_IDENTITY,
    MSG_GETFIRST,
    (TW_MEMREF) &Source);

  switch (twrc)
  {
  case TWRC_SUCCESS:
    m_DataSources.push_back(Source);
    break;

  case TWRC_FAILURE:
    printError(0, "Failed to get the data source info!");
    break;

  case TWRC_ENDOFLIST:
    return;
    break;
  }

  // get the rest of the sources
  do
  {
    memset(&Source, 0, sizeof(TW_IDENTITY));

    twrc = _DSM_Entry(
      &m_MyInfo,
      0,
      DG_CONTROL,
      DAT_IDENTITY,
      MSG_GETNEXT,
      (TW_MEMREF) &Source);

    switch (twrc)
    {
    case TWRC_SUCCESS:
      m_DataSources.push_back(Source);
      break;

    case TWRC_FAILURE:
      printError(0, "Failed to get the rest of the data source info!");
      return;
      break;

    case TWRC_ENDOFLIST:
      return;
      break;
    }
  }
  while (TWRC_SUCCESS == twrc);

  return;
}


//////////////////////////////////////////////////////////////////////////////
TW_INT16 TwainApp::printError(pTW_IDENTITY _pdestID, const string& _errorMsg)
{
  TW_INT16 c = TWCC_SUCCESS;

#ifdef _WINDOWS
  TRACE("app: ");

  if(_errorMsg.length() > 0)
  {
    TRACE(_errorMsg.c_str());
  }
  else
  {
    TRACE("An error has occurred.");
  }

  if(TWRC_SUCCESS == getTWCC(_pdestID, c))
  {
    TRACE(" The condition code is: %s\n", convertConditionCode_toString(c));
  }
  else
  {
    TRACE("\n");
  }
#else
  cerr << "app: ";

  if(_errorMsg.length() > 0)
  {
    cerr << _errorMsg;
  }
  else
  {
    cerr << "An error has occurred.";
  }

  if(TWRC_SUCCESS == getTWCC(_pdestID, c))
  {
    cerr << " The condition code is: " << convertConditionCode_toString(c) << endl;
  }
#endif

  return c;
}


//////////////////////////////////////////////////////////////////////////////
TW_UINT16 TwainApp::getTWCC(pTW_IDENTITY _pdestID, TW_INT16& _cc)
{
  TW_STATUS status;
  memset(&status, 0, sizeof(TW_STATUS));

  TW_UINT16 twrc = _DSM_Entry(
    &m_MyInfo,
    _pdestID,
    DG_CONTROL,
    DAT_STATUS,
    MSG_GET,
    (TW_MEMREF)&status);

  if(TWRC_SUCCESS == twrc)
  {
    _cc = status.ConditionCode;
  }

  return twrc;
}

//////////////////////////////////////////////////////////////////////////////
bool TwainApp::enableDS(BOOL bShowUI)
{
  bool bret = true;

  if(m_DSMState < 4)
  {
    PrintCMDMessage("You need to open the data source first.\n");
    return false;
  }

  m_ui.ShowUI = bShowUI;
  m_ui.ModalUI = TRUE;
  m_ui.hParent = 0;
  m_DSMState = 5;

  TW_UINT16 twrc = DSM_Entry(DG_CONTROL, DAT_USERINTERFACE, MSG_ENABLEDS, (TW_MEMREF)&(m_ui));

  if( TWRC_SUCCESS != twrc &&
      TWRC_CHECKSTATUS != twrc )
  {
    m_DSMState = 4;
    printError(m_pDataSource, "Cannot enable source");
    bret = false;
  }

  // Usually at this point the application sits here and waits for the
  // scan to start. We are notified that we can start a scan through
  // the DSM's callback mechanism. The callback was registered when the DSM
  // was opened.
  // If callbacks are not being used, then the DSM will be polled to see
  // when the DS is ready to start scanning.

  return bret;
}

//////////////////////////////////////////////////////////////////////////////
void TwainApp::disableDS()
{
  if(m_DSMState < 5)
  {
    PrintCMDMessage("You need to enable the data source first.\n");
    return;
  }

  TW_UINT16 twrc = DSM_Entry( DG_CONTROL, DAT_USERINTERFACE, MSG_DISABLEDS, (TW_MEMREF)&(m_ui) );

  if(TWRC_SUCCESS == twrc)
  {
    m_DSMState = 4;
  }
  else
  {
    printError(m_pDataSource, "Cannot disable source");
  }

  return;
}


//////////////////////////////////////////////////////////////////////////////
bool TwainApp::updateIMAGEINFO()
{
  // clear our image info structure
  memset(&m_ImageInfo, 0, sizeof(m_ImageInfo));

  // get the image details
  PrintCMDMessage("app: Getting the image info...\n");
  TW_UINT16 twrc = DSM_Entry( DG_IMAGE, DAT_IMAGEINFO, MSG_GET, (TW_MEMREF)&(m_ImageInfo));

  if(TWRC_SUCCESS != twrc)
  {
    printError(m_pDataSource, "Error while trying to get the image information!");
  }
  return TWRC_SUCCESS==twrc? true:false;
}

//////////////////////////////////////////////////////////////////////////////
void TwainApp::initiateTransfer_Native()
{
  PrintCMDMessage("app: Starting a TWSX_NATIVE transfer...\n");

  TW_STR255 szOutFileName;
  bool      bPendingXfers = true;
  TW_UINT16 twrc          = TWRC_SUCCESS;

  while(bPendingXfers)
  {
    m_nXferNum++;
    memset(szOutFileName, 0, sizeof(szOutFileName));

    if(!updateIMAGEINFO())
    {
      break;
    }

    TW_MEMREF hImg = 0;

    PrintCMDMessage("app: Starting the transfer...\n");
    twrc = DSM_Entry( DG_IMAGE, DAT_IMAGENATIVEXFER, MSG_GET, (TW_MEMREF)&hImg);

    if(TWRC_XFERDONE == twrc)
    {
      // -Here we get a handle to a DIB. Save it to disk as a bmp. 
      // -After saving it to disk, I could open it up again using FreeImage
      // if I wanted to do more transforms on it or save it as a different format.
      PBITMAPINFOHEADER pDIB = (PBITMAPINFOHEADER)_DSM_LockMemory(hImg);

      if(0 == pDIB)
      {
        printError(m_pDataSource, "App: Unable to lock memory, transfer failed");
        break;
      }

      // Set the filename to save to
      SSNPRINTF(szOutFileName, sizeof(szOutFileName), sizeof(szOutFileName), "FROM_SCANNER_%06dN.bmp", m_nXferNum);

      // Save the image to disk
      FILE *pFile;
      FOPEN(pFile, szOutFileName, "wb");
      if(pFile == 0)
      {
        perror("fopen");
      }
      else
      {
        DWORD dwPaletteSize = 0;

        switch(pDIB->biBitCount)
        {
        case 1:
          dwPaletteSize = 2;
          break;
        case 8:
          dwPaletteSize = 256;
          break;
        case 24:
          break;
        default:
          assert(0); //Not going to work!
          break;
        }

        // If the driver did not fill in the biSizeImage field, then compute it
        // Each scan line of the image is aligned on a DWORD (32bit) boundary
        if( pDIB->biSizeImage == 0 )
        {
          pDIB->biSizeImage = ((((pDIB->biWidth * pDIB->biBitCount) + 31) & ~31) / 8) * pDIB->biHeight;

          // If a compression scheme is used the result may infact be larger
          // Increase the size to account for this.
          if (pDIB->biCompression != 0)
          {
            pDIB->biSizeImage = (pDIB->biSizeImage * 3) / 2;
          }
        }

        int nImageSize = pDIB->biSizeImage + (sizeof(RGBQUAD)*dwPaletteSize)+sizeof(BITMAPINFOHEADER);

        BITMAPFILEHEADER bmpFIH = {0};
        bmpFIH.bfType = ( (WORD) ('M' << 8) | 'B');
        bmpFIH.bfSize = nImageSize + sizeof(BITMAPFILEHEADER);
        bmpFIH.bfOffBits = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+(sizeof(RGBQUAD)*dwPaletteSize);
  
        fwrite(&bmpFIH, 1, sizeof(BITMAPFILEHEADER), pFile);
        fwrite(pDIB, 1, nImageSize, pFile);
        fclose(pFile);
        pFile = 0;

        PrintCMDMessage("app: File \"%s\" saved...\n", szOutFileName);
#ifdef _WINDOWS
        ShellExecute(m_Parent, "open", szOutFileName, NULL, NULL, SW_SHOWNORMAL);
#endif
      }

      _DSM_UnlockMemory(hImg);
      _DSM_Free(hImg);
      pDIB = 0;
      
      updateEXIMAGEINFO();

      // see if there are any more transfers to do
      PrintCMDMessage("app: Checking to see if there are more images to transfer...\n");
      TW_PENDINGXFERS pendxfers;
      memset( &pendxfers, 0, sizeof(pendxfers) );

      twrc = DSM_Entry( DG_CONTROL, DAT_PENDINGXFERS, MSG_ENDXFER, (TW_MEMREF)&pendxfers);

      if(TWRC_SUCCESS == twrc)
      {
        PrintCMDMessage("app: Remaining images to transfer: %u\n", pendxfers.Count);

        if(0 == pendxfers.Count)
        {
          // nothing left to transfer, finished.
          bPendingXfers = false;
        }
      }
      else
      {
        printError(m_pDataSource, "failed to properly end the transfer");
        bPendingXfers = false;
      }
    }
    else if(TWRC_CANCEL == twrc)
    {
      printError(m_pDataSource, "Canceled transfer image");
      break;
    }
    else if(TWRC_FAILURE == twrc)
    {
      printError(m_pDataSource, "Failed to transfer image");
      break;
    }
  }

  // Check to see if we left the scan loop before we were actualy done scanning
  // This will hapen if we had an error.  Need to let the DS know we are not going 
  // to transfer more images
  if(bPendingXfers == true)
  {
      PrintCMDMessage("app: Stop any transfer we may have started but could not finish...\n");
      TW_PENDINGXFERS pendxfers;
      memset( &pendxfers, 0, sizeof(pendxfers) );

      twrc = DSM_Entry( DG_CONTROL, DAT_PENDINGXFERS, MSG_ENDXFER, (TW_MEMREF)&pendxfers);

      // We need to get rid of any pending transfers
      if(0 != pendxfers.Count)
      {
        memset( &pendxfers, 0, sizeof(pendxfers) );

        DSM_Entry( DG_CONTROL, DAT_PENDINGXFERS, MSG_RESET, (TW_MEMREF)&pendxfers);
      }
  }

  // adjust our state now that the scanning session is done
  m_DSMState = 5;

  PrintCMDMessage("app: DONE!\n");

  return;
}
void TwainApp::updateEXIMAGEINFO()
{
  int TableBarCodeExtImgInfo[] = { 
         TWEI_BARCODETYPE, 
         TWEI_BARCODETEXTLENGTH,
         TWEI_BARCODETEXT, 
         TWEI_BARCODEX, 
         TWEI_BARCODEY, 
         TWEI_BARCODEROTATION, 
         TWEI_BARCODECONFIDENCE };
  int TableOtherExtImgInfo[] = {
         TWEI_PIXELFLAVOR, 
         TWEI_ENDORSEDTEXT, 
         TWEI_BOOKNAME,
         TWEI_CHAPTERNUMBER, 
         TWEI_DOCUMENTNUMBER, 
         TWEI_PAGENUMBER, 
         TWEI_PAGESIDE, 
         TWEI_CAMERA, 
         TWEI_FRAMENUMBER, 
         TWEI_FRAME,
         TWEI_MAGTYPE, 
         TWEI_MAGDATA };

  int       num_BarInfos    = sizeof(TableBarCodeExtImgInfo) / sizeof(TableBarCodeExtImgInfo[0]);
  int       num_OtherInfos  = sizeof(TableOtherExtImgInfo) / sizeof(TableOtherExtImgInfo[0]);
  TW_UINT16 twrc            = TWRC_SUCCESS;

  m_strExImageInfo = "";

  try
  {
    TW_EXTIMAGEINFO exImgInfo;
    exImgInfo.NumInfos = 1;
    exImgInfo.Info[0].InfoID = TWEI_BARCODECOUNT;
    exImgInfo.Info[0].NumItems=0;
    exImgInfo.Info[0].ItemType = TWTY_UINT32;
    exImgInfo.Info[0].Item = 0;
    exImgInfo.Info[0].CondCode = 0;
    
    twrc = DSM_Entry(DG_IMAGE, DAT_EXTIMAGEINFO, MSG_GET, (TW_MEMREF)&exImgInfo);
    if(twrc!= TWRC_SUCCESS)
    {
      m_strExImageInfo = "Not Supported";
      return;
    }

    int cur_Info  = 0;
    int num_Infos = num_OtherInfos;

    if(TWRC_SUCCESS==exImgInfo.Info[0].CondCode)
    {
      num_Infos += (num_BarInfos * (TW_UINT32)exImgInfo.Info[0].Item);
    }

    TW_HANDLE hexInfo = _DSM_Alloc(sizeof(TW_EXTIMAGEINFO)+sizeof(TW_INFO)*(num_Infos-1));
    TW_EXTIMAGEINFO *pexImgInfo = (TW_EXTIMAGEINFO*) _DSM_LockMemory(hexInfo);
    memset(pexImgInfo, 0,sizeof(TW_EXTIMAGEINFO)+sizeof(TW_INFO)*(num_Infos-1));
    pexImgInfo->NumInfos = num_Infos;

    if(TWRC_SUCCESS==exImgInfo.Info[0].CondCode)
    {
      for(UINT nCount = 0; nCount < (UINT)exImgInfo.Info[0].Item; nCount++)
      {
        for (int nBarItem = 0; nBarItem < num_BarInfos; nBarItem++)
        {
          pexImgInfo->Info[cur_Info++].InfoID = TableBarCodeExtImgInfo[nBarItem];
        }
      }
    }

    for (int nItem = 0; nItem < num_OtherInfos; nItem++)
    {
      pexImgInfo->Info[cur_Info++].InfoID = TableOtherExtImgInfo[nItem];
    }

    twrc = DSM_Entry(DG_IMAGE, DAT_EXTIMAGEINFO, MSG_GET, (TW_MEMREF)pexImgInfo);
    if(twrc!= TWRC_SUCCESS)
    {
      m_strExImageInfo = "Not Supported";
      return;
    }

    for(int nIndex = 0; nIndex < num_Infos; nIndex++)
    {
       if(pexImgInfo->Info[nIndex].CondCode != TWRC_INFONOTSUPPORTED)
       {
         m_strExImageInfo += convertExtImageInfoName_toString(pexImgInfo->Info[nIndex].InfoID);
         m_strExImageInfo += "\t";
         m_strExImageInfo += convertExtImageInfoItem_toString(pexImgInfo->Info[nIndex]);
         m_strExImageInfo +="\r\n";
       }

       // We have no more use for the item so free the one that are handles
       if( getTWTYsize(pexImgInfo->Info[nIndex].ItemType)*pexImgInfo->Info[nIndex].NumItems > sizeof(TW_UINT32) )
       {
         _DSM_Free((TW_HANDLE)pexImgInfo->Info[nIndex].Item);
       }
    }

    _DSM_UnlockMemory(hexInfo);
    _DSM_Free(hexInfo);
  }
	catch(...) 
	{ 
    //Log("Failure reading extended image info: %s", LPCSTR(error->m_strDescription));
	}
  return;
}


//////////////////////////////////////////////////////////////////////////////
void TwainApp::initiateTransfer_File(TW_UINT16 fileformat /*= TWFF_TIFF*/)
{
  PrintCMDMessage("app: Starting a TWSX_FILE transfer...\n");

  // start the transfer
  bool      bPendingXfers = true;
  TW_UINT16 twrc          = TWRC_SUCCESS;

  while(bPendingXfers)
  {
    m_nXferNum++;

    if(!updateIMAGEINFO())
    {
      break;
    }

    // setup the file xfer
    TW_SETUPFILEXFER filexfer;
    memset(&filexfer, 0, sizeof(filexfer));

    const char * pExt = convertICAP_IMAGEFILEFORMAT_toString(fileformat);

    SSNPRINTF(filexfer.FileName, sizeof(filexfer.FileName), sizeof(filexfer.FileName), "FROM_SCANNER_%06dF%s", m_nXferNum, pExt);

    filexfer.Format = fileformat;

    PrintCMDMessage("app: Sending file transfer details...\n");
    twrc = DSM_Entry( DG_CONTROL, DAT_SETUPFILEXFER, MSG_SET, (TW_MEMREF)&(filexfer));

    if(TWRC_SUCCESS != twrc)
    {
      printError(m_pDataSource, "Error while trying to setup the file transfer");
      break;
    }

    PrintCMDMessage("app: Starting file transfer...\n");
    twrc = DSM_Entry( DG_IMAGE, DAT_IMAGEFILEXFER, MSG_GET, 0);

    if(TWRC_XFERDONE == twrc)
    {
      // Findout where the file was actualy saved
      twrc = DSM_Entry( DG_CONTROL, DAT_SETUPFILEXFER, MSG_GET, (TW_MEMREF)&(filexfer));

      PrintCMDMessage("app: File \"%s\" saved...\n", filexfer.FileName);
#ifdef _WINDOWS
      ShellExecute(m_Parent, "open", filexfer.FileName, NULL, NULL, SW_SHOWNORMAL);
#endif
      
      updateEXIMAGEINFO();

      // see if there are any more transfers to do
      PrintCMDMessage("app: Checking to see if there are more images to transfer...\n");
      TW_PENDINGXFERS pendxfers;
      memset(&pendxfers, 0, sizeof(pendxfers));

      twrc = DSM_Entry( DG_CONTROL, DAT_PENDINGXFERS, MSG_ENDXFER, (TW_MEMREF)&pendxfers);

      if(TWRC_SUCCESS == twrc)
      {
        PrintCMDMessage("app: Remaining images to transfer: %u\n", pendxfers.Count);

        if(0 == pendxfers.Count)
        {
          // nothing left to transfer, finished.
          bPendingXfers = false;
        }
      }
      else
      {
        printError(m_pDataSource, "failed to properly end the transfer");
        bPendingXfers = false;
      }
    }
    else if(TWRC_CANCEL == twrc)
    {
      printError(m_pDataSource, "Canceled transfer image");
      break;
    }
    else if(TWRC_FAILURE == twrc)
    {
      printError(m_pDataSource, "Failed to transfer image");
      break;
    }
  }

  // Check to see if we left the scan loop before we were actualy done scanning
  // This will hapen if we had an error.  Need to let the DS know we are not going 
  // to transfer more images
  if(bPendingXfers == true)
  {
      PrintCMDMessage("app: Stop any transfer we may have started but could not finish...\n");
      TW_PENDINGXFERS pendxfers;
      memset( &pendxfers, 0, sizeof(pendxfers) );
      twrc = DSM_Entry( DG_CONTROL, DAT_PENDINGXFERS, MSG_ENDXFER, (TW_MEMREF)&pendxfers);

      // We need to get rid of any pending transfers
      if(0 != pendxfers.Count)
      {
        memset( &pendxfers, 0, sizeof(pendxfers) );
        DSM_Entry( DG_CONTROL, DAT_PENDINGXFERS, MSG_RESET, (TW_MEMREF)&pendxfers);
      }
  }

  // adjust our state now that the scanning session is done
  m_DSMState = 5;

  PrintCMDMessage("app: DONE!\n");

  return;
}

//////////////////////////////////////////////////////////////////////////////
void TwainApp::initiateTransfer_Memory()
{
  PrintCMDMessage("app: Starting a TWSX_MEMORY transfer...\n");

  // For memory transfers, the FreeImage library will not be used, instead a
  // tiff will be progressively written. This method was chosen because it
  // is possible that a 4GB image could be transferred and an image of that
  // size can not fit in most systems memory.

  CTiffWriter      *pTifImg = 0;
  TW_STR255         szOutFileName;
  TW_SETUPMEMXFER   SourcesBufferSizes;   /**< Used to set up the buffer size used by memory transfer method */
  bool              bPendingXfers = true;
  TW_UINT16         twrc          = TWRC_SUCCESS;

  // start the transfer
  while(bPendingXfers)
  {
    m_nXferNum++;
    memset(szOutFileName, 0, sizeof(szOutFileName));

    if(!updateIMAGEINFO())
    {
      break;
    }

    // Set the filename to save to
    SSNPRINTF(szOutFileName, sizeof(szOutFileName), sizeof(szOutFileName), "FROM_SCANNER_%06dM.tif", m_nXferNum);

    // get the buffer sizes that the source wants to use
    PrintCMDMessage("app: getting the buffer sizes...\n");
    memset(&SourcesBufferSizes, 0, sizeof(SourcesBufferSizes));

    twrc = DSM_Entry( DG_CONTROL, DAT_SETUPMEMXFER, MSG_GET, (TW_MEMREF)&(SourcesBufferSizes));

    if(TWRC_SUCCESS != twrc)
    {
      printError(m_pDataSource, "Error while trying to get the buffer sizes from the source!");
      break;
    }

    // -setup a buffer to hold the strip from the data source
    // -this buffer is a template that will be used to reset the real
    // buffer before each call to get a strip.
    TW_IMAGEMEMXFER memXferBufTemplate;
    memXferBufTemplate.Compression = TWON_DONTCARE16;
    memXferBufTemplate.BytesPerRow = TWON_DONTCARE32;
    memXferBufTemplate.Columns = TWON_DONTCARE32;
    memXferBufTemplate.Rows = TWON_DONTCARE32;
    memXferBufTemplate.XOffset = TWON_DONTCARE32;
    memXferBufTemplate.YOffset = TWON_DONTCARE32;
    memXferBufTemplate.BytesWritten = TWON_DONTCARE32;

    memXferBufTemplate.Memory.Flags = TWMF_APPOWNS | TWMF_POINTER;
    memXferBufTemplate.Memory.Length = SourcesBufferSizes.Preferred;

    TW_HANDLE hMem = (TW_HANDLE)_DSM_Alloc(SourcesBufferSizes.Preferred);
    if(0 == hMem)
    {
      printError(0, "Error allocating memory");
      break;
    }

    memXferBufTemplate.Memory.TheMem = (TW_MEMREF)_DSM_LockMemory(hMem);

    // this is the real buffer that will be sent to the data source
    TW_IMAGEMEMXFER memXferBuf;

    // this is set to true once one row has been successfully acquired. We have
    // to track this because we can't transition to state 7 until a row has been
    // received.
    bool bScanStarted = false;

    int nBytePerRow = (((m_ImageInfo.ImageWidth * m_ImageInfo.BitsPerPixel)+7)/8);

    // now that the memory has been setup, get the data from the scanner
    PrintCMDMessage("app: starting the memory transfer...\n");
    while(1)
    {
      // reset the xfer buffer
      memcpy(&memXferBuf, &memXferBufTemplate, sizeof(memXferBufTemplate));

      // clear the row data buffer
      memset(memXferBuf.Memory.TheMem, 0, memXferBuf.Memory.Length);

      // get the row data
      twrc = DSM_Entry( DG_IMAGE, DAT_IMAGEMEMXFER, MSG_GET, (TW_MEMREF)&(memXferBuf));

      if(TWRC_SUCCESS == twrc || TWRC_XFERDONE == twrc)
      {
        if(!bScanStarted)
        {
          // the state can be changed to state 7 now that we have successfully
          // received at least one strip
          m_DSMState = 7;
          bScanStarted = true;

          // write the tiff header now that all info needed for the header has
          // been received.

          pTifImg = new CTiffWriter(szOutFileName,
            m_ImageInfo.ImageWidth,
            m_ImageInfo.ImageLength,
            m_ImageInfo.BitsPerPixel,
            nBytePerRow);
          
          pTifImg->setXResolution(m_ImageInfo.XResolution.Whole, 1);
          pTifImg->setYResolution(m_ImageInfo.YResolution.Whole, 1);

          pTifImg->writeImageHeader();
        }

        char* pbuf = reinterpret_cast<char*>(memXferBuf.Memory.TheMem);

        // write the received image data to the image file
        for(unsigned int x = 0; x < memXferBuf.Rows; ++x)
        {
          pTifImg->WriteTIFFData(pbuf, nBytePerRow);
          pbuf += memXferBuf.BytesPerRow;
        }

        if(TWRC_XFERDONE == twrc)
        {
          // deleting the CTiffWriter object will close the file
          if(pTifImg)
          {
            delete pTifImg;
            pTifImg = 0;
          }

          PrintCMDMessage("app: File \"%s\" saved...\n", szOutFileName);
#ifdef _WINDOWS
          ShellExecute(m_Parent, "open", szOutFileName, NULL, NULL, SW_SHOWNORMAL);
#endif
          updateEXIMAGEINFO();
          break;
        }
      }
      else if(TWRC_CANCEL == twrc)
      {
        printError(m_pDataSource, "Canceled transfer while trying to get a strip of data from the source!");
        break;
      }
      else if(TWRC_FAILURE == twrc)
      {
        printError(m_pDataSource, "Error while trying to get a strip of data from the source!");
        break;
      }
    }

    // cleanup
    if(pTifImg)
    {
      delete pTifImg;
      pTifImg = 0;
    }
    // cleanup memory used to transfer image
    _DSM_UnlockMemory(hMem);
    _DSM_Free(hMem);

    if(TWRC_XFERDONE != twrc)
    {
      // We were not able to transfer an image don't try to transfer more
      break;
    }

    // The transfer is done. Tell the source
    PrintCMDMessage("app: Checking to see if there are more images to transfer...\n");
    TW_PENDINGXFERS pendxfers;
    memset( &pendxfers, 0, sizeof(pendxfers) );

    twrc = DSM_Entry( DG_CONTROL, DAT_PENDINGXFERS, MSG_ENDXFER, (TW_MEMREF)&pendxfers);

    if(TWRC_SUCCESS == twrc)
    {
      PrintCMDMessage("app: Remaining images to transfer: %u\n", pendxfers.Count);
      if(0 == pendxfers.Count)
      {
        // nothing left to transfer, finished.
        bPendingXfers = false;
      }
    }
    else
    {
      printError(m_pDataSource, "failed to properly end the transfer");
      bPendingXfers = false;
    }

  }

  // Check to see if we left the scan loop before we were actualy done scanning
  // This will hapen if we had an error.  Need to let the DS know we are not going 
  // to transfer more images
  if(bPendingXfers == true)
  {
      PrintCMDMessage("app: Stop any transfer we may have started but could not finish...\n");
      TW_PENDINGXFERS pendxfers;
      memset( &pendxfers, 0, sizeof(pendxfers) );
      twrc = DSM_Entry( DG_CONTROL, DAT_PENDINGXFERS, MSG_ENDXFER, (TW_MEMREF)&pendxfers);

      // We need to get rid of any pending transfers
      if(0 != pendxfers.Count)
      {
        memset( &pendxfers, 0, sizeof(pendxfers) );
        DSM_Entry( DG_CONTROL, DAT_PENDINGXFERS, MSG_RESET, (TW_MEMREF)&pendxfers);
      }
  }

  // adjust our state now that the scanning session is done
  m_DSMState = 5;

  PrintCMDMessage("app: DONE!\n");

  return;
}

//////////////////////////////////////////////////////////////////////////////
FIBITMAP* TwainApp::createDIB()
{
  FIBITMAP *pDIB = FreeImage_Allocate(m_ImageInfo.ImageWidth,
    m_ImageInfo.ImageLength,
    24);

  if(0 == pDIB)
  {
    PrintCMDMessage("app: Error creating image structure in memory!\n");
    assert(0);
    return 0;
  }

  // Adjust dib to currently set capabilities. If the cap specs a 24 bit image,
  // there is no need to do anything since it was created as 24 bit to begin
  // with.
  if(m_ImageInfo.SamplesPerPixel != 3)// !=RGB
  {
    FIBITMAP *pdib = 0;
    switch(m_ImageInfo.SamplesPerPixel)
    {
    case 1: //TWPT_BW
      pdib = FreeImage_Threshold(pDIB, 128);
      break;

    case 2: //TWPT_GRAY
      pdib = FreeImage_ConvertTo8Bits(pDIB);
      break;
    }

    if(0 == pdib)
    {
      printError(0, "Error: Could not setup in memory image structure");
      assert(0);
      return 0;
    }
    else
    {
      FreeImage_Unload(pDIB);
      pDIB = pdib;
    }
  }

  return pDIB;
}

//////////////////////////////////////////////////////////////////////////////
pTW_IDENTITY TwainApp::getAppIdentity()
{
  return &m_MyInfo;
}


//////////////////////////////////////////////////////////////////////////////
pTW_IDENTITY TwainApp::getDataSource(TW_INT16 _index /*= -1*/) const
{
  if(_index < 0 )
  {
    return m_pDataSource;
  }
  else
  {
    if(((unsigned int)_index) < m_DataSources.size())
    {
      return (pTW_IDENTITY)&m_DataSources[_index];
    }
    else
    {
      return NULL;
    }
  }
}

//////////////////////////////////////////////////////////////////////////////
TW_INT16 TwainApp::get_CAP(TW_CAPABILITY& _cap)
{
  if(m_DSMState < 4)
  {
    PrintCMDMessage("You need to open a data source first.\n");
    return TWCC_SEQERROR;
  }

  TW_INT16 CondCode = TWCC_SUCCESS;

  // Check if this capability structure has memory already alloc'd.
  // If it does, free that memory before the call else we'll have a memory
  // leak because the source allocates memory during a MSG_GET.
  if(0 != _cap.hContainer)
  {
    _DSM_Free(_cap.hContainer);
    _cap.hContainer = 0;
  }

  _cap.ConType = TWON_DONTCARE16;

  // capability structure is set, make the call to the source now
  TW_UINT16 twrc = DSM_Entry( DG_CONTROL, DAT_CAPABILITY, MSG_GET, (TW_MEMREF)&_cap);

  switch(twrc)
  {
  case TWRC_FAILURE:
    string strErr = "Failed to get the capability: [";
    strErr += convertCAP_toString(_cap.Cap);
    strErr += "]";
    
    CondCode = printError(m_pDataSource, strErr);
    break;
  }

  return CondCode;
}


//////////////////////////////////////////////////////////////////////////////
TW_UINT16 TwainApp::set_CapabilityOneValue(TW_UINT16 Cap, const int _value, TW_UINT16 _type)
{
  TW_INT16        twrc = TWRC_FAILURE;
  TW_CAPABILITY   cap;
         
  cap.Cap         = Cap;
  cap.ConType     = TWON_ONEVALUE;
  cap.hContainer  = _DSM_Alloc(sizeof(TW_ONEVALUE));// Largest int size
  if(0 == cap.hContainer)
  {
    printError(0, "Error allocating memory");
    return false;
  }
  pTW_ONEVALUE pVal = (pTW_ONEVALUE)_DSM_LockMemory(cap.hContainer);

  pVal->ItemType  = _type;
  pVal->Item      = _value;

  // capability structure is set, make the call to the source now
  twrc = DSM_Entry( DG_CONTROL, DAT_CAPABILITY, MSG_SET, (TW_MEMREF)&(cap));
  if(TWRC_SUCCESS != twrc)
  {
    printError(m_pDataSource, "Could not set capability");
  }

  _DSM_UnlockMemory(cap.hContainer);
  _DSM_Free(cap.hContainer);
  return twrc;
}

//////////////////////////////////////////////////////////////////////////////
TW_UINT16 TwainApp::set_CapabilityOneValue(TW_UINT16 Cap, const pTW_FIX32 _pValue)
{
  TW_INT16        twrc = TWRC_FAILURE;
  TW_CAPABILITY   cap;

  cap.Cap         = Cap;
  cap.ConType     = TWON_ONEVALUE;
  cap.hContainer  = _DSM_Alloc(sizeof(TW_ONEVALUE_FIX32));
  if(0 == cap.hContainer)
  {
    printError(0, "Error allocating memory");
    return false;
  }

  pTW_ONEVALUE_FIX32 pVal = (pTW_ONEVALUE_FIX32)_DSM_LockMemory(cap.hContainer);

  pVal->ItemType = TWTY_FIX32;
  pVal->Item     = *_pValue;

  // capability structure is set, make the call to the source now
  twrc = DSM_Entry( DG_CONTROL, DAT_CAPABILITY, MSG_SET, (TW_MEMREF)&(cap));
  if(TWRC_SUCCESS != twrc)
  {
    printError(m_pDataSource, "Could not set capability");
  }

  _DSM_UnlockMemory(cap.hContainer);
  _DSM_Free(cap.hContainer);

  return twrc;
}

//////////////////////////////////////////////////////////////////////////////
TW_UINT16 TwainApp::set_CapabilityOneValue(TW_UINT16 Cap, const pTW_FRAME _pValue)
{
  TW_INT16        twrc = TWRC_FAILURE;
  TW_CAPABILITY   cap;

  cap.Cap         = Cap;
  cap.ConType     = TWON_ONEVALUE;
  cap.hContainer  = _DSM_Alloc(sizeof(TW_ONEVALUE_FRAME));
  if(0 == cap.hContainer)
  {
    printError(0, "Error allocating memory");
    return false;
  }

  pTW_ONEVALUE_FRAME pVal = (pTW_ONEVALUE_FRAME)_DSM_LockMemory(cap.hContainer);

  pVal->ItemType = TWTY_FRAME;
  pVal->Item     = *_pValue;

  // capability structure is set, make the call to the source now
  twrc = DSM_Entry( DG_CONTROL, DAT_CAPABILITY, MSG_SET, (TW_MEMREF)&(cap));
  if(TWRC_SUCCESS != twrc)
  {
    printError(m_pDataSource, "Could not set capability");
  }

  _DSM_UnlockMemory(cap.hContainer);
  _DSM_Free(cap.hContainer);

  return twrc;
}


