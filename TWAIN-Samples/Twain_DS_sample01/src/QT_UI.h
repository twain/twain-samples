/***************************************************************************
* Copyright � 2007 TWAIN Working Group:  
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
* @file QT_UI.h
* UI base class - QT based 
* @author TWAIN Working Group
* @date Nov 2009
*/
#include "CTWAINDS_FreeImage.h"

#ifndef __QT_UI_H__
#define __QT_UI_H__

#include "TWAIN_UI.h"
#include <QThread>
#include <QtCore/QCoreApplication>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMessageBox>

class MainForm;
class QT_UI;

class UIThread : public QThread
{
public:
   UIThread(QT_UI *pUI, TW_USERINTERFACE Data){m_pUI=pUI;m_Data=Data;}
   void run();
   TW_USERINTERFACE m_Data;
   QT_UI *m_pUI;
   QApplication *m_pApp;
   MainForm *m_pDlg;
};

class QT_UI : public CTWAIN_UI
{
public:
  QT_UI(CTWAINDS_FreeImage *pDS);
  ~QT_UI();
  /**
  * Will show the TWAIN GUI
  * @param[in] _pData contains info about if the UI should be shown etc.
  * @return a valid TWRC_xxxx return code.
  */
  TW_INT16 DisplayTWAINGUI(TW_USERINTERFACE Data, bool bSetup, bool bIndicators);

  /**
  * Close the user interface for TWAIN
  */
  void DestroyTWAINGUI();
  void UpdateProgress(bool bShow, unsigned char ucProgress, unsigned int unPageNo, string strProgressTitle);
  unsigned int MessageBox(string strMessage,string strTitle, unsigned int unIconID);
  bool processEvent(pTW_EVENT _pEvent);

  MainForm *m_pDlg;
  QWidget  *m_pChildWnd;
  UIThread *m_pUIThread;
  QApplication  *m_pQtApp;
};

#endif // __QT_UI_H__
