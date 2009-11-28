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
* @file mfcDlgConfigure.h
* Header file for dialog for the MFC TWAIN App Sample application
* @author JFL Peripheral Solutions Inc.
* @date April 2007
*/

#pragma once

#ifndef __AFXWIN_H__
  #error include 'stdafx.h' before including this file for PCH
#endif

#include "CommonTWAIN.h"
#include "..\src\twainapp.h"
#include "afxcmn.h"

class CTW_Enum_Dlg;

struct CapSetting
{
  TW_UINT16   CapID;      // The ID of the Capability
  TW_UINT8    byChanged;  // Each bit is used to indicate if that colume has changed.
  TW_UINT8    bReadOnly;  // The Cap is only read only
};

/**
* Main dialalog window for applicaiton
*/
class CmfcDlgConfigure : public CDialog
{
// Construction
public:
/**
* standard constructor
*/
  CmfcDlgConfigure(CWnd* pParent, int nIndex);

// Dialog Data
  enum { IDD = IDD_MFC32_CONFIGURE };

  protected:
  virtual void DoDataExchange(CDataExchange* pDX);// DDX/DDV support
  void ListSupportedCaps();
  void PopulateCurentValues(bool bCheckForChange = true);
  void StartScan();

// Implementation
protected:
  HICON       m_hIcon;
  int         m_nIndex;
  CapSetting *m_pCapSettings;

  // Generated message map functions
  virtual BOOL OnInitDialog();
  afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
  afx_msg void OnPaint();
  afx_msg HCURSOR OnQueryDragIcon();
  DECLARE_MESSAGE_MAP()

public:
  afx_msg void OnDestroy();
  afx_msg void OnBnClickedScan();
  afx_msg void OnBnClickedUIOnly();
  afx_msg void OnBnClickedCancel();
  afx_msg void OnNMDblclkCaps(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnNMCustomdrawCaps(NMHDR *pNMHDR, LRESULT *pResult);
  void UpdateImageInfo();
  void UpdateExtImageInfo();
  void MarkUnchanged();

  CString     m_sStc_DS;
  CString     m_sStc_ImageInfo;
  CString     m_sStc_ExtImageInfo;
  CListCtrl   m_ListCtrl_Caps;
  BOOL        m_bShowUI;
};
