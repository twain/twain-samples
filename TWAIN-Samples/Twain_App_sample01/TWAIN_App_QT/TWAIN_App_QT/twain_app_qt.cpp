#include "stdafx.h"
#include "twain_app_qt.h"
#include "..\..\src\twainapp.h"
#include "ContainerWrapper.h"
#include "twainstring.h"
#include "MeterDlg.h"
#include "EnumSet.h"
#include "SetState.h"
#include "StructureEdit.h"
#include "ExtImageInfoSel.h"

/**
* Event handler for relaying events to the TWAIN DS (required for TWAIN versions below 2.2
* @param[in] pMsg pointer to the OS specific message structure
* @return true if the TWAIN DS handles the event
*/
static bool eventTWFilter(void *pMsg)
{
  bool bRC = false;
  CQTWAINApp *pApp = dynamic_cast<CQTWAINApp*>(QApplication::instance());
  //First make sure we have the correct type of instance, there is a window and in the right TWAIN state
  if(pApp && pApp->m_pMainWnd && (5<=pApp->m_pMainWnd->GetCurrentState()))
  {
    //Initialize the event structure
    TW_EVENT twEvent = { pMsg, MSG_NULL };
    //Dispatch the even to the Data Source
    if(TWRC_DSEVENT==pApp->m_pMainWnd->ProcessEvent(twEvent))
    {
      //The event was handled by the DataSource
      bRC = true;
    }
  }
  //The event was not handled by the DataSource
  return bRC;
}

CQTWAINApp::CQTWAINApp(int &argc, char **argv, int nVer /*= QT_VERSION*/)
  : QApplication(argc, argv, nVer)
  , m_pMainWnd(NULL)
{
  //Configure the Event filter for TWAIN DAT_EVENT handling
  QAbstractEventDispatcher::instance()->setEventFilter(eventTWFilter);
  return;
}

CQTWAINApp::~CQTWAINApp()
{
  return;
}

TWAIN_App_QT::TWAIN_App_QT(QScriptEngine *pEngine, QWidget *parent, Qt::WFlags flags)
  : QMainWindow(parent, flags)
  , m_pEngine(pEngine)
  , m_bSignalCalledInTWAIN(false)
  , m_nImageCount(0)
  , m_pMemoryFile(NULL)
  , m_pMemoryDataStream(NULL)
{
  ui.setupUi(this);

  //Fixup the Triplet list
  ui.treTriplet->header()->setResizeMode(0, QHeaderView::ResizeToContents);
  ui.treTriplet->header()->setResizeMode(1, QHeaderView::ResizeToContents);
  //Fixup the Capability list
  ui.treCapability->header()->setResizeMode(0, QHeaderView::ResizeToContents);
  ui.treCapability->header()->setResizeMode(1, QHeaderView::ResizeToContents);
  ui.treCapability->header()->setResizeMode(2, QHeaderView::ResizeToContents);
  ui.treCapability->header()->setResizeMode(3, QHeaderView::ResizeToContents);
  ui.treCapability->header()->setResizeMode(4, QHeaderView::ResizeToContents);
  ui.treCapability->header()->setResizeMode(5, QHeaderView::ResizeToContents);
  ui.treCapability->header()->setResizeMode(6, QHeaderView::ResizeToContents);
  ui.treCapability->header()->setResizeMode(7, QHeaderView::ResizeToContents);
  ui.treCapability->header()->setResizeMode(8, QHeaderView::ResizeToContents);

  //clear the current source identity
  memset(&m_twSourceIdentity, 0, sizeof(m_twSourceIdentity));

  // Set up our Unique Application Identity
  m_twAppIdentity.Version.MajorNum = 2;
  m_twAppIdentity.Version.MinorNum = 1;
  m_twAppIdentity.Version.Language = TWLG_ENGLISH_CANADIAN;
  m_twAppIdentity.Version.Country = TWCY_CANADA;
  SSTRCPY(m_twAppIdentity.Version.Info, sizeof(m_twAppIdentity.Version.Info), "2.1.2");
  m_twAppIdentity.ProtocolMajor = 2;
  m_twAppIdentity.ProtocolMinor = 1;
  m_twAppIdentity.SupportedGroups = DF_APP2 | DG_IMAGE | DG_CONTROL;
  SSTRCPY(m_twAppIdentity.Manufacturer, sizeof(m_twAppIdentity.Manufacturer), "TWAIN Working Group");
  SSTRCPY(m_twAppIdentity.ProductFamily, sizeof(m_twAppIdentity.ProductFamily), "Sample Applications");
  SSTRCPY(m_twAppIdentity.ProductName, sizeof(m_twAppIdentity.ProductName), "QT Sample App");

  //Populate the triplet list, this never changes and only needs to be done once
  PopulateTripletList();
  //Populate the capability list with all known capabilities, only needs to be done once
  PopulateCapabilityList();
  //Restore the geometry of the Window
  QSettings settings(QSettings::NativeFormat, QSettings::UserScope, m_twAppIdentity.Manufacturer, m_twAppIdentity.ProductName, this);
  int nVersion = settings.value("Version", -1).toInt();
  restoreState(settings.value("State").toByteArray(), nVersion);
  restoreGeometry(settings.value("Main.Geometry").toByteArray());
  ui.splConfigAcquire->restoreState(settings.value("CfgAcq.State").toByteArray());
  ui.splTripletCapability->restoreState(settings.value("TripCap.State").toByteArray());
  //Restore the state of the controls
  ui.chkExtImageInfo->setCheckState(settings.value("ExtImageInfo.State", false).toBool()?Qt::Checked:Qt::Unchecked);
  ui.chkSave->setCheckState(settings.value("Save.State", false).toBool()?Qt::Checked:Qt::Unchecked);
  ui.chkAutoDisable->setCheckState(settings.value("CloseDSReq.State", false).toBool()?Qt::Checked:Qt::Unchecked);
  ui.radAuto->setChecked(settings.value("Auto.State", true).toBool());
  ui.radManual->setChecked(settings.value("Manual.State", false).toBool());
  ui.radShowUI->setChecked(settings.value("ShowUI.State", true).toBool());
  ui.radNoUI->setChecked(settings.value("NoUI.State", false).toBool());
  ui.radUIOnly->setChecked(settings.value("UIOnly.State", false).toBool());
  //Restore destination file location
  ui.edtSavePath->setText(settings.value("SavePath.State", "").toString());
  //Restore extended image info values
  QList<QVariant> localList = settings.value("ExtInfos.State").toList();
  if(localList.count())
  {
    for(QList<QVariant>::iterator iter = localList.begin(); iter != localList.end(); iter++)
    {
      m_lstExtImageInfo.push_back(static_cast<TW_UINT16>(iter->toUInt()));
    }
  }

  //Clear members used for operations
  memset(&m_twFileSystem, 0, sizeof(m_twFileSystem));
  memset(&m_twCustomDSData, 0, sizeof(m_twCustomDSData));
  memset(&m_twSetupMemXfer, 0, sizeof(m_twSetupMemXfer));
  memset(&m_twSetupFileXfer, 0, sizeof(m_twSetupFileXfer));
  memset(&m_twPendingXfers, 0, sizeof(m_twPendingXfers));
  memset(&m_twStatus, 0, sizeof(m_twStatus));
  memset(&m_twDeviceEvent, 0, sizeof(m_twDeviceEvent));
  memset(&m_twImageLayout, 0, sizeof(m_twImageLayout));

  //Configure the GUI for startup
  EnableGUIByCurrentState();
  //Display the items in the edit box
  FillExtImageInfoEdit();

  //Make this always a queued connection to avoid re-entrancy issues
  connect( this, SIGNAL( customDataSourceSignal() ), this, SLOT( DataSourceSignal() ), Qt::QueuedConnection);
  //Make this an auto-connection so that status is updated immediately whenever possible
  connect( this, SIGNAL( customAppendStatusText(QString&) ), this, SLOT( AppendStatusText(QString&) ), Qt::AutoConnection);

  if(pEngine)
  {
    //add all the objects that are to be exposed for scripting
    pEngine->globalObject().setProperty("btnLoadDSM", pEngine->newQObject(ui.btnLoadDSM));
    pEngine->globalObject().setProperty("btnUnloadDSM", pEngine->newQObject(ui.btnUnloadDSM));

    pEngine->globalObject().setProperty("btnOpenDSM", pEngine->newQObject(ui.btnOpenDSM));
    pEngine->globalObject().setProperty("btnCloseDSM", pEngine->newQObject(ui.btnCloseDSM));

    pEngine->globalObject().setProperty("btnSetDefault", pEngine->newQObject(ui.btnSetDefault));
    pEngine->globalObject().setProperty("btnOpen", pEngine->newQObject(ui.btnOpen));
    pEngine->globalObject().setProperty("btnClose", pEngine->newQObject(ui.btnClose));

    pEngine->globalObject().setProperty("btnEnable", pEngine->newQObject(ui.btnEnable));
    pEngine->globalObject().setProperty("btnDisable", pEngine->newQObject(ui.btnDisable));

    pEngine->globalObject().setProperty("radNoUI", pEngine->newQObject(ui.radNoUI));
    pEngine->globalObject().setProperty("radShowUI", pEngine->newQObject(ui.radShowUI));
    pEngine->globalObject().setProperty("radUIOnly", pEngine->newQObject(ui.radUIOnly));

    pEngine->globalObject().setProperty("treCapability", pEngine->newQObject(ui.treCapability));
    pEngine->globalObject().setProperty("treTriplet", pEngine->newQObject(ui.treTriplet));

    pEngine->globalObject().setProperty("chkSave", pEngine->newQObject(ui.chkSave));
    pEngine->globalObject().setProperty("edtSavePath", pEngine->newQObject(ui.edtSavePath));
    pEngine->globalObject().setProperty("chkExtImageInfo", pEngine->newQObject(ui.chkExtImageInfo));
    pEngine->globalObject().setProperty("chkSave", pEngine->newQObject(ui.chkSave));

    pEngine->globalObject().setProperty("chkAutoDisable", pEngine->newQObject(ui.chkAutoDisable));
    pEngine->globalObject().setProperty("radAuto", pEngine->newQObject(ui.radAuto));
    pEngine->globalObject().setProperty("radManual", pEngine->newQObject(ui.radManual));

    pEngine->globalObject().setProperty("btnTransfer", pEngine->newQObject(ui.btnTransfer));
    pEngine->globalObject().setProperty("btnAbort", pEngine->newQObject(ui.btnAbort));
    pEngine->globalObject().setProperty("edtStatus", pEngine->newQObject(ui.edtStatus));
    pEngine->globalObject().setProperty("wndMain", pEngine->newQObject(this));
  }
  return;
}

TWAIN_App_QT::~TWAIN_App_QT()
{
  return;
}

