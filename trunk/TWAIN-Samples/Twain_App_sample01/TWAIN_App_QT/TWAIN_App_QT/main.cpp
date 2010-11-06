#include "stdafx.h"
#include "twain_app_qt.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
  //Instantiate the application
  CQTWAINApp a(argc, argv);
  //Instantiate the scripting engine
  QScriptEngine qsEngine;

  QString strStr = QLibraryInfo::location(QLibraryInfo::PluginsPath);
  //Create the main window
  TWAIN_App_QT w(&qsEngine);
  a.m_pMainWnd = &w;
  //Make the main window visible
  w.show();
  //Check for scripts
  if(3==argc && ("-s"==QString(argv[1]).toLower()))
  {
    QFile qfScript(argv[2]);
    //try to load the script file
    if(qfScript.open(QIODevice::ReadOnly))
    {
      //submit the script to the script engine
      QScriptValue qsv = qsEngine.evaluate(qfScript.readAll(), argv[2]);
      qfScript.close();
      if(qsEngine.hasUncaughtException())
      {
        QMessageBox::warning(&w, "Script Error", qsv.toString());
      }
    }
  }
  //proceed with normal application execution
  return a.exec();
}
