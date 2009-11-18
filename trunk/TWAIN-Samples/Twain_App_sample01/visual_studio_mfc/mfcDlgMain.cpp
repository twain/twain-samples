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
* @file mfc32DlgMain.cpp
* Implementation file for dialog for the MFC TWAIN App Sample application
* @author JFL Peripheral Solutions Inc.
* @date October 2007
*/

#include "stdafx.h"
#include "mfc.h"
#include "mfcDlgMain.h"
#include "mfcDlgConfigure.h"

#include "twain.h"
#include "..\src\twainapp.h"
#include "..\src\dsminterface.h"
#include "TwainString.h"
#include ".\TW_Array_Dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
  CAboutDlg();

// Dialog Data
  enum { IDD = IDD_ABOUTBOX };

  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
  DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// Cmfc32DlgMain dialog



Cmfc32DlgMain::Cmfc32DlgMain(CWnd* pParent /*=NULL*/)
  : CDialog(Cmfc32DlgMain::IDD, pParent)
  , _pTWAINApp(NULL)
  , m_sStc_DS(_T(""))
{
  m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void Cmfc32DlgMain::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_STC_DS, m_sStc_DS);
  DDX_Control(pDX, IDL_DS, m_lst_DS);
  DDX_Control(pDX, IDB_CONNECT_DS, m_btn_Connect_DS);
  DDX_Control(pDX, IDB_DEFAULT_DS, m_btn_Default_DS);
}

BEGIN_MESSAGE_MAP(Cmfc32DlgMain, CDialog)
  ON_WM_SYSCOMMAND()
  ON_WM_PAINT()
  ON_WM_QUERYDRAGICON()
  //}}AFX_MSG_MAP
  ON_WM_DESTROY()
  ON_LBN_SELCHANGE(IDL_DS, OnLbnSelchangeDS)
  ON_LBN_DBLCLK(IDL_DS, OnLbnDblclkDs)
  ON_BN_CLICKED(IDB_CONNECT_DS, OnBnClickedConnectDs)
  ON_BN_CLICKED(IDB_DEFAULT_DS, OnBnClickedDefaultDs)
END_MESSAGE_MAP()


// Cmfc32DlgMain message handlers

BOOL Cmfc32DlgMain::OnInitDialog()
{
  CDialog::OnInitDialog();

  // Add "About..." menu item to system menu.

  // IDM_ABOUTBOX must be in the system command range.
  ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
  ASSERT(IDM_ABOUTBOX < 0xF000);

  CMenu* pSysMenu = GetSystemMenu(FALSE);
  if (pSysMenu != NULL)
  {
    CString strAboutMenu;
    strAboutMenu.LoadString(IDS_ABOUTBOX);
    if (!strAboutMenu.IsEmpty())
    {
      pSysMenu->AppendMenu(MF_SEPARATOR);
      pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
    }
  }

  // Set the icon for this dialog.  The framework does this automatically
  //  when the application's main window is not a dialog
  SetIcon(m_hIcon, TRUE);     // Set big icon
  SetIcon(m_hIcon, FALSE);    // Set small icon

  _pTWAINApp = new TwainApp(m_hWnd);

  PopulateDSList();

  return TRUE;  // return TRUE  unless you set the focus to a control
}

void Cmfc32DlgMain::OnSysCommand(UINT nID, LPARAM lParam)
{
  if ((nID & 0xFFF0) == IDM_ABOUTBOX)
  {
    CAboutDlg dlgAbout;
    dlgAbout.DoModal();
  }
  else
  {
    CDialog::OnSysCommand(nID, lParam);
  }
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void Cmfc32DlgMain::OnPaint() 
{
  if (IsIconic())
  {
    CPaintDC dc(this); // device context for painting

    SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

    // Center icon in client rectangle
    int cxIcon = GetSystemMetrics(SM_CXICON);
    int cyIcon = GetSystemMetrics(SM_CYICON);
    CRect rect;
    GetClientRect(&rect);
    int x = (rect.Width() - cxIcon + 1) / 2;
    int y = (rect.Height() - cyIcon + 1) / 2;

    // Draw the icon
    dc.DrawIcon(x, y, m_hIcon);
  }
  else
  {
    CDialog::OnPaint();
  }
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR Cmfc32DlgMain::OnQueryDragIcon()
{
  return static_cast<HCURSOR>(m_hIcon);
}

void Cmfc32DlgMain::OnDestroy()
{
  CDialog::OnDestroy();
  if(_pTWAINApp)
  {
    _pTWAINApp->exit();
    delete _pTWAINApp;
    _pTWAINApp = NULL;
  }
  return;
}

void Cmfc32DlgMain::PopulateDSList()
{
  _pTWAINApp->connectDSM();
  if(3 == _pTWAINApp->m_DSMState)
  {
    pTW_IDENTITY pID = NULL;
    int   i = 0;
    int   index = 0;
    int   nDefault = -1;

    if( NULL != (pID = _pTWAINApp->getDefaultDataSource()) ) // Get Default
    {
      nDefault = pID->Id;
    }

    while( NULL != (pID = _pTWAINApp->getDataSource(i)) )
    {
      index = m_lst_DS.AddString( pID->ProductName );
      if(LB_ERR == index)
      {
        break;
      }

      m_lst_DS.SetItemData( index, pID->Id );

      if(nDefault == pID->Id)
      {
        m_lst_DS.SetCurSel(index);
      }

      i++;
    }

    if( 0 < m_lst_DS.GetCount())
    {
      m_lst_DS.EnableWindow(true);
      m_btn_Connect_DS.EnableWindow(true);
      m_btn_Default_DS.EnableWindow(true);
      if(nDefault == -1)
      {
        m_lst_DS.SetCurSel(0);
      }
      OnLbnSelchangeDS();
    }

    _pTWAINApp->disconnectDSM();
  }
}

void Cmfc32DlgMain::OnLbnSelchangeDS()
{
  int           sel   = m_lst_DS.GetCurSel();
  TW_UINT32     index = (TW_UINT32)m_lst_DS.GetItemData(sel);
  pTW_IDENTITY  pID   = NULL;
  int           i     = 0;
  
  while(NULL != (pID = _pTWAINApp->getDataSource(i)) )
  {
    if( pID->Id == index )
    {
      m_sStc_DS.Format("Manufacturer: %s\nProductFamily: %s",pID->Manufacturer,pID->ProductFamily );
      UpdateData(false);
      break;
    }
    i++;
  }
}

void Cmfc32DlgMain::OnLbnDblclkDs()
{
  OnBnClickedConnectDs();
}

void Cmfc32DlgMain::OnBnClickedConnectDs()
{
  int sel = m_lst_DS.GetCurSel();
  TW_UINT32 ds_ID = (TW_UINT32)m_lst_DS.GetItemData(sel);
  ASSERT(ds_ID > 0 );

  Cmfc32DlgConfigure dlg(this, ds_ID);
  INT_PTR nResponse = dlg.DoModal();
}

void Cmfc32DlgMain::OnBnClickedDefaultDs()
{
  int sel = m_lst_DS.GetCurSel();

  _pTWAINApp->connectDSM();
  if(3 == _pTWAINApp->m_DSMState)
  {
    _pTWAINApp->setDefaultDataSource(sel);

  }
  _pTWAINApp->disconnectDSM();
}