void TWAIN_App_QT::EnableGUIByCurrentState()
{
  //the following are always configurable
  ui.chkSave->setEnabled( true );
  ui.edtSavePath->setEnabled( (Qt::Checked==ui.chkSave->checkState())?true:false );
  ui.btnSelPath->setEnabled( (Qt::Checked==ui.chkSave->checkState())?true:false );
  ui.chkExtImageInfo->setEnabled( true );
  ui.btnSet->setEnabled( (Qt::Checked==ui.chkExtImageInfo->checkState())?true:false );
  ui.edtExtImageInfo->setEnabled( (Qt::Checked==ui.chkExtImageInfo->checkState())?true:false );

  //must be in state 1 for these operations 
  ui.btnLoadDSM->setEnabled( (1==m_uiState)?true:false );

  //must be in state 2 for these operations 
  ui.btnUnloadDSM->setEnabled( (2==m_uiState)?true:false );
  ui.btnOpenDSM->setEnabled( (2==m_uiState)?true:false );
  ui.btnRecover->setEnabled( (2<=m_uiState)?true:false );

  //must be in state 3 for these operations 
  ui.btnCloseDSM->setEnabled( (3==m_uiState)?true:false );
  ui.btnOpen->setEnabled( (3==m_uiState)?true:false );
  ui.cbxSources->setEnabled( (3==m_uiState)?true:false );
  ui.btnSetDefault->setEnabled( (3<=m_uiState)?true:false );
  ui.chkCallback->setEnabled( (3==m_uiState)?true:false );

  //must be in or at least state 4 for these operations 
  ui.btnClose->setEnabled( (4==m_uiState)?true:false );
  ui.btnEnable->setEnabled( (4==m_uiState)?true:false );
  ui.radNoUI->setEnabled( (4==m_uiState)?true:false );
  ui.radShowUI->setEnabled( (4==m_uiState)?true:false );
  ui.radUIOnly->setEnabled( (4==m_uiState)?true:false );
  ui.treCapability->setEnabled( (4<=m_uiState)?true:false );
  ui.treTriplet->setEnabled( (4<=m_uiState)?true:false );
  ui.btnAllSupported->setEnabled( (4==m_uiState)?true:false );

  //must be in state 4 or 5 for this operation
  ui.chkAutoDisable->setEnabled( ((4==m_uiState)||(5==m_uiState))?true:false );
  ui.radAuto->setEnabled( ((4==m_uiState)||(5==m_uiState))?true:false );
  ui.radManual->setEnabled( ((4==m_uiState)||(5==m_uiState))?true:false );

  //must be in state 5 for this operation
  ui.btnDisable->setEnabled( (5==m_uiState)?true:false );

  //must be in state 6 for this operation
  ui.btnTransfer->setEnabled( (ui.radManual->isChecked() && (6==m_uiState))?true:false );
  //must be in at least state 6 for this operation
  ui.btnAbort->setEnabled( (6<=m_uiState)?true:false );
  //must be in state 7 for this operation
  ui.btnNext->setEnabled( (ui.radManual->isChecked() && (7==m_uiState))?true:false );
  return;
}

void OutputDebugStringFmt(LPCTSTR lpOutputString, ...)
{
  //setup the variable argument list
  va_list args;
  va_start(args, lpOutputString);

#ifdef _WIN32
  //count the characters in the resulting string
  int nCount = _vsctprintf(lpOutputString, args ) + 1;
  //allocate the fromatted string
  TCHAR *pszFormatted = new TCHAR[nCount];
  if(pszFormatted)
  {  
    //format the string
    _vstprintf_s(pszFormatted, nCount, lpOutputString, args);
    //output the string
    OutputDebugString(pszFormatted);
    //cleanup the string
    delete [] pszFormatted;
    pszFormatted = NULL;
  }
#endif //#ifdef _WIN32
  return;
}

void TWAIN_App_QT::PopulateDSList()
{
  //Clear the combobox
  ui.cbxSources->clear();
  TW_IDENTITY twDataSource = {0};
  //Populate with all the connected data sources
  if(TWRC_SUCCESS==DSM_Entry(DG_CONTROL, DAT_IDENTITY, MSG_GETFIRST, &twDataSource))
  {
    do
    {
      ui.cbxSources->addItem(twDataSource.ProductName);
    }while(TWRC_SUCCESS==DSM_Entry(DG_CONTROL, DAT_IDENTITY, MSG_GETNEXT, &twDataSource));
  }
  //Update with the current default data source
  if(TWRC_SUCCESS==DSM_Entry(DG_CONTROL, DAT_IDENTITY, MSG_GETDEFAULT, &twDataSource))
  {
    ui.cbxSources->setCurrentIndex( ui.cbxSources->findText(twDataSource.ProductName) );
  }
  return;
}

void TWAIN_App_QT::OnError(const TW_STATUS &twStatus)
{
  #pragma message("TODO: do something with the error")
  return;
}

void TWAIN_App_QT::OnNativeTransfer(const TW_MEMREF &pData, bool &bAbort, bool &bFree)
{
  if(ui.chkSave)
  {
    //convert to a handle
    TW_HANDLE hImage = *static_cast<TW_HANDLE*>(pData);
    if(hImage)
    {
      //lock the handle and extract the pointer
      PBITMAPINFOHEADER pDIB = static_cast<PBITMAPINFOHEADER>(_DSM_LockMemory(hImage));
      if(pDIB)
      {
        QString strDestName = QString("%1/Image%2.bmp").arg(ui.edtSavePath->text()).arg(m_nImageCount++);
        if(strDestName.length())
        {
          if(QFile::exists(strDestName))
          {
            //remove the original file
            QFile::remove(strDestName);
          }
          //attempt to re-create the file
          QFile img(strDestName);
          //Open the new file for writing
          if(img.open(QIODevice::ReadWrite))
          {
            //Calculate the offset
            int nOffSet  = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + (sizeof(RGBQUAD)*pDIB->biClrUsed);
            //Calculate the size of the DIB
            int nDIBSize = sizeof(BITMAPINFOHEADER) + (sizeof(RGBQUAD)*pDIB->biClrUsed) + pDIB->biSizeImage;
            if(BI_RGB==pDIB->biCompression)
            {
              //Note: Calculate the size of the image manually when uncompressed - do not trust the biSizeImage member
              nDIBSize = sizeof(BITMAPINFOHEADER) + (sizeof(RGBQUAD)*pDIB->biClrUsed) + ((((pDIB->biWidth * pDIB->biBitCount + 31)/32) * 4) * pDIB->biHeight);
            }
            //Prepare a stream for writing
            QDataStream ds(&img);
            //Initialize the BitmapFileHeader structure
            BITMAPFILEHEADER bmpFIH;
            memset(&bmpFIH, 0, sizeof(BITMAPFILEHEADER));
            bmpFIH.bfType    = ( (WORD) ('M' << 8) | 'B');
            bmpFIH.bfOffBits = nOffSet;
            bmpFIH.bfSize    = sizeof(BITMAPFILEHEADER) + nDIBSize;
            //Write the BitmapFileHeader to disk
            ds.writeRawData(reinterpret_cast<char *>(&bmpFIH), sizeof(BITMAPFILEHEADER));
            //Write the Bitmap to disk
            ds.writeRawData(reinterpret_cast<char *>(pDIB), nDIBSize);
            //Close the file
            img.close();
          }
        }
        //unlock the handle
        _DSM_UnlockMemory(hImage);
      }
    }
  }
  return;
}

void TWAIN_App_QT::OnImageBegin(const TW_IMAGEINFO &twInfo, TW_UINT16 twXferMech)
{
  m_nPaletteOffset = 0;
  //be sure to clean up anything that was left over
  if(m_pMemoryDataStream)
  {
    delete m_pMemoryDataStream;
    m_pMemoryDataStream = NULL;
  }
  if(m_pMemoryFile)
  {
    delete m_pMemoryFile;
    m_pMemoryFile = NULL;
  }

  if((TWSX_MEMORY==twXferMech)||(TWSX_MEMFILE==twXferMech)&&ui.chkSave)
  {
    //Determine the file format we are going to use
    if(TWSX_MEMFILE!=twXferMech)
    {
      //This sample Application only supports Memory Transfer of BMP/JPG/TIF (Uncomp/Jpg/G4)
      switch(twInfo.Compression)
      {
        case TWCP_NONE:
          //uncompressed images will be stored as .bmp
          m_twMemoryFileFormat = TWFF_BMP;
          break;
        case TWCP_JPEG:
          //jpeg compressed images will be stored as a .jpg
          m_twMemoryFileFormat = TWFF_JFIF;
          break;
        case TWCP_GROUP4:
          //Group4 compressed images will be stored as .tif
          m_twMemoryFileFormat = TWFF_TIFF;
          break;
      }
    }
    else
    {
      //This is file in memory transfer more, file format is stored in the capability
      GetCapabilityOneValue(ICAP_IMAGEFILEFORMAT, m_twMemoryFileFormat);
    }

    //Construct the appropriate file name
    QString strDestName = QString("%1/Image%2.%3").arg(ui.edtSavePath->text()).arg(m_nImageCount++).arg(convertFileFmt_toExtension(m_twMemoryFileFormat));
    if(strDestName.length())
    {
      if(QFile::exists(strDestName))
      {
        //remove the original file
        QFile::remove(strDestName);
      }
      //Create the new file
      m_pMemoryFile = new QFile(strDestName);
      if(m_pMemoryFile && m_pMemoryFile->open(QIODevice::ReadWrite))
      {
        //Create a data stream to write to
        m_pMemoryDataStream = new QDataStream(m_pMemoryFile);
        switch(m_twMemoryFileFormat)
        {
          case TWFF_BMP:
            {
              memset(&m_bmfhBitmap, 0, sizeof(m_bmfhBitmap));
              memset(&m_bmihBitmap, 0, sizeof(m_bmihBitmap));

              //fill in the bitmap info structure
              m_bmihBitmap.biSize = sizeof(BITMAPINFOHEADER);
              m_bmihBitmap.biPlanes = 1;
              m_bmihBitmap.biBitCount = twInfo.BitsPerPixel;
              m_bmihBitmap.biXPelsPerMeter = FIX32ToFloat(twInfo.XResolution) * 39.37F + 0.5;
              m_bmihBitmap.biYPelsPerMeter = FIX32ToFloat(twInfo.YResolution) * 39.37F + 0.5;
              m_bmihBitmap.biCompression = BI_RGB;
              switch(twInfo.PixelType)
              {
                case TWPT_RGB:
                  //in the case of RGB there is no Palette in a DIB
                  m_bmihBitmap.biClrUsed = 0;
                  break;
                case TWPT_BW:
                case TWPT_GRAY:
                case TWPT_PALETTE:
                  //compute the palette size
                  m_bmihBitmap.biClrUsed = 1<<twInfo.BitsPerPixel;
                  m_nPaletteOffset = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
                  break;
              }
              //fill in the bitmap file header
              m_bmfhBitmap.bfType    = ( (WORD) ('M' << 8) | 'B');
              m_bmfhBitmap.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + (sizeof(RGBQUAD)*m_bmihBitmap.biClrUsed);
              if(false==m_bUsingUndefinedImageSize)
              {
                m_bmihBitmap.biWidth = twInfo.ImageWidth;
                m_bmihBitmap.biHeight = -twInfo.ImageLength;
                m_bmihBitmap.biSizeImage = (((m_bmihBitmap.biWidth * m_bmihBitmap.biBitCount + 31)/32) * 4) * m_bmihBitmap.biHeight;
                m_bmfhBitmap.bfSize    = sizeof(BITMAPFILEHEADER) + m_bmihBitmap.biSizeImage;
              }
              RGBQUAD *pbmiColors = NULL;
              //check if a palette is required
              if(m_bmihBitmap.biClrUsed)
              {
                //for now just dump an empty palette in here
                pbmiColors = new RGBQUAD[m_bmihBitmap.biClrUsed];
                memset(pbmiColors, 0, sizeof(RGBQUAD)*m_bmihBitmap.biClrUsed);
                if(TWPT_PALETTE==twInfo.PixelType)
                {
                  TW_PALETTE8 twPalette = {0};
                  //Get the Palette8 structure and transfer
                  if(TWRC_SUCCESS==DSM_Entry(DG_IMAGE, DAT_PALETTE8, MSG_GET, &twPalette, &m_twSourceIdentity))
                  {
                    if((TWPA_RGB==twPalette.PaletteType)||(TWPA_GRAY==twPalette.PaletteType))
                    {
                      //just in case the palette sizes are different
                      if(m_bmihBitmap.biClrUsed!=twPalette.NumColors)
                      {
                        //realistically this should never happen
                        m_bmihBitmap.biClrUsed = twPalette.NumColors;
                        delete [] pbmiColors;
                        pbmiColors = new RGBQUAD[m_bmihBitmap.biClrUsed];
                        memset(pbmiColors, 0, sizeof(RGBQUAD)*m_bmihBitmap.biClrUsed);
                      }
                      //copy the palette
                      for(int nIndex = 0; nIndex < twPalette.NumColors; nIndex++)
                      {
                        pbmiColors[nIndex].rgbRed = twPalette.Colors[nIndex].Channel1;
                        pbmiColors[nIndex].rgbGreen = twPalette.Colors[nIndex].Channel2;
                        pbmiColors[nIndex].rgbBlue = twPalette.Colors[nIndex].Channel3;
                      }
                    }
                  }
                }
                else
                {
                  //assume our palette is TWPF_CHOCOLATE (black is zero) - can be corrected at image end if necessary
							    int nIncr = 0xFF / ( m_bmihBitmap.biClrUsed - 1 );
							    int nVal = 0;
							    for (int nIndex=0; nIndex<m_bmihBitmap.biClrUsed; nIndex++)
							    {
                    //create the palette
								    nVal = nIndex * nIncr;
								    pbmiColors[nIndex].rgbRed = static_cast<BYTE>(nVal);
								    pbmiColors[nIndex].rgbGreen = static_cast<BYTE>(nVal);
								    pbmiColors[nIndex].rgbBlue = static_cast<BYTE>(nVal);
								    pbmiColors[nIndex].rgbReserved = 0;
							    }
                }
              }
              //all the colors used are important
              m_bmihBitmap.biClrImportant = m_bmihBitmap.biClrUsed;
              //dump the file header
              m_pMemoryDataStream->writeRawData(reinterpret_cast<char*>(&m_bmfhBitmap), sizeof(m_bmfhBitmap));
              //dump the bitmap info header
              m_pMemoryDataStream->writeRawData(reinterpret_cast<char*>(&m_bmihBitmap), sizeof(m_bmihBitmap));
              if(pbmiColors)
              {
                //dump the palette in the file
                m_pMemoryDataStream->writeRawData(reinterpret_cast<char*>(pbmiColors), sizeof(RGBQUAD)*m_bmihBitmap.biClrUsed);
                //delete the palette memory
                delete [] pbmiColors;
                pbmiColors = NULL;
              }
            }
            break;
          case TWFF_TIFF:
            //fill in the tiff file header and structure
            #pragma message("TODO: implement TIFF file creation")
          default:
            //do nothing
            break;
        }
      }
    }    
  }
  return;
}

