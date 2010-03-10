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
* @file mfcDlgConfigure.cpp
* Implementation file for dialog for the MFC TWAIN App Sample application
* @author JFL Peripheral Solutions Inc.
* @date October 2007
*/

#include "stdafx.h"
#include "..\src\DSMInterface.h"
#include "TwainString.h"
#include "mfc.h"
#include "mfcDlgConfigure.h"
#include "TW_Enum_Dlg.h"
#include <shlobj.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/**
* gloabals
*/
CmfcDlgConfigure *g_pTWAINApp = NULL;
extern bool gUSE_CALLBACKS;    // defined in TwainApp.cpp

//////////////////////////////////////////////////////////////////////////////
/**
* Callback funtion for DS.  This is a callback function that will be called by
* the source when it is ready for the application to start a scan. This 
* callback needs to be registered with the DSM before it can be called.
* It is important that the application returns right away after recieving this
* message.  Set a flag and return.  Do not process the callback in this function.
*/
TW_UINT16 FAR PASCAL DSMCallback(pTW_IDENTITY _pOrigin,
            pTW_IDENTITY _pDest,
            TW_UINT32    _DG,
            TW_UINT16    _DAT,
            TW_UINT16    _MSG,
            TW_MEMREF    _pData)
{
  TW_UINT16 twrc      = TWRC_FAILURE;
  // _pData stores the RefCon from the when the callback was registered
  // RefCon is a TW_INT32 and can not store a pointer for 64bit

  // we are only waiting for callbacks from our datasource, so validate
  // that the originator.
  if( 0 != _pOrigin 
   && 0 != g_pTWAINApp
   && g_pTWAINApp->getDataSource()
   && _pOrigin->Id == g_pTWAINApp->getDataSource()->Id )
  {
    switch (_MSG)
    {
      case MSG_XFERREADY:
      case MSG_CLOSEDSREQ:
      case MSG_CLOSEDSOK:
      case MSG_NULL:
        g_pTWAINApp->m_DSMessage = _MSG;
        twrc = TWRC_SUCCESS;
        break;

      default:
        TRACE("Error - Unknown message in callback routine");
        g_pTWAINApp->m_DSMessage = MSG_NULL;
        twrc = TWRC_FAILURE;
        break;
    }
  }

  return twrc;
}

// CmfcDlgConfigureConfigure dialog



CmfcDlgConfigure::CmfcDlgConfigure(CWnd* pParent, int nIndex)
  : CDialog(CmfcDlgConfigure::IDD, pParent), TwainApp(pParent->m_hWnd)
  ,m_sStr_DS(_T(""))
  ,m_sStr_ImageInfo(_T(""))
  ,m_sStr_ExtImageInfo(_T(""))
  ,m_bShowUI(FALSE)
  ,m_bKeepEnabled(FALSE)
  ,m_pCapSettings(NULL)
{
  // fill our identity structure
  SSTRCPY(m_MyInfo.Manufacturer, sizeof(m_MyInfo.Manufacturer), "TWAIN Working Group");
  SSTRCPY(m_MyInfo.ProductFamily, sizeof(m_MyInfo.ProductFamily), "Sample TWAIN App");
  SSTRCPY(m_MyInfo.ProductName, sizeof(m_MyInfo.ProductName), "MFC GUI TWAIN app");

  m_nIndex = nIndex;
  m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CmfcDlgConfigure::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_STC_DS, m_sStr_DS);
  DDX_Text(pDX, IDC_IMAGEINFO, m_sStr_ImageInfo);
  DDX_Text(pDX, IDC_EXTIMAGEINFO, m_sStr_ExtImageInfo);
  DDX_Check(pDX, IDC_SHOWUI, m_bShowUI);
  DDX_Check(pDX, IDC_KEEPOPEN, m_bKeepEnabled);
  DDX_Control(pDX, IDLC_CAPS, m_ListCtrl_Caps);
  DDX_Control(pDX, IDC_SAVEPATH, m_EdtSavePath);
}

BEGIN_MESSAGE_MAP(CmfcDlgConfigure, CDialog)
  ON_WM_SYSCOMMAND()
  ON_WM_PAINT()
  ON_WM_QUERYDRAGICON()
  //}}AFX_MSG_MAP
  ON_WM_DESTROY()
  ON_BN_CLICKED(IDB_SCAN, OnBnClickedScan)
  ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
  ON_NOTIFY(NM_DBLCLK, IDLC_CAPS, &CmfcDlgConfigure::OnNMDblclkCaps)
  ON_NOTIFY(NM_CUSTOMDRAW, IDLC_CAPS, &CmfcDlgConfigure::OnNMCustomdrawCaps)
  ON_BN_CLICKED(IDB_UIONLY, &CmfcDlgConfigure::OnBnClickedUIOnly)
  ON_BN_CLICKED(IDB_PATH, &CmfcDlgConfigure::OnBnClickedPath)
