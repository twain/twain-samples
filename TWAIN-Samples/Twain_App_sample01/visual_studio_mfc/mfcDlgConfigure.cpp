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
#include "mfc.h"
#include "mfcDlgConfigure.h"

#include "..\src\dsminterface.h"
#include "TwainString.h"
#include ".\TW_Array_Dlg.h"
#include ".\mfcdlgconfigure.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/**
* gloabals
*/
TwainApp *g_pTWAINApp = NULL;
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

// Cmfc32DlgConfigureConfigure dialog



Cmfc32DlgConfigure::Cmfc32DlgConfigure(CWnd* pParent, int nIndex)
  : CDialog(Cmfc32DlgConfigure::IDD, pParent)
  ,m_sStc_DS(_T(""))
  ,m_sStc_ImageInfo(_T(""))
  ,m_sStc_ExtImageInfo(_T(""))
  ,m_bShowUI(FALSE)
{
  m_nIndex = nIndex;
  m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void Cmfc32DlgConfigure::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_STC_DS, m_sStc_DS);
  DDX_Text(pDX, IDC_IMAGEINFO, m_sStc_ImageInfo);
  DDX_Text(pDX, IDC_EXTIMAGEINFO, m_sStc_ExtImageInfo);
  DDX_Check(pDX, IDC_SHOWUI, m_bShowUI);
  DDX_Control(pDX, IDLC_CAPS, m_ListCtrl_Caps);
}

BEGIN_MESSAGE_MAP(Cmfc32DlgConfigure, CDialog)
  ON_WM_SYSCOMMAND()
  ON_WM_PAINT()
  ON_WM_QUERYDRAGICON()
  //}}AFX_MSG_MAP
  ON_WM_DESTROY()
  ON_BN_CLICKED(IDB_SCAN, OnBnClickedScan)
  ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
  ON_NOTIFY(NM_DBLCLK, IDLC_CAPS, &Cmfc32DlgConfigure::OnNMDblclkCaps)
END_MESSAGE_MAP()


// Cmfc32DlgConfigure message handlers