void TWAIN_App_QT::OnImageEnd(const TW_IMAGEINFO &twInfo, TW_UINT16 twXferMech)
{
  pTW_EXTIMAGEINFO pExtInfo = NULL;
  if(m_bSupportsExtImageInfo && (Qt::Checked == ui.chkExtImageInfo->checkState()))
  {
    pExtInfo = CreateExtImageInfo(m_lstExtImageInfo);
    if(pExtInfo)
    {
      if(TWRC_SUCCESS==DSM_Entry(DG_IMAGE, DAT_EXTIMAGEINFO, MSG_GET, pExtInfo, &m_twSourceIdentity))
      {
        TraceStruct(*pExtInfo);
      }
    }
  }

  //Construct info
  if((TWSX_MEMORY==twXferMech)&&ui.chkSave)
  {
    if(m_bUsingUndefinedImageSize)
    {
      //go back and update some file information
      switch(m_twMemoryFileFormat)
      {
        case TWFF_BMP:
          m_bmihBitmap.biWidth = twInfo.ImageWidth;
          m_bmihBitmap.biHeight = -twInfo.ImageLength;
          m_bmihBitmap.biSizeImage = (((m_bmihBitmap.biWidth * m_bmihBitmap.biBitCount + 31)/32) * 4) * m_bmihBitmap.biHeight;
          m_bmfhBitmap.bfSize = sizeof(BITMAPFILEHEADER) + m_bmihBitmap.biSizeImage;
          //go back to the beginning of the file
          m_pMemoryFile->seek(0);
          //dump the file header
          m_pMemoryDataStream->writeRawData(reinterpret_cast<char*>(&m_bmfhBitmap), sizeof(m_bmfhBitmap));
          //dump the bitmap info header
          m_pMemoryDataStream->writeRawData(reinterpret_cast<char*>(&m_bmihBitmap), sizeof(m_bmihBitmap));
          if(m_bmihBitmap.biClrUsed && (TWPT_PALETTE!=twInfo.PixelType)) /*TWPT_GRAY or TWPT_BW*/
          {
            RGBQUAD *pbmiColors = new RGBQUAD[m_bmihBitmap.biClrUsed];
            //for now just dump an empty palette in here
            memset(pbmiColors, 0, sizeof(RGBQUAD)*m_bmihBitmap.biClrUsed);
            //Get the pixelflavor
            TW_UINT16 twPixelFlavor = TWPF_CHOCOLATE;
            pTW_INFO pInfo = FindInfo(pExtInfo, TWEI_PIXELFLAVOR);
            if(pInfo && (TWRC_SUCCESS==pInfo->ReturnCode))
            {
              //we got it from the ExtendedImageInfo
              twPixelFlavor = static_cast<TW_UINT16>(pInfo->Item);
            }
            else
            {
              //not found there get try the capability
              GetCapabilityOneValue(ICAP_PIXELFLAVOR, twPixelFlavor);
            }
            //if it is not chocolate then we have to rethink the palette
            if(TWPF_CHOCOLATE!=twPixelFlavor)
            {
              //assume our palette is TWPF_CHOCOLATE (black is zero) - can be corrected at image end if necessary
				      int nIncr = 0xFF / ( m_bmihBitmap.biClrUsed - 1 );
				      int nVal = 0;
				      for (int nIndex=0; nIndex<m_bmihBitmap.biClrUsed; nIndex++)
				      {
                //create the palette
                nVal = 0xFF - nIndex * nIncr; 
					      pbmiColors[nIndex].rgbRed = static_cast<BYTE>(nVal);
					      pbmiColors[nIndex].rgbGreen = static_cast<BYTE>(nVal);
					      pbmiColors[nIndex].rgbBlue = static_cast<BYTE>(nVal);
					      pbmiColors[nIndex].rgbReserved = 0;
				      }
              //dump the palette in the file
              m_pMemoryDataStream->writeRawData(reinterpret_cast<char*>(pbmiColors), sizeof(RGBQUAD)*m_bmihBitmap.biClrUsed);
            }
            //cleanup the variables
            delete [] pbmiColors;
            pbmiColors = NULL;
          }
          break;
        case TWFF_TIFF:
          #pragma message("TODO: implement TIFF undefined image size")
          break;
      }
    }
  }

  if(pExtInfo)
  {
    CleanupExtImageInfo(pExtInfo);
  }
  
  if(m_pMemoryDataStream)
  {
    //Delete the memory stream
    delete m_pMemoryDataStream;
    m_pMemoryDataStream = NULL;
  }
  if(m_pMemoryFile)
  {
    //Close the file
    m_pMemoryFile->close();
    //Delete the file
    delete m_pMemoryFile;
    m_pMemoryFile = NULL;
  }
  return;
}

void TWAIN_App_QT::OnMemoryTransfer(const TW_IMAGEINFO &twInfo, const TW_IMAGEMEMXFER &twData, const TW_UINT16 &twRC, bool &bAbort)
{
  if(ui.chkSave)
  {
    switch(m_twMemoryFileFormat)
    {
      case TWFF_TIFF:
        //only bother writing if there is data to write
        if((TWCP_GROUP4==twData.Compression)&&twData.BytesWritten)
        {
          #pragma message("TODO: implement TIFF file writing")
        }
        break;
      case TWFF_BMP:
        //only bother writing if there is data to write
        if((TWCP_NONE==twData.Compression)&&twData.BytesWritten)
        {
          //TWAIN Specfication says that rows are DWORD aligned so this 'should' be easy
          if(0==(twData.BytesPerRow%4))
          {
            //dump entire block to disk
            m_pMemoryDataStream->writeRawData(static_cast<char *>(twData.Memory.TheMem), twData.BytesWritten);
          }
          else
          {
            #pragma message("TODO: this code path not tested")
            //DataSource is not TWAIN compliant - align ourselves
            int nBytesPerRow = (((twData.Columns * twInfo.BitsPerPixel + 31)/32) * 4);
            int nDifference = min(0, nBytesPerRow-twData.BytesPerRow);
            char *pExtra = NULL;
            if(nDifference)
            {
              pExtra = new char[nDifference];
            }
            //get a pointer to the start of the data
            char *pData = static_cast<char *>(twData.Memory.TheMem);
            //iterate through each line
            for(int nRow = 0; nRow < twData.Rows; nRow++)
            {
              //dump out a line
              m_pMemoryDataStream->writeRawData(static_cast<char *>(twData.Memory.TheMem), min(twData.BytesPerRow, nBytesPerRow));
              if(nDifference)
              {
                //correct the alignment
                m_pMemoryDataStream->writeRawData(pExtra, nDifference);
              }
              //increment the data position by one line
              pData += twData.BytesPerRow;
            }
            
            if(pExtra)
            {
              delete [] pExtra;
              pExtra = NULL;
            }
          }
        }
        break;
      case TWFF_JFIF:
        //only bother writing if there is data to write
        if((TWCP_JPEG==twData.Compression)&&twData.BytesWritten)
        {
          //dump this block to disk
          m_pMemoryDataStream->writeRawData(static_cast<char *>(twData.Memory.TheMem), twData.BytesWritten);
        }
        break;
      default:
        //not supported - ignore
        break;
    }
  }
  //Flush the event queue
  QApplication::instance()->processEvents();
  return;
}

void TWAIN_App_QT::OnFileTransfer(const TW_SETUPFILEXFER &twFileXfer, bool &bAbort)
{
  if(ui.chkSave)
  {
    if(ui.edtSavePath->text().length())
    {
      QFile img(twFileXfer.FileName);
      if(img.exists())
      {
        QString strDestName = QString("%1/Image%2.%3").arg(ui.edtSavePath->text()).arg(m_nImageCount++).arg(convertFileFmt_toExtension(twFileXfer.Format));
        if(strDestName.length())
        {
          //does the intended destination file exist?
          if(QFile::exists(strDestName))
          {
            //make sure the existing file is gone / makes it possible to move
            QFile::remove(strDestName);
          }
          //rename is faster
          if(false==img.rename(strDestName))
          {
            //copy is required if rename is not possible
            img.copy(strDestName);
          }
        }
      }
    }
  }
  //Flush the event queue
  QApplication::instance()->processEvents();
  return;
}

void TWAIN_App_QT::OnFileMemTransfer(const TW_IMAGEMEMXFER &twData, const TW_UINT16 &twRC, bool &bAbort)
{
  if(ui.chkSave)
  {
    //only bother writing if there is data to write
    if(twData.BytesWritten)
    {
      //dump this block to disk
      m_pMemoryDataStream->writeRawData(static_cast<char *>(twData.Memory.TheMem), twData.BytesWritten);
    }
  }
  //Flush the event queue
  QApplication::instance()->processEvents();
  return;
}

