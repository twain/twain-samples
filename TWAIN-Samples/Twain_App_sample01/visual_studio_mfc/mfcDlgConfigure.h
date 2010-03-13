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
#include "afxwin.h"

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
class CmfcDlgConfigure : public CDialog, public TwainApp
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
  void UpdateButtons();

// Implementation
protected:
  HICON       m_hIcon;
  int         m_nIndex;
  CapSetting *m_pCapSettings;
  CEdit       m_EdtSavePath;

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
  afx_msg void OnBnClickedPath();

  void UpdateImageInfo();
  void UpdateExtImageInfo();
  bool GetSavePath();
  bool SetSavePath(const TCHAR *pPath);

/**
* Override the virtual function to retrieve the extended image info for the current image
* @return true on succes
*/
  virtual void updateEXTIMAGEINFO();

/**
* Mark all the SupportedCaps as unchanged
*/
  void MarkUnchanged();

  CString     m_sStr_DS;              /**< The string to display of the opened DS */
  CString     m_sStr_ImageInfo;       /**< The string to display of the last ImageInfo returned by the scanner */
  CString     m_sStr_ExtImageInfo;    /**< The string to display of the extImageInfo returned by the scanner */
  CListCtrl   m_ListCtrl_Caps;        /**< The Dialog Control that displays all supported Capabilites and their current values */
  BOOL        m_bShowUI;              /**< Scan with the GUI visable */
  BOOL        m_bKeepEnabled;         /**< Keep the DS GUI displayed after a scan - User press cancel to finish scanning */
  bool        m_bBusy;                /**< The application is waiting for a responce from TWAIN messages*/
};
