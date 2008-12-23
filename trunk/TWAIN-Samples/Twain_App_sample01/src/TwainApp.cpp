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
#include "main.h"

#include <iostream>
#include <cstdlib>
#include <assert.h>
#include <stdio.h>

#include "TwainApp.h"
#include "CTiffWriter.h"
#include "TwainApp_ui.h"

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

//////////////////////////////////////////////////////////////////////////////
/**
* Output error messages for Free Image Format.
* @param[in] fif Free Image Format
* @param[in] message error string to display
*/
void FreeImageErrorHandler(FREE_IMAGE_FORMAT fif, const char *message)
{
  cout << "\n*** ";
  cout << FreeImage_GetFormatFromFIF(fif) << " Format" << endl;
  cout << message << " ***" << endl;
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
    cout << "The DSM has already been opened, close it first" << endl;
    return;
  }

  if(!LoadDSMLib(kTWAIN_DS_DIR kTWAIN_DSM_DLL_NAME))
  {
    cerr << "The DSM could not be opened. Please ensure that it is installed into a directory that is in the library path: " << kTWAIN_DS_DIR kTWAIN_DSM_DLL_NAME << endl;
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
    cout << "DG_CONTROL / DAT_PARENT / MSG_OPENDSM Failed: " << ret << endl;
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
      cout << "DG_CONTROL / DAT_ENTRYPOINT / MSG_GET Failed: " << ret << endl;
      return;
    }
  }

  cout << "Successfully opened the DSM" << endl;
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
    cout << "The DSM has not been opened, open it first" << endl;
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
    cout << "Successfully closed the DSM" << endl;
    m_DSMState = 2;
  }
  else
  {
    printError(0, "Failed to close the DSM");
  }

  return;
}

//////////////////////////////////////////////////////////////////////////////
void TwainApp::printIdentStruct(const TW_IDENTITY& _ident)
{
  cout << "\n"
    << "Id: " << _ident.Id << "\n"
    << "Version: " << _ident.Version.MajorNum << "." << _ident.Version.MinorNum << "\n"
    << "SupportedGroups: " << _ident.SupportedGroups << "\n"
    << "Manufacturer: " << _ident.Manufacturer << "\n"
    << "ProductFamily: " << _ident.ProductFamily << "\n"
    << "ProductName: " << _ident.ProductName << "\n"
    << endl;

  return;
}

//////////////////////////////////////////////////////////////////////////////
void TwainApp::printIdentityStruct(const TW_UINT32 _identityID)
{
  for(unsigned int x = 0; x < m_DataSources.size(); ++x)
  {
    if(_identityID == m_DataSources[x].Id)
    {
      printIdentStruct(m_DataSources[x]);
      break;
    }
  }

  return;
}

//////////////////////////////////////////////////////////////////////////////
void TwainApp::printAvailableDataSources()
{
  if(m_DSMState < 3)
  {
    cout << "The DSM has not been opened yet, please open it first" << endl;
    return;
  }

  // print the Id and name of each available source
  for(unsigned int x = 0; x < m_DataSources.size(); ++x)
  {
    cout << m_DataSources[x].Id << ". " << m_DataSources[x].ProductName << " by " << m_DataSources[x].Manufacturer << endl;
  }

  return;
}