void TWAIN_App_QT::TraceMessage(const char *pszFormatString, ...)
{
  //setup the variable argument list
  va_list args;
  va_start(args, pszFormatString);

  int nCount = _vscprintf(pszFormatString, args ) + 1;
  //allocate the fromatted string
  char *pszFormatted = new char[nCount];
  if(pszFormatted)
  {  
    //format the string
    vsprintf_s(pszFormatted, nCount, pszFormatString, args);
    //Output the result
    emit customAppendStatusText(QString::fromAscii(pszFormatted));
    //cleanup local pointers
    delete [] pszFormatted;
    pszFormatted = NULL;
  }
  return;
}

void TWAIN_App_QT::on_btnLoadDSM_clicked(bool bChecked)
{
  //Load the DataSource Manager
  LoadDSM();
  //Update the state of the GUI
  EnableGUIByCurrentState();
  return;
}

void TWAIN_App_QT::on_btnUnloadDSM_clicked(bool bChecked)
{
  //Unload the DataSource manager
  FreeDSM();
  //Update the state of the GUI
  EnableGUIByCurrentState();
  return;
}

void TWAIN_App_QT::on_btnOpenDSM_clicked(bool bChecked)
{
  //Open the DataSource Manager
  OpenDSM(winId());
  //Populate the DataSource list
  PopulateDSList();
  //Update the state of the GUI
  EnableGUIByCurrentState();
  return;
}

void TWAIN_App_QT::on_btnCloseDSM_clicked(bool bChecked)
{
  //Close the DataSource Manager
  CloseDSM();
  //Update the state of the GUI
  EnableGUIByCurrentState();
  return;
}

void TWAIN_App_QT::on_btnOpen_clicked(bool bChecked)
{
  //Display the wait cursor, this could take a while
  QApplication::setOverrideCursor(Qt::WaitCursor);
  //Open the current DataSource
  OpenDS( ui.cbxSources->currentText().toAscii() );
  //Restore the cursor
  QApplication::restoreOverrideCursor();
  //Populate or re-populate the Capability list
  RefreshCapabilityList();
  //Update the state of the GUI
  EnableGUIByCurrentState();
  //See if we are supposed to use callbacks
  if(Qt::Checked==ui.chkCallback->checkState())
  {
    //Automatically set the callback
    SetCallback();
  }
  return;
}

void TWAIN_App_QT::on_btnClose_clicked(bool bChecked)
{
  //Close the Data Source
  CloseDS();
  //Update the state of the GUI
  EnableGUIByCurrentState();
  return;
}

void TWAIN_App_QT::on_btnSet_clicked(bool bChecked)
{
  //Display a UI for selecting EXTIMAGEINFO values to read
  ExtImageInfoSel dlg(m_lstExtImageInfo, this);
  dlg.exec();
  FillExtImageInfoEdit();
  return;
}

void TWAIN_App_QT::on_btnEnable_clicked(bool bChecked)
{
  //Is this being open with the intent to transfer?
  if(ui.radUIOnly->isChecked())
  {
    //Enable for settings only
    EnableDSUIOnly(winId());
  }
  else
  {
    //Enable with intent to transfer UI or UIless
    EnableDS(winId(), ui.radNoUI->isChecked()?false:true);
  }
  //Update the state of the GUI
  EnableGUIByCurrentState();
  return;
}

void TWAIN_App_QT::on_btnDisable_clicked(bool bChecked)
{
  //Disable the DataSource
  DisableDS();
  //Update the state of the GUI
  EnableGUIByCurrentState();
  return;
}

void TWAIN_App_QT::on_btnRecover_clicked(bool bChecked)
{
  //Return to state 1
  SetState dlg(this, GetCurrentState());
  if(QDialog::Accepted==dlg.exec())
  {
    ReturnToState(dlg.m_nState);
  }
  //Update the state of the GUI
  EnableGUIByCurrentState();
  return;
}

void TWAIN_App_QT::on_chkExtImageInfo_clicked(bool bChecked)
{
  //Update the state of the GUI
  EnableGUIByCurrentState();
  return;
}

void TWAIN_App_QT::on_chkSave_clicked(bool bChecked)
{
  //Update the state of the GUI
  EnableGUIByCurrentState();
  return;
}

void TWAIN_App_QT::closeEvent(QCloseEvent *pEvent)
{
  ReturnToState(1);
//    QSettings register("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders", QSettings::NativeFormat);
//    QString allUserDesktop( register.value("Common Desktop").toString());
  QSettings settings(QSettings::NativeFormat, QSettings::UserScope, m_twAppIdentity.Manufacturer, m_twAppIdentity.ProductName, this);
  settings.setValue("Version", 1);
  settings.setValue("State", saveState(1));
  settings.setValue("Main.Geometry", saveGeometry());
  settings.setValue("CfgAcq.State", ui.splConfigAcquire->saveState());
  settings.setValue("TripCap.State", ui.splTripletCapability->saveState());
  settings.setValue("ExtImageInfo.State", (Qt::Checked==ui.chkExtImageInfo->checkState())?true:false);
  settings.setValue("Save.State", (Qt::Checked==ui.chkSave->checkState())?true:false);
  settings.setValue("CloseDSReq.State", (Qt::Checked==ui.chkAutoDisable->checkState())?true:false);
  settings.setValue("Auto.State", ui.radAuto->isChecked());
  settings.setValue("Manual.State", ui.radManual->isChecked());
  settings.setValue("ShowUI.State", ui.radShowUI->isChecked());
  settings.setValue("NoUI.State", ui.radNoUI->isChecked());
  settings.setValue("UIOnly.State", ui.radUIOnly->isChecked());
  //save the current file destination location
  settings.setValue("SavePath.State", ui.edtSavePath->text());
  //save the current extended image info properties
  QList<QVariant> localList;
  for(vector<TW_UINT16>::iterator iter = m_lstExtImageInfo.begin(); iter != m_lstExtImageInfo.end(); iter++)
  {
    localList.push_back(QVariant::fromValue(*iter));
  }
  if(localList.count())
  {
    settings.setValue("ExtInfos.State", localList);
  }
  QMainWindow::closeEvent(pEvent);
  return;
}

void TWAIN_App_QT::on_btnSetDefault_clicked(bool bChecked)
{
  TW_IDENTITY twIdentity = {0};
  //Get the entire identity from the DSM
  if(TWRC_SUCCESS==GetDataSourceID(ui.cbxSources->currentText().toAscii(), twIdentity))
  {
    //Instruct the DSM to make this the default
    DSM_Entry(DG_CONTROL, DAT_IDENTITY, MSG_SET, &twIdentity);
  }
  return;
}

void TWAIN_App_QT::OnSignalDSRequest()
{
  emit customDataSourceSignal();
  return;
}

void TWAIN_App_QT::DataSourceSignal()
{
  //it is absolutely critical that we never process signals while the DSM is on the stack
  if(!m_bInTWAIN)
  {
    m_bSignalCalledInTWAIN = false;
    //figure out what signal was requested
    switch(GetDSRequest())
    {
      case MSG_NULL:
        break;
      case MSG_XFERREADY:
        m_nImageCount = 0;
        if(ui.radAuto->isChecked())
        {
          //Automatically transfer all the images
          while((TWRC_SUCCESS==DoTransfer())&&(TWRC_SUCCESS==DoEndXfer())&&(6==m_uiState));
        }
        //Remove this signal from the queue
        ClearDSRequest();
        break;
      case MSG_CLOSEDSREQ:
        if(Qt::Checked==ui.chkAutoDisable->checkState())
        {
          DisableDS();
        }
        //Remove this signal from the queue
        ClearDSRequest();
        break;
      case MSG_CLOSEDSOK:
        DisableDS();
        //Remove this signal from the queue
        ClearDSRequest();
        break;
      case MSG_DEVICEEVENT:
        //Remove this signal from the queue
        ClearDSRequest();
        break;
    }
  }
  else
  {
    m_bSignalCalledInTWAIN = true;
  }
  //Update the state of the GUI
  EnableGUIByCurrentState();
  return;
}

void TWAIN_App_QT::AppendStatusText(QString &strText)
{
  ui.edtStatus->append(strText);
  return;
}

void TWAIN_App_QT::on_btnTransfer_clicked(bool bChecked)
{
  //Do a transfer
  DoTransfer();
  //Update the state of the GUI
  EnableGUIByCurrentState();
  return;
}

void TWAIN_App_QT::on_btnAbort_clicked(bool bChecked)
{
  //Return to state 5
  ReturnToState(5);
  //Update the state of the GUI
  EnableGUIByCurrentState();
  return;
}

void TWAIN_App_QT::on_radAuto_toggled(bool bChecked)
{
  //Update the state of the GUI
  EnableGUIByCurrentState();
  return;
}

void TWAIN_App_QT::on_radManual_toggled(bool bChecked)
{
  //Update the state of the GUI
  EnableGUIByCurrentState();
  return;
}

const TW_UINT16 kCONTROL_TABLE[][2] =
{
//  * DAT_CAPABILITY omitted / handled elsewhere
//  DAT_CAPABILITY, MSG_GET,
//  DAT_CAPABILITY, MSG_GETCURRENT,
//  DAT_CAPABILITY, MSG_GETDEFAULT,
//  DAT_CAPABILITY, MSG_RESET,
//  DAT_CAPABILITY, MSG_SET,
//  DAT_CAPABILITY, MSG_QUERYSUPPORT,
//  DAT_CAPABILITY, MSG_GETHELP,
//  DAT_CAPABILITY, MSG_GETLABEL,
//  DAT_CAPABILITY, MSG_GETLABELENUM,
//  * DAT_EVENT omitted / handled by app
//  DAT_EVENT, MSG_PROCESSEVENT,
//  * DAT_IDENTITY state transition operations omitted / handled by app
//  DAT_IDENTITY, MSG_CLOSEDS,
//  DAT_IDENTITY, MSG_GETDEFAULT,
//  DAT_IDENTITY, MSG_GETFIRST,
//  DAT_IDENTITY, MSG_GETNEXT,
//  DAT_IDENTITY, MSG_OPENDS,
//  DAT_IDENTITY, MSG_USERSELECT,
//  DAT_IDENTITY, MSG_SET,
//  * DAT_PARENT omitted / handled by app
//  DAT_PARENT, MSG_CLOSEDSM,
//  DAT_PARENT, MSG_OPENDSM,
//  * DAT_PENDINGXFERS state transition operations omitted / handled by app
//  DAT_PENDINGXFERS, MSG_ENDXFER,
//  DAT_PENDINGXFERS, MSG_RESET,
  DAT_PENDINGXFERS, MSG_GET,
  DAT_PENDINGXFERS, MSG_STOPFEEDER,
  DAT_SETUPMEMXFER, MSG_GET,
  DAT_SETUPFILEXFER, MSG_GET,
  DAT_SETUPFILEXFER, MSG_SET,
  DAT_STATUS, MSG_GET,
//  * DAT_USERINTERFACE state transition operations omitted / handled by app
//  DAT_USERINTERFACE, MSG_ENABLEDS,
//  DAT_USERINTERFACE, MSG_DISABLEDS,
  DAT_XFERGROUP, MSG_GET,
  DAT_CUSTOMDSDATA, MSG_GET,
  DAT_CUSTOMDSDATA, MSG_SET,
  DAT_DEVICEEVENT, MSG_GET,
  DAT_FILESYSTEM, MSG_CHANGEDIRECTORY,
  DAT_FILESYSTEM, MSG_GETFIRSTFILE,
  DAT_FILESYSTEM, MSG_GETNEXTFILE,
  DAT_FILESYSTEM, MSG_GETCLOSE,
  DAT_FILESYSTEM, MSG_GETINFO,
  DAT_FILESYSTEM, MSG_FORMATMEDIA,
  DAT_FILESYSTEM, MSG_CREATEDIRECTORY,
  DAT_FILESYSTEM, MSG_RENAME,
  DAT_FILESYSTEM, MSG_DELETE,
  DAT_FILESYSTEM, MSG_COPY,
  DAT_FILESYSTEM, MSG_AUTOMATICCAPTUREDIRECTORY,
  DAT_PASSTHRU, MSG_PASSTHRU,
//  * DAT_CALLBACK omitted / handled by app
//  DAT_CALLBACK, MSG_REGISTER_CALLBACK,
  DAT_STATUSUTF8, MSG_GET,
//  * DAT_ENTRYPOINT omitted / only issued by a Source
//  DAT_ENTRYPOINT, MSG_GET,
//  DAT_ENTRYPOINT, MSG_SET,
//  * DAT_NULL omitted / only issued by a Source
//  DAT_NULL, MSG_CLOSEDSOK,
//  DAT_NULL, MSG_CLOSEDSREQ,
//  DAT_NULL, MSG_DEVICEEVENT,
//  DAT_NULL, MSG_XFERREADY,
  TWON_DONTCARE16,
};

