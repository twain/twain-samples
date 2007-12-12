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
* @file TWAINSampleDlg.cpp
* Sample DS Dlg implementation file.
* @author JFL Peripheral Solutions Inc.
* @date October 2005
*/

//#include "stdafx.h"
//#include "TWAINGUI.h"
#include "TWAINDS_SampleDlg.h"
#include "..\src\CTWAINDS_Sample1.h"
extern CTWAINDS_Base* g_pTWAINLayer;

// CTWAINSampleDlg dialog

IMPLEMENT_DYNAMIC(CTWAINSampleDlg, CDialog)
CTWAINSampleDlg::CTWAINSampleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTWAINSampleDlg::IDD, pParent)
{
}

CTWAINSampleDlg::~CTWAINSampleDlg()
{
}

void CTWAINSampleDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_CBX_RESOLUTION, m_cbxResolution);
  DDX_Control(pDX, IDC_CBX_PAPERSIZE, m_cbxPaperSize);
  DDX_Control(pDX, IDC_CBX_PIXELTYPE, m_cbxPixelType);
}


BEGIN_MESSAGE_MAP(CTWAINSampleDlg, CDialog)
  ON_BN_CLICKED(IDOK, OnBnClickedOk)
  ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()


// CTWAINSampleDlg message handlers

BOOL CTWAINSampleDlg::OnInitDialog()
{
  CDialog::OnInitDialog();

  // JMH: This is where you initialize all the controls
  //m_cbxResolution
  //m_cbxPaperSize
  //m_cbxPixelType
  // JMH: I suggest creating functions for syncing the contents of these controls
  // Each control should have a function to check to see if they are populated
  // According to the content of their respective TWAIN Capabilities...
  // So they can be re-used to update the contents when settings change
  //UpdateResolution();
  //UpdatePaperSize();
  //UpdatePixelType();

  return TRUE;  // return TRUE unless you set the focus to a control
  // EXCEPTION: OCX Property Pages should return FALSE
}


void CTWAINSampleDlg::OnBnClickedOk()
{
  g_pTWAINLayer->DoXferReadyEvent();
  return;
}

void CTWAINSampleDlg::OnBnClickedCancel()
{
  g_pTWAINLayer->DoCloseDSRequestEvent();
  return;
}
