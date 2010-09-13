#include "stdafx.h"
#include "twain_app_qt.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
  CQTWAINApp a(argc, argv);
  TWAIN_App_QT w;
  a.m_pMainWnd = &w;
  w.show();
  return a.exec();
}