const TW_UINT16 kIMAGE_TABLE[][2] =
{
//  * DAT_IMAGEINFO omitted / handled by app
//  DAT_IMAGEINFO, MSG_GET,
  DAT_IMAGELAYOUT, MSG_GET,
  DAT_IMAGELAYOUT, MSG_GETDEFAULT,
  DAT_IMAGELAYOUT, MSG_RESET,
  DAT_IMAGELAYOUT, MSG_SET,
//  * DAT_IMAGEMEMXFER state transition operations omitted / handled by app
//  DAT_IMAGEMEMXFER, MSG_GET,
//  * DAT_IMAGENATIVEXFER state transition operations omitted / handled by app
//  DAT_IMAGENATIVEXFER, MSG_GET,
//  * DAT_IMAGEFILEXFER state transition operations omitted / handled by app
//  DAT_IMAGEFILEXFER, MSG_GET,
  DAT_CIECOLOR, MSG_GET,
  DAT_GRAYRESPONSE, MSG_RESET,
  DAT_GRAYRESPONSE, MSG_SET,
  DAT_RGBRESPONSE, MSG_RESET,
  DAT_RGBRESPONSE, MSG_SET,
  DAT_JPEGCOMPRESSION, MSG_GET,
  DAT_JPEGCOMPRESSION, MSG_GETDEFAULT,
  DAT_JPEGCOMPRESSION, MSG_RESET,
  DAT_JPEGCOMPRESSION, MSG_SET,
  DAT_PALETTE8, MSG_GET,
  DAT_PALETTE8, MSG_GETDEFAULT,
  DAT_PALETTE8, MSG_RESET,
  DAT_PALETTE8, MSG_SET,
  DAT_EXTIMAGEINFO, MSG_GET,
  DAT_ICCPROFILE, MSG_GET,
//  * DAT_IMAGEMEMFILEXFER state transition operations omitted / handled by app
//  DAT_IMAGEMEMFILEXFER, MSG_GET,
  TWON_DONTCARE16,
};

const TW_UINT16 kICAP_TABLE[] =
{
  ICAP_COMPRESSION,
  ICAP_PIXELTYPE,
  ICAP_UNITS,
  ICAP_XFERMECH,
  ICAP_AUTOBRIGHT,
  ICAP_BRIGHTNESS,
  ICAP_CONTRAST,
  ICAP_CUSTHALFTONE,
  ICAP_EXPOSURETIME,
  ICAP_FILTER,
  ICAP_FLASHUSED,
  ICAP_GAMMA,
  ICAP_HALFTONES,
  ICAP_HIGHLIGHT,
  ICAP_IMAGEFILEFORMAT,
  ICAP_LAMPSTATE,
  ICAP_LIGHTSOURCE,
  ICAP_ORIENTATION,
  ICAP_PHYSICALWIDTH,
  ICAP_PHYSICALHEIGHT,
  ICAP_SHADOW,
  ICAP_FRAMES,
  ICAP_XNATIVERESOLUTION,
  ICAP_YNATIVERESOLUTION,
  ICAP_XRESOLUTION,
  ICAP_YRESOLUTION,
  ICAP_MAXFRAMES,
  ICAP_TILES,
  ICAP_BITORDER,
  ICAP_CCITTKFACTOR,
  ICAP_LIGHTPATH,
  ICAP_PIXELFLAVOR,
  ICAP_PLANARCHUNKY,
  ICAP_ROTATION,
  ICAP_SUPPORTEDSIZES,
  ICAP_THRESHOLD,
  ICAP_XSCALING,
  ICAP_YSCALING,
  ICAP_BITORDERCODES,
  ICAP_PIXELFLAVORCODES,
  ICAP_JPEGPIXELTYPE,
  ICAP_TIMEFILL,
  ICAP_BITDEPTH,
  ICAP_BITDEPTHREDUCTION,
  ICAP_UNDEFINEDIMAGESIZE,
  ICAP_IMAGEDATASET,
  ICAP_EXTIMAGEINFO,
  ICAP_MINIMUMHEIGHT,
  ICAP_MINIMUMWIDTH,
  ICAP_AUTODISCARDBLANKPAGES,
  ICAP_FLIPROTATION,
  ICAP_BARCODEDETECTIONENABLED,
  ICAP_SUPPORTEDBARCODETYPES,
  ICAP_BARCODEMAXSEARCHPRIORITIES,
  ICAP_BARCODESEARCHPRIORITIES,
  ICAP_BARCODESEARCHMODE,
  ICAP_BARCODEMAXRETRIES,
  ICAP_BARCODETIMEOUT,
  ICAP_ZOOMFACTOR,
  ICAP_PATCHCODEDETECTIONENABLED,
  ICAP_SUPPORTEDPATCHCODETYPES,
  ICAP_PATCHCODEMAXSEARCHPRIORITIES,
  ICAP_PATCHCODESEARCHPRIORITIES,
  ICAP_PATCHCODESEARCHMODE,
  ICAP_PATCHCODEMAXRETRIES,
  ICAP_PATCHCODETIMEOUT,
  ICAP_FLASHUSED2,
  ICAP_IMAGEFILTER,
  ICAP_NOISEFILTER,
  ICAP_OVERSCAN,
  ICAP_AUTOMATICBORDERDETECTION,
  ICAP_AUTOMATICDESKEW,
  ICAP_AUTOMATICROTATE,
  ICAP_JPEGQUALITY,
  ICAP_FEEDERTYPE,
  ICAP_ICCPROFILE,
  ICAP_AUTOSIZE,
  ICAP_AUTOMATICCROPUSESFRAME,
  ICAP_AUTOMATICLENGTHDETECTION,
  ICAP_AUTOMATICCOLORENABLED,
  ICAP_AUTOMATICCOLORNONCOLORPIXELTYPE,
  ICAP_COLORMANAGEMENTENABLED,
  ICAP_IMAGEMERGE,
  ICAP_IMAGEMERGEHEIGHTTHRESHOLD,
  ICAP_SUPPORTEDEXTIMAGEINFO,
  TWON_DONTCARE16,
};

const TW_UINT16 kCAP_TABLE[] =
{
  CAP_XFERCOUNT,
  CAP_AUTHOR,
  CAP_CAPTION,
  CAP_FEEDERENABLED,
  CAP_FEEDERLOADED,
  CAP_TIMEDATE,
  CAP_SUPPORTEDCAPS,
  CAP_EXTENDEDCAPS,
  CAP_AUTOFEED,
  CAP_CLEARPAGE,
  CAP_FEEDPAGE,
  CAP_REWINDPAGE,
  CAP_INDICATORS,
  CAP_PAPERDETECTABLE,
  CAP_UICONTROLLABLE,
  CAP_DEVICEONLINE,
  CAP_AUTOSCAN,
  CAP_THUMBNAILSENABLED,
  CAP_DUPLEX,
  CAP_DUPLEXENABLED,
  CAP_ENABLEDSUIONLY,
  CAP_CUSTOMDSDATA,
  CAP_ENDORSER,
  CAP_JOBCONTROL,
  CAP_ALARMS,
  CAP_ALARMVOLUME,
  CAP_AUTOMATICCAPTURE,
  CAP_TIMEBEFOREFIRSTCAPTURE,
  CAP_TIMEBETWEENCAPTURES,
  CAP_CLEARBUFFERS,
  CAP_MAXBATCHBUFFERS,
  CAP_DEVICETIMEDATE,
  CAP_POWERSUPPLY,
  CAP_CAMERAPREVIEWUI,
  CAP_DEVICEEVENT,
  CAP_SERIALNUMBER,
  CAP_PRINTER,
  CAP_PRINTERENABLED,
  CAP_PRINTERINDEX,
  CAP_PRINTERMODE,
  CAP_PRINTERSTRING,
  CAP_PRINTERSUFFIX,
  CAP_LANGUAGE,
  CAP_FEEDERALIGNMENT,
  CAP_FEEDERORDER,
  CAP_REACQUIREALLOWED,
  CAP_BATTERYMINUTES,
  CAP_BATTERYPERCENTAGE,
  CAP_CAMERASIDE,
  CAP_SEGMENTED,
  CAP_CAMERAENABLED,
  CAP_CAMERAORDER,
  CAP_MICRENABLED,
  CAP_FEEDERPREP,
  CAP_FEEDERPOCKET,
  CAP_AUTOMATICSENSEMEDIUM,
  CAP_CUSTOMINTERFACEGUID,
  TWON_DONTCARE16,
};

void TWAIN_App_QT::PopulateTripletList()
{
  //Clear the tree
  ui.treTriplet->clear();

  //Create the top level DG_CONTROL item and store define for later use
  QTreeWidgetItem *pItem = new QTreeWidgetItem(ui.treTriplet);
  pItem->setText(0, QString::fromAscii(convertDataGroup_toString(DG_CONTROL)));
  pItem->setData(0, Qt::UserRole, DG_CONTROL);
  ui.treTriplet->addTopLevelItem(pItem);
  //Insert all DAT_* of DG_CONTROL type
  int nIndex = 0;
  while(TWON_DONTCARE16!=kCONTROL_TABLE[nIndex][0])
  {
    //Create the new subitem for the corresponding DAT_ and store the define for later use
    QTreeWidgetItem *pSubItem = new QTreeWidgetItem(pItem);
    pSubItem->setText(0, convertDataArgType_toString(kCONTROL_TABLE[nIndex][0]));
    pSubItem->setData(0, Qt::UserRole, kCONTROL_TABLE[nIndex][0]);
    pSubItem->setText(1, convertMessage_toString(kCONTROL_TABLE[nIndex][1]));
    pSubItem->setData(1, Qt::UserRole, kCONTROL_TABLE[nIndex][1]);
    nIndex++;
  }

  //Create the top level DG_IMAGE item and store define for later use
  pItem = new QTreeWidgetItem(ui.treTriplet);
  pItem->setText(0, QString::fromAscii(convertDataGroup_toString(DG_IMAGE)));
  pItem->setData(0, Qt::UserRole, DG_IMAGE);
  ui.treTriplet->addTopLevelItem(pItem);
  //Insert all DAT_* of DG_IMAGE type
  nIndex = 0;
  while(TWON_DONTCARE16!=kIMAGE_TABLE[nIndex][0])
  {
    //Create the new subitem for the corresponding DAT_ and store the define for later use
    QTreeWidgetItem *pSubItem = new QTreeWidgetItem(pItem);
    pSubItem->setText(0, convertDataArgType_toString(kIMAGE_TABLE[nIndex][0]));
    pSubItem->setData(0, Qt::UserRole, kIMAGE_TABLE[nIndex][0]);
    pSubItem->setText(1, convertMessage_toString(kIMAGE_TABLE[nIndex][1]));
    pSubItem->setData(1, Qt::UserRole, kIMAGE_TABLE[nIndex][1]);
    nIndex++;
  }
  return;
}