//////////////////////////////////////////////////////////////////////////////
void TwainApp::loadDS(const TW_UINT32 _dsID)
{
  // The application must be in state 3 to open a Data Source.
  if(m_DSMState < 3)
  {
    cout << "The DSM needs to be opened first." << endl;
    return;
  }
  else if(m_DSMState > 3)
  {
    cout << "A source has already been opened, please close it first" << endl;
    return;
  }

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
    cout << "Data source with id: [" << _dsID << "] can not be found" << endl;
    return;
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
    cout << "Data source successfully opened!" << endl;
    // Transition application to state 4
    m_DSMState = 4;

    // get all the initial capabilities of this source
    initCaps();

    callback.CallBackProc = (TW_MEMREF)DSMCallback;    

    if(TWRC_SUCCESS != (twrc = DSM_Entry(DG_CONTROL, DAT_CALLBACK, MSG_REGISTER_CALLBACK, (TW_MEMREF)&callback)))
    {
      cout << "DG_CONTROL / DAT_CALLBACK / MSG_REGISTER_CALLBACK Failed: " << twrc << endl;
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
    cout << "You need to open a data source first." << endl;
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
    cout << "Data source successfully closed" << endl;

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
    cout << "You need to open the DSM first." << endl;
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

TW_IDENTITY Source; /**< used to store the source that is return by getDefaultDataSource */
//////////////////////////////////////////////////////////////////////////////
pTW_IDENTITY TwainApp::getDefaultDataSource()
{
  if(m_DSMState < 3)
  {
    cout << "You need to open the DSM first." << endl;
    return NULL;
  }

  // get default

  memset(&Source, 0, sizeof(TW_IDENTITY));

  TW_UINT16 twrc;

  twrc = _DSM_Entry(
    &m_MyInfo,
    0,
    DG_CONTROL,
    DAT_IDENTITY,
    MSG_GETDEFAULT,
    (TW_MEMREF) &Source);

  switch (twrc)
  {
    case TWRC_SUCCESS:
      break;

    case TWRC_FAILURE:
      printError(0, "Failed to get the data source info!");
      break;
  }

  return &Source;
}

//////////////////////////////////////////////////////////////////////////////
void TwainApp::printError(pTW_IDENTITY _pdestID, const string& _errorMsg)
{
  TW_INT16 c;

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
    cerr << " The condition code is: " << c << endl;
  }

  return;
}

//////////////////////////////////////////////////////////////////////////////
void TwainApp::initCaps()
{
  if(m_DSMState < 3)
  {
    cout << "The DSM needs to be opened first." << endl;
    return;
  }
  else if(m_DSMState < 4)
  {
    cout << "A Data Source needs to be opened first." << endl;
    return;
  }

  // get the default pixel type
  m_CAP_XFERCOUNT.Cap = CAP_XFERCOUNT;
  get_CAP(m_CAP_XFERCOUNT);

  m_ICAP_PIXELTYPE.Cap = ICAP_PIXELTYPE;
  get_CAP(m_ICAP_PIXELTYPE);

  m_ICAP_XFERMECH.Cap = ICAP_XFERMECH;
  get_CAP(m_ICAP_XFERMECH);
  
  m_ICAP_IMAGEFILEFORMAT.Cap = ICAP_IMAGEFILEFORMAT;
  get_CAP(m_ICAP_IMAGEFILEFORMAT);
    
  m_ICAP_COMPRESSION.Cap = ICAP_COMPRESSION;
  get_CAP(m_ICAP_COMPRESSION);
    
  m_ICAP_UNITS.Cap = ICAP_UNITS;
  get_CAP(m_ICAP_UNITS);

  m_ICAP_BITDEPTH.Cap = ICAP_BITDEPTH;
  get_CAP(m_ICAP_BITDEPTH);

  m_ICAP_XRESOLUTION.Cap = ICAP_XRESOLUTION;
  get_CAP(m_ICAP_XRESOLUTION);

  m_ICAP_YRESOLUTION.Cap = ICAP_YRESOLUTION;
  get_CAP(m_ICAP_YRESOLUTION);

  m_ICAP_FRAMES.Cap = ICAP_FRAMES;
  get_CAP(m_ICAP_FRAMES);

  return;
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
bool TwainApp::enableDS()
{
  bool bret = true;

  if(m_DSMState < 4)
  {
    cout << "You need to open the data source first." << endl;
    return false;
  }

  m_ui.ShowUI = TRUE;
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
    cout << "You need to enable the data source first." << endl;
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
void TwainApp::startScan()
{
  if(m_DSMState != 6)
  {
    cerr << "A scan cannot be initiated unless we are in state 6" << endl;
    return;
  }

  TW_UINT16 mech;
  if(!getICAP_XFERMECH(mech))
  {
    cerr << "Error: could not get the transfer mechanism" << endl;
    return;
  }

  switch (mech)
  {
  case TWSX_NATIVE:
    initiateTransfer_Native();
    break;

  case TWSX_FILE:
    initiateTransfer_File();
    break;

  case TWSX_MEMORY:
    initiateTransfer_Memory();
    break;
  }

  return;
}

//////////////////////////////////////////////////////////////////////////////
bool TwainApp::updateIMAGEINFO()
{
  // clear our image info structure
  memset(&m_ImageInfo, 0, sizeof(m_ImageInfo));

  // get the image details
  cout << "app: Getting the image info..." << endl;
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
  cout << "app: Starting a TWSX_NATIVE transfer..." << endl;

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

    cout << "app: Starting the transfer..." << endl;
    twrc = DSM_Entry( DG_IMAGE, DAT_IMAGENATIVEXFER, MSG_GET, (TW_MEMREF)&hImg);

    if(TWRC_XFERDONE == twrc)
    {
      // -Here we get a handle to a DIB. Save it to disk as a bmp. 
      // -After saving it to disk, I could open it up again using FreeImage
      // if I wanted to do more transforms on it or save it as a different format.
      PBITMAPINFOHEADER pDIB = (PBITMAPINFOHEADER)_DSM_LockMemory(hImg);

      if(0 == pDIB)
      {
        cerr << "App: Unable to lock memory, transfer failed" << endl;
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

#ifdef _WINDOWS
        ShellExecute(m_Parent, "open", szOutFileName, NULL, NULL, SW_SHOWNORMAL);
#else
         cout << "app: File \"" << szOutFileName << "\" saved..." << endl;
#endif
      }

      _DSM_UnlockMemory(hImg);
      _DSM_Free(hImg);
      pDIB = 0;
      
      updateEXIMAGEINFO();

      // see if there are any more transfers to do
      cout << "app: Checking to see if there are more images to transfer..." << endl;
      TW_PENDINGXFERS pendxfers;
      memset( &pendxfers, 0, sizeof(pendxfers) );

      twrc = DSM_Entry( DG_CONTROL, DAT_PENDINGXFERS, MSG_ENDXFER, (TW_MEMREF)&pendxfers);

      if(TWRC_SUCCESS == twrc)
      {
        cout << "app: Remaining images to transfer: " << pendxfers.Count << endl;

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
      cout << "app: Stop any transfer we may have started but could not finish..." << endl;
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

  cout << "app: DONE!" << endl;

  return;
}
void TwainApp::updateEXIMAGEINFO()
{
  try
  {
    TW_EXTIMAGEINFO exImgInfo;
    exImgInfo.NumInfos = 1;
    exImgInfo.Info[0].InfoID = TWEI_BARCODECOUNT;
    exImgInfo.Info[0].NumItems=0;
    exImgInfo.Info[0].ItemType = TWTY_UINT32;
    exImgInfo.Info[0].Item = 0;
    exImgInfo.Info[0].CondCode = 0;
    
    TW_UINT16 twrc          = TWRC_SUCCESS;
    twrc =     _DSM_Entry(
      &m_MyInfo,
      m_pDataSource,
      DG_IMAGE,
      DAT_EXTIMAGEINFO,
      MSG_GET,
      (TW_MEMREF)&exImgInfo);
    twrc = 0;
    if(twrc!= TWRC_SUCCESS)
    {
      return;
    }

/** 
* @def NUMBER_INFOS   
*   Number of extended image infos to retrieve.
* @def BARCODE_INFOS  
*   Number of extended image info for each barcode to retrieve.
*/
#define NUMBER_INFOS 10
#define BARCODE_INFOS 6
    int num_infos = NUMBER_INFOS;
    int cur_info = 0;
    if(TWRC_SUCCESS==exImgInfo.Info[0].CondCode)
    {
      num_infos += (BARCODE_INFOS * (TW_UINT32)exImgInfo.Info[0].Item);
    }

    TW_HANDLE hexInfo = _DSM_Alloc(sizeof(TW_EXTIMAGEINFO)+sizeof(TW_INFO)*(num_infos-1));
    TW_EXTIMAGEINFO *pexImgInfo = (TW_EXTIMAGEINFO*) _DSM_LockMemory(hexInfo);
    memset(pexImgInfo, 0,sizeof(TW_EXTIMAGEINFO)+sizeof(TW_INFO)*(num_infos-1));
    pexImgInfo->NumInfos = num_infos;

    if(TWRC_SUCCESS==exImgInfo.Info[0].CondCode)
    {
      for(UINT nCount = 0; nCount < exImgInfo.Info[0].Item; nCount++)
      {
        pexImgInfo->Info[cur_info].InfoID = TWEI_BARCODETYPE;
        pexImgInfo->Info[cur_info].ItemType = TWTY_UINT32;
        cur_info++;
        pexImgInfo->Info[cur_info].InfoID = TWEI_BARCODETEXTLENGTH;
        pexImgInfo->Info[cur_info].ItemType = TWTY_UINT32;
        cur_info++;
        pexImgInfo->Info[cur_info].InfoID = TWEI_BARCODETEXT;
        pexImgInfo->Info[cur_info].ItemType = 0;
        cur_info++;
        pexImgInfo->Info[cur_info].InfoID = TWEI_BARCODEX;
        pexImgInfo->Info[cur_info].ItemType = TWTY_UINT32;
        cur_info++;
        pexImgInfo->Info[cur_info].InfoID = TWEI_BARCODEY;
        pexImgInfo->Info[cur_info].ItemType = TWTY_UINT32;
        cur_info++;
        pexImgInfo->Info[cur_info].InfoID = TWEI_BARCODEROTATION;
        pexImgInfo->Info[cur_info].ItemType = TWTY_UINT32;
        cur_info++;
        pexImgInfo->Info[cur_info].InfoID = TWEI_BARCODECONFIDENCE;
        pexImgInfo->Info[cur_info].ItemType = TWTY_UINT32;
        cur_info++;      
      }
    }

    pexImgInfo->Info[cur_info].InfoID = TWEI_BOOKNAME;
    pexImgInfo->Info[cur_info].ItemType = 0;
    cur_info++;      

    pexImgInfo->Info[cur_info].InfoID = TWEI_CHAPTERNUMBER;
    pexImgInfo->Info[cur_info].ItemType = TWTY_UINT32;
    cur_info++;      

    pexImgInfo->Info[cur_info].InfoID = TWEI_DOCUMENTNUMBER;
    pexImgInfo->Info[cur_info].ItemType = TWTY_UINT32;
    cur_info++;      

    pexImgInfo->Info[cur_info].InfoID = TWEI_PAGENUMBER;
    pexImgInfo->Info[cur_info].ItemType = TWTY_UINT32;
    cur_info++;      

    pexImgInfo->Info[cur_info].InfoID = TWEI_PAGESIDE;
    pexImgInfo->Info[cur_info].ItemType = TWTY_UINT32;
    cur_info++;      

    pexImgInfo->Info[cur_info].InfoID = TWEI_CAMERA;
    pexImgInfo->Info[cur_info].ItemType = 0;
    cur_info++;      

    pexImgInfo->Info[cur_info].InfoID = TWEI_FRAMENUMBER;
    pexImgInfo->Info[cur_info].ItemType = TWTY_UINT32;
    cur_info++;      

    pexImgInfo->Info[cur_info].InfoID = TWEI_FRAME;
    pexImgInfo->Info[cur_info].ItemType = 0;
    cur_info++;      

    pexImgInfo->Info[cur_info].InfoID = TWEI_MAGTYPE;
    pexImgInfo->Info[cur_info].ItemType = TWTY_UINT16;
    cur_info++;      

    pexImgInfo->Info[cur_info].InfoID = TWEI_MAGDATA;
    pexImgInfo->Info[cur_info].ItemType = 0;
    cur_info++;      

    twrc =     _DSM_Entry(
      &m_MyInfo,
      m_pDataSource,
      DG_IMAGE,
      DAT_EXTIMAGEINFO,
      MSG_GET,
      (TW_MEMREF)pexImgInfo);
    twrc = 0;
    if(twrc!= TWRC_SUCCESS)
    {
      return;
    }
    m_strExImageInfo = "";
    for(int nIndex = 0; nIndex < num_infos; nIndex++)
    {
      m_strExImageInfo +=GetExtImageInfoName(pexImgInfo->Info[nIndex].InfoID);
      m_strExImageInfo +="\t";
      if(TWRC_SUCCESS==pexImgInfo->Info[nIndex].CondCode)
      {
        char chTemp[1024];
        switch(pexImgInfo->Info[nIndex].ItemType)
        {
          case TWTY_UINT32:
          case TWTY_UINT16:
            sprintf_s(chTemp,1024,"%d",pexImgInfo->Info[nIndex].Item);
            break;
          case TWTY_STR32:
          case TWTY_STR64:
          case TWTY_STR128:
          case TWTY_STR255:
          case TWTY_STR1024:
            {
            char *chTest = (char *)_DSM_LockMemory((TW_HANDLE)pexImgInfo->Info[nIndex].Item);

            sprintf_s(chTemp,1024,"%s",chTest);
            _DSM_UnlockMemory((TW_HANDLE)pexImgInfo->Info[nIndex].Item);
            _DSM_Free((TW_HANDLE)pexImgInfo->Info[nIndex].Item);
            }
            break;
          case TWTY_FRAME:
            {
            TW_FRAME *pFrame = (TW_FRAME *)_DSM_LockMemory((TW_HANDLE)pexImgInfo->Info[nIndex].Item);

            sprintf_s(chTemp,1024,"\r\n\tLeft\t%0.2f\r\n\tTop\t%.2f\r\n\tRight\t%.2f\r\n\tBottom\t%.2f",
                      FIX32ToFloat(pFrame->Left),FIX32ToFloat(pFrame->Top),
                      FIX32ToFloat(pFrame->Right),FIX32ToFloat(pFrame->Bottom));
            _DSM_UnlockMemory((TW_HANDLE)pexImgInfo->Info[nIndex].Item);
            _DSM_Free((TW_HANDLE)pexImgInfo->Info[nIndex].Item);
            }
            break;
          default:
            sprintf_s(chTemp,1024,"Unsupporetd type");
            break;
        }
        m_strExImageInfo +=chTemp;
      }
      else
      {
        m_strExImageInfo +="Unsupported";
      }
      m_strExImageInfo +="\r\n";
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

string TwainApp::GetExtImageInfoName(int InfoID)
{
  string text;

  switch(InfoID)
  {
  case TWEI_BARCODEX:
    text = "TWEI_BARCODEX:";
    break;

  case TWEI_BARCODEY:
    text = "TWEI_BARCODEY:";
    break;

	case TWEI_BARCODETEXT:
    text = "TWEI_BARCODETEXT:";
    break;

	case TWEI_BARCODETYPE:
    text = "TWEI_BARCODETYPE:";
    break;

	case TWEI_DESHADETOP:
    text = "TWEI_DESHADETOP:";
    break;

	case TWEI_DESHADELEFT:
    text = "TWEI_DESHADELEFT:";
    break;

	case TWEI_DESHADEHEIGHT:
    text = "TWEI_DESHADEHEIGHT:";
    break;

	case TWEI_DESHADEWIDTH:
    text = "TWEI_DESHADEWIDTH:";
    break;

	case TWEI_DESHADESIZE:
    text = "TWEI_DESHADESIZE:";
    break;

	case TWEI_SPECKLESREMOVED:
    text = "TWEI_SPECKLESREMOVED:";
    break;

	case TWEI_HORZLINEXCOORD:
    text = "TWEI_HORZLINEXCOORD:";
    break;

	case TWEI_HORZLINEYCOORD:
    text = "TWEI_HORZLINEYCOORD:";
    break;

	case TWEI_HORZLINELENGTH:
    text = "TWEI_HORZLINELENGTH:";
    break;

	case TWEI_HORZLINETHICKNESS:
    text = "TWEI_HORZLINETHICKNESS:";
    break;

	case TWEI_VERTLINEXCOORD:
    text = "TWEI_VERTLINEXCOORD:";
    break;

	case TWEI_VERTLINEYCOORD:
    text = "TWEI_VERTLINEYCOORD:";
    break;

	case TWEI_VERTLINELENGTH:
    text = "TWEI_VERTLINELENGTH:";
    break;

	case TWEI_VERTLINETHICKNESS:
    text = "TWEI_VERTLINETHICKNESS:";
    break;

	case TWEI_PATCHCODE:
    text = "TWEI_PATCHCODE:";
    break;

	case TWEI_ENDORSEDTEXT:
    text = "TWEI_ENDORSEDTEXT:";
    break;

	case TWEI_FORMCONFIDENCE:
    text = "TWEI_FORMCONFIDENCE:";
    break;

	case TWEI_FORMTEMPLATEMATCH:
    text = "TWEI_FORMTEMPLATEMATCH:";
    break;

	case TWEI_FORMTEMPLATEPAGEMATCH:
    text = "TWEI_FORMTEMPLATEPAGEMATCH:";
    break;

	case TWEI_FORMHORZDOCOFFSET:
    text = "TWEI_FORMHORZDOCOFFSET:";
    break;

	case TWEI_FORMVERTDOCOFFSET:
    text = "TWEI_FORMVERTDOCOFFSET:";
    break;

	case TWEI_BARCODECOUNT:
    text = "TWEI_BARCODECOUNT:";
    break;

	case TWEI_BARCODECONFIDENCE:
    text = "TWEI_BARCODECONFIDENCE:";
    break;

	case TWEI_BARCODEROTATION:
    text = "TWEI_BARCODEROTATION:";
    break;

	case TWEI_BARCODETEXTLENGTH:
    text = "TWEI_BARCODETEXTLENGTH:";
    break;

	case TWEI_DESHADECOUNT:
    text = "TWEI_DESHADECOUNT:";
    break;

	case TWEI_DESHADEBLACKCOUNTOLD:
    text = "TWEI_DESHADEBLACKCOUNTOLD:";
    break;

	case TWEI_DESHADEBLACKCOUNTNEW:
    text = "TWEI_DESHADEBLACKCOUNTNEW:";
    break;

	case TWEI_DESHADEBLACKRLMIN:
    text = "TWEI_DESHADEBLACKRLMIN:";
    break;

	case TWEI_DESHADEBLACKRLMAX:
    text = "TWEI_DESHADEBLACKRLMAX:";
    break;

	case TWEI_DESHADEWHITECOUNTOLD:
    text = "TWEI_DESHADEWHITECOUNTOLD:";
    break;

	case TWEI_DESHADEWHITECOUNTNEW:
    text = "TWEI_DESHADEWHITECOUNTNEW:";
    break;

	case TWEI_DESHADEWHITERLMIN:
    text = "TWEI_DESHADEWHITERLMIN:";
    break;

	case TWEI_DESHADEWHITERLAVE:
    text = "TWEI_DESHADEWHITERLAVE:";
    break;

	case TWEI_DESHADEWHITERLMAX:
    text = "TWEI_DESHADEWHITERLMAX:";
    break;

	case TWEI_BLACKSPECKLESREMOVED:
    text = "TWEI_BLACKSPECKLESREMOVED:";
    break;

	case TWEI_WHITESPECKLESREMOVED:
    text = "TWEI_WHITESPECKLESREMOVED:";
    break;

	case TWEI_HORZLINECOUNT:
    text = "TWEI_HORZLINECOUNT:";
    break;

	case TWEI_VERTLINECOUNT:
    text = "TWEI_VERTLINECOUNT:";
    break;

	case TWEI_DESKEWSTATUS:
    text = "TWEI_DESKEWSTATUS:";
    break;

	case TWEI_SKEWORIGINALANGLE:
    text = "TWEI_SKEWORIGINALANGLE:";
    break;

	case TWEI_SKEWFINALANGLE:
    text = "TWEI_SKEWFINALANGLE:";
    break;

	case TWEI_SKEWCONFIDENCE:
    text = "TWEI_SKEWCONFIDENCE:";
    break;

	case TWEI_SKEWWINDOWX1:
    text = "TWEI_SKEWWINDOWX1:";
    break;

	case TWEI_SKEWWINDOWY1:
    text = "TWEI_SKEWWINDOWY1:";
    break;

	case TWEI_SKEWWINDOWX2:
    text = "TWEI_SKEWWINDOWX2:";
    break;

	case TWEI_SKEWWINDOWY2:
    text = "TWEI_SKEWWINDOWY2:";
    break;

	case TWEI_SKEWWINDOWX3:
    text = "TWEI_SKEWWINDOWX3:";
    break;

	case TWEI_SKEWWINDOWY3:
    text = "TWEI_SKEWWINDOWY3:";
    break;

	case TWEI_SKEWWINDOWX4:
    text = "TWEI_SKEWWINDOWX4:";
    break;

	case TWEI_SKEWWINDOWY4:
    text = "TWEI_SKEWWINDOWY4:";
    break;

	case TWEI_BOOKNAME:
    text = "TWEI_BOOKNAME:";
    break;

	case TWEI_CHAPTERNUMBER:
    text = "TWEI_CHAPTERNUMBER:";
    break;

	case TWEI_DOCUMENTNUMBER:
    text = "TWEI_DOCUMENTNUMBER:";
    break;

	case TWEI_PAGENUMBER:
    text = "TWEI_PAGENUMBER:";
    break;

	case TWEI_CAMERA:
    text = "TWEI_CAMERA:";
    break;

	case TWEI_FRAMENUMBER:
    text = "TWEI_FRAMENUMBER:";
    break;

	case TWEI_FRAME:
    text = "TWEI_FRAME:";
    break;

	case TWEI_PIXELFLAVOR:
    text = "TWEI_PIXELFLAVOR:";
    break;

	case TWEI_PAGESIDE:
    text = "TWEI_PAGESIDE:";
    break;

	case TWEI_MAGDATA:
    text = "TWEI_MAGDATA:";
    break;

	case TWEI_MAGTYPE:
    text = "TWEI_MAGTYPE:";
    break;

  default:
    {
      char chTemp[256];
      sprintf_s(chTemp,256,"ExtImageInfo ID 0x:4X",InfoID);
      text = chTemp;
    }
    break;
  }

  return text;
}

//////////////////////////////////////////////////////////////////////////////
void TwainApp::initiateTransfer_File()
{
  cout << "app: Starting a TWSX_FILE transfer..." << endl;

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

    TW_UINT16 fileformat = TWFF_TIFF;
    if(!getICAP_IMAGEFILEFORMAT(fileformat))
    {
      // Default back to TIFF
      fileformat = TWFF_TIFF;
    }
    const char * pExt = convertICAP_IMAGEFILEFORMAT_toString(fileformat);

    SSNPRINTF(filexfer.FileName, sizeof(filexfer.FileName), sizeof(filexfer.FileName), "FROM_SCANNER_%06dF%s", m_nXferNum, pExt);

    filexfer.Format = fileformat;

    cout << "app: Sending file transfer details..." << endl;
    twrc = DSM_Entry( DG_CONTROL, DAT_SETUPFILEXFER, MSG_SET, (TW_MEMREF)&(filexfer));

    if(TWRC_SUCCESS != twrc)
    {
      printError(m_pDataSource, "Error while trying to setup the file transfer");
      break;
    }

    cout << "app: Starting file transfer..." << endl;
    twrc = DSM_Entry( DG_IMAGE, DAT_IMAGEFILEXFER, MSG_GET, 0);

    if(TWRC_XFERDONE == twrc)
    {
      // Findout where the file was actualy saved
      twrc = DSM_Entry( DG_CONTROL, DAT_SETUPFILEXFER, MSG_GET, (TW_MEMREF)&(filexfer));

#ifdef _WINDOWS
        ShellExecute(m_Parent, "open", filexfer.FileName, NULL, NULL, SW_SHOWNORMAL);
#else
         cout << "app: File \"" << filexfer.FileName << "\" saved..." << endl;
#endif
      // see if there are any more transfers to do
      cout << "app: Checking to see if there are more images to transfer..." << endl;
      TW_PENDINGXFERS pendxfers;
      memset(&pendxfers, 0, sizeof(pendxfers));

      twrc = DSM_Entry( DG_CONTROL, DAT_PENDINGXFERS, MSG_ENDXFER, (TW_MEMREF)&pendxfers);

      if(TWRC_SUCCESS == twrc)
      {
        cout << "app: Remaining images to transfer: " << pendxfers.Count << endl;

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
      cout << "app: Stop any transfer we may have started but could not finish..." << endl;
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

  cout << "app: DONE!" << endl;

  return;
}

//////////////////////////////////////////////////////////////////////////////
void TwainApp::initiateTransfer_Memory()
{
  cout << "app: Starting a TWSX_MEMORY transfer..." << endl;

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
    cout << "app: getting the buffer sizes..." << endl;
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
    cout << "app: starting the memory transfer..." << endl;
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

          TW_FIX32 res;
          getICAP_XRESOLUTION(res);
          pTifImg->setXResolution(res.Whole, 1);

          getICAP_YRESOLUTION(res);
          pTifImg->setYResolution(res.Whole, 1);

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

#ifdef _WINDOWS
          ShellExecute(m_Parent, "open", szOutFileName, NULL, NULL, SW_SHOWNORMAL);
#else
          cout << "app: File \"" << szOutFileName << "\" saved..." << endl;
#endif

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
    cout << "app: Checking to see if there are more images to transfer..." << endl;
    TW_PENDINGXFERS pendxfers;
    memset( &pendxfers, 0, sizeof(pendxfers) );

    twrc = DSM_Entry( DG_CONTROL, DAT_PENDINGXFERS, MSG_ENDXFER, (TW_MEMREF)&pendxfers);

    if(TWRC_SUCCESS == twrc)
    {
      cout << "app: Remaining images to transfer: " << pendxfers.Count << endl;
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
      cout << "app: Stop any transfer we may have started but could not finish..." << endl;
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

  cout << "app: DONE!" << endl;

  return;
}

//////////////////////////////////////////////////////////////////////////////
FIBITMAP* TwainApp::createDIB()
{
  TW_UINT16 nbd;

  if(!getICAP_PIXELTYPE(nbd))
  {
    cerr << "Error: could not figure out bitdepth to be used." << endl;
    assert(0);
    return 0;
  }

  FIBITMAP *pDIB = FreeImage_Allocate(m_ImageInfo.ImageWidth,
    m_ImageInfo.ImageLength,
    24);

  if(0 == pDIB)
  {
    cout << "app: Error creating image structure in memory!" << endl;
    assert(0);
    return 0;
  }

  // Adjust dib to currently set capabilities. If the cap specs a 24 bit image,
  // there is no need to do anything since it was created as 24 bit to begin
  // with.
  if(nbd != TWPT_RGB)
  {
    FIBITMAP *pdib = 0;
    switch(nbd)
    {
    case TWPT_BW:
      pdib = FreeImage_Threshold(pDIB, 128);
      break;

    case TWPT_GRAY:
      pdib = FreeImage_ConvertTo8Bits(pDIB);
      break;
    }

    if(0 == pdib)
    {
      cerr << "Error: Could not setup in memory image structure" << endl;
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
bool TwainApp::get_CAP(TW_CAPABILITY& _cap)
{
  if(m_DSMState < 4)
  {
    cout << "You need to open a data source first." << endl;
    return false;
  }

  bool bret = false;

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
  case TWRC_SUCCESS:
    bret = true;
    break;

  case TWRC_FAILURE:
    string strErr = "Failed to get the capability: [";
    strErr += (char)(_cap.Cap);
    strErr += "]";
    
    printError(m_pDataSource, strErr);
    break;
  }

  return bret;
}

//////////////////////////////////////////////////////////////////////////////
bool TwainApp::set_CapabilityOneValue(TW_UINT16 Cap, const TW_INT16 _value)
{
  bool  bResult = true;
  TW_CAPABILITY cap;
  cap.Cap     = Cap;
  cap.ConType = TWON_ONEVALUE;

  cap.hContainer = (TW_HANDLE)_DSM_Alloc(sizeof(TW_ONEVALUE_INT16));
  if(0 == cap.hContainer)
  {
    printError(0, "Error allocating memory");
    return false;
  }
  pTW_ONEVALUE_INT16 pVal = (pTW_ONEVALUE_INT16)_DSM_LockMemory(cap.hContainer);

  pVal->ItemType = TWTY_INT16;
  pVal->Item = _value;

  // capability structure is set, make the call to the source now
  TW_INT16 twrc = DSM_Entry( DG_CONTROL, DAT_CAPABILITY, MSG_SET, (TW_MEMREF)&(cap));

  if(TWRC_FAILURE == twrc)
  {
    printError(m_pDataSource, "Could not set capability");
    bResult = false;
  }

  _DSM_UnlockMemory(cap.hContainer);
  _DSM_Free(cap.hContainer);
  return bResult;
}

bool TwainApp::set_CapabilityOneValue(TW_UINT16 Cap, const TW_UINT16 _value)
{
  bool  bResult = true;
  TW_CAPABILITY cap;
  cap.Cap     = Cap;
  cap.ConType = TWON_ONEVALUE;

  cap.hContainer = (TW_HANDLE)_DSM_Alloc(sizeof(TW_ONEVALUE_UINT16));
  if(0 == cap.hContainer)
  {
    printError(0, "Error allocating memory");
    return false;
  }
  pTW_ONEVALUE_UINT16 pVal = (pTW_ONEVALUE_UINT16)_DSM_LockMemory(cap.hContainer);

  pVal->ItemType = TWTY_UINT16;
  pVal->Item = _value;

  // capability structure is set, make the call to the source now
  TW_INT16 twrc = DSM_Entry( DG_CONTROL, DAT_CAPABILITY, MSG_SET, (TW_MEMREF)&(cap));

  if(TWRC_FAILURE == twrc)
  {
    printError(m_pDataSource, "Could not set capability");
    bResult = false;
  }

  _DSM_UnlockMemory(cap.hContainer);
  _DSM_Free(cap.hContainer);
  return bResult;
}


bool TwainApp::set_CapabilityOneValue(TW_UINT16 Cap, const pTW_FIX32 _pValue)
{
  bool  bResult = true;
  TW_CAPABILITY cap;
  cap.Cap = Cap;
  cap.ConType = TWON_ONEVALUE;

  cap.hContainer = (TW_HANDLE)_DSM_Alloc(sizeof(TW_ONEVALUE_FIX32));
  if(0 == cap.hContainer)
  {
    printError(0, "Error allocating memory");
    return false;
  }

  pTW_ONEVALUE_FIX32 pVal = (pTW_ONEVALUE_FIX32)_DSM_LockMemory(cap.hContainer);

  pVal->ItemType = TWTY_FIX32;
  pVal->Item = *_pValue;

  // capability structure is set, make the call to the source now
  TW_UINT16 twrc = DSM_Entry( DG_CONTROL, DAT_CAPABILITY, MSG_SET, (TW_MEMREF)&(cap));

  if(TWRC_FAILURE == twrc)
  {
    printError(m_pDataSource, "Could not set capability");
    bResult = false;
  }

  _DSM_UnlockMemory(cap.hContainer);
  _DSM_Free(cap.hContainer);

  return bResult;
}

//////////////////////////////////////////////////////////////////////////////
void TwainApp::set_CAP_XFERCOUNT(const TW_INT16 _count)
{
  set_CapabilityOneValue(CAP_XFERCOUNT, _count);

  // now that we have set it, re-get it to ensure it was set
  if(get_CAP(m_CAP_XFERCOUNT))
  {
    TW_INT16 count;
    if(getCAP_XFERCOUNT(count) &&
      count == _count)
    {
      cout << "Capability successfully set!" << endl;
    }
  }

  return;
}

//////////////////////////////////////////////////////////////////////////////
void TwainApp::set_ICAP_UNITS(const TW_UINT16 _val)
{
  set_CapabilityOneValue(ICAP_UNITS, _val);

  // now that we have set it, re-get it to ensure it was set
  if(get_CAP(m_ICAP_UNITS))
  {
    if(TWON_ENUMERATION == m_ICAP_UNITS.ConType &&
      0 != m_ICAP_UNITS.hContainer)
    {
      pTW_ENUMERATION pCapPT = (pTW_ENUMERATION)_DSM_LockMemory(m_ICAP_UNITS.hContainer);

      if(_val == pCapPT->ItemList[pCapPT->CurrentIndex])
      {
        cout << "Capability successfully set!" << endl;

        // successfully setting this cap means that we have to re-obtain the X/Y resolutions as well
        get_CAP(m_ICAP_XRESOLUTION);
        get_CAP(m_ICAP_YRESOLUTION);
      }
      _DSM_UnlockMemory(m_ICAP_UNITS.hContainer);
    }
  }

  return;
}

//////////////////////////////////////////////////////////////////////////////
void TwainApp::set_ICAP_PIXELTYPE(const TW_UINT16 _pt)
{
  set_CapabilityOneValue(ICAP_PIXELTYPE, _pt);

  // now that we have set it, re-get it to ensure it was set
  if(get_CAP(m_ICAP_PIXELTYPE))
  {
    if(TWON_ENUMERATION == m_ICAP_PIXELTYPE.ConType &&
      0 != m_ICAP_PIXELTYPE.hContainer)
    {
      pTW_ENUMERATION pCapPT = (pTW_ENUMERATION)_DSM_LockMemory(m_ICAP_PIXELTYPE.hContainer);

      if(_pt == ((TW_UINT16*)(&pCapPT->ItemList))[pCapPT->CurrentIndex])
      {
        cout << "Capability successfully set!" << endl;
      }
      _DSM_UnlockMemory(m_ICAP_PIXELTYPE.hContainer);
    }
  }

  // The ICAP_BITDEPTH is dependant on this cap so set it to match. This app
  // only supports 1 bit depth per color.
  switch(_pt)
  {
  case TWPT_BW:
    set_ICAP_BITDEPTH(1);
    break;

  case TWPT_GRAY:
    set_ICAP_BITDEPTH(8);
    break;

  case TWPT_RGB:
    set_ICAP_BITDEPTH(24);
    break;
  }

  get_CAP(m_ICAP_BITDEPTH);

  return;
}

//////////////////////////////////////////////////////////////////////////////
void TwainApp::set_ICAP_RESOLUTION(const TW_UINT16 _ICAP, const pTW_FIX32 _pVal)
{
  if((ICAP_XRESOLUTION != _ICAP) &&
     (ICAP_YRESOLUTION != _ICAP))
  {
    cerr << "Invalid resolution passed in! Resolution set failed." << endl;
    return;
  }

  set_CapabilityOneValue(_ICAP, _pVal);

  // Get the new RESOLUTION caps values to see if the set was successfull.
  get_CAP(m_ICAP_XRESOLUTION);
  get_CAP(m_ICAP_YRESOLUTION);

  pTW_CAPABILITY pCapRes = 0;

  if(ICAP_XRESOLUTION == _ICAP)
  {
    pCapRes = &m_ICAP_XRESOLUTION;
  }
  else
  {
    pCapRes = &m_ICAP_YRESOLUTION;
  }
  
  // check ICAP_XRESOLUTION
  if(TWON_ENUMERATION == pCapRes->ConType &&
    0 != pCapRes->hContainer)
  {
    pTW_ENUMERATION_FIX32 pdat = (pTW_ENUMERATION_FIX32)pCapRes->hContainer;

    if(TWTY_FIX32 == pdat->ItemType &&
      _pVal->Whole == pdat->ItemList[pdat->CurrentIndex].Whole &&
      _pVal->Frac == pdat->ItemList[pdat->CurrentIndex].Frac)
    {
      cout << "Resolution successfully set!" << endl;
    }
  }
  return;
}

//////////////////////////////////////////////////////////////////////////////
void TwainApp::set_ICAP_FRAMES(const pTW_FRAME _pFrame)
{
  TW_CAPABILITY cap;
  cap.Cap = ICAP_FRAMES;
  cap.ConType = TWON_ONEVALUE;

  cap.hContainer = (TW_HANDLE)_DSM_Alloc(sizeof(TW_ONEVALUE_FRAME));
  if(0 == cap.hContainer)
  {
    printError(0, "Error allocating memory");
    return;
  }

  pTW_ONEVALUE_FRAME pVal = (pTW_ONEVALUE_FRAME)_DSM_LockMemory(cap.hContainer);

  pVal->ItemType = TWTY_FRAME;
  pVal->Item = *_pFrame;

  // capability structure is set, make the call to the source now
  TW_UINT16 twrc = DSM_Entry( DG_CONTROL, DAT_CAPABILITY, MSG_SET, (TW_MEMREF)&(cap));

  if(TWRC_FAILURE == twrc)
  {
    printError(m_pDataSource, "Could not set capability");
  }

  _DSM_UnlockMemory(cap.hContainer);
  _DSM_Free(cap.hContainer);

  // now that we have set it, re-get it to ensure it was set
  if(get_CAP(m_ICAP_FRAMES))
  {
    if(TWON_ENUMERATION == m_ICAP_FRAMES.ConType &&
      0 != m_ICAP_FRAMES.hContainer)
    {
      pTW_ENUMERATION_FRAME pCapPT = (pTW_ENUMERATION_FRAME)_DSM_LockMemory(m_ICAP_FRAMES.hContainer);
      
      pTW_FRAME ptframe = &(((pTW_FRAME)(&pCapPT->ItemList))[pCapPT->CurrentIndex]);

      if( (_pFrame->Bottom == ptframe->Bottom) &&
          (_pFrame->Top == ptframe->Top) &&
          (_pFrame->Left == ptframe->Left) &&
          (_pFrame->Right == ptframe->Right))
      {
        cout << "Capability successfully set!" << endl;
      }
      _DSM_UnlockMemory(m_ICAP_FRAMES.hContainer);
    }
  }

  return;
}

//////////////////////////////////////////////////////////////////////////////
void TwainApp::set_ICAP_XFERMECH(const TW_UINT16 _mech)
{
  set_CapabilityOneValue(ICAP_XFERMECH, _mech);

  // now that we have set it, re-get it to ensure it was set
  if(get_CAP(m_ICAP_XFERMECH))
  {
    TW_UINT16 mech;
    if(getICAP_XFERMECH(mech) &&
      mech == _mech)
    {
      cout << "Capability successfully set!" << endl;
    }
  }

  // Update compression and FileFormat after xfer is set
  get_CAP(m_ICAP_COMPRESSION);
  get_CAP(m_ICAP_IMAGEFILEFORMAT);

  return;
}

//////////////////////////////////////////////////////////////////////////////
void TwainApp::set_ICAP_IMAGEFILEFORMAT(const TW_UINT16 _fileformat)
{
  set_CapabilityOneValue(ICAP_IMAGEFILEFORMAT, _fileformat);

  // now that we have set it, re-get it to ensure it was set
  if(get_CAP(m_ICAP_IMAGEFILEFORMAT))
  {
    TW_UINT16 fileformat;
    if(getICAP_IMAGEFILEFORMAT(fileformat) &&
      fileformat == _fileformat)
    {
      cout << "Capability successfully set!" << endl;
    }
  }

  // Update compression after xfer is set
  get_CAP(m_ICAP_COMPRESSION);

  return;
}

//////////////////////////////////////////////////////////////////////////////
void TwainApp::set_ICAP_COMPRESSION(const TW_UINT16 _comp)
{
  set_CapabilityOneValue(ICAP_COMPRESSION, _comp);

  // now that we have set it, re-get it to ensure it was set
  if(get_CAP(m_ICAP_COMPRESSION))
  {
    TW_UINT16 comp;
    if(getICAP_COMPRESSION(comp) &&
      comp == _comp)
    {
      cout << "Capability successfully set!" << endl;
    }
  }

  return;
}

//////////////////////////////////////////////////////////////////////////////
void TwainApp::set_ICAP_BITDEPTH(const TW_UINT16 _nVal)
{
  TW_CAPABILITY cap;
  cap.Cap = ICAP_BITDEPTH;
  cap.ConType = TWON_ENUMERATION;

  cap.hContainer = (TW_HANDLE)_DSM_Alloc(sizeof(TW_ENUMERATION) + (sizeof(TW_UINT16)));
  if(0 == cap.hContainer)
  {
    printError(0, "Error allocating memory");
    return;
  }

  pTW_ENUMERATION pVal = (pTW_ENUMERATION)_DSM_LockMemory(cap.hContainer);

  pVal->ItemType = TWTY_UINT16;
  pVal->NumItems = 1;
  pVal->CurrentIndex = 0;
  pVal->DefaultIndex = 0;
  ((TW_UINT16*)(&pVal->ItemList))[0] = _nVal;

  // capability structure is set, make the call to the source now
  TW_UINT16 twrc = DSM_Entry( DG_CONTROL, DAT_CAPABILITY, MSG_SET, (TW_MEMREF)&(cap));

  if(TWRC_FAILURE == twrc)
  {
    printError(m_pDataSource, "Could not set capability");
  }

  _DSM_UnlockMemory(cap.hContainer);
  _DSM_Free(cap.hContainer);

  return;
}

//////////////////////////////////////////////////////////////////////////////
bool TwainApp::getICAP_UNITS(TW_UINT16& _val)
{
  bool bret = false;

  if(0 != m_ICAP_UNITS.hContainer)
  {
    if(TWON_ENUMERATION == m_ICAP_UNITS.ConType)
    {
      pTW_ENUMERATION pCapPT = (pTW_ENUMERATION)_DSM_LockMemory(m_ICAP_UNITS.hContainer);

      if(TWTY_UINT16 == pCapPT->ItemType)
      {
        _val = ((TW_UINT16*)(&pCapPT->ItemList))[pCapPT->CurrentIndex];
        bret = true;
      }
      _DSM_UnlockMemory(m_ICAP_UNITS.hContainer);
    }
    else if(TWON_ONEVALUE == m_ICAP_UNITS.ConType)
    {
      pTW_ONEVALUE pCapPT = (pTW_ONEVALUE)_DSM_LockMemory(m_ICAP_UNITS.hContainer);
      
      if(TWTY_UINT16 == pCapPT->ItemType)
      {
        _val = (TW_UINT16)(pCapPT->Item);
        bret = true;
      }
      _DSM_UnlockMemory(m_ICAP_UNITS.hContainer);
    }
  }

  return bret;
}

//////////////////////////////////////////////////////////////////////////////
bool TwainApp::getCAP_XFERCOUNT(TW_INT16& _val)
{
  bool bret = false;

  if(TWON_ONEVALUE == m_CAP_XFERCOUNT.ConType &&
    0 != m_CAP_XFERCOUNT.hContainer)
  {
    pTW_ONEVALUE_INT16 pCap = (pTW_ONEVALUE_INT16)_DSM_LockMemory(m_CAP_XFERCOUNT.hContainer);

    if(TWTY_INT16 == pCap->ItemType)
    {
      _val = pCap->Item;
      bret = true;
    }
    _DSM_UnlockMemory(m_CAP_XFERCOUNT.hContainer);
  }

  return bret;
}

//////////////////////////////////////////////////////////////////////////////
bool TwainApp::getICAP_XFERMECH(TW_UINT16& _val)
{
  bool bret = false;

  if(0 != m_ICAP_XFERMECH.hContainer)
  {
    if(TWON_ENUMERATION == m_ICAP_XFERMECH.ConType)
    {
      pTW_ENUMERATION pCapPT = (pTW_ENUMERATION)_DSM_LockMemory(m_ICAP_XFERMECH.hContainer);

      if(TWTY_UINT16 == pCapPT->ItemType)
      {
        _val = ((TW_UINT16*)(&pCapPT->ItemList))[pCapPT->CurrentIndex];
        bret = true;
      }
      _DSM_UnlockMemory(m_ICAP_XFERMECH.hContainer);
    }
    else if(TWON_ONEVALUE == m_ICAP_XFERMECH.ConType)
    {
      pTW_ONEVALUE pCapPT = (pTW_ONEVALUE)_DSM_LockMemory(m_ICAP_XFERMECH.hContainer);
      
      if(TWTY_UINT16 == pCapPT->ItemType)
      {
        _val = (TW_UINT16)(pCapPT->Item);
        bret = true;
      }
      _DSM_UnlockMemory(m_ICAP_XFERMECH.hContainer);
    }
  }

  return bret;
}

//////////////////////////////////////////////////////////////////////////////
bool TwainApp::getICAP_PIXELTYPE(TW_UINT16& _val)
{
  bool bret = false;

  if(0 != m_ICAP_PIXELTYPE.hContainer)
  {
    if(TWON_ENUMERATION == m_ICAP_PIXELTYPE.ConType)
    {
      pTW_ENUMERATION pCapPT = (pTW_ENUMERATION)_DSM_LockMemory(m_ICAP_PIXELTYPE.hContainer);

      if(TWTY_UINT16 == pCapPT->ItemType)
      {
        _val = ((TW_UINT16*)(&pCapPT->ItemList))[pCapPT->CurrentIndex];
        bret = true;
      }
      _DSM_UnlockMemory(m_ICAP_PIXELTYPE.hContainer);
    }
    else if(TWON_ONEVALUE == m_ICAP_PIXELTYPE.ConType)
    {
      pTW_ONEVALUE pCapPT = (pTW_ONEVALUE)_DSM_LockMemory(m_ICAP_PIXELTYPE.hContainer);
      
      if(TWTY_UINT16 == pCapPT->ItemType)
      {
        _val = (TW_UINT16)(pCapPT->Item);
        bret = true;
      }
      _DSM_UnlockMemory(m_ICAP_PIXELTYPE.hContainer);
    }
  }

  return bret;
}

//////////////////////////////////////////////////////////////////////////////
bool TwainApp::getICAP_BITDEPTH(TW_UINT16& _val)
{
  bool bret = false;

  if(0 != m_ICAP_BITDEPTH.hContainer)
  {
    if(TWON_ENUMERATION == m_ICAP_BITDEPTH.ConType)
    {
      pTW_ENUMERATION pCapPT = (pTW_ENUMERATION)_DSM_LockMemory(m_ICAP_BITDEPTH.hContainer);

      if(TWTY_UINT16 == pCapPT->ItemType)
      {
        _val = ((TW_UINT16*)(&pCapPT->ItemList))[pCapPT->CurrentIndex];
        bret = true;
      }
      _DSM_UnlockMemory(m_ICAP_BITDEPTH.hContainer);
    }
    else if(TWON_ONEVALUE == m_ICAP_BITDEPTH.ConType)
    {
      pTW_ONEVALUE pCapPT = (pTW_ONEVALUE)_DSM_LockMemory(m_ICAP_BITDEPTH.hContainer);
      
      if(TWTY_UINT16 == pCapPT->ItemType)
      {
        _val = (TW_UINT16)(pCapPT->Item);
        bret = true;
      }
      _DSM_UnlockMemory(m_ICAP_BITDEPTH.hContainer);
    }
  }

  return bret;
}

bool TwainApp::getICAP_IMAGEFILEFORMAT(TW_UINT16& _val)
{
  bool bret = false;

  if(0 != m_ICAP_IMAGEFILEFORMAT.hContainer)
  {
    if(TWON_ENUMERATION == m_ICAP_IMAGEFILEFORMAT.ConType)
    {
      pTW_ENUMERATION pCapPT = (pTW_ENUMERATION)_DSM_LockMemory(m_ICAP_IMAGEFILEFORMAT.hContainer);

      if(TWTY_UINT16 == pCapPT->ItemType)
      {
        _val = ((TW_UINT16*)(&pCapPT->ItemList))[pCapPT->CurrentIndex];
        bret = true;
      }
      _DSM_UnlockMemory(m_ICAP_IMAGEFILEFORMAT.hContainer);
    }
    else if(TWON_ONEVALUE == m_ICAP_IMAGEFILEFORMAT.ConType)
    {
      pTW_ONEVALUE pCapPT = (pTW_ONEVALUE)_DSM_LockMemory(m_ICAP_IMAGEFILEFORMAT.hContainer);
      
      if(TWTY_UINT16 == pCapPT->ItemType)
      {
        _val = (TW_UINT16)(pCapPT->Item);
        bret = true;
      }
      _DSM_UnlockMemory(m_ICAP_IMAGEFILEFORMAT.hContainer);
    }
  }

  return bret;
}

bool TwainApp::getICAP_COMPRESSION(TW_UINT16& _val)
{
  bool bret = false;

  if(0 != m_ICAP_COMPRESSION.hContainer)
  {
    if(TWON_ENUMERATION == m_ICAP_COMPRESSION.ConType)
    {
      pTW_ENUMERATION pCapPT = (pTW_ENUMERATION)_DSM_LockMemory(m_ICAP_COMPRESSION.hContainer);

      if(TWTY_UINT16 == pCapPT->ItemType)
      {
        _val = ((TW_UINT16*)(&pCapPT->ItemList))[pCapPT->CurrentIndex];
        bret = true;
      }
      _DSM_UnlockMemory(m_ICAP_COMPRESSION.hContainer);
    }
    else if(TWON_ONEVALUE == m_ICAP_COMPRESSION.ConType)
    {
      pTW_ONEVALUE pCapPT = (pTW_ONEVALUE)_DSM_LockMemory(m_ICAP_COMPRESSION.hContainer);
      
      if(TWTY_UINT16 == pCapPT->ItemType)
      {
        _val = (TW_UINT16)(pCapPT->Item);
        bret = true;
      }
      _DSM_UnlockMemory(m_ICAP_COMPRESSION.hContainer);
    }
  }

  return bret;
}

//////////////////////////////////////////////////////////////////////////////
bool TwainApp::getICAP_XRESOLUTION(TW_FIX32& _xres)
{
  bool bret = false;

  if(0 != m_ICAP_XRESOLUTION.hContainer)
  {
    if(TWON_ENUMERATION == m_ICAP_XRESOLUTION.ConType)
    {
      pTW_ENUMERATION_FIX32 pCapPT = (pTW_ENUMERATION_FIX32)_DSM_LockMemory(m_ICAP_XRESOLUTION.hContainer);

      if(TWTY_FIX32 == pCapPT->ItemType)
      {
        _xres = pCapPT->ItemList[pCapPT->CurrentIndex];
        bret = true;
      }
      _DSM_UnlockMemory(m_ICAP_XRESOLUTION.hContainer);
    }
    else if(TWON_ONEVALUE == m_ICAP_XRESOLUTION.ConType)
    {
      pTW_ONEVALUE_FIX32 pCapPT = (pTW_ONEVALUE_FIX32)_DSM_LockMemory(m_ICAP_XRESOLUTION.hContainer);
      
      if(TWTY_FIX32 == pCapPT->ItemType)
      {
        _xres = pCapPT->Item;
        bret = true;
      }
      _DSM_UnlockMemory(m_ICAP_XRESOLUTION.hContainer);
    }
  }
  
  return bret;
}

//////////////////////////////////////////////////////////////////////////////
bool TwainApp::getICAP_YRESOLUTION(TW_FIX32& _yres)
{
  bool bret = false;

  if(0 != m_ICAP_YRESOLUTION.hContainer)
  {
    if(TWON_ENUMERATION == m_ICAP_YRESOLUTION.ConType)
    {
      pTW_ENUMERATION_FIX32 pCapPT = (pTW_ENUMERATION_FIX32)_DSM_LockMemory(m_ICAP_YRESOLUTION.hContainer);

      if(TWTY_FIX32 == pCapPT->ItemType)
      {
        _yres = pCapPT->ItemList[pCapPT->CurrentIndex];
        bret = true;
      }
      _DSM_UnlockMemory(m_ICAP_YRESOLUTION.hContainer);
    }
    else if(TWON_ONEVALUE == m_ICAP_YRESOLUTION.ConType)
    {
      pTW_ONEVALUE_FIX32 pCapPT = (pTW_ONEVALUE_FIX32)_DSM_LockMemory(m_ICAP_YRESOLUTION.hContainer);
      
      if(TWTY_FIX32 == pCapPT->ItemType)
      {
        _yres = pCapPT->Item;
        bret = true;
      }
      _DSM_UnlockMemory(m_ICAP_YRESOLUTION.hContainer);
    }
  }
  
  return bret;
}


//////////////////////////////////////////////////////////////////////////////