END_MESSAGE_MAP()


// CmfcDlgConfigure message handlers

BOOL CmfcDlgConfigure::OnInitDialog()
{
  CDialog::OnInitDialog();

  // Set the icon for this dialog.  The framework does this automatically
  //  when the application's main window is not a dialog
  SetIcon(m_hIcon, TRUE);     // Set big icon
  SetIcon(m_hIcon, FALSE);    // Set small icon

  GetSavePath();
  m_EdtSavePath.SetWindowText(m_strSavePath.c_str());

  g_pTWAINApp = this;

  connectDSM();
  loadDS(m_nIndex);

  if( NULL != m_pDataSource )
  {
    m_sStr_DS.Format( "Scanner: %s\nManufacturer: %s\nProductFamily: %s\nInfo: %s", 
      m_pDataSource->ProductName, m_pDataSource->Manufacturer, m_pDataSource->ProductFamily, 
      m_pDataSource->Version.Info);
    UpdateData(false);
  }

  if(4 <= m_DSMState)
  {
    DWORD style = m_ListCtrl_Caps.GetExtendedStyle();
    m_ListCtrl_Caps.SetExtendedStyle(style | LVS_EX_FULLROWSELECT);

    LVCOLUMN    col;
    memset(&col, 0, sizeof(col));
    col.mask      = LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;

    col.pszText   = "Capability";
    col.iSubItem  = 0;
    col.cx        = 160;
    m_ListCtrl_Caps.InsertColumn(0, &col);
    
    col.pszText   = "Current Value";
    col.iSubItem  = 1;
    col.cx        = 95;
    m_ListCtrl_Caps.InsertColumn(1, &col);

    col.pszText   = "Querey";
    col.iSubItem  = 2;
    col.cx        = 65;
    m_ListCtrl_Caps.InsertColumn(2, &col);

    col.pszText   = "#";
    col.iSubItem  = 3;
    col.cx        = 20;
    m_ListCtrl_Caps.InsertColumn(3, &col);

    col.pszText   = "Type";
    col.iSubItem  = 4;
    col.cx        = 95;
    m_ListCtrl_Caps.InsertColumn(4, &col);

    ListSupportedCaps();
    PopulateCurentValues(false);

    CEdit *pWnd = NULL;

    pWnd = (CEdit*)GetDlgItem(IDC_EXTIMAGEINFO);
    if(pWnd)
    {
      pWnd->SetTabStops(105);
    }

    pWnd = (CEdit*)GetDlgItem(IDC_IMAGEINFO);
    if(pWnd)
    {
      INT   TabStops[3] = {35, 95, 145};
      pWnd->SetTabStops(3, TabStops);
    }
  }

  return TRUE;  // return TRUE  unless you set the focus to a control
}

