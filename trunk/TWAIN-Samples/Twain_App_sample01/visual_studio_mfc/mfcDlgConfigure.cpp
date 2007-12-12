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

#include "CommonTWAIN.h"
#include "..\src\twainapp.h"
#include "..\src\dsminterface.h"
#include "..\src\TwainApp_ui.h"
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
*/
TW_UINT16 FAR PASCAL DSMCallback(pTW_IDENTITY _pOrigin,
            pTW_IDENTITY _pDest,
            TW_UINT32    _DG,
            TW_UINT16    _DAT,
            TW_UINT16    _MSG,
            TW_MEMREF    _pData)
{
  TW_UINT16 twrc = TWRC_SUCCESS;

  // we are only waiting for callbacks from our datasource, so validate
  // that the originator.
  if( 0 == _pOrigin ||
      _pOrigin->Id != g_pTWAINApp->getDataSource()->Id )
  {
    return TWRC_FAILURE;
  }
  switch (_MSG)
  {
    case MSG_XFERREADY:
      g_pTWAINApp->m_DSMState = 6;
      // move to state 6 as a result of the data source. We can start a scan now.
      g_pTWAINApp->startScan();
      // Scan is done, disable the ds, thus moving us back to state 4 where we
      // can negotiate caps again.
      g_pTWAINApp->disableDS();
      break;

    case MSG_CLOSEDSREQ:
      g_pTWAINApp->disableDS();
      break;

    case MSG_CLOSEDSOK:
      g_pTWAINApp->disableDS();
      break;

    case MSG_NULL:
      // no message returned from the source
      break;

    default:
      TRACE("Error - Unknown message in callback routine");
      twrc = TWRC_FAILURE;
      break;
  }
  return twrc;
}

// Cmfc32DlgConfigureConfigure dialog



Cmfc32DlgConfigure::Cmfc32DlgConfigure(CWnd* pParent, int nIndex)
  : CDialog(Cmfc32DlgConfigure::IDD, pParent)
  , m_sStc_DS(_T(""))
  ,m_sStc_ImageInfo(_T(""))
  ,m_sStc_ExtImageInfo(_T(""))
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
  DDX_Control(pDX, IDL_CAPS, m_lst_Caps);
}