void TWAIN_App_QT::PopulateCapabilityList()
{
  //Clear the tree
  ui.treCapability->clear();

  //Create the top level CAP_ items and store define for later use
  QTreeWidgetItem *pItem = new QTreeWidgetItem(ui.treCapability);
  pItem->setText(0, "CAP");
  pItem->setData(0, Qt::UserRole, TWON_DONTCARE16);
  ui.treCapability->addTopLevelItem(pItem);
  //Insert all DAT_* of DG_CONTROL type
  int nIndex = 0;
  while(TWON_DONTCARE16!=kCAP_TABLE[nIndex])
  {
    //Create the new subitem for the corresponding DAT_ and store the define for later use
    QTreeWidgetItem *pSubItem = new QTreeWidgetItem(pItem);
    pSubItem->setText(0, convertCAP_toString(kCAP_TABLE[nIndex]));
    pSubItem->setData(0, Qt::UserRole, kCAP_TABLE[nIndex]);
    nIndex++;
  }

  //Create the top level ICAP_ items and store define for later use
  pItem = new QTreeWidgetItem(ui.treCapability);
  pItem->setText(0, "ICAP");
  pItem->setData(0, Qt::UserRole, TWON_DONTCARE16);
  ui.treCapability->addTopLevelItem(pItem);
  //Insert all DAT_* of DG_CONTROL type
  nIndex = 0;
  while(TWON_DONTCARE16!=kICAP_TABLE[nIndex])
  {
    //Create the new subitem for the corresponding DAT_ and store the define for later use
    QTreeWidgetItem *pSubItem = new QTreeWidgetItem(pItem);
    pSubItem->setText(0, convertCAP_toString(kICAP_TABLE[nIndex]));
    pSubItem->setData(0, Qt::UserRole, kICAP_TABLE[nIndex]);
    nIndex++;
  }
  return;
}

enum CapabilityColumns
{
  ColCapId=0,
  ColCurrent,
  ColDefault,
  ColCount,
  ColMin,
  ColMax,
  ColStep,
  ColType,
  ColSupport,
  ColLast,
};

void TWAIN_App_QT::RefreshCapabilityList(bool bUpdateAll/*=false*/)
{
  MeterDlg dlg;

  TW_HANDLE hSupportedCaps = NULL;
  TW_UINT16 twConType = TWON_DONTCARE16;
  if(TWRC_SUCCESS==GetCapabilitySupported(CAP_SUPPORTEDCAPS, hSupportedCaps, twConType))
  {
    CContainerWrapper cwSupportedCaps(hSupportedCaps, twConType);
    if(cwSupportedCaps.IsValid())
    {
      dlg.show();
      //Iterate through all the Capability groups
      for(int nIndex = 0; nIndex < ui.treCapability->topLevelItemCount(); nIndex++)
      {
        QTreeWidgetItem *pItem = ui.treCapability->topLevelItem(nIndex);
        dlg.ui.lblFirst->setText(QString("Updating %1 Capabilities").arg(pItem->text(0)));
        dlg.ui.prbMeter->setRange(0, pItem->childCount());
        dlg.ui.prbMeter->reset();
        //Iterate through all the Capabilities
        for(int nChildIndex = 0; nChildIndex < pItem->childCount(); nChildIndex++)
        {
          dlg.ui.prbMeter->setValue(nChildIndex);
          QTreeWidgetItem *pSubItem = pItem->child(nChildIndex);
          dlg.ui.lblSecond->setText(pSubItem->text(0));
          if(true==cwSupportedCaps.ContainsItem(static_cast<TW_UINT16>(pSubItem->data(0, Qt::UserRole).toUInt())))
          {
            pSubItem->setDisabled(false);
            if(bUpdateAll)
            {
              FillItemFromCapability(pSubItem);
            }
          }
          else
          {
            for(int nCol = ColCurrent; nCol < ColLast; nCol++)
            {
              //clear the columns
              pSubItem->setText(nCol, "");
            }
            //disable the item
            pSubItem->setDisabled(true);
          }
        }
      }
    }    
  }
  return;
}

void TWAIN_App_QT::FillItemFromCapability(QTreeWidgetItem *pItem)
{
  //Assume all columns are un-used
  pItem->setText(ColCurrent, "*");
  pItem->setText(ColDefault, "*");
  pItem->setText(ColCount, "*");
  pItem->setText(ColMin, "*");
  pItem->setText(ColMax, "*");
  pItem->setText(ColStep, "*");
  //Extract the Capability ID
  TW_UINT16 twCapId = static_cast<TW_UINT16>(pItem->data(0, Qt::UserRole).toUInt());
  //Find out what operations are supported
  TW_UINT32 twQuerySupport = 0;
  if(TWRC_SUCCESS==GetCapabilityOneValue(twCapId, twQuerySupport, MSG_QUERYSUPPORT))
  {
    //Update the QuerySupport column
    pItem->setText(ColSupport, convertQuerySupport_toString(twQuerySupport));
    //Determine if a MSG_GET operation is possible
    if(twQuerySupport & TWQC_GET)
    {
      //Call MSG_GET to find the supported values
      TW_HANDLE hCapability = NULL;
      TW_UINT16 twConType = TWON_DONTCARE16;
      if(TWRC_SUCCESS==GetCapabilitySupported(twCapId, hCapability, twConType))
      {
        //Crack the container
        CContainerWrapper cwCap(hCapability, twConType);
        if(cwCap.IsValid())
        {
          //Item type is common for all capabilities
          pItem->setText(ColType, convertTWTY_toString(cwCap.GetItemType()));
          //Fill the actual item
          switch(cwCap.GetConType())
          {
            case TWON_ENUMERATION:
              pItem->setText(ColDefault, CapabilityValueToString(twCapId, cwCap.GetItemType(), cwCap.GetDefault()));
            case TWON_ONEVALUE:
              pItem->setText(ColCurrent, CapabilityValueToString(twCapId, cwCap.GetItemType(), cwCap.GetCurrent()));
            case TWON_ARRAY:
              //Fill in the count
              pItem->setText(ColCount, QString::number(cwCap.GetCount()));
              break;
            case TWON_RANGE:
              //Fill in the current, default, minimum, maximum and step
              pItem->setText(ColCurrent, CapabilityValueToString(twCapId, cwCap.GetItemType(), cwCap.GetCurrent()));
              pItem->setText(ColDefault, CapabilityValueToString(twCapId, cwCap.GetItemType(), cwCap.GetDefault()));
              pItem->setText(ColMin, CapabilityValueToString(twCapId, cwCap.GetItemType(), cwCap.GetMin()));
              pItem->setText(ColMax, CapabilityValueToString(twCapId, cwCap.GetItemType(), cwCap.GetMax()));
              pItem->setText(ColStep, CapabilityValueToString(twCapId, cwCap.GetItemType(), cwCap.GetStep()));
              break;
            default:
              break;
          }
        }
      }
    }
  }
  return;
}

void TWAIN_App_QT::on_treCapability_itemClicked(QTreeWidgetItem *pItem, int nColumn)
{
  if(TWON_DONTCARE16 != static_cast<TW_UINT16>(pItem->data(0, Qt::UserRole).toUInt()))
  {
    FillItemFromCapability(pItem);
  }
  return;
}

void TWAIN_App_QT::on_treCapability_itemDoubleClicked(QTreeWidgetItem *pItem, int nColumn)
{
  if(4==GetCurrentState())
  {
    if(TWON_DONTCARE16 != static_cast<TW_UINT16>(pItem->data(0, Qt::UserRole).toUInt()))
    {
      DoSetCapability(pItem);
    }
  }
  return;
}

void TWAIN_App_QT::on_treCapability_itemActivated(QTreeWidgetItem *pItem, int nColumn)
{
  return;
}

void TWAIN_App_QT::DoSetCapability(QTreeWidgetItem *pItem)
{
  //Extract the Capability ID
  TW_UINT16 twCapId = static_cast<TW_UINT16>(pItem->data(0, Qt::UserRole).toUInt());
  //Find out what operations are supported
  TW_UINT32 twQuerySupport = 0;
  if(TWRC_SUCCESS==GetCapabilityOneValue(twCapId, twQuerySupport, MSG_QUERYSUPPORT))
  {
    //Update the QuerySupport column
    pItem->setText(ColSupport, convertQuerySupport_toString(twQuerySupport));
    //Determine if a MSG_GET operation is possible
    if(twQuerySupport & TWQC_GET)
    {
      //Call MSG_GET to find the supported values
      TW_HANDLE hCapability = NULL;
      TW_UINT16 twConType = TWON_DONTCARE16;
      if(TWRC_SUCCESS==GetCapabilitySupported(twCapId, hCapability, twConType))
      {
        //Crack the container
        CContainerWrapper cwCap(hCapability, twConType);
        if(cwCap.IsValid())
        {
          switch(cwCap.GetConType())
          {
            case TWON_ONEVALUE:
              #pragma message("TODO: Support set on items returning TWON_ONEVALUE for supported")
              break;
            case TWON_ENUMERATION:
              {
                EnumSet dlgEnumSet;
                for(TW_UINT32 uiIndex = 0; uiIndex < cwCap.GetCount(); uiIndex++)
                {
                  
                  QListWidgetItem *pListItem = new QListWidgetItem(dlgEnumSet.ui.lstValues);
                  if(sizeof(TW_UINT32)>=getTWTYsize(cwCap.GetItemType()))
                  {
                    TW_UINT32 twValue = 0;
                    memcpy(&twValue, cwCap.GetItem(uiIndex), getTWTYsize(cwCap.GetItemType()));
                    pListItem->setData(Qt::UserRole, QVariant::fromValue(twValue));
                    pListItem->setText(CapabilityValueToString(twCapId, cwCap.GetItemType(), cwCap.GetItem(uiIndex)));
                    dlgEnumSet.ui.lstValues->addItem(pListItem);
                    pListItem->setSelected( (cwCap.GetCurrent()==cwCap.GetItem(uiIndex))?true:false);
                  }
                }
                if(QDialog::Accepted == dlgEnumSet.exec())
                {
                  TW_UINT32 twValue = static_cast<TW_UINT32>(dlgEnumSet.ui.lstValues->selectedItems()[0]->data(Qt::UserRole).toUInt());
                  SetCapabilityOneValue(twCapId, static_cast<TW_UINT16>(twValue));
                  FillItemFromCapability(pItem);
                }
              }
              break;
            case TWON_RANGE:
              #pragma message("TODO: Support set on items returning TWON_RANGE for supported")
              break;
            case TWON_ARRAY:
              #pragma message("TODO: Support set on items returning TWON_ARRAY for supported")
              break;
          }
        }
      }
    }
  }
  return;
}

void TWAIN_App_QT::PostDSMCall()
{
  //default does nothing
  if(m_bSignalCalledInTWAIN)
  {
    //means that a signal was called recursively and lost
    DataSourceSignal();
  }
  return;
}

void TWAIN_App_QT::on_btnSelPath_clicked(bool bChecked)
{
  QString strResult = QFileDialog::getExistingDirectory(this, "Select Output Folder", ui.edtSavePath->text());
  if(strResult.length())
  {
    ui.edtSavePath->setText(strResult);
  }
  return;
}