BOOL Cmfc32DlgConfigure::OnInitDialog()
{
  CDialog::OnInitDialog();

  // Set the icon for this dialog.  The framework does this automatically
  //  when the application's main window is not a dialog
  SetIcon(m_hIcon, TRUE);     // Set big icon
  SetIcon(m_hIcon, FALSE);    // Set small icon

  g_pTWAINApp = new TwainApp(m_hWnd);

  g_pTWAINApp->connectDSM();
  g_pTWAINApp->loadDS(m_nIndex);

  pTW_IDENTITY pID = g_pTWAINApp->getDataSource();
    
  if( NULL != pID )
  {
    m_sStc_DS.Format( "Scanner: %s\nManufacturer: %s\nProductFamily: %s\nInfo: %s", 
      pID->ProductName, pID->Manufacturer, pID->ProductFamily, 
      pID->Version.Info);
    UpdateData(false);
  }

  if(4 <= g_pTWAINApp->m_DSMState)
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
    col.cx        = 100;
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
    PopulateCurentValues();

    CEdit *pWnd = NULL;

    pWnd = (CEdit*)GetDlgItem(IDC_EXTIMAGEINFO);
    if(pWnd)
    {
      pWnd->SetTabStops(94);
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

void Cmfc32DlgConfigure::OnSysCommand(UINT nID, LPARAM lParam)
{
  CDialog::OnSysCommand(nID, lParam);
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void Cmfc32DlgConfigure::OnPaint() 
{
  CDialog::OnPaint();
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR Cmfc32DlgConfigure::OnQueryDragIcon()
{
  return static_cast<HCURSOR>(m_hIcon);
}

void Cmfc32DlgConfigure::OnDestroy()
{
  if(g_pTWAINApp)
  {
    g_pTWAINApp->exit();
    delete g_pTWAINApp;
    g_pTWAINApp = NULL;
  }

  CDialog::OnDestroy();

  return;
}

void Cmfc32DlgConfigure::ListSupportedCaps()
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

  g_pTWAINApp->get_CAP(CapSupportedCaps, MSG_GET);

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

  for(i=0; i<nCount; i++)
  {
    // we are not listing these CAPABILITIES
    if(pCapSupCaps->ItemList[i] != CAP_SUPPORTEDCAPS)
    {
      if(TWRC_SUCCESS == g_pTWAINApp->GetLabel(pCapSupCaps->ItemList[i], sCapName))
      {
        Item.pszText    = (LPSTR)sCapName.c_str();
      }
      else
      {
        Item.pszText    = (LPSTR)convertCAP_toString(pCapSupCaps->ItemList[i]);
      }

      Item.iItem      = i;
      Item.lParam     = (LPARAM)pCapSupCaps->ItemList[i];

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

void Cmfc32DlgConfigure::PopulateCurentValues()
{
  TW_UINT32         nItem;
  TW_UINT32         nCount    = m_ListCtrl_Caps.GetItemCount();
  TW_UINT16         CondCode;
  LV_ITEM           Item;
  TW_UINT16         cap       = 0;
  TW_CAPABILITY     Cap       = {0};
  TW_UINT32         QS        = 0;
  string            sItemValue;

  for(nItem=0; nItem<nCount; nItem++)
  {
    memset(&Item, 0, sizeof(Item));
    Item.iItem      = nItem;
    Item.iSubItem   = 0;
    Item.mask       = LVIF_PARAM;

    m_ListCtrl_Caps.GetItem(&Item);
    cap = (TW_UINT16)Item.lParam;

    // All the subitems are added as text
    Item.mask       = LVIF_TEXT;

    // get the capability that is supported
    Cap.Cap         = cap;
    Cap.hContainer  = 0;
    Cap.ConType     = TWON_DONTCARE16;

    // We will ignor what Query Supported has reproted about Message Get and get the current anyway.
    CondCode = g_pTWAINApp->get_CAP(Cap, MSG_GET);
    if(CondCode==TWCC_SUCCESS)
    {
      pTW_ONEVALUE pCap = (pTW_ONEVALUE)_DSM_LockMemory(Cap.hContainer);
      if(pCap)
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
              UINT nArrayCount = 100;
              int Val[100];
              GetArray(&Cap, Val, &nArrayCount);
              sItemValue = "";
              for(UINT i =0; i< nArrayCount; i++)
              {
                if(sItemValue.length()!=0)
                   sItemValue += ", ";
                sItemValue += convertCAP_Item_toString(Cap.Cap, (TW_UINT32)Val[i], type);
              }
            }
            else
            {
              TW_UINT32 uVal;
              getcurrent(&Cap, uVal);
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
              sItemValue = "";
            }
            else
            {
              getcurrent(&Cap, sItemValue);
            }
            break;
          }

          case TWTY_FIX32:
          {
            if(Cap.ConType == TWON_ARRAY)
            {
              sItemValue = "";
            }
            else
            {
              TW_FIX32 fix32;
              getcurrent(&Cap, fix32);
              CString value;
              value.Format("%d.%d", fix32.Whole, (int)((fix32.Frac/65536.0 + .0005)*1000) );
              sItemValue = value;
            }
            break;
          }

          case TWTY_FRAME:
          {
            if(Cap.ConType == TWON_ARRAY)
            {
              sItemValue = "";
            }
            else
            {
              TW_FRAME frame;
              getcurrent(&Cap, frame);
              CString value;
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
    }
    else
    {
      CString sError;
      sError.Format("<<%s>>", convertConditionCode_toString(CondCode));
      sItemValue = sError;
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

    Item.iSubItem   = 2;
    Item.pszText    = (char *)sItemValue.c_str();
    m_ListCtrl_Caps.SetItem(&Item);

    if( Cap.ConType == TWON_ARRAY
     || Cap.ConType == TWON_ENUMERATION )
    {
      TW_UINT32 nNumItems = 0;
      pTW_ARRAY pCap = (pTW_ARRAY)_DSM_LockMemory(Cap.hContainer);
      if(pCap)
      {
        nNumItems = pCap->NumItems;
        _DSM_UnlockMemory(Cap.hContainer);
      }
      CString Value;
      Value.Format("%d", nNumItems);
      sItemValue = Value;

      Item.iSubItem   = 3;
      Item.pszText    = (char *)sItemValue.c_str();
      m_ListCtrl_Caps.SetItem(&Item);
    }

    g_pTWAINApp->QuerySupport_CAP(cap, QS);

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

    Item.iSubItem   = 4;
    Item.pszText    = (char *)sItemValue.c_str();
    m_ListCtrl_Caps.SetItem(&Item);
  }
}


void Cmfc32DlgConfigure::OnNMDblclkCaps(NMHDR *pNMHDR, LRESULT *pResult)
{
  LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

  TW_UINT16       CAPLstData  = 0;
  bool            bChange     = false;
  CTW_Array_Dlg   Dlg(this);
  TW_CAPABILITY   Cap;
  LV_ITEM           Item;

  memset(&Item, 0, sizeof(Item));
  Item.iItem      = pNMItemActivate->iItem;
  Item.iSubItem   = 0;
  Item.mask       = LVIF_PARAM;
  m_ListCtrl_Caps.GetItem(&Item);

  // get the capability that is supported
  Cap.Cap         = (TW_UINT16)Item.lParam;
  Cap.hContainer  = 0;

  if(TWCC_SUCCESS==g_pTWAINApp->get_CAP(Cap, MSG_GET))
  {
    if( Cap.ConType == TWON_ENUMERATION 
       || Cap.ConType == TWON_ONEVALUE )
    {

      switch(Cap.Cap)
      {
        case CAP_XFERCOUNT:
        {
          TW_UINT32 Val;
          getcurrent(&Cap, Val);
          Val = (TW_INT16)Val == -1? 1: -1;// flop
          g_pTWAINApp->set_CapabilityOneValue(Cap.Cap, Val, TWTY_INT16);
          bChange = true;
          break;
        }

        default:
        {
          if( IDOK == GetUpdateValue(&Cap, &Dlg) )
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
                  twrc = g_pTWAINApp->set_CapabilityOneValue(Cap.Cap, (TW_UINT16)Dlg.m_SelectionData, type);
                  break;
                }

                case TWTY_FIX32:
                {
                  TW_FIX32 TWfix32 = FloatToFIX32( (float)(Dlg.m_SelectionData/100.0));
                  twrc = g_pTWAINApp->set_CapabilityOneValue(Cap.Cap, &TWfix32);
                  break;
                }
              }

              if(twrc == TWRC_SUCCESS || twrc == TWRC_CHECKSTATUS)
              {
                bChange = true;
              }
            }
          }
          break;
        }
      }
    }
  }

  if(bChange)
  {
    // Modifiying one CAP can change several others - repopulate the list of CAPS
    PopulateCurentValues();
  }
  *pResult = 0;
}

int Cmfc32DlgConfigure::GetUpdateValue( pTW_CAPABILITY pCap, CTW_Array_Dlg *pDlg)
{
  int nResponse = 0;

  if(pCap && 0 != pCap->hContainer)
    {
      switch(pCap->ConType)
      {
        case TWON_ENUMERATION:
        {
            pTW_ENUMERATION pCapPT = (pTW_ENUMERATION)_DSM_LockMemory(pCap->hContainer);
            CString         str;

            pDlg->m_Str_Name = convertCAP_toString(pCap->Cap);;

            switch(pCapPT->ItemType)
            {
              case TWTY_INT8:
              case TWTY_INT16:
              case TWTY_INT32:
              case TWTY_UINT8:
              case TWTY_UINT16:
              case TWTY_UINT32:
              case TWTY_BOOL:
              {
                for(TW_UINT32 x=0; x<pCapPT->NumItems; ++x)
                {
                  str   = convertCAP_Item_toString(pCap->Cap, ((pTW_UINT16)(&pCapPT->ItemList))[x], pCapPT->ItemType);
                  pDlg->m_itemString.Add(str);
                  pDlg->m_itemData.Add(((pTW_UINT16)(&pCapPT->ItemList))[x]);
                }
                pDlg->m_SelectionData = (DWORD)((pTW_UINT16)(&pCapPT->ItemList))[pCapPT->CurrentIndex];
              }
              break;

              case TWTY_STR32:
              case TWTY_STR64:
              case TWTY_STR128:
              case TWTY_STR255:
              {
                string sVal;
                for(TW_UINT32 x=0; x<pCapPT->NumItems; ++x)
                {
                  getcurrent(pCap, sVal);
                  str = sVal.c_str();
                  pDlg->m_itemString.Add(str);
                  pDlg->m_itemData.Add(x);
                }
                pDlg->m_SelectionData = pCapPT->CurrentIndex;
                break;
              }

              case TWTY_FIX32:
              {
                float valf;

                for(TW_UINT32 x=0; x<pCapPT->NumItems; ++x)
                {
                  valf = FIX32ToFloat( ((pTW_FIX32)(&pCapPT->ItemList))[x] );
                  str.Format( "%0.1f", valf);
                  pDlg->m_itemString.Add(str);
                  pDlg->m_itemData.Add((DWORD) valf*100);
                }
                valf = FIX32ToFloat( ((pTW_FIX32)(&pCapPT->ItemList))[pCapPT->CurrentIndex] );
                pDlg->m_SelectionData = (DWORD)valf*100;
              }
              break;
          }
          nResponse = (int)pDlg->DoModal();
          _DSM_UnlockMemory(pCap->hContainer);
        }
        break;

        case TWON_ONEVALUE:
        {
          pTW_ONEVALUE pCapPT = (pTW_ONEVALUE)_DSM_LockMemory(pCap->hContainer);
          switch(pCapPT->ItemType)
          {
            case TWTY_BOOL:
            {
              pDlg->m_SelectionData = pCapPT->Item? FALSE:TRUE;
              nResponse = IDOK;
            }
            break;

            default:
            break;
          }
          _DSM_UnlockMemory(pCap->hContainer);
        }
        break;
      }
    }
  return nResponse;
}

void Cmfc32DlgConfigure::OnBnClickedScan()
{
  g_pTWAINApp->m_DSMessage = (TW_UINT16)-1;

  UpdateData(true);

  // -Enable the data source. This puts us in state 5 which means that we
  // have to wait for the data source to tell us to move to state 6 and
  // start the transfer.  Once in state 5, no more set ops can be done on the
  // caps, only get ops.
  // -The scan will not start until the source calls the callback function
  // that was registered earlier.
  if( !g_pTWAINApp->enableDS(GetSafeHwnd(), m_bShowUI) )
  {
    return;
  }

  // now we have to wait until we hear something back from the DS.
  while((TW_UINT16)-1 == g_pTWAINApp->m_DSMessage)
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
    twRC = _DSM_Entry( g_pTWAINApp->getAppIdentity(),
                g_pTWAINApp->getDataSource(),
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
          g_pTWAINApp->m_DSMessage = twEvent.TWMessage;
          break;

        default:
          TRACE("\nError - Unknown message in MSG_PROCESSEVENT loop\n");
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

  if(g_pTWAINApp->m_DSMessage == MSG_XFERREADY)
  {
    // move to state 6 as a result of the data source. We can start a scan now.
    g_pTWAINApp->m_DSMState = 6;
    g_pTWAINApp->updateIMAGEINFO();
    UpdateImageInfo();
    StartScan();
    UpdateExtImageInfo();
  }

  // Scan is done, disable the ds, thus moving us back to state 4 where we
  // can negotiate caps again.
  g_pTWAINApp->disableDS();

  return;
}

void Cmfc32DlgConfigure::StartScan()
{
  TW_UINT32       mech;
  TW_CAPABILITY   Cap;

  Cap.Cap = ICAP_XFERMECH;
  Cap.hContainer = 0;

  if( TWCC_SUCCESS!=g_pTWAINApp->get_CAP(Cap, MSG_GETCURRENT)
    ||!getcurrent(&Cap, mech) )
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
   g_pTWAINApp->initiateTransfer_Native();
    break;

  case TWSX_FILE:
    {
      TW_UINT32 fileformat = TWFF_TIFF;
      Cap.Cap = ICAP_IMAGEFILEFORMAT;
      Cap.hContainer = 0;

      if(TWCC_SUCCESS==g_pTWAINApp->get_CAP(Cap, MSG_GETCURRENT))
      {
        getcurrent(&Cap, fileformat);
      }

      if(Cap.hContainer)
      {
       _DSM_Free(Cap.hContainer);
      }

      g_pTWAINApp->initiateTransfer_File((TW_UINT16)fileformat);
    }
    break;

  case TWSX_MEMORY:
    g_pTWAINApp->initiateTransfer_Memory();
    break;
  }

  return;
}

void Cmfc32DlgConfigure::OnBnClickedCancel()
{
  // TODO: Add your control notification handler code here
  g_pTWAINApp->exit();
  OnCancel();
}


void Cmfc32DlgConfigure::UpdateImageInfo()
{
  pTW_IMAGEINFO pII = g_pTWAINApp->getIMAGEINFO();
  m_sStc_ImageInfo.Format( 
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


void Cmfc32DlgConfigure::UpdateExtImageInfo()
{
  m_sStc_ExtImageInfo = g_pTWAINApp->getEXIMAGEINFO().c_str();
  UpdateData(false);
}