BEGIN_MESSAGE_MAP(Cmfc32DlgConfigure, CDialog)
  ON_WM_SYSCOMMAND()
  ON_WM_PAINT()
  ON_WM_QUERYDRAGICON()
  //}}AFX_MSG_MAP
  ON_WM_DESTROY()
  ON_LBN_SELCHANGE(IDL_CAPS, OnLbnSelchangeCAPS)
  ON_BN_CLICKED(IDB_SCAN, OnBnClickedScan)
  ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
  ON_LBN_DBLCLK(IDL_CAPS, OnLbnDblclkCaps)
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
    ListCaps();
    
    if( 0 < m_lst_Caps.GetCount())
      {
        m_lst_Caps.EnableWindow(true);
        m_lst_Caps.SetCurSel(0);
        OnLbnSelchangeCAPS();
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



void Cmfc32DlgConfigure::ListCaps()
{
  TW_UINT16   uVal;
  TW_INT16    nVal;
  TW_FIX32    fVal;
  int         i = 0;
  int         index = 0;
  int         dx=0;
  CString     str;
  CString     name;
  CSize       sz;
  CDC        *pDC   = m_lst_Caps.GetDC();

  m_lst_Caps.ResetContent();

  if( g_pTWAINApp->getCAP_XFERCOUNT(nVal) )
  {
    name = convertICAP_toString(CAP_XFERCOUNT);
    sz = pDC->GetTextExtent(name);
    if (sz.cx > dx)
    {
      dx = sz.cx;
    }
    str.Format("%s:\t%d", name, nVal);
    index = m_lst_Caps.InsertString( -1,  str );
    if(LB_ERR != index)
    {
      m_lst_Caps.SetItemData( index, CAP_XFERCOUNT );
    }
  }

  if( g_pTWAINApp->getICAP_XFERMECH(uVal) )
  {
    name = convertICAP_toString(ICAP_XFERMECH);
    sz = pDC->GetTextExtent(name);
    if (sz.cx > dx)
    {
      dx = sz.cx;
    }
    str.Format("%s:\t%s", name, convertICAP_XFERMECH_toString(uVal));
    index = m_lst_Caps.InsertString( -1,  str );
    if(LB_ERR != index)
    {
      m_lst_Caps.SetItemData( index, ICAP_XFERMECH );
    }
  }

  if( g_pTWAINApp->getICAP_PIXELTYPE(uVal) )
  {
    name = convertICAP_toString(ICAP_PIXELTYPE);
    sz = pDC->GetTextExtent(name);
    if (sz.cx > dx)
    {
      dx = sz.cx;
    }
    str.Format( "%s:\t%s", name, convertICAP_PIXELTYPE_toString(uVal) );
    index = m_lst_Caps.InsertString( -1,  str );
    if(LB_ERR != index)
    {
      m_lst_Caps.SetItemData( index, ICAP_PIXELTYPE );
    }
  }

  if( g_pTWAINApp->getICAP_BITDEPTH(uVal) )
  {
    name = convertICAP_toString(ICAP_BITDEPTH);
    sz = pDC->GetTextExtent(name);
    if (sz.cx > dx)
    {
      dx = sz.cx;
    }
    str.Format("%s:\t%d", name, uVal);
    index = m_lst_Caps.InsertString( -1,  str );
    if(LB_ERR != index)
    {
      m_lst_Caps.SetItemData( index, ICAP_BITDEPTH );
    }
  }

  if( g_pTWAINApp->getICAP_IMAGEFILEFORMAT(uVal) )
  {
    name = convertICAP_toString(ICAP_IMAGEFILEFORMAT);
    sz = pDC->GetTextExtent(name);
    if (sz.cx > dx)
    {
      dx = sz.cx;
    }
    str.Format("%s:\t%s", name, convertICAP_IMAGEFILEFORMAT_toString(uVal) );
    index = m_lst_Caps.InsertString( -1,  str );
    if(LB_ERR != index)
    {
      m_lst_Caps.SetItemData( index, ICAP_IMAGEFILEFORMAT );
    }
  }

  if( g_pTWAINApp->getICAP_COMPRESSION(uVal) )
  {
    name = convertICAP_toString(ICAP_COMPRESSION);
    sz = pDC->GetTextExtent(name);
    if (sz.cx > dx)
    {
      dx = sz.cx;
    }
    str.Format("%s:\t%s", name, convertICAP_COMPRESSION_toString(uVal) );
    index = m_lst_Caps.InsertString( -1,  str );
    if(LB_ERR != index)
    {
      m_lst_Caps.SetItemData( index, ICAP_COMPRESSION );
    }
  }

  if( g_pTWAINApp->getICAP_XRESOLUTION(fVal) )
  {
    name = convertICAP_toString(ICAP_XRESOLUTION);
    sz = pDC->GetTextExtent(name);
    if (sz.cx > dx)
    {
      dx = sz.cx;
    }
    str.Format("%s:\t%0.1f", name, FIX32ToFloat(fVal) );
    index = m_lst_Caps.InsertString( -1,  str );
    if(LB_ERR != index)
    {
      m_lst_Caps.SetItemData( index, ICAP_XRESOLUTION );
    }
  }

  if( g_pTWAINApp->getICAP_YRESOLUTION(fVal) )
  {
    name = convertICAP_toString(ICAP_YRESOLUTION);
    sz = pDC->GetTextExtent(name);
    if (sz.cx > dx)
    {
      dx = sz.cx;
    }
    str.Format("%s:\t%0.1f", name, FIX32ToFloat(fVal) );
    index = m_lst_Caps.InsertString( -1,  str );
    if(LB_ERR != index)
    {
      m_lst_Caps.SetItemData( index, ICAP_YRESOLUTION );
    }
  }

  sz = pDC->GetTextExtent("   ");
  dx += sz.cx;
  m_lst_Caps.ReleaseDC(pDC);

  // NOTE: Convert pixels to dialog units.
  m_lst_Caps.SetTabStops((dx*4) / LOWORD(::GetDialogBaseUnits()));
}



void Cmfc32DlgConfigure::OnLbnSelchangeCAPS()
{
  // TODO: Add your control notification handler code here
}

void Cmfc32DlgConfigure::OnLbnDblclkCaps()
{
  int             CAPLstIndex = 0;
  TW_UINT16       CAPLstData  = 0;
  bool            bChange     = false;
  pTW_CAPABILITY  pCap        = NULL;
  CString         str;
  CTW_Array_Dlg   Dlg(this);

  CAPLstIndex = m_lst_Caps.GetCurSel();
  if(LB_ERR != CAPLstIndex)
  {
    switch(m_lst_Caps.GetItemData(CAPLstIndex))
    {
    case CAP_XFERCOUNT:
      /// @todo support updating OneValue
      TW_INT16 val;
      g_pTWAINApp->getCAP_XFERCOUNT(val);
      val = val == -1? 1: -1;// flop
      g_pTWAINApp->set_CAP_XFERCOUNT(val);
      bChange = true;
      break;

    case ICAP_XFERMECH:
      pCap = &g_pTWAINApp->m_ICAP_XFERMECH;
      if(0 == pCap->hContainer)
      {
        return;
      }
      switch(pCap->ConType)
      {
        case TWON_ENUMERATION:
          {
            pTW_ENUMERATION pCapPT = (pTW_ENUMERATION)_DSM_LockMemory(pCap->hContainer);
            if(TWTY_UINT16 == pCapPT->ItemType)
            {
              Dlg.m_Str_Name = convertICAP_toString(pCap->Cap);;

              for(TW_UINT32 x=0; x<pCapPT->NumItems; ++x)
              {
                str   = convertICAP_XFERMECH_toString(((pTW_UINT16)(&pCapPT->ItemList))[x]);
                Dlg.m_itemString.Add(str);
                Dlg.m_itemData.Add(((pTW_UINT16)(&pCapPT->ItemList))[x]);
              }
              Dlg.m_SelectionData = (DWORD)((pTW_UINT16)(&pCapPT->ItemList))[pCapPT->CurrentIndex];
              INT_PTR nResponse = Dlg.DoModal();
              if(nResponse == IDOK)
              {
                g_pTWAINApp->set_ICAP_XFERMECH((TW_UINT16)Dlg.m_SelectionData);
                bChange = true;
              }
            }
            _DSM_UnlockMemory((TW_MEMREF)pCapPT);
          }
          break;
        case TWON_ONEVALUE:
          /** TODO: add ICAP_XFERMECH : TWON_ONEVALUE */
          break;
      }
      break;

    case ICAP_IMAGEFILEFORMAT:
      pCap = &g_pTWAINApp->m_ICAP_IMAGEFILEFORMAT;
      if(0 == pCap->hContainer)
      {
        return;
      }
      switch(pCap->ConType)
      {
        case TWON_ENUMERATION:
          {
            pTW_ENUMERATION pCapPT = (pTW_ENUMERATION)_DSM_LockMemory(pCap->hContainer);
            if(TWTY_UINT16 == pCapPT->ItemType)
            {
              Dlg.m_Str_Name = convertICAP_toString(pCap->Cap);;

              for(TW_UINT32 x=0; x<pCapPT->NumItems; ++x)
              {
                str   = convertICAP_IMAGEFILEFORMAT_toString(((pTW_UINT16)(&pCapPT->ItemList))[x]);
                Dlg.m_itemString.Add(str);
                Dlg.m_itemData.Add(((pTW_UINT16)(&pCapPT->ItemList))[x]);
              }
              Dlg.m_SelectionData = (DWORD)((pTW_UINT16)(&pCapPT->ItemList))[pCapPT->CurrentIndex];
              INT_PTR nResponse = Dlg.DoModal();
              if(nResponse == IDOK)
              {
                g_pTWAINApp->set_ICAP_IMAGEFILEFORMAT((TW_UINT16)Dlg.m_SelectionData);
                bChange = true;
              }
            }
            _DSM_UnlockMemory((TW_MEMREF)pCapPT);
          }
          break;
        case TWON_ONEVALUE:
          /** TODO: add ICAP_IMAGEFILEFORMAT : TWON_ONEVALUE */
          break;
      }
      break;

    case ICAP_COMPRESSION:
      pCap = &g_pTWAINApp->m_ICAP_COMPRESSION;
      if(0 == pCap->hContainer)
      {
        return;
      }
      switch(pCap->ConType)
      {
        case TWON_ENUMERATION:
          {
            pTW_ENUMERATION pCapPT = (pTW_ENUMERATION)_DSM_LockMemory(pCap->hContainer);
            if(TWTY_UINT16 == pCapPT->ItemType)
            {
              Dlg.m_Str_Name = convertICAP_toString(pCap->Cap);;

              for(TW_UINT32 x=0; x<pCapPT->NumItems; ++x)
              {
                str   = convertICAP_COMPRESSION_toString(((pTW_UINT16)(&pCapPT->ItemList))[x]);
                Dlg.m_itemString.Add(str);
                Dlg.m_itemData.Add(((pTW_UINT16)(&pCapPT->ItemList))[x]);
              }
              Dlg.m_SelectionData = (DWORD)((pTW_UINT16)(&pCapPT->ItemList))[pCapPT->CurrentIndex];
              INT_PTR nResponse = Dlg.DoModal();
              if(nResponse == IDOK)
              {
                g_pTWAINApp->set_ICAP_COMPRESSION((TW_UINT16)Dlg.m_SelectionData);
                bChange = true;
              }
            }
            _DSM_UnlockMemory((TW_MEMREF)pCapPT);
          }
          break;
        case TWON_ONEVALUE:
          /** TODO: add ICAP_COMPRESSION : TWON_ONEVALUE */
          break;
      }
      break;

    case ICAP_PIXELTYPE:
      pCap = &g_pTWAINApp->m_ICAP_PIXELTYPE;
      if(0 == pCap->hContainer)
      {
        return;
      }
      switch(pCap->ConType)
      {
        case TWON_ENUMERATION:
          {
            pTW_ENUMERATION pCapPT = (pTW_ENUMERATION)_DSM_LockMemory(pCap->hContainer);
            if(TWTY_UINT16 == pCapPT->ItemType)
            {
              Dlg.m_Str_Name = convertICAP_toString(pCap->Cap);;

              for(TW_UINT32 x=0; x<pCapPT->NumItems; ++x)
              {
                str   = convertICAP_PIXELTYPE_toString(((pTW_UINT16)(&pCapPT->ItemList))[x]);
                Dlg.m_itemString.Add(str);
                Dlg.m_itemData.Add(((pTW_UINT16)(&pCapPT->ItemList))[x]);
              }
              Dlg.m_SelectionData = (DWORD)((pTW_UINT16)(&pCapPT->ItemList))[pCapPT->CurrentIndex];
              INT_PTR nResponse = Dlg.DoModal();
              if(nResponse == IDOK)
              {
                g_pTWAINApp->set_ICAP_PIXELTYPE((TW_UINT16)Dlg.m_SelectionData);
                bChange = true;
              }
            }
            _DSM_UnlockMemory((TW_MEMREF)pCapPT);
          }
          break;
        case TWON_ONEVALUE:
          /** TODO: add ICAP_PIXELTYPE : TWON_ONEVALUE */
          break;
      }
      break;

    case ICAP_BITDEPTH:
      pCap = &g_pTWAINApp->m_ICAP_BITDEPTH;
      if(0 == pCap->hContainer)
      {
        return;
      }
      switch(pCap->ConType)
      {
        case TWON_ENUMERATION:
          {
            pTW_ENUMERATION pCapPT = (pTW_ENUMERATION)_DSM_LockMemory(pCap->hContainer);
            if(TWTY_UINT16 == pCapPT->ItemType)
            {
              Dlg.m_Str_Name = convertICAP_toString(pCap->Cap);;

              for(TW_UINT32 x=0; x<pCapPT->NumItems; ++x)
              {
                str.Format("%d", ((pTW_UINT16)(&pCapPT->ItemList))[x]);
                Dlg.m_itemString.Add(str);
                Dlg.m_itemData.Add(((pTW_UINT16)(&pCapPT->ItemList))[x]);
              }
              Dlg.m_SelectionData = (DWORD)((pTW_UINT16)(&pCapPT->ItemList))[pCapPT->CurrentIndex];
              INT_PTR nResponse = Dlg.DoModal();
              if(nResponse == IDOK)
              {
                g_pTWAINApp->set_ICAP_BITDEPTH((TW_UINT16)Dlg.m_SelectionData);
                bChange = true;
              }
            }
            _DSM_UnlockMemory((TW_MEMREF)pCapPT);
          }
          break;
        case TWON_ONEVALUE:
          /** TODO: add ICAP_BITDEPTH : TWON_ONEVALUE */
          break;
      }
      break;

    case ICAP_XRESOLUTION:
    case ICAP_YRESOLUTION:
      pCap = &g_pTWAINApp->m_ICAP_XRESOLUTION;
      if(0 == pCap->hContainer)
      {
        return;
      }
      switch(pCap->ConType)
      {
        case TWON_ENUMERATION:
          {
            pTW_ENUMERATION pCapPT = (pTW_ENUMERATION)_DSM_LockMemory(pCap->hContainer);
            if(TWTY_FIX32 == pCapPT->ItemType)
            {
              Dlg.m_Str_Name = convertICAP_toString(pCap->Cap);;
              float valf;

              for(TW_UINT32 x=0; x<pCapPT->NumItems; ++x)
              {
                valf = FIX32ToFloat( ((pTW_FIX32)(&pCapPT->ItemList))[x] );
                str.Format( "%0.1f", valf);
                Dlg.m_itemString.Add(str);
                Dlg.m_itemData.Add((DWORD) valf*100);
              }
              valf = FIX32ToFloat( ((pTW_FIX32)(&pCapPT->ItemList))[pCapPT->CurrentIndex] );
              Dlg.m_SelectionData = (DWORD)valf*100;
              INT_PTR nResponse = Dlg.DoModal();
              if(nResponse == IDOK)
              {
                TW_FIX32 TWfix32 = FloatToFIX32( (float)(Dlg.m_SelectionData/100.0));
                g_pTWAINApp->set_ICAP_RESOLUTION(ICAP_XRESOLUTION, &TWfix32);
                g_pTWAINApp->set_ICAP_RESOLUTION(ICAP_YRESOLUTION, &TWfix32);
                bChange = true;
              }
            }
            _DSM_UnlockMemory((TW_MEMREF)pCapPT);
          }
          break;
        case TWON_ONEVALUE:
          /** TODO: add ICAP_XRESOLUTION : TWON_ONEVALUE */
          break;
      }
      break;

    case LB_ERR:
    default:
      bChange = false;
      break;

    }

    if(bChange)
    {
      // Modifiying one CAP can change several others - repopulate the list of CAPS
      ListCaps();
        
      if( 0 < m_lst_Caps.GetCount())
      {
        m_lst_Caps.EnableWindow(true);
        m_lst_Caps.SetCurSel(CAPLstIndex);
        OnLbnSelchangeCAPS();
      }
    }
  }
}

void Cmfc32DlgConfigure::OnBnClickedScan()
{
  // -Enable the data source. This puts us in state 5 which means that we
  // have to wait for the data source to tell us to move to state 6 and
  // start the transfer.  Once in state 5, no more set ops can be done on the
  // caps, only get ops.
  // -The scan will not start until the source calls the callback function
  // that was registered earlier.
  if( !g_pTWAINApp->enableDS() )
  {
    return;
  }

  // If we are using callbacks, there is nothing to do here except sleep
  // and wait for our callback from the DS.  If we are not using them, 
  // then we have to poll the DSM.
  if(gUSE_CALLBACKS)
  {
//#ifdef TWH_CMP_MSC
//    Sleep(1000);
//#else
//    sleep(1);
//#endif //TWH_CMP_MSC

    // At this point the source has sent us a callback saying that it is ready to
    // transfer the image.

    // move to state 6 as a result of the data source. We can start a scan now.
//    g_pTWAINApp->startScan();

    // Scan is done, disable the ds, thus moving us back to state 4 where we
    // can negotiate caps again.
//    g_pTWAINApp->disableDS();
    //OnOK();
  }

}

BOOL Cmfc32DlgConfigure::PreTranslateMessage(MSG* pMsg)
{
  if(5 <= g_pTWAINApp->m_DSMState)
  {
    TW_INT16  rc;
    TW_EVENT  twEvent = {0};

    twEvent.pEvent    = (TW_MEMREF)pMsg;
    twEvent.TWMessage = MSG_NULL;

    rc = _DSM_Entry( g_pTWAINApp->getAppIdentity(),
                g_pTWAINApp->getDataSource(),
                DG_CONTROL,
                DAT_EVENT,
                MSG_PROCESSEVENT,
                (TW_MEMREF)&twEvent);

    if( TWRC_DSEVENT == rc )
    {
          switch (twEvent.TWMessage)
          {
            case MSG_XFERREADY:
                  
                  
                  g_pTWAINApp->m_DSMState = 6;
                  g_pTWAINApp->startScan();
                  UpdateImageInfo();
                  //UpdateExtImageInfo();
                  g_pTWAINApp->disableDS();
                  break;
            case MSG_CLOSEDSREQ:
                  g_pTWAINApp->disableDS();
                  break;
            case MSG_CLOSEDSOK:
                  g_pTWAINApp->disableDS();
                  break;
            case MSG_NULL:
                  // no message returned from the source
                  break;
          }
      return FALSE;
    }
  }

  return CDialog::PreTranslateMessage(pMsg);
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
                          "Length:\t%d \tPlanar:       \t%d\r\n"
                          "Res x: \t%d \tPixelType:    \t%d\r\n"
                          "Res y: \t%d \tCompression:  \t%d\r\n",
                          pII->ImageWidth,  pII->BitsPerPixel, 
                          pII->ImageLength, pII->Planar,
                          pII->XResolution, pII->PixelType, 
                          pII->YResolution, pII->Compression  );
  // \tBitsPerSample:\t%d\r\n"pII->BitsPerSample,
  UpdateData(false);
}


void Cmfc32DlgConfigure::UpdateExtImageInfo()
{

}
