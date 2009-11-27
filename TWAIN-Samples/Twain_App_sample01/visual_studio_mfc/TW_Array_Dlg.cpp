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
* @file TW_Array_Dlg.cpp
* Display an array of items to choose
* @author TWAIN Working Group
* @date October 2007
*/


#include "stdafx.h"
#include "mfc.h"
#include "TW_Array_Dlg.h"
#include ".\tw_array_dlg.h"


// CTW_Array_Dlg dialog

IMPLEMENT_DYNAMIC(CTW_Array_Dlg, CDialog)
CTW_Array_Dlg::CTW_Array_Dlg(CWnd* pParent /*=NULL*/)
  : CDialog(CTW_Array_Dlg::IDD, pParent)
{
}

CTW_Array_Dlg::~CTW_Array_Dlg()
{
}

void CTW_Array_Dlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  if(pDX->m_bSaveAndValidate)
  {
    CListBox *pList = (CListBox*)GetDlgItem(IDC_LIST1);
    if(pList)
    {
      int index = pList->GetCurSel();
      if(index != LB_ERR)
      {
        pList->GetText(index, m_SelectionString);
        m_SelectionData = (DWORD)pList->GetItemData(index);
      }
    }
  }
  else
  {
    CListBox *pList = (CListBox*)GetDlgItem(IDC_LIST1);
    if(pList)
    {
      pList->ResetContent();
      ASSERT(m_itemString.GetSize() == m_itemData.GetSize());//.GetCount());
      int index;
      for(int i = 0; i<m_itemString.GetSize(); i++) //GetCount()
      {
        index = pList->AddString(m_itemString.GetAt(i));
        if(index != LB_ERR)
        {
          pList->SetItemData(index, m_itemData.GetAt(i));
          if(m_SelectionData == i)
          {
            pList->SetCurSel(index);
          }
        }
      }
    }
  }
}


BEGIN_MESSAGE_MAP(CTW_Array_Dlg, CDialog)
  ON_LBN_DBLCLK(IDC_LIST1, OnLbnDblclkList1)
END_MESSAGE_MAP()


// CTW_Array_Dlg message handlers

BOOL CTW_Array_Dlg::OnInitDialog()
{
  CDialog::OnInitDialog();

  // TODO:  Add extra initialization here
  SetWindowText(m_Str_Name);

  return TRUE;  // return TRUE unless you set the focus to a control
  // EXCEPTION: OCX Property Pages should return FALSE
}

void CTW_Array_Dlg::OnLbnDblclkList1()
{
  CDialog::OnOK();
}
