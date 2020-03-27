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
* @file CTWAINDS_FreeImage.cpp
* The main Data Source class.
* This class is derived from the Base class describing a TWAIN DS.
* @author TWAIN Working Group
* @date April 2007
*/
#include "CommonDS.h"
#include "QT_UI.h"
#include "Qt_MainForm.h"


CTWAIN_UI* CreateUI(CTWAINDS_FreeImage *pDS)
{
  return new QT_UI(pDS);
}

void DestroyUI(CTWAIN_UI* pUI)
{
  if(pUI)
  {
    delete ((QT_UI*)pUI);
  }
}

void UIThread::run()
{
  int argc = 0;
  m_pApp = new QApplication(argc, 0);
  if(m_Data.ShowUI)
  {
    m_pDlg = new MainForm(m_pUI);
    m_pDlg->show();
    m_pApp->exec();
  }
  delete m_pApp;
  m_pApp = 0;
}

QT_UI::QT_UI(CTWAINDS_FreeImage *pDS):CTWAIN_UI(pDS)
{
  m_pDlg=NULL;
  m_pChildWnd=NULL;
  m_pUIThread=NULL;
  m_pQtApp = NULL;
}

QT_UI::~QT_UI()
{
  if(m_pQtApp)
  {
    delete m_pQtApp;
	m_pQtApp = 0;
    //qApp = NULL; QT will do it in QCoreApplication destructor
  }
}


TW_INT16 QT_UI::DisplayTWAINGUI(TW_USERINTERFACE Data, bool bSetup, bool bIndicators)
{
  TW_INT16 nRes = CTWAIN_UI::DisplayTWAINGUI(Data, bSetup, bIndicators);
  if(nRes)
  {
    return nRes;
  }
  if(bSetup)
  {
    Data.ShowUI=1;
  }

  if(Data.ShowUI==0 && !bIndicators)
  {
    return TWRC_SUCCESS;
  }
  QApplication * pApp = qApp;
  if(qApp)
  {
    int nV1App=0, nV2App=0, nV3App=0;
    SSCANF(qVersion(),"%d.%d.%d",&nV1App, &nV2App, &nV3App);
    int nV1DS=(BYTE)(QT_VERSION>>16);
    int nV2DS=(BYTE)(QT_VERSION>>8);
    int nV3DS=(BYTE)QT_VERSION;
    if(nV1App<nV1DS || (nV1App==nV1DS && nV2App<nV2DS))
    {
      return TWRC_FAILURE;
    }
  }
#if defined (TWNDS_OS_WIN) || defined (TWNDS_OS_APPLE)
  if (!pApp)
  {
    int argc = 0;
    pApp = m_pQtApp = new QApplication(argc, 0);
//	  m_pQtApp->setAttribute(Qt::AA_DontUseNativeMenuBar,true);
	  m_pQtApp->setAttribute(Qt::AA_MacPluginApplication,true);
  }
#endif
  m_pChildWnd = NULL;
  if(!pApp)
  {
#if defined (TWNDS_OS_WIN)
    return TWRC_FAILURE;
#elif defined(TWNDS_OS_LINUX)// || defined (TWNDS_OS_APPLE)
    m_pUIThread = new UIThread(this, Data);
    m_pUIThread->start();   
#endif
  }
  else
  {
#if defined(TWNDS_OS_WIN) //|| defined (TWNDS_OS_APPLE)
    if(Data.hParent)
    {
      m_pChildWnd = new QWidget();
      long ll =  GetWindowLong((HWND)(m_pChildWnd->winId()),GWL_STYLE);
      SetWindowLong((HWND)(m_pChildWnd->winId()), GWL_STYLE, WS_CHILD|ll);
      SetParent((HWND)(m_pChildWnd->winId()), (HWND)Data.hParent);
    }
#endif
    
    if(Data.ShowUI)
    {
      m_pDlg = new MainForm(this,m_pChildWnd);
      if(m_pDlg)
      {
        m_pDlg->show();
      }
      else
      {
        return TWRC_FAILURE;
      }
    }
  }
  return TWRC_SUCCESS;
}

void QT_UI::DestroyTWAINGUI()
{
  CTWAIN_UI::DestroyTWAINGUI();
  
  if(m_pDlg)
  {
    delete m_pDlg;
    m_pDlg = NULL;
  }
  if(m_pChildWnd)
  {
    delete m_pChildWnd;
    m_pChildWnd = NULL;
  }
  if(m_pUIThread)
  {
    QEvent *pQEvent = new QEvent(QEvent::Quit);
   // pQEvent->setAccepted(false);
    m_pUIThread->m_pApp->postEvent(m_pUIThread->m_pApp,pQEvent);

    while(m_pUIThread->isRunning())
       ;
    delete m_pUIThread;
	m_pUIThread = 0;
  }
	if(m_pQtApp)
	{
		m_pQtApp->flush();
		m_pQtApp->exit();
		delete m_pQtApp;
		m_pQtApp = NULL;
	}
}

void QT_UI::UpdateProgress(bool bShow, unsigned char ucProgress, unsigned int unPageNo, string strProgressTitle)
{
  ;
}

unsigned int QT_UI::MessageBox(string strMessage,string strTitle, unsigned int unIconID)
{
  QMessageBox msgBox(QMessageBox::NoIcon, strTitle.c_str(),strMessage.c_str());
  return msgBox.exec();
}

bool QT_UI::processEvent(pTW_EVENT _pEvent)
{
#if defined (TWNDS_OS_WIN)
  if(qApp)
  {
    qApp->sendPostedEvents();
  }
#endif
  return false;
}