void CmfcDlgConfigure::OnSysCommand(UINT nID, LPARAM lParam)
{
  CDialog::OnSysCommand(nID, lParam);
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CmfcDlgConfigure::OnPaint() 
{
  CDialog::OnPaint();
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CmfcDlgConfigure::OnQueryDragIcon()
{
  return static_cast<HCURSOR>(m_hIcon);
}

void CmfcDlgConfigure::OnDestroy()
{
  exit();
  g_pTWAINApp = NULL;

  if(m_pCapSettings)
  {
    delete[] m_pCapSettings;
    m_pCapSettings = 0;
  }

  CDialog::OnDestroy();

  return;
}

void CmfcDlgConfigure::ListSupportedCaps()
{
  TW_UINT32         i;
  TW_UINT32         nCount          = 0;
  TW_CAPABILITY     CapSupportedCaps;
  pTW_ARRAY_UINT16  pCapSupCaps     = 0;
  string            sCapName;

  m_ListCtrl_Caps.DeleteAllItems();

  LV_ITEM           Item;
  memset(&Item, 0, sizeof(Item));
  Item.iSubItem   = 0;
  Item.mask       = LVIF_PARAM | LVIF_TEXT;

  // get the supported capabilies
  CapSupportedCaps.Cap = CAP_SUPPORTEDCAPS;
  CapSupportedCaps.hContainer = 0;

  get_CAP(CapSupportedCaps, MSG_GET);

  if(TWON_ARRAY != CapSupportedCaps.ConType)
  {
    goto cleanup;
  }

  pCapSupCaps = (pTW_ARRAY_UINT16)_DSM_LockMemory(CapSupportedCaps.hContainer);

  if(TWTY_UINT16 != pCapSupCaps->ItemType)
  {
    goto cleanup;
  }

  nCount = pCapSupCaps->NumItems;
  m_ListCtrl_Caps.SetItemCount(nCount);

  if(m_pCapSettings)
  {
    delete[] m_pCapSettings;
  }
  m_pCapSettings = new CapSetting[nCount];

  for(i=0; i<nCount; i++)
  {
    // we are not listing this CAPABILITIES
    if(pCapSupCaps->ItemList[i] != CAP_SUPPORTEDCAPS)
    {
      if(TWRC_SUCCESS == GetLabel(pCapSupCaps->ItemList[i], sCapName))
      {
        Item.pszText    = (LPSTR)sCapName.c_str();
      }
      else
      {
        Item.pszText    = (LPSTR)convertCAP_toString(pCapSupCaps->ItemList[i]);
      }

      m_pCapSettings[i].CapID     = pCapSupCaps->ItemList[i];
      m_pCapSettings[i].byChanged  = 0;
      m_pCapSettings[i].bReadOnly = FALSE;
      Item.lParam = (LPARAM)&m_pCapSettings[i];
      Item.iItem  = i;

      m_ListCtrl_Caps.InsertItem(&Item);
    }
  }

cleanup:
  if(pCapSupCaps)
  {
    _DSM_UnlockMemory(CapSupportedCaps.hContainer);
  }
  if(CapSupportedCaps.hContainer)
  {
    _DSM_Free(CapSupportedCaps.hContainer);
  }
}

void CmfcDlgConfigure::PopulateCurentValues(bool bCheckForChange /*=true*/)
{
  TW_UINT32         nItem;
  TW_UINT32         nCount    = m_ListCtrl_Caps.GetItemCount();
  TW_UINT16         CondCode;
  LV_ITEM           Item;
  CapSetting       *pCapSetting = 0;
  TW_CAPABILITY     Cap         = {0};
  TW_UINT32         QS          = 0;
  string            sItemValue;
  BOOL              byChanged;
  BOOL              bReadOnly;
  CWaitCursor       wait;

  for(nItem=0; nItem<nCount; nItem++)
  {
    memset(&Item, 0, sizeof(Item));
    Item.iItem      = nItem;
    Item.iSubItem   = 0;
    Item.mask       = LVIF_PARAM;

    byChanged = 0;
    bReadOnly = FALSE;

    m_ListCtrl_Caps.GetItem(&Item);
    pCapSetting = (CapSetting*)Item.lParam;

    // All the subitems are added as text
    Item.mask       = LVIF_TEXT;

    // get the capability that is supported
    Cap.Cap         = pCapSetting->CapID;
    Cap.hContainer  = 0;
    Cap.ConType     = TWON_DONTCARE16;

    // We will ignor what Query Supported has reproted about Message Get and get the current anyway.
    CondCode = get_CAP(Cap, MSG_GET);
    if(CondCode==TWCC_SUCCESS)
    {
      pTW_ONEVALUE pCap = NULL;
      if( Cap.hContainer
       && NULL != (pCap = (pTW_ONEVALUE)_DSM_LockMemory(Cap.hContainer)) )
      {
        TW_UINT16 type = pCap->ItemType;
        _DSM_UnlockMemory(Cap.hContainer);

        switch(type)
        {
          case TWTY_INT8:
          case TWTY_UINT8:
          case TWTY_INT16:
          case TWTY_UINT16:
          case TWTY_INT32:
          case TWTY_UINT32:
          case TWTY_BOOL:
          {
            if(Cap.ConType == TWON_ARRAY)
            {
              TW_UINT32 uVal;
              TW_UINT32 uCount = 0;
              sItemValue = "";
              while( GetItem(&Cap, uCount, uVal) )
              {
                if(sItemValue.length()!=0)
                   sItemValue += ", ";
                sItemValue += convertCAP_Item_toString(Cap.Cap, uVal, type);
                uCount++;
              }
            }
            else
            {
              TW_UINT32 uVal;
              getCurrent(&Cap, uVal);
              sItemValue = convertCAP_Item_toString(Cap.Cap, uVal, type);
            }
            break;
          }

          case TWTY_STR32:
          case TWTY_STR64:
          case TWTY_STR128:
          case TWTY_STR255:
          {
            if(Cap.ConType == TWON_ARRAY)
            {
              string sVal;
              TW_UINT32 uCount = 0;
              sItemValue = "";
              while( GetItem(&Cap, uCount, sVal) )
              {
                if(sItemValue.length()!=0)
                   sItemValue += ", ";
                sItemValue += sVal;
                uCount++;
              }
            }
            else
            {
              getCurrent(&Cap, sItemValue);
            }
            break;
          }

          case TWTY_FIX32:
          {
            TW_FIX32 fix32;
            CString value;

            if(Cap.ConType == TWON_ARRAY)
            {
              TW_UINT32 uCount = 0;
              sItemValue = "";
              while( GetItem(&Cap, uCount, fix32) )
              {
                if(sItemValue.length()!=0)
                   sItemValue += ", ";
                value.Format("%d.%d", fix32.Whole, (int)((fix32.Frac/65536.0 + .0005)*1000) );
                sItemValue = value;
                uCount++;
              }
            }
            else
            {
              getCurrent(&Cap, fix32);
              value.Format("%d.%d", fix32.Whole, (int)((fix32.Frac/65536.0 + .0005)*1000) );
              sItemValue = value;
            }
            break;
          }

          case TWTY_FRAME:
          {
            TW_FRAME frame;
            CString value;

            if(Cap.ConType == TWON_ARRAY)
            {
              TW_UINT32 uCount = 0;
              sItemValue = "";
              while( GetItem(&Cap, uCount, frame) )
              {
                if(sItemValue.length()!=0)
                   sItemValue += ", ";
                value.Format( "%d.%d  %d.%d  %d.%d  %d.%d", 
                     frame.Left.Whole,   (int)((frame.Left.Frac/65536.0 + .0005)*1000),
                     frame.Right.Whole,  (int)((frame.Right.Frac/65536.0 + .0005)*1000),
                     frame.Top.Whole,    (int)((frame.Top.Frac/65536.0 + .0005)*1000),
                     frame.Bottom.Whole, (int)((frame.Bottom.Frac/65536.0 + .0005)*1000) );
                sItemValue = value;
                uCount++;
              }
            }
            else
            {
              getCurrent(&Cap, frame);
              value.Format( "%d.%d  %d.%d  %d.%d  %d.%d", 
                   frame.Left.Whole,   (int)((frame.Left.Frac/65536.0 + .0005)*1000),
                   frame.Right.Whole,  (int)((frame.Right.Frac/65536.0 + .0005)*1000),
                   frame.Top.Whole,    (int)((frame.Top.Frac/65536.0 + .0005)*1000),
                   frame.Bottom.Whole, (int)((frame.Bottom.Frac/65536.0 + .0005)*1000) );
              sItemValue = value;
            }
            break;
          }
          default:
          {
            sItemValue = "?";
            break;
          }
        }
      }
      else //if(Cap.hContainer)
      {
        sItemValue = "<<Invalid Container>>";
      }
    }
    else
    {
      CString sError;
      sError.Format("<<%s>>", convertConditionCode_toString(CondCode));
      sItemValue = sError;
    }

    // Get the current test to see if it has changed.
    if(bCheckForChange)
    {
      char oldString[260];
      Item.cchTextMax = 260;
      Item.iSubItem   = 1;
      Item.pszText = oldString;
      m_ListCtrl_Caps.GetItem(&Item);
      if(0 != strncmp(sItemValue.c_str(), oldString, 259))
      {
        byChanged |= 1<<Item.iSubItem;
      }
    }

    Item.iSubItem   = 1;
    Item.pszText    = (char *)sItemValue.c_str();
    m_ListCtrl_Caps.SetItem(&Item);

    switch(Cap.ConType)
    {
      case TWON_ARRAY:
        sItemValue = "Array";
      break;

      case TWON_ENUMERATION:
        sItemValue = "Enumeration";
      break;

      case TWON_ONEVALUE:
        sItemValue = "One Value";
      break;

      case TWON_RANGE:
        sItemValue = "Range";
      break;
    }

    // Get the current test to see if it has changed.
    if(bCheckForChange)
    {
      char oldString[260];
      Item.cchTextMax = 260;
      Item.iSubItem   = 2;
      Item.pszText = oldString;
      m_ListCtrl_Caps.GetItem(&Item);
      if(0 != strcmp(sItemValue.c_str(), oldString))
      {
        byChanged |= 1<<Item.iSubItem;
      }
    }

    Item.iSubItem   = 2;
    Item.pszText    = (char *)sItemValue.c_str();
    m_ListCtrl_Caps.SetItem(&Item);
    TW_UINT32 nNumItems = 0;

    if( Cap.ConType == TWON_ARRAY
     || Cap.ConType == TWON_ENUMERATION )
    {
      pTW_ARRAY pCap = (pTW_ARRAY)_DSM_LockMemory(Cap.hContainer);
      if(pCap)
      {
        nNumItems = pCap->NumItems;
        _DSM_UnlockMemory(Cap.hContainer);
      }

      CString Value;
      Value.Format("%d", nNumItems);
      sItemValue = Value;

      // Get the current test to see if it has changed.
      if(bCheckForChange)
      {
        char oldString[260];
        Item.cchTextMax = 260;
        Item.iSubItem   = 3;
        Item.pszText = oldString;
        m_ListCtrl_Caps.GetItem(&Item);
        if(0 != strcmp(sItemValue.c_str(), oldString))
        {
          byChanged |= 1<<Item.iSubItem;
        }
      }

      Item.iSubItem   = 3;
      Item.pszText    = (char *)sItemValue.c_str();
      m_ListCtrl_Caps.SetItem(&Item);
    }

    QuerySupport_CAP(Cap.Cap, QS);

    sItemValue = "";
    if( QS & (TWQC_GET|TWQC_GETDEFAULT|TWQC_GETCURRENT) )
    {
      sItemValue += "Get";
    }
    if(QS & TWQC_SET)
    {
      if(0 != sItemValue.length())
        sItemValue += ", ";
      sItemValue += "Set";
    }
    if(QS & TWQC_RESET)
    {
      if(0 != sItemValue.length())
        sItemValue += ", ";
      sItemValue += "Reset";
    }

    // Get the current test to see if it has changed.
    if(bCheckForChange)
    {
      char oldString[260];
      Item.cchTextMax = 260;
      Item.iSubItem   = 4;
      Item.pszText = oldString;
      m_ListCtrl_Caps.GetItem(&Item);
      if(0 != strcmp(sItemValue.c_str(), oldString))
      {
        byChanged |= 1<<Item.iSubItem;
      }
    }

    Item.iSubItem   = 4;
    Item.pszText    = (char *)sItemValue.c_str();
    m_ListCtrl_Caps.SetItem(&Item);

    if( ( Cap.ConType == TWON_ENUMERATION && nNumItems <= 1 ) 
     || !( QS & TWQC_SET ) )
    {
      bReadOnly = TRUE;
    }

    pCapSetting->byChanged = byChanged;
    pCapSetting->bReadOnly = bReadOnly;
  }
  m_ListCtrl_Caps.Invalidate(FALSE);
}

void CmfcDlgConfigure::MarkUnchanged()
{
  TW_UINT32 nItem;
  TW_UINT32 nCount = m_ListCtrl_Caps.GetItemCount();
  for(nItem=0; nItem<nCount; nItem++)
  {
    m_pCapSettings[nItem].byChanged  = 0;
  }
  m_ListCtrl_Caps.Invalidate(FALSE);
}

// This method sets the new current value.
// TODO set constraints using Enumerations and Ranges
void CmfcDlgConfigure::OnNMDblclkCaps(NMHDR *pNMHDR, LRESULT *pResult)
{
  LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

  TW_UINT16       CAPLstData  = 0;
  bool            bChange     = false;
  TW_CAPABILITY   Cap;
  LV_ITEM         Item;

  memset(&Item, 0, sizeof(Item));
  Item.iItem      = pNMItemActivate->iItem;
  Item.iSubItem   = 0;
  Item.mask       = LVIF_PARAM;
  m_ListCtrl_Caps.GetItem(&Item);

  // get the capability that is supported
  CapSetting *pCapSettings = (CapSetting *)Item.lParam;
  Cap.Cap         = pCapSettings->CapID;
  Cap.hContainer  = 0;

  // get possible values to populate dialog box with
  if(TWCC_SUCCESS==get_CAP(Cap, MSG_GET))
  {
    // Special Case first
    if(CAP_XFERCOUNT == Cap.Cap)
    {
      TW_UINT32 Val;
      getCurrent(&Cap, Val);
      Val = (TW_INT16)Val == -1? 1: -1;// flop
      set_CapabilityOneValue(Cap.Cap, Val, TWTY_INT16);
      bChange = true;
    }
    else if( Cap.ConType == TWON_ENUMERATION )
    {
      CTW_Enum_Dlg    Dlg(this);
      Dlg.m_pCap = &Cap;

      if( IDOK == Dlg.DoModal() )
      {
        TW_UINT16 type = TWTY_UINT16;
        TW_INT16  twrc = TWRC_FAILURE;

        pTW_ONEVALUE pCap = (pTW_ONEVALUE)_DSM_LockMemory(Cap.hContainer);
        if(pCap)
        {
          type = pCap->ItemType;
          _DSM_UnlockMemory(Cap.hContainer);

          switch(type)
          {
            case TWTY_INT8:
            case TWTY_INT16:
            case TWTY_INT32:
            case TWTY_UINT8:
            case TWTY_UINT16:
            case TWTY_UINT32:
            case TWTY_BOOL:
            {
              twrc = set_CapabilityOneValue(Cap.Cap, (TW_UINT16)Dlg.m_SelectionData, type);
              break;
            }

            case TWTY_FIX32:
            {
              TW_FIX32 TWfix32 = FloatToFIX32( (float)(Dlg.m_SelectionData/100.0));
              twrc = set_CapabilityOneValue(Cap.Cap, &TWfix32);
              break;
            }

            default:
              MessageBox("Setting this data type is not implemented.  Patches welcome.", "Not Implemented");
              break;
          }

          if(twrc == TWRC_SUCCESS || twrc == TWRC_CHECKSTATUS)
          {
            bChange = true;
          }
        }
      }
    }
    else if(Cap.ConType == TWON_ONEVALUE)
    {
      pTW_ONEVALUE pCapPT = (pTW_ONEVALUE)_DSM_LockMemory(Cap.hContainer);
      switch(pCapPT->ItemType)
      {
        case TWTY_BOOL:
        {
          // Not displaying a dialog just try to set the opposit value
          TW_INT16  twrc = set_CapabilityOneValue(Cap.Cap, (TW_UINT16)pCapPT->Item? FALSE:TRUE, TWTY_BOOL);
          if(twrc == TWRC_SUCCESS || twrc == TWRC_CHECKSTATUS)
          {
            bChange = true;
          }
        }
        break;

        default:
          //TODO create a dialog to set OneValue
          if(!pCapSettings->bReadOnly)
          {
            MessageBox("Setting OneValue is not implemented.  Patches welcome.", "Not Implemented");
          }
        break;
      }
      _DSM_UnlockMemory(Cap.hContainer);
    }
    else
    {
      //ToDo create a dialog to set Ranges and arrays
      if(!pCapSettings->bReadOnly)
      {
        MessageBox("Setting Ranges and arrays is not implemented.  Patches welcome.", "Not Implemented");
      }
    }
  }

  if(bChange)
  {
    // Modifiying one CAP can change several others - repopulate the list of CAPS
    PopulateCurentValues(true);
  }
  else
  {
    MarkUnchanged();
  }
  *pResult = 0;
}

void CmfcDlgConfigure::OnBnClickedScan()
{
  if(m_DSMState > 4)
    return; // Already Enabled

  UpdateData(true);

  // -Enable the data source. This puts us in state 5 which means that we
  // have to wait for the data source to tell us to move to state 6 and
  // start the transfer.  Once in state 5, no more set ops can be done on the
  // caps, only get ops.
  // -The scan will not start until the source calls the callback function
  // that was registered earlier.
  if( !enableDS(GetSafeHwnd(), m_bShowUI) )
  {
    return;
  }

  do // If showing UI and KeepUIOpen is Enabled then we will loop here until cancle
  {

    m_DSMessage = (TW_UINT16)-1;

    // now we have to wait until we hear something back from the DS.
    while((TW_UINT16)-1 == m_DSMessage)
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
      twRC = _DSM_Entry( getAppIdentity(),
                  m_pDataSource,
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
          case MSG_NULL:
            m_DSMessage = twEvent.TWMessage;
            break;

          case MSG_CLOSEDSOK:
            TRACE("\nError - MSG_CLOSEDSOK in MSG_PROCESSEVENT loop for Scan\n");
            break;

          default:
            TRACE("\nError - Unknown message in MSG_PROCESSEVENT loop for Scan\n");
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

    if(m_DSMessage == MSG_XFERREADY)
    {
      // move to state 6 as a result of the data source. We can start a scan now.
      m_DSMState = 6;
      updateIMAGEINFO();
      UpdateImageInfo();
      StartScan();
    }
  }while(m_bShowUI && m_bKeepEnabled && m_DSMessage != MSG_CLOSEDSREQ);

  // Scan is done, disable the ds, thus moving us back to state 4 where we
  // can negotiate caps again.
  disableDS();

  // update showing new values
  PopulateCurentValues(true);

  return;
}

void CmfcDlgConfigure::OnBnClickedUIOnly()
{
  if(m_DSMState > 4)
    return; // Already Enabled

  m_DSMessage = (TW_UINT16)-1;

  UpdateData(true);

  // Enable the data source. This puts us in state 5 which means that we
  // have to wait for the data source to tell us close.
  // Once in state 5, no more set ops can be done on the caps, only get ops.
  if( !enableDSUIOnly(GetSafeHwnd()) )
  {
    return;
  }

  // now we have to wait until we hear something back from the DS.
  while((TW_UINT16)-1 == m_DSMessage)
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
    twRC = _DSM_Entry( getAppIdentity(),
                m_pDataSource,
                DG_CONTROL,
                DAT_EVENT,
                MSG_PROCESSEVENT,
                (TW_MEMREF)&twEvent);

    if(!gUSE_CALLBACKS && twRC==TWRC_DSEVENT)
    {
      // check for message from Source
      switch (twEvent.TWMessage)
      {
        case MSG_CLOSEDSREQ:
        case MSG_CLOSEDSOK:
        case MSG_NULL:
          m_DSMessage = twEvent.TWMessage;
          break;

        case MSG_XFERREADY: // Should never get this when Show UI Only
          TRACE("\nError - MSG_XFERREADY in MSG_PROCESSEVENT loop for Show UI Only\n");
          break;

        default:
          TRACE("\nError - Unknown message in MSG_PROCESSEVENT loop for Show UI Only\n");
          break;
      }
    }
    if(twRC!=TWRC_DSEVENT)
	  {   
		  TranslateMessage ((LPMSG)&Msg);
		  DispatchMessage ((LPMSG)&Msg);
	  }
  }

  // ShowUI is done, disable the ds, thus moving us back to state 4 where we
  // can negotiate caps again.
  disableDS();

  // update showing new values
  PopulateCurentValues(true);

  return;
}

void CmfcDlgConfigure::StartScan()
{
  TW_UINT32       mech;
  TW_CAPABILITY   Cap;

  Cap.Cap = ICAP_XFERMECH;
  Cap.hContainer = 0;

  if( TWCC_SUCCESS!=get_CAP(Cap, MSG_GETCURRENT)
    ||!getCurrent(&Cap, mech) )
  {
      TRACE("Error: could not get the transfer mechanism");
      return;
  }

  if(Cap.hContainer)
  {
   _DSM_Free(Cap.hContainer);
  }

  switch ( (TW_UINT16)mech )
  {
  case TWSX_NATIVE:
   initiateTransfer_Native();
    break;

  case TWSX_FILE:
    {
      TW_UINT32 fileformat = TWFF_TIFF;
      Cap.Cap = ICAP_IMAGEFILEFORMAT;
      Cap.hContainer = 0;

      if(TWCC_SUCCESS==get_CAP(Cap, MSG_GETCURRENT))
      {
        getCurrent(&Cap, fileformat);
      }

      if(Cap.hContainer)
      {
       _DSM_Free(Cap.hContainer);
      }

      initiateTransfer_File((TW_UINT16)fileformat);
    }
    break;

  case TWSX_MEMORY:
    initiateTransfer_Memory();
    break;
  }

  return;
}

void CmfcDlgConfigure::OnBnClickedCancel()
{
  // TODO: Add your control notification handler code here
  m_DSMessage = MSG_NULL;

  exit();
  OnCancel();
}


void CmfcDlgConfigure::UpdateImageInfo()
{
  pTW_IMAGEINFO pII = getIMAGEINFO();
  m_sStr_ImageInfo.Format( 
                          "Width: \t%d \tBitsPerPixel: \t%d\r\n"
                          "Length:\t%d \tPlanar:       \t%s\r\n"
                          "Res x: \t%d \tPixelType:    \t%s\r\n"
                          "Res y: \t%d \tCompression:  \t%s\r\n",
                          pII->ImageWidth,  pII->BitsPerPixel, 
                          pII->ImageLength, convertICAP_PLANARCHUNKY_toString(pII->Planar),
                          pII->XResolution, convertICAP_PIXELTYPE_toString(pII->PixelType), 
                          pII->YResolution, convertICAP_COMPRESSION_toString(pII->Compression) );
  // \tBitsPerSample:\t%d\r\n"pII->BitsPerSample,
  UpdateData(false);
}

void CmfcDlgConfigure::updateEXTIMAGEINFO()
{
  // Call base class first
  TwainApp::updateEXTIMAGEINFO();
  m_sStr_ExtImageInfo += getEXIMAGEINFO().c_str();
  m_sStr_ExtImageInfo += "\r\n";
  UpdateData(false);
}


void CmfcDlgConfigure::OnNMCustomdrawCaps(NMHDR *pNMHDR, LRESULT *pResult)
{
  LPNMLVCUSTOMDRAW lpLVCustomDraw = reinterpret_cast<LPNMLVCUSTOMDRAW>(pNMHDR);

  switch(lpLVCustomDraw->nmcd.dwDrawStage)
  {
    case CDDS_PREPAINT:
    {
      *pResult = CDRF_NOTIFYITEMDRAW;          // ask for item notifications.
    }
    break;

    case CDDS_ITEMPREPAINT:
    {
      CapSetting *pCapSettings = (CapSetting *)lpLVCustomDraw->nmcd.lItemlParam;
      if(pCapSettings->byChanged)
      {
        if(pCapSettings->bReadOnly)
        {
          // Changed and readonly = Light Red
          lpLVCustomDraw->clrText = RGB(255, 96, 96);
        }
        else
        {
          // Changed only = Dark Red
          lpLVCustomDraw->clrText = RGB(255, 0, 0);
        }
        *pResult = CDRF_NOTIFYITEMDRAW;
      }
      else if(pCapSettings->bReadOnly)
      {
        // Unchanged but readoly = gray
        lpLVCustomDraw->clrText = RGB(128, 128, 128);
        *pResult = CDRF_NEWFONT;
      }
      else
      {
        // Default = black
        *pResult = CDRF_DODEFAULT;
      }
    }
    break;
    
    case CDDS_ITEMPREPAINT | CDDS_SUBITEM:
    {
      // Only getting sub item notification when capability has changed.
      CapSetting *pCapSettings = (CapSetting *)lpLVCustomDraw->nmcd.lItemlParam;
      if(pCapSettings->byChanged)
      {
        if( (1<<lpLVCustomDraw->iSubItem) & pCapSettings->byChanged )
        {
          lpLVCustomDraw->clrTextBk = RGB(200, 255, 255);
        }
        else
        {
          lpLVCustomDraw->clrTextBk = RGB(255, 255, 255);
        }
        *pResult = CDRF_NEWFONT;
      }
      else
      {
        // Default = black
        *pResult = CDRF_DODEFAULT;
      }
    }
    break;

    default:
    {
      *pResult = CDRF_DODEFAULT;
    }
    break;
  }
}

#define KEYPATH _T("SOFTWARE\\TWAIN Working Group\\TWAIN2 Sample\\") 

bool CmfcDlgConfigure::GetSavePath()
{
  bool      bReturn = false;
  TCHAR     szSavePath[MAX_PATH];
  HKEY      hKey;
  DWORD     disposition;	//create the key; show if we have errors or not

  if(ERROR_SUCCESS == RegCreateKeyEx(HKEY_CURRENT_USER, KEYPATH, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_READ, NULL, &hKey, &disposition))
  {
    DWORD dataType  = REG_EXPAND_SZ;
    DWORD dataSize  = sizeof(szSavePath);
    
    memset(szSavePath, 0, dataSize);

    //Read it if we can
    if( ERROR_SUCCESS == RegQueryValueEx(hKey, _T("SavePath"), NULL, &dataType, (BYTE*)&szSavePath, &dataSize) )
    {
      m_strSavePath = szSavePath;
      bReturn = true;
    }
    RegCloseKey(hKey);
  }

  return bReturn;
}

bool CmfcDlgConfigure::SetSavePath(const TCHAR *pPath)
{
  bool      bReturn = false;
  TCHAR     szSavePath[MAX_PATH];
  HKEY      hKey;
  DWORD     disposition;	//create the key; show if we have errors or not

  _tcscpy_s( szSavePath, sizeof(szSavePath), pPath );

  if( ERROR_SUCCESS == RegCreateKeyEx(HKEY_CURRENT_USER, KEYPATH, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, &disposition) )
  {
    if( ERROR_SUCCESS == RegSetValueEx( hKey, _T("SavePath"), 0, REG_EXPAND_SZ, (CONST BYTE *)szSavePath, _tcslen(szSavePath) ) )
    {
      bReturn = true;
    }
    RegCloseKey(hKey);
  }

  return bReturn;
}

int CALLBACK BrowseForFolderCallback(HWND hwnd,UINT uMsg,LPARAM lp, LPARAM pData)
{
	char szPath[MAX_PATH];

	switch(uMsg)
	{
		case BFFM_INITIALIZED:
			SendMessage(hwnd, BFFM_SETSELECTION, TRUE, pData);
			break;

		case BFFM_SELCHANGED: 
			if (SHGetPathFromIDList((LPITEMIDLIST) lp ,szPath)) 
			{
				SendMessage(hwnd, BFFM_SETSTATUSTEXT,0,(LPARAM)szPath);	
			}
			break;
	}

	return 0;
}

void CmfcDlgConfigure::OnBnClickedPath()
{
  bool          bReturn       = false;
  LPITEMIDLIST  pidlRoot      = NULL;
  char          szPath[MAX_PATH + 1];
  BROWSEINFO    bi            = {0};  // This struct holds the various options for the dialog
  LPMALLOC      pMalloc       = NULL; // Get the elusive Shell Task Allocator!!!

  if (SUCCEEDED(SHGetMalloc(&pMalloc)))
  {
    bi.hwndOwner = m_hWnd;
    bi.pszDisplayName = NULL;
    bi.lpszTitle = "Select Folder to Save Files";
    bi.ulFlags = BIF_NEWDIALOGSTYLE | BIF_RETURNONLYFSDIRS;
    bi.lpfn = BrowseForFolderCallback;
    bi.lParam = (LPARAM)m_strSavePath.c_str();

    // Now cause the dialog to appear.
    if(NULL!=(pidlRoot = SHBrowseForFolder(&bi)))
    {
      // Get a ASCII pathname from the LPITEMIDLIST struct.
      bReturn = SHGetPathFromIDList(pidlRoot, szPath)?true:false;

      // Free the returned item identifier list using the shell's task allocator
      pMalloc->Free(pidlRoot);
      pMalloc->Release();
    }
  }

  if(bReturn)
  {
    SetSavePath(szPath);
    GetSavePath();
    m_EdtSavePath.SetWindowText(szPath);
  }
}