void TWAIN_App_QT::on_treTriplet_itemDoubleClicked(QTreeWidgetItem *pItem, int nColum)
{
  switch(pItem->data(0, Qt::UserRole).toUInt())
  {
    case DAT_PENDINGXFERS:
      OnPendingXfers(pItem->data(1, Qt::UserRole).toUInt());
      break;
    case DAT_SETUPMEMXFER:
      OnSetupMemXfer(pItem->data(1, Qt::UserRole).toUInt());
      break;
    case DAT_SETUPFILEXFER:
      OnSetupFileXfer(pItem->data(1, Qt::UserRole).toUInt());
      break;
    case DAT_STATUS:
      OnStatus(pItem->data(1, Qt::UserRole).toUInt());
      break;
    case DAT_XFERGROUP:
      OnXferGroup(pItem->data(1, Qt::UserRole).toUInt());
      break;
    case DAT_CUSTOMDSDATA:
      OnCustomDSData(pItem->data(1, Qt::UserRole).toUInt());
      break;
    case DAT_DEVICEEVENT:
      OnDeviceEvent(pItem->data(1, Qt::UserRole).toUInt());
      break;
    case DAT_FILESYSTEM:
      OnFileSystem(pItem->data(1, Qt::UserRole).toUInt());
      break;
    case DAT_PASSTHRU:
      OnPassThru(pItem->data(1, Qt::UserRole).toUInt());
      break;
    case DAT_STATUSUTF8:
      OnStatusUTF8(pItem->data(1, Qt::UserRole).toUInt());
      break;
    case DAT_IMAGELAYOUT:
      OnImageLayout(pItem->data(1, Qt::UserRole).toUInt());
      break;
    case DAT_CIECOLOR:
      OnCIEColor(pItem->data(1, Qt::UserRole).toUInt());
      break;
    case DAT_GRAYRESPONSE:
      OnGrayResponse(pItem->data(1, Qt::UserRole).toUInt());
      break;
    case DAT_RGBRESPONSE:
      OnRGBResponse(pItem->data(1, Qt::UserRole).toUInt());
      break;
    case DAT_JPEGCOMPRESSION:
      OnJPEGCompression(pItem->data(1, Qt::UserRole).toUInt());
      break;
    case DAT_PALETTE8:
      OnPalette8(pItem->data(1, Qt::UserRole).toUInt());
      break;
    case DAT_EXTIMAGEINFO:
      OnExtImageInfo(pItem->data(1, Qt::UserRole).toUInt());
      break;
    case DAT_ICCPROFILE:
      OnICCProfile(pItem->data(1, Qt::UserRole).toUInt());
      break;
  }
  return;
}

void TWAIN_App_QT::OnPendingXfers(TW_UINT16 twMsg)
{
  switch(twMsg)
  {
  }
  return;
}

void TWAIN_App_QT::OnSetupMemXfer(TW_UINT16 twMsg)
{
  switch(twMsg)
  {
    case MSG_GET:
      if(TWRC_SUCCESS==DSM_Entry(DG_CONTROL, DAT_SETUPMEMXFER, MSG_GET, &m_twSetupMemXfer, &m_twSourceIdentity))
      {
        TraceStruct(m_twSetupMemXfer);
      }
      break;
  }
  return;
}

void TWAIN_App_QT::OnSetupFileXfer(TW_UINT16 twMsg)
{
  StructureEdit seDialog(this);
  switch(twMsg)
  {
    case MSG_GET:
      memset(&m_twSetupFileXfer, 0, sizeof(m_twSetupFileXfer));
      if(TWRC_SUCCESS==DSM_Entry(DG_CONTROL, DAT_SETUPFILEXFER, MSG_GET, &m_twSetupFileXfer, &m_twSourceIdentity))
      {
        TraceStruct(m_twSetupFileXfer);
      }
      break;
    case MSG_SET:
      //apply the structure
      seDialog.SetTWAINStructure(&m_twSetupFileXfer, DAT_SETUPFILEXFER);
      if(QDialog::Accepted==seDialog.exec())
      {
        //Execute the command
        if(TWRC_SUCCESS==DSM_Entry(DG_CONTROL, DAT_SETUPFILEXFER, MSG_SET, &m_twSetupFileXfer, &m_twSourceIdentity))
        {
          //Trace the result
          TraceStruct(m_twSetupFileXfer);
        }
      }
      break;
  }
  return;
}

void TWAIN_App_QT::OnStatus(TW_UINT16 twMsg)
{
  memset(&m_twStatus, 0, sizeof(m_twStatus));
  switch(twMsg)
  {
    case MSG_GET:
      //just pull the last known status
      GetLastStatus(m_twStatus);
      TraceStruct(m_twStatus);
      break;
  }
  return;
}

void TWAIN_App_QT::OnXferGroup(TW_UINT16 twMsg)
{
  switch(twMsg)
  {
  }
  return;
}

void TWAIN_App_QT::OnCustomDSData(TW_UINT16 twMsg)
{
  //first figure out if CUSTOMDSDATA is supported
  bool bSupports = false;
  if((TWRC_SUCCESS==GetCapabilityOneValue(CAP_CUSTOMDSDATA, bSupports)) && bSupports)
  {
    //clear the existing structure
    memset(&m_twCustomDSData, 0, sizeof(m_twCustomDSData));
    switch(twMsg)
    {
      case MSG_GET:
        {
          QString strResult = QFileDialog::getSaveFileName(this, "Save Custom DS Data", QString("%1/%2.bin").arg(ui.edtSavePath->text()).arg(m_twSourceIdentity.ProductName), "*.bin");
          //find out where the user wants to load the result
          if(strResult.length())
          {
            if(TWRC_SUCCESS==DSM_Entry(DG_CONTROL, DAT_CUSTOMDSDATA, MSG_GET, &m_twCustomDSData, &m_twSourceIdentity))
            {
              //trace the structure for the user
              TraceStruct(m_twCustomDSData);
              //Obtain a pointer to the custom DS Data block
              char *pBuffer = static_cast<char*>(_DSM_LockMemory(m_twCustomDSData.hData));
              if(pBuffer)
              {
                if(QFile::exists(strResult))
                {
                  //remove the original file
                  QFile::remove(strResult);
                }
                //attempt to re-create the file
                QFile img(strResult);
                //Open the new file for writing
                if(img.open(QIODevice::ReadWrite))
                {
                  //Prepare a stream for writing
                  QDataStream ds(&img);
                  ds.writeRawData(pBuffer, m_twCustomDSData.InfoLength);
                  img.close();
                }
                //Cleanup the CustomDSData block
                _DSM_UnlockMemory(m_twCustomDSData.hData);
                _DSM_Free(m_twCustomDSData.hData);
                m_twCustomDSData.hData = NULL;
              }
            }
          }
        }
        break;
      case MSG_SET:
        {
          //find out where the user wants to save the result
          QString strResult = QFileDialog::getOpenFileName(this, "Load Custom DS Data", ui.edtSavePath->text(), "*.bin");
          //Get the Custom DS Data
          if(strResult.length() && QFile::exists(strResult))
          {
            //attempt to re-create the file
            QFile img(strResult);
            m_twCustomDSData.InfoLength = img.size();
            m_twCustomDSData.hData = _DSM_Alloc(m_twCustomDSData.InfoLength);
            //Obtain a pointer to the custom DS Data block
            char *pBuffer = static_cast<char*>(_DSM_LockMemory(m_twCustomDSData.hData));
            if(pBuffer)
            {
              //Open the new file for writing
              if(img.open(QIODevice::ReadOnly))
              {
                //Prepare a stream for writing
                QDataStream ds(&img);
                m_twCustomDSData.InfoLength = ds.readRawData(pBuffer, m_twCustomDSData.InfoLength);
                if(-1!=(int)m_twCustomDSData.InfoLength)
                {
                  //trace the structure for the user
                  TraceStruct(m_twCustomDSData);
                  DSM_Entry(DG_CONTROL, DAT_CUSTOMDSDATA, MSG_SET, &m_twCustomDSData, &m_twSourceIdentity);
                }
                img.close();
              }
              //Cleanup the CustomDSData block
              _DSM_UnlockMemory(m_twCustomDSData.hData);
              _DSM_Free(m_twCustomDSData.hData);
            }
          }
        }
        break;
    }
  }
  return;
}

void TWAIN_App_QT::OnDeviceEvent(TW_UINT16 twMsg)
{
  switch(twMsg)
  {
    case MSG_GET:
      memset(&m_twDeviceEvent, 0, sizeof(m_twDeviceEvent));
      if(TWRC_SUCCESS==DSM_Entry(DG_CONTROL, DAT_DEVICEEVENT, MSG_GET, &m_twDeviceEvent, &m_twSourceIdentity))
      {
        TraceStruct(m_twDeviceEvent);
      }
      break;
  }
  return;
}

void TWAIN_App_QT::OnFileSystem(TW_UINT16 twMsg)
{
  //Allow the user to edit the structure
  StructureEdit seDialog(this);
  switch(twMsg)
  {
    case MSG_CREATEDIRECTORY:
    case MSG_DELETE:
    case MSG_FORMATMEDIA:
    case MSG_RENAME:
    case MSG_COPY:
    case MSG_AUTOMATICCAPTUREDIRECTORY:
    case MSG_CHANGEDIRECTORY:
    case MSG_GETINFO:
      //apply the structure
      seDialog.SetTWAINStructure(&m_twFileSystem, DAT_FILESYSTEM);
      if(QDialog::Accepted!=seDialog.exec())
      {
        break;
      }
    case MSG_GETCLOSE:
    case MSG_GETNEXTFILE:
    case MSG_GETFIRSTFILE:
      //Execute the command
      if(TWRC_SUCCESS==DSM_Entry(DG_CONTROL, DAT_FILESYSTEM, twMsg, &m_twFileSystem, &m_twSourceIdentity))
      {
        //Trace the result
        TraceStruct(m_twFileSystem);
      }
      break;
  }
  return;
}

void TWAIN_App_QT::OnPassThru(TW_UINT16 twMsg)
{
  switch(twMsg)
  {
  }
  return;
}

void TWAIN_App_QT::OnStatusUTF8(TW_UINT16 twMsg)
{
  switch(twMsg)
  {
  }
  return;
}

void TWAIN_App_QT::OnImageLayout(TW_UINT16 twMsg)
{
  StructureEdit seDialog(this);
  switch(twMsg)
  {
    case MSG_SET:
      //apply the structure
      seDialog.SetTWAINStructure(&m_twImageLayout, DAT_IMAGELAYOUT);
      if(QDialog::Accepted!=seDialog.exec())
      {
        break;
      }
    case MSG_GET:
    case MSG_GETDEFAULT:
    case MSG_RESET:
      if(TWRC_SUCCESS==DSM_Entry(DG_IMAGE, DAT_IMAGELAYOUT, twMsg, &m_twImageLayout, &m_twSourceIdentity))
      {
        TraceStruct(m_twImageLayout);
      }
      break;
  }
  return;
}

void TWAIN_App_QT::OnCIEColor(TW_UINT16 twMsg)
{
  switch(twMsg)
  {
  }
  return;
}

void TWAIN_App_QT::OnGrayResponse(TW_UINT16 twMsg)
{
  switch(twMsg)
  {
  }
  return;
}

void TWAIN_App_QT::OnRGBResponse(TW_UINT16 twMsg)
{
  switch(twMsg)
  {
  }
  return;
}

void TWAIN_App_QT::OnJPEGCompression(TW_UINT16 twMsg)
{
  switch(twMsg)
  {
  }
  return;
}

