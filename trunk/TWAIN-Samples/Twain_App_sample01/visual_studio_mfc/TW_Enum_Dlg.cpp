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
* @file TW_Enum_Dlg.cpp
* Display an array of items to choose
* @author TWAIN Working Group
* @date October 2007
*/


#include "stdafx.h"
#include "mfc.h"
#include "TW_Enum_Dlg.h"
#include "..\src\DSMInterface.h"
#include "TwainString.h"

// CTW_Enum_Dlg dialog

IMPLEMENT_DYNAMIC(CTW_Enum_Dlg, CDialog)
CTW_Enum_Dlg::CTW_Enum_Dlg(CWnd* pParent /*=NULL*/)
  : CDialog(CTW_Enum_Dlg::IDD, pParent)
  ,m_pCap(NULL)
  ,m_SelectionData(-1)
  ,m_SelectionString(_T(""))
{
}

CTW_Enum_Dlg::~CTW_Enum_Dlg()
{
}

void CTW_Enum_Dlg::DoDataExchange(CDataExchange* pDX)
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
}


BEGIN_MESSAGE_MAP(CTW_Enum_Dlg, CDialog)
  ON_LBN_DBLCLK(IDC_LIST1, OnLbnDblclkList1)
END_MESSAGE_MAP()


// CTW_Enum_Dlg message handlers

BOOL CTW_Enum_Dlg::OnInitDialog()
{
  CDialog::OnInitDialog();

  if(m_pCap && 0 != m_pCap->hContainer && m_pCap->ConType == TWON_ENUMERATION)
  {
    SetWindowText(convertCAP_toString(m_pCap->Cap));

    TW_UINT16       Type      = TWTY_UINT16;
    TW_UINT32       NumItems  = 0;
    TW_UINT32       CurIndex  = 0;
    CListBox       *pList = (CListBox*)GetDlgItem(IDC_LIST1);
    pTW_ENUMERATION pCapEnum  = (pTW_ENUMERATION)_DSM_LockMemory(m_pCap->hContainer);

    if(pCapEnum)
    {
      Type     = pCapEnum->ItemType;
      NumItems = pCapEnum->NumItems;
      CurIndex = pCapEnum->CurrentIndex;
      _DSM_UnlockMemory(m_pCap->hContainer);
      pCapEnum = NULL;
    }

    if(pList && NumItems)
    {
      CString         str;
      int             index;
      TW_UINT32       item;

      switch(Type)
      {
        case TWTY_INT8:
        case TWTY_INT16:
        case TWTY_INT32:
        case TWTY_UINT8:
        case TWTY_UINT16:
        case TWTY_UINT32:
        case TWTY_BOOL:
        {
          TW_UINT32       nVal;
          
          for(item=0; item<NumItems; ++item)
          {
            GetItem(m_pCap, item, nVal);
            str   = convertCAP_Item_toString(m_pCap->Cap, nVal, Type);
            index = pList->AddString(str);
            if(index != LB_ERR)
            {
              pList->SetItemData(index, nVal);
              if(CurIndex == item)
              {
                pList->SetCurSel(index);
              }
            }
          }
        }
        break;

        case TWTY_STR32:
        case TWTY_STR64:
        case TWTY_STR128:
        case TWTY_STR255:
        {
          string sVal;
          for(item=0; item<NumItems; ++item)
          {
            GetItem(m_pCap, item, sVal);
            str = sVal.c_str();
            index = pList->AddString(str);
            if(index != LB_ERR)
            {
              pList->SetItemData(index, item);
              if(CurIndex == item)
              {
                pList->SetCurSel(index);
              }
            }
          }
          break;
        }

        case TWTY_FIX32:
        {
          float     fVal;
          TW_FIX32  Fix32;
          for(item=0; item<NumItems; ++item)
          {
            GetItem(m_pCap, item, Fix32);
            fVal = FIX32ToFloat( Fix32 );
            str.Format( "%0.1f", fVal);
            index = pList->AddString(str);
            if(index != LB_ERR)
            {
              pList->SetItemData(index, (DWORD)(fVal*100));
              if(CurIndex == item)
              {
                pList->SetCurSel(index);
              }
            }
          }
        }
        break;

        case TWTY_FRAME:
        {
          TW_FRAME  Frame;
          for(item=0; item<NumItems; ++item)
          {
            GetItem(m_pCap, item, Frame);
            str.Format( "%d.%d  %d.%d  %d.%d  %d.%d", 
                 Frame.Left.Whole,   (int)((Frame.Left.Frac/65536.0 + .0005)*1000),
                 Frame.Right.Whole,  (int)((Frame.Right.Frac/65536.0 + .0005)*1000),
                 Frame.Top.Whole,    (int)((Frame.Top.Frac/65536.0 + .0005)*1000),
                 Frame.Bottom.Whole, (int)((Frame.Bottom.Frac/65536.0 + .0005)*1000) );

            index = pList->AddString(str);
            if(index != LB_ERR)
            {
              pList->SetItemData(index, item);
              if(CurIndex == item)
              {
                pList->SetCurSel(index);
              }
            }
          }
        }
        break;

        default:
          MessageBox("Datatype not implemented.  Patches welcome.", "Not Implemented");
          //return IDCANCEL;
        break;
      }
    }
  }

  return TRUE;  // return TRUE unless you set the focus to a control
  // EXCEPTION: OCX Property Pages should return FALSE
}

void CTW_Enum_Dlg::OnLbnDblclkList1()
{
  CDialog::OnOK();
}