void TWAIN_App_QT::OnPalette8(TW_UINT16 twMsg)
{
  switch(twMsg)
  {
  }
  return;
}

void TWAIN_App_QT::OnExtImageInfo(TW_UINT16 twMsg)
{
  switch(twMsg)
  {
    case MSG_GET:
      {
        //make a copy of the extended info used for all operations
        vector<TW_UINT16> lstExtImageInfo = m_lstExtImageInfo;
        //Display a UI for selecting EXTIMAGEINFO values to read
        ExtImageInfoSel dlg(lstExtImageInfo, this);
        if(QDialog::Accepted==dlg.exec())
        {
          pTW_EXTIMAGEINFO pInfo = CreateExtImageInfo(m_lstExtImageInfo);
          if(pInfo)
          {
            if(TWRC_SUCCESS==DSM_Entry(DG_IMAGE, DAT_EXTIMAGEINFO, MSG_GET, pInfo, &m_twSourceIdentity))
            {
              TraceStruct(*pInfo);
            }
            CleanupExtImageInfo(pInfo);
          }
        }
      }
      break;
  }
  return;
}

void TWAIN_App_QT::OnICCProfile(TW_UINT16 twMsg)
{
  switch(twMsg)
  {
  }
  return;
}

void TWAIN_App_QT::TraceStruct(const TW_PENDINGXFERS &twData)
{
  return;
}

void TWAIN_App_QT::TraceStruct(const TW_SETUPMEMXFER &twData)
{
  TraceMessage("MinBufSize = %d, MaxBufSize = %d, Preferred = %d", twData.MinBufSize, twData.MaxBufSize, twData.Preferred);
  return;
}

void TWAIN_App_QT::TraceStruct(const TW_SETUPFILEXFER &twData)
{
  TraceMessage("Format = %s, FileName = \"%s\"", CapabilityValueToString(ICAP_IMAGEFILEFORMAT, TWTY_UINT16, &twData.Format), twData.FileName);
  return;
}

void TWAIN_App_QT::TraceStruct(const TW_STATUS &twData)
{
  TraceMessage("ConditionCode = %s, Data = 0x%02X", convertConditionCode_toString(twData.ConditionCode), twData.Data);
  return;
}

void TWAIN_App_QT::TraceStruct(const DWORD &twData)
{
  return;
}

void TWAIN_App_QT::TraceStruct(const TW_CUSTOMDSDATA &twData)
{
  TraceMessage("InfoLength = %d, hData = 0x%0X", twData.InfoLength, twData.hData);
  return;
}

void TWAIN_App_QT::TraceStruct(const TW_DEVICEEVENT &twData)
{
  TW_UINT16 twEventId = twData.Event;
  TraceMessage("Event = %s, DeviceName = \"%s\", BatteryMinutes = %d, BatteryPercentage = %d, PowerSupply = %d,",
    CapabilityValueToString(CAP_DEVICEEVENT, TWTY_UINT16, &twEventId), twData.DeviceName, twData.BatteryMinutes, twData.BatteryPercentage, twData.PowerSupply);
  TraceMessage("\tXRes = %.2f, YRes = %.2f, FlashUsed2 = %d, AutmaticCapture = %d, TimeBeforeFirstCapture = %d, TimeBetweenCaptures = %d",
    FIX32ToFloat(twData.XResolution), FIX32ToFloat(twData.YResolution), twData.FlashUsed2, twData.AutomaticCapture, twData.TimeBeforeFirstCapture, twData.TimeBetweenCaptures);
  return;
}

void TWAIN_App_QT::TraceStruct(const TW_FILESYSTEM &twData)
{
  TraceMessage("InputName = \"%s\", OutputName = \"%s\", Context = 0x%04X, FileType = %s, DeviceGroupMask = %d",
    twData.InputName, twData.OutputName, twData.Context, convertFileType_toString(twData.FileType), twData.DeviceGroupMask);
  return;
}

void TWAIN_App_QT::TraceStruct(const TW_PASSTHRU &twData)
{
  return;
}

void TWAIN_App_QT::TraceStruct(const TW_STATUSUTF8 &twData)
{
  return;
}

void TWAIN_App_QT::TraceStruct(const TW_IMAGELAYOUT &twData)
{
  TraceMessage("Left = %.2f, Top = %.2f, Right = %.2f, Bottom = %.2f, DocumentNumber = %d, PageNumber = %d, FrameNumber = %d",
    FIX32ToFloat(twData.Frame.Left), FIX32ToFloat(twData.Frame.Top), FIX32ToFloat(twData.Frame.Right), FIX32ToFloat(twData.Frame.Bottom), 
      twData.DocumentNumber, twData.PageNumber, twData.FrameNumber);
  return;
}

void TWAIN_App_QT::TraceStruct(const TW_CIECOLOR &twData)
{
  return;
}

void TWAIN_App_QT::TraceStruct(const TW_GRAYRESPONSE &twData)
{
  return;
}

void TWAIN_App_QT::TraceStruct(const TW_RGBRESPONSE &twData)
{
  return;
}

void TWAIN_App_QT::TraceStruct(const TW_PALETTE8 &twData)
{
  return;
}

void TWAIN_App_QT::TraceStruct(const TW_EXTIMAGEINFO &twData)
{
  TraceMessage("NumInfos = %d", twData.NumInfos);
  for(int nIndex = 0; nIndex < twData.NumInfos; nIndex++)
  {
    if(TWRC_SUCCESS==twData.Info[nIndex].ReturnCode)
    {
      TraceMessage("\tInfoID = %s, NumItems = %d, ItemType = %s, ReturnCode = %s, Item = %s", 
        convertExtImageInfoName_toString(twData.Info[nIndex].InfoID), twData.Info[nIndex].NumItems, convertTWTY_toString(twData.Info[nIndex].ItemType), convertReturnCode_toString(twData.Info[nIndex].ReturnCode), convertExtImageInfoItem_toString(twData.Info[nIndex]));
    }
  }
  return;
}

void TWAIN_App_QT::TraceStruct(const TW_MEMORY &twData)
{
  return;
}

pTW_EXTIMAGEINFO TWAIN_App_QT::CreateExtImageInfo(const vector<TW_UINT16> &lstExtIInfo)
{
  size_t nStructSize = sizeof(TW_EXTIMAGEINFO) + (sizeof(TW_INFO) * lstExtIInfo.size());
  pTW_EXTIMAGEINFO pRet = reinterpret_cast<pTW_EXTIMAGEINFO>(new BYTE[nStructSize]);
  memset(pRet, 0, nStructSize);
  if(pRet)
  {
    pRet->NumInfos = lstExtIInfo.size();
    for(int nIndex = 0; nIndex < pRet->NumInfos; nIndex++)
    {
      pRet->Info[nIndex].InfoID = lstExtIInfo[nIndex];
      pRet->Info[nIndex].ItemType = TWON_DONTCARE16;
      pRet->Info[nIndex].ReturnCode = TWRC_INFONOTSUPPORTED;
    }
  }
  return pRet;
}

pTW_INFO TWAIN_App_QT::FindInfo(pTW_EXTIMAGEINFO pInfo, TW_UINT16 twInfoID)
{
  pTW_INFO pRet = NULL;
  if(pInfo)
  {
    for(int nIndex = 0; nIndex < pInfo->NumInfos; nIndex++)
    {
      if(twInfoID==pInfo->Info[nIndex].InfoID)
      {
        pRet = &pInfo->Info[nIndex];
        break;
      }
    }
  }
  return pRet;
}

void TWAIN_App_QT::CleanupExtImageInfo(pTW_EXTIMAGEINFO &pInfo)
{
  if(pInfo)
  {
    delete [] reinterpret_cast<BYTE*>(pInfo);
    pInfo = NULL;
  }
  return;
}

void TWAIN_App_QT::FillExtImageInfoEdit()
{
  QString strText;
//m_lstExtImageInfo
  for(vector<TW_UINT16>::iterator iter = m_lstExtImageInfo.begin(); iter != m_lstExtImageInfo.end(); iter++)
  {
    strText += convertExtImageInfoName_toString(*iter);
    if(m_lstExtImageInfo.end()!=(iter+1))
    {
      strText += ", ";
    }
  }
  ui.edtExtImageInfo->setText(strText);
  return;
}

void TWAIN_App_QT::on_mnuLoadFromState1_triggered(bool bChecked)
{
  //what script to execute
  QString strResult = QFileDialog::getOpenFileName(this, "Load Script", "", "Scripts *.js *.qs");
  //Get the Custom DS Data
  if(strResult.length() && QFile::exists(strResult))
  {
    QFile qfScript(strResult);
    //try to load the script file
    if(qfScript.open(QIODevice::ReadOnly))
    {
      //move to state 1
      ReturnToState(1);
      //Update the state of the GUI
      EnableGUIByCurrentState();
      //submit the script to the script engine
      QScriptValue qsv = m_pEngine->evaluate(qfScript.readAll(), strResult);
      qfScript.close();
      //check for errors
      if(m_pEngine->hasUncaughtException())
      {
        //notify the user
        QMessageBox::warning(this, "Script Error", qsv.toString());
      }
    }
  }
  return;
}

void TWAIN_App_QT::on_mnuLoadFromCurrentState_triggered(bool bChecked)
{
  //what script to execute
  QString strResult = QFileDialog::getOpenFileName(this, "Load Script", "", "Scripts *.js *.qs");
  //Get the Custom DS Data
  if(strResult.length() && QFile::exists(strResult))
  {
    QFile qfScript(strResult);
    //try to load the script file
    if(qfScript.open(QIODevice::ReadOnly))
    {
      //submit the script to the script engine
      QScriptValue qsv = m_pEngine->evaluate(qfScript.readAll(), strResult);
      qfScript.close();
      //check for errors
      if(m_pEngine->hasUncaughtException())
      {
        //notify the user
        QMessageBox::warning(this, "Script Error", qsv.toString());
      }
    }
  }
  return;
}

QStringList TWAIN_App_QT::GetSourceList()
{
  QStringList lst;
  for(int nIndex = 0; nIndex < ui.cbxSources->count(); nIndex++)
  {
    lst.push_back(ui.cbxSources->itemText(nIndex));
  }
  return lst;
}

QString TWAIN_App_QT::GetCurrentSource()
{
  return ui.cbxSources->currentText();
}


bool TWAIN_App_QT::SetCurrentSource(QString strNewSource)
{
  int nIndex = ui.cbxSources->findText(strNewSource);
  if(-1!=nIndex)
  {
    ui.cbxSources->setCurrentIndex(nIndex);
  }
  return (ui.cbxSources->currentIndex()==nIndex)?true:false;
}

bool TWAIN_App_QT::LowerStateTo(int nState)
{
  bool bRC = ReturnToState(nState);
  //Update the state of the GUI
  EnableGUIByCurrentState();
  return bRC;
}

int TWAIN_App_QT::GetCurrentState()
{
  return CTWAINSession::GetCurrentState();
}

void TWAIN_App_QT::AppendScriptStatusText(QString strText)
{
  TraceMessage(strText.toAscii());
  return;
}

void TWAIN_App_QT::on_btnNext_clicked(bool bChecked)
{
  //Issue the DAT_PENDINGXFERS/MSG_ENDXFER call
  DoEndXfer();
  //Update the state of the GUI
  EnableGUIByCurrentState();
  return;
}

void TWAIN_App_QT::on_btnAllSupported_clicked(bool bChecked)
{
  //Iterate throught the capability list and force update the actual values
  RefreshCapabilityList(true);
  return;
}
